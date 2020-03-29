#include "Mesh.h"
#include "Common.h"
#include "Debug.h"
#include "D3DResource.h"
#include "hoLib.h"
#include "WorldTransMtrxStack.h"
#include "MyAllocateHierarchy.h"
#include "Direct3D.h"
#include "Math.h"
#include "Path.h"
#include <algorithm>
#include "Vector.h"

namespace ho {

//コンストラクタ
MeshManager::MeshManager()
{
}

//デストラクタ
MeshManager::~MeshManager() 
{
	//全てのメッシュオブジェクトを削除
	for (std::vector<Mesh *>::iterator itr = vectorpMeshObj.begin(); itr != vectorpMeshObj.end(); itr++)
		SDELETE(*itr);
}

//メッシュオブジェクトを登録
DWORD MeshManager::AddMesh(Mesh *pMeshObj) 
{
	vectorpMeshObj.push_back(pMeshObj);

	return vectorpMeshObj.size(); //登録されたメッシュのインデックスが返る
}

//メッシュオブジェクトを削除
void MeshManager::DeleteMesh(Mesh *pMeshObj) 
{
	//該当するメッシュオブジェクトを探して削除
	for (std::vector<Mesh *>::iterator itr = vectorpMeshObj.begin(); itr != vectorpMeshObj.end(); itr++)
		if (*itr == pMeshObj)
			SDELETE(*itr);

	return;
}

//メッシュオブジェクトを削除
void MeshManager::DeleteMesh(LPD3DXMESH pMesh) 
{
	//該当するメッシュオブジェクトを探して削除
	for (std::vector<Mesh *>::iterator itr = vectorpMeshObj.begin(); itr != vectorpMeshObj.end(); itr++)
		if (*itr) //メッシュオブジェクトが存在する場合
			if ((*itr)->GetpD3DXMesh() == pMesh)
				SDELETE(*itr);

	return;
}









//コンストラクタ（作成済みのメッシュを包む）
Mesh::Mesh(LPD3DXMESH pD3DXMesh)
{
	this->pDirect3DObj = NULL;
	this->pD3DResourceManagerObj = NULL;
	this->pD3DXMesh = pD3DXMesh;
	this->pAH = NULL;

	Options = pD3DXMesh->GetOptions(); //作成時のオプションを保存

	CalcRadius(); //半径（原点から最も遠い頂点までの距離）を計算する
}

//コンストラクタ（ファイルからロード）
Mesh::Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pD3DResourceManagerObj, double TextureSizeRatio) 
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pDirect3DObj->GetpD3DDevice();
	this->pAC = NULL;
	this->pAH = NULL;
	this->pFrame = NULL;
	this->pD3DResourceManagerObj = pD3DResourceManagerObj;

	HRESULT hr;
	LPD3DXBUFFER pMaterialBuf; //マテリアルバッファのポインタ
	DWORD MaterialNum; //マテリアルの数
	
	hr = D3DXLoadMeshFromX(pFilename, D3DXMESH_MANAGED, pD3DDevice, NULL, &pMaterialBuf, NULL, &MaterialNum, &pD3DXMesh); //ファイルからメッシュをロード

	if (FAILED(hr)) //メッシュ作成に失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("メッシュの作成に失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		pD3DXMesh = NULL;
	} else {
		Options = pD3DXMesh->GetOptions(); //作成時のオプションを保存

		//最適化
		int Faces = GetNumFaces(); //ポリゴン数を取得
		std::vector<DWORD> vectorAdjacency(Faces * 3);
		pD3DXMesh->GenerateAdjacency(0.00001f, &vectorAdjacency.at(0));
		pD3DXMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, &vectorAdjacency.at(0), 0, 0, 0);

		int Faces2 = GetNumFaces();
		tstring str;
		ho::tPrint(str, TEXT("%s をロードしました。(%d/%d Polygon)"), pFilename, Faces, Faces2);
		ER(str.c_str(), __WFILE__, __LINE__, NULL);

		D3DXMATERIAL *pMaterial = (D3DXMATERIAL *)pMaterialBuf->GetBufferPointer(); //マテリアル構造体のポインタを得る
		D3DXIMAGE_INFO d3dxii;

		tstring strDir = ho::GetDirectory(pFilename); //ファイル名からディレクトリ名（プログラム本体からXファイルまでの相対パス）のみを抽出

		//マテリアルの数だけマテリアル構造体からマテリアルとテクスチャを取得
		for (DWORD i = 0; i < MaterialNum; i++)
		{
			//マテリアルのアンビエントをディフューズの1/4の明るさにする
			pMaterial[i].MatD3D.Ambient = pMaterial[i].MatD3D.Diffuse;
			pMaterial[i].MatD3D.Ambient.r *= 0.25f;
			pMaterial[i].MatD3D.Ambient.g *= 0.25f;
			pMaterial[i].MatD3D.Ambient.b *= 0.25f;

			pMaterial[i].MatD3D.Power *= 4.0f; //スペキュラを拡散させる

			listMaterial.push_back(pMaterial[i].MatD3D); //マテリアルをリストに追加

			ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
			if (pMaterial[i].pTextureFilename && lstrlen((LPCWSTR)pMaterial[i].pTextureFilename) > 0) //テクスチャのファイル名が設定されている場合
			{
				tstring strFilename = ho::CharToWChar(pMaterial[i].pTextureFilename); //テクスチャファイル名をマルチバイトからワイド文字に変換
				strFilename = strDir + strFilename; //最初にディレクトリ名（プログラム本体からXファイルまでの相対パス）を追加

				D3DXGetImageInfoFromFile((TCHAR *)strFilename.c_str(), &d3dxii); //テクスチャの情報を取得
				hr = D3DXCreateTextureFromFileEx(pD3DDevice, (TCHAR *)strFilename.c_str(), UINT(d3dxii.Width * TextureSizeRatio), UINT(d3dxii.Height * TextureSizeRatio), 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &d3dxii, NULL, cpTexture.ToCreator()); //ファイルからテクスチャを読む
				if (FAILED(hr))
					ER(TEXT("テクスチャのロードに失敗しました。"), __WFILE__, __LINE__, true);
			}
			listcpTexture.push_back(cpTexture); //テクスチャをリストに追加
			if (pD3DResourceManagerObj && cpTexture.GetppInterface())
				pD3DResourceManagerObj->Regist(new ho::Texture(cpTexture)); //テクスチャマネージャに追加
		}
		
		RELEASE(pMaterialBuf);	//マテリアルバッファの解放
		this->pD3DDevice = pD3DDevice;

		CalcRadius(); //半径（原点から最も遠い頂点までの距離）を計算する
	}
}

//コンストラクタ（ファイルからアニメーション付きでロード）
Mesh::Mesh(ho::Direct3D *pDirect3DObj, TCHAR *pFilename, D3DResourceManager *pTextureManagerObj, BOOL Animation) 
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pDirect3DObj->GetpD3DDevice();
	this->pD3DXMesh = NULL;
	this->pD3DResourceManagerObj = pD3DResourceManagerObj;
	this->pAH = NULL;

	pAH = new ho::MyAllocateHierarchy; //フレーム階層構造クラスのオブジェクト作成

	HRESULT hr = D3DXLoadMeshHierarchyFromX(pFilename, D3DXMESH_MANAGED, pD3DDevice, pAH, NULL, (D3DXFRAME **)&pFrame, &pAC);
	if (FAILED(hr))
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("アニメーション付きメッシュのロードに失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		SDELETE(pAH);
	} else {
		*pAH->GetppRootFrame() = pFrame; //フレーム階層構造クラスのオブジェクトへルートフレームへのポインタを保存
		pAH->SetInitTransMatrix(pFrame); //初期状態の変換行列を保存（再帰関数）
		this->pD3DDevice = pD3DDevice;

		int Faces = GetNumFaces(); //ポリゴン数を取得
		tstring str;
		ho::tPrint(str, TEXT("%s をロードしました。(%d Polygon)"), pFilename, Faces);
		ER(str.c_str(), __WFILE__, __LINE__, true);

		CalcRadius(); //半径（原点から最も遠い頂点までの距離）を計算する
	}
}

//コンストラクタ（メッシュバッファから作成）
Mesh::Mesh(MeshBuffer *pMeshBufferObj, ho::Direct3D *pDirect3DObj, D3DResourceManager *pD3DResourceManagerObj) 
{
	this->pDirect3DObj = pDirect3DObj;
	this->pD3DDevice = pDirect3DObj->GetpD3DDevice();
	this->pAC = NULL;
	this->pAH = NULL;
	this->pFrame = NULL;
	this->pD3DResourceManagerObj = pD3DResourceManagerObj;

	HRESULT hr;
	hr = D3DXCreateMesh(pMeshBufferObj->GetIndicesNum() / 3, pMeshBufferObj->GetVerticesNum(), D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED, ho::D3DVERTEX::ve, pD3DDevice, &pD3DXMesh); //メッシュを作成

	if (FAILED(hr)) //メッシュ作成に失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("メッシュの作成に失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		pD3DXMesh = NULL;
	} else {
		Options = pD3DXMesh->GetOptions(); //作成時のオプションを保存

		CopyToBuffer(pMeshBufferObj); //作成したメッシュへメッシュバッファからデータをコピーする
		
		std::map<int, MeshBuffer::SUBSET> *pMapSubset = pMeshBufferObj->GetpmapSubset(); //サブセットのマップを取得
		//すべてのサブセットを走査
		for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
		{
			listMaterial.push_back(itr->second.Material); //マテリアルをコピー
			listcpTexture.push_back(itr->second.cpTexture); //テクスチャをコピー
			if (pD3DResourceManagerObj)
				pD3DResourceManagerObj->Regist(new ho::Texture(itr->second.cpTexture)); //テクスチャマネージャに追加
		}
		this->pD3DDevice = pD3DDevice;

		CalcRadius(); //半径（原点から最も遠い頂点までの距離）を計算する
	}
}

//デストラクタ
Mesh::~Mesh() 
{
	RELEASE(pD3DXMesh);
	SDELETE(pAH);
}

//作成したメッシュへメッシュバッファからデータをコピーする
BOOL Mesh::CopyToBuffer(MeshBuffer *pMeshBufferObj) 
{
	HRESULT hr;
	LPVOID pBuffer; //バッファへのポインタ
	std::map<int, MeshBuffer::SUBSET> *pMapSubset = pMeshBufferObj->GetpmapSubset(); //サブセットのマップを取得

	hr = pD3DXMesh->LockVertexBuffer(0, &pBuffer); //頂点バッファをロック
	if (FAILED(hr)) //ロックに失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("頂点バッファのロックに失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}
	//すべてのサブセットを走査
	for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
	{
		int CopySize = sizeof(ho::D3DVERTEX) * itr->second.vectorVt.size(); //コピーするサイズ
		memcpy(pBuffer, &itr->second.vectorVt.at(0), CopySize); //頂点情報を頂点バッファへコピー
		pBuffer = (BYTE *)pBuffer + CopySize; //コピーした分だけ頂点バッファを進める
	}
	hr = pD3DXMesh->UnlockVertexBuffer(); //頂点バッファのロック解除
	if (FAILED(hr)) //ロック解除に失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("頂点バッファのロック解除に失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}

	hr = pD3DXMesh->LockIndexBuffer(0, &pBuffer); //インデックスバッファをロック
	if (FAILED(hr)) //ロックに失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("インデックスバッファのロックに失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}
	//すべてのサブセットを走査
	int IndexOffset = 0; //インデックスのオフセット
	for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
	{
		for (DWORD i = 0; i < itr->second.vectorIndex.size(); i++)
		{
			*(WORD *)pBuffer = IndexOffset + itr->second.vectorIndex.at(i); //オフセットを加えてコピー
			pBuffer = (WORD *)pBuffer + 1; //コピーした分だけ頂点バッファを進める
		}
		IndexOffset += itr->second.vectorIndex.size(); //コピーしたインデックスの分だけオフセットを加算
	}
	hr = pD3DXMesh->UnlockIndexBuffer(); //インデックスバッファのロック解除
	if (FAILED(hr)) //ロック解除に失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("インデックスバッファのロック解除に失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}

	std::vector<D3DXATTRIBUTERANGE> vectorAR; //属性構造体配列
	D3DXATTRIBUTERANGE ar; //属性構造体
	ZeroMemory(&ar, sizeof(D3DXATTRIBUTERANGE));
	DWORD i = 0;
	for (std::map<int, MeshBuffer::SUBSET>::iterator itr = pMapSubset->begin(); itr != pMapSubset->end(); itr++)
	{
		ar.AttribId = i;
		ar.FaceStart += ar.FaceCount; //面の開始数
		ar.FaceCount = itr->second.vectorIndex.size() / 3; //面の数
		ar.VertexStart += ar.VertexCount; //頂点の開始位置
		ar.VertexCount = itr->second.vectorVt.size(); //頂点の数

		vectorAR.push_back(ar); //配列に追加
		i++;
	}
	hr = pD3DXMesh->SetAttributeTable(&vectorAR.at(0), vectorAR.size()); //メッシュへ属性を設定
	if (FAILED(hr)) //属性の設定に失敗した場合
	{
		tstring str;
		ho::Direct3D::GetErrorStr(str, hr);
		str = TEXT("属性の設定に失敗しました。") + str;
		ER(str.c_str(), __WFILE__, __LINE__, true);
		return FALSE;
	}

	return TRUE;
}

//描画
void Mesh::Draw() 
{
	if (!pD3DDevice) //無効の場合
		return;

	//デバイスの設定を保存
	DWORD OldFVF;
	pD3DDevice->GetFVF(&OldFVF);
	D3DMATERIAL9 OldMaterial;
	pD3DDevice->GetMaterial(&OldMaterial);
	IDirect3DBaseTexture9 *pOldTexture;
	pD3DDevice->GetTexture(0, &pOldTexture);
	DWORD TextureStageState[7];
	for (int i = 0; i < 7; i++)
		pD3DDevice->GetTextureStageState(0, D3DTEXTURESTAGESTATETYPE(i), &TextureStageState[i]);

	//色、透明成分、共にテクスチャと頂点のディフューズを混ぜる
	pD3DDevice->SetTextureStageState(0,	D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_COLOROP, D3DTOP_MODULATE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pD3DDevice->SetTextureStageState(0,	D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	if (pD3DXMesh)
		DrawNormal(); //標準メッシュの描画
	else
		DrawAnimation(); //アニメーション付きメッシュの描画

	//デバイスの設定を復元
	pD3DDevice->SetTexture(0, pOldTexture);
	pD3DDevice->SetMaterial(&OldMaterial);
	pD3DDevice->SetFVF(OldFVF);
	for (int i = 0; i < 7; i++)
		pD3DDevice->SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE(i), TextureStageState[i]);

	return;
}

//標準メッシュの描画
void Mesh::DrawNormal()
{
	DWORD Subset; //サブセット数
	pD3DXMesh->GetAttributeTable(NULL, &Subset); //サブセット数を取得
	//Subset = listMaterial.size();

	//頂点フォーマットを取得
	D3DVERTEXELEMENT9 ve[MAX_FVF_DECL_SIZE];
	pD3DXMesh->GetDeclaration(ve);
	/*
	//頂点宣言を作成
	IDirect3DVertexDeclaration9 *pVertexDec;
	pD3DDevice->CreateVertexDeclaration(ve, &pVertexDec);
	//頂点宣言を設定
	pD3DDevice->SetVertexDeclaration(pVertexDec);
	*/

	//全てのサブセットを走査
	std::list<D3DMATERIAL9>::iterator itrMaterial = listMaterial.begin();
	std::list<ComPtr<LPDIRECT3DTEXTURE9>>::iterator itrcpTexture = listcpTexture.begin();
	for (DWORD i = 0; i < Subset; i++)
	{
		pDirect3DObj->SetMaterial(&(D3DMATERIAL9)*itrMaterial);
		pD3DDevice->SetTexture(0, *itrcpTexture);

		//■頂点カラーなどの伝達方法は再考する
		/*
		if (ve[2].Usage == 10) //頂点カラーを含む場合
			pDirect3DObj->SetShaderTechnique(TRUE); //シェーダのテクニックの設定
		else
			pDirect3DObj->SetShaderTechnique(FALSE); //シェーダのテクニックの設定
			*/
		//pDirect3DObj->ShaderBeginDraw(); //シェーダの描画開始
		pD3DXMesh->DrawSubset(i);

		//pDirect3DObj->ShaderEndDraw(); //シェーダの描画終了

		itrMaterial++;
		itrcpTexture++;
	}

	return;
}

//アニメーション付きメッシュの描画
void Mesh::DrawAnimation()
{
	D3DXMATRIX mtrxWorld; //ワールド変換行列
	pD3DDevice->GetTransform(D3DTS_WORLD, &mtrxWorld);
	WorldTransMtrxStack WTMS(&mtrxWorld); //行列スタッククラスのオブジェクト作成
	std::list<ho::D3DXFRAME_WORLD *> *plistpDrawFrame; //描画フレームリストへのポインタ

	WTMS.UpdateFrame(pFrame); //フレームのワールド変換行列を更新
	plistpDrawFrame = WTMS.GetplistpDrawFrame(); //描画フレームリストへのポインタを取得

	//描画用フレームリストを走査して順番に描画
	int i, MaterialNum;
	for (std::list<ho::D3DXFRAME_WORLD *>::iterator itr = plistpDrawFrame->begin(); itr != plistpDrawFrame->end(); itr++)
	{
		pDirect3DObj->SetWorldMatrix(&(*itr)->WorldTransMatrix); //デバイスへワールド変換行列を設定
		MaterialNum = (*itr)->pMeshContainer->NumMaterials;
		for (i = 0; i < MaterialNum; i++)
		{
			pDirect3DObj->SetMaterial(&((*itr)->pMeshContainer->pMaterials)[i].MatD3D); //マテリアルを設定
			//pD3DDevice->SetTexture(0, ((D3DXMATERIAL_TEXTURE *)(*itr)->pMeshContainer->pMaterials)[i].pD3DTexture); //テクスチャを設定
			//pDirect3DObj->SetShaderTechnique(FALSE); //シェーダのテクニックの設定

			//pDirect3DObj->ShaderBeginDraw(); //シェーダの描画開始
			(*itr)->pMeshContainer->MeshData.pMesh->DrawSubset(i);
			//pDirect3DObj->ShaderEndDraw(); //シェーダの描画終了
		}
	}

	return;
}

//面の数の合計を取得
DWORD Mesh::GetNumFaces() 
{
	DWORD Faces = 0;

	if (pD3DXMesh) //標準メッシュの場合
		Faces = pD3DXMesh->GetNumFaces();
	else if (pFrame) //アニメーション付きメッシュの場合
		Faces = pFrame->GetNumFacesReflect();
	
	return Faces;
}

//フレーム名からフレームへのポインタを検索して取得する
ho::D3DXFRAME_WORLD *Mesh::FindpFrame(LPSTR str) 
{
	if (!pFrame) //アニメーションのxファイルではない場合
		return NULL;

	return pFrame->FindFrame(str);
}

//半径（原点から最も遠い頂点までの距離）を計算する
void Mesh::CalcRadius() 
{
	if (pD3DXMesh) //アニメーションなしのメッシュの場合
	{
		DWORD BytesPerVertex = pD3DXMesh->GetNumBytesPerVertex(); //頂点1つあたりのバイト数を取得
		DWORD Vertices = pD3DXMesh->GetNumVertices(); //頂点数を取得

		double MaxDistance = 0, Distance;
		LPVOID pBuffer;
		pD3DXMesh->LockVertexBuffer(NULL, &pBuffer); //頂点バッファをロック
		for (DWORD i = 0; i < Vertices; i++) //すべての頂点を走査
		{
			Distance = ((D3DXVECTOR3 *)((BYTE *)pBuffer + BytesPerVertex * i))->GetPower(); //原点からの距離を計算
			if (Distance > MaxDistance)
				MaxDistance = Distance;
		}
		pD3DXMesh->UnlockVertexBuffer(); //頂点バッファのロックを解除
		Radius = (float)MaxDistance;
	} else { //アニメーション付きのメッシュの場合
		D3DXMATRIX mtrx;
		D3DXMatrixIdentity(&mtrx); //基準行列を作成
		WorldTransMtrxStack WTMS(&mtrx); //行列スタッククラスのオブジェクト作成
		std::list<ho::D3DXFRAME_WORLD *> *plistpDrawFrame; //描画フレームリストへのポインタ

		WTMS.UpdateFrame(pFrame); //フレームのワールド変換行列を更新
		plistpDrawFrame = WTMS.GetplistpDrawFrame(); //描画フレームリストへのポインタを取得
		
		double MaxDistance = 0, Distance;

		for (std::list<ho::D3DXFRAME_WORLD *>::iterator itr = plistpDrawFrame->begin(); itr != plistpDrawFrame->end(); itr++) //フレームリストを走査
		{
			LPD3DXMESH pD3DXMesh = (*itr)->pMeshContainer->MeshData.pMesh;
			DWORD BytesPerVertex = pD3DXMesh->GetNumBytesPerVertex(); //頂点1つあたりのバイト数を取得
			DWORD Vertices = pD3DXMesh->GetNumVertices(); //頂点数を取得

			D3DXVECTOR3 vecVertex;
			LPVOID pBuffer;
			pD3DXMesh->LockVertexBuffer(NULL, &pBuffer); //頂点バッファをロック
			for (DWORD i = 0; i < Vertices; i++) //すべての頂点を走査
			{
				vecVertex = *((D3DXVECTOR3 *)((BYTE *)pBuffer + BytesPerVertex * i)); //頂点情報から座標を取得
				D3DXVec3TransformCoord(&vecVertex, &vecVertex, &(*itr)->WorldTransMatrix); //座標を現在のフレームの行列で変換
				Distance = vecVertex.GetPower(); //原点からの距離を計算
				if (Distance > MaxDistance)
					MaxDistance = Distance;
			}
			pD3DXMesh->UnlockVertexBuffer(); //頂点バッファのロックを解除

		}
		
		Radius = (float)MaxDistance;
	}

	return;
}










//サブセットを追加
BOOL MeshBuffer::CreateSubset(int Index, SUBSET *pSubset) 
{
	mapSubset.insert(std::pair<int, SUBSET>(Index, *pSubset));

	return TRUE;
}

//サブセットに頂点情報と頂点インデックスを追加
BOOL MeshBuffer::AddVertex(int SubsetIndex, const D3DVERTEX *pVt, int Vertices, const WORD *pIndex, int Indices)
{
	if (!mapSubset.count(SubsetIndex)) //サブセットが存在しない場合
		return FALSE;

	int IndexOffset = mapSubset.at(SubsetIndex).vectorVt.size(); //頂点インデックス番号のオフセット

	//頂点情報を追加
	for (int i = 0; i < Vertices; i++)
		mapSubset.at(SubsetIndex).vectorVt.push_back(pVt[i]);

	//頂点インデックスを追加
	for (int i = 0; i < Indices; i++)
		mapSubset.at(SubsetIndex).vectorIndex.push_back(pIndex[i] + IndexOffset);

	return TRUE;
}

//頂点数の合計を得る
DWORD MeshBuffer::GetVerticesNum()
{
	DWORD Size = 0;

	//全てのサブセットを走査
	for (std::map<int, SUBSET>::iterator itr = mapSubset.begin(); itr != mapSubset.end(); itr++)
		Size += itr->second.vectorVt.size();

	return Size;
}

//頂点インデックス数の合計を得る
DWORD MeshBuffer::GetIndicesNum() 
{
	DWORD Size = 0;

	//全てのサブセットを走査
	for (std::map<int, SUBSET>::iterator itr = mapSubset.begin(); itr != mapSubset.end(); itr++)
		Size += itr->second.vectorIndex.size();

	return Size;
}







	//ID3DXMeshの頂点情報から端の位置を取得
	D3DXVECTOR3 GetD3DXMeshCornerPos(LPD3DXMESH pMesh, int Axis, bool Max, bool World)
	{
		int NumVertices = pMesh->GetNumVertices(); //メッシュの頂点数を取得
		if (NumVertices <= 0) //頂点が存在しない場合
			return D3DXVECTOR3(0, 0, 0);

		std::vector<D3DXVECTOR3> vectorVec;
		LPVOID pBuffer;
		HRESULT hr = pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pBuffer); //頂点バッファをロック
		if (D3D_OK == hr) //成功した場合
		{
			DWORD BytesPerVertex = pMesh->GetNumBytesPerVertex(); //頂点情報1つあたりのバイト数を得る
			D3DXVECTOR3 vec;

			//全ての頂点を走査
			for (int i = 0; i < NumVertices; i++)
			{
				vec.x = *(float *)pBuffer;
				vec.y = *((float *)pBuffer + 1);
				vec.z = *((float *)pBuffer + 2);
				//if (World) //モデルの原点から見たらワールド座標で比較する場合
					//D3DXVec3TransformCoord(&vec, &vec, &(*itr)->WorldTransMatrix); //ワールド行列変換で座標を変換
				vectorVec.push_back(vec);
				pBuffer = (BYTE *)pBuffer + BytesPerVertex;
			}

			pMesh->UnlockVertexBuffer(); //ロック解除
		} else {
			tstring str;
			Direct3D::GetErrorStr(str, hr);
			ER(str.c_str(), __WFILE__, __LINE__, true);
			return D3DXVECTOR3(0, 0, 0);
		}

		if (Max) //最大値が欲しい場合
		{
			if (Axis == 0) //X軸の場合
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortX2); 
			else if (Axis == 1) //Y軸の場合
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortY2); 
			else if (Axis == 2) //Z軸の場合
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortZ2);
		} else { //最小値が欲しい場合
			if (Axis == 0) //X軸の場合
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortX1); 
			else if (Axis == 1) //Y軸の場合
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortY1); 
			else if (Axis == 2) //Z軸の場合
				std::sort(vectorVec.begin(), vectorVec.end(), D3DXVector3SortZ1);
		}

		return vectorVec.at(0);
	}

	bool D3DXVector3SortX1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.x < vec2.x; }
	bool D3DXVector3SortX2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.x > vec2.x; }
	bool D3DXVector3SortY1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.y < vec2.y; }
	bool D3DXVector3SortY2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.y > vec2.y; }
	bool D3DXVector3SortZ1(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.z < vec2.z; }
	bool D3DXVector3SortZ2(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2) { return vec1.z > vec2.z; }
}
