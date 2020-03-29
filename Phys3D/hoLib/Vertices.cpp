#include "Vertices.h"
#include "Direct3D.h"
#include "Math.h"

namespace ho
{
	//コンストラクタ
	Vertices::Vertices()
	{
		pVertexBuffer = NULL;
		pIndexBuffer = NULL;

		Clear(); //追加中の頂点とインデックスを削除
	}

	//デストラクタ
	Vertices::~Vertices()
	{
		Release(); //頂点バッファとインデックスバッファを解放
	}

	//頂点とインデックスを追加
	void Vertices::Add(D3DVERTEX *pD3DVERTEX, int Vertices, WORD *pIndex, int Indexes, UINT Primitives)
	{
		int OldVertices = vectorD3DVERTEX.size(); //追加前の頂点数を記憶

		//頂点の追加
		if (pD3DVERTEX && Vertices)
			for (int i = 0; i < Vertices; i++)
				vectorD3DVERTEX.push_back(pD3DVERTEX[i]);

		//インデックスの追加
		if (pIndex && Indexes)
			for (int i = 0; i < Indexes; i++)
				vectorIndex.push_back(OldVertices + pIndex[i]);

		this->Primitives += Primitives; //ポリゴン数を追加

		return;
	}

	//頂点バッファをインデックスバッファを生成
	void Vertices::CreateBuffer(LPDIRECT3DDEVICE9 pD3DDevice, DWORD Usage, D3DPOOL Pool)
	{
		Release(); //頂点バッファとインデックスバッファを解放

		//頂点バッファを作成
		if (vectorD3DVERTEX.size())
		{
			pD3DDevice->CreateVertexBuffer(vectorD3DVERTEX.size() * sizeof(D3DVERTEX), Usage, D3DVERTEX::FVF, Pool, &pVertexBuffer, NULL);
			if (pVertexBuffer) //頂点バッファの作成に成功した場合
			{
				LPVOID pBuffer; //バッファの内容を示すポインタ
				pVertexBuffer->Lock(0, 0, &pBuffer, 0); //バッファをロック
				memcpy(pBuffer, &vectorD3DVERTEX.at(0), vectorD3DVERTEX.size() * sizeof(D3DVERTEX)); //頂点配列の内容をバッファへコピー
				pVertexBuffer->Unlock(); //バッファのロック解除
			}
		}

		//インデックスバッファを作成
		if (vectorIndex.size())
		{
			pD3DDevice->CreateIndexBuffer(vectorIndex.size() * sizeof(WORD), Usage, D3DFMT_INDEX16, Pool, &pIndexBuffer, NULL);
			if (pIndexBuffer) //インデックスバッファの作成に成功した場合
			{
				LPVOID pBuffer; //バッファの内容を示すポインタ
				pIndexBuffer->Lock(0, 0, &pBuffer, 0); //バッファをロック
				memcpy(pBuffer, &vectorIndex.at(0), vectorIndex.size() * sizeof(WORD)); //インデックス配列の内容をバッファへコピー
				pIndexBuffer->Unlock(); //バッファのロック解除
			}
		}

		return;
	}

	//追加中の頂点とインデックスを削除
	void Vertices::Clear()
	{
		vectorD3DVERTEX.clear();
		vectorD3DVERTEX.resize(0);

		vectorIndex.clear();
		vectorIndex.resize(0);

		Primitives = 0;

		return;
	}

	//頂点バッファとインデックスバッファを解放
	void Vertices::Release()
	{
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);

		return;
	}

	//描画
	void Vertices::Draw(LPDIRECT3DDEVICE9 pD3DDevice, D3DXMATRIX *pmtrx) 
	{
		if (!pD3DDevice || !pVertexBuffer)
			return;

		D3DXMATRIX mtrxOld; //ワールド変換行列保存用
		pD3DDevice->GetTransform(D3DTS_WORLD, &mtrxOld); //ワールド行列を保存
		pD3DDevice->SetTransform(D3DTS_WORLD, pmtrx); //デバイスにマトリックスを設定

		pD3DDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(D3DVERTEX)); //頂点バッファをデバイスのストリームにセット
		if (pIndexBuffer) //インデックスバッファが存在する場合
		{
			pD3DDevice->SetIndices(pIndexBuffer); //インデックスバッファをセット
			pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vectorD3DVERTEX.size(), 0, Primitives); //頂点インデックスありで描画
		} else { //存在しない場合
			pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, Primitives); //頂点インデックスなしで描画
		}

		pD3DDevice->SetTransform(D3DTS_WORLD, &mtrxOld); //デバイスのワールド変換行列を復元

		return;
	}

	//作成済み頂点バッファから最大半径を計算して取得する
	float Vertices::GetMaxRadius() 
	{
		/*主に視錐台カリング時などに使える値が返るが、その都度計算が入るので、
		描画毎にこの関数を呼び出すことは避けることが望ましい。*/

		float MaxRadius = 0; //最大半径

		if (pVertexBuffer) //頂点バッファが存在する場合
		{
			D3DVERTEXBUFFER_DESC vbd;
			pVertexBuffer->GetDesc(&vbd); //バッファ情報を取得
			int Num = vbd.Size / vbd.Size; //頂点数を計算

			LPVOID pBuffer; //バッファの内容を示すポインタ
			pVertexBuffer->Lock(0, 0, &pBuffer, 0); //バッファをロック

			//全ての頂点の座標を走査して原点から最も遠い座標までの距離を求めて半径とする
			D3DXVECTOR3 vec;
			float Radius;
			MaxRadius = FLT_MIN;
			for (int i = 0; i < Num; i++)
			{
				//頂点情報の中から座標を抜き出す
				vec.x = *(float *)((BYTE *)pBuffer + vbd.Size * i);
				vec.y = *(((float *)((BYTE *)pBuffer + vbd.Size * i)) + 1);
				vec.z = *(((float *)((BYTE *)pBuffer + vbd.Size * i)) + 1);
				Radius = vec.GetPower(); //原点から座標までの距離を計算して半径とする
				if (Radius > MaxRadius)
					MaxRadius = Radius;
			}

			pVertexBuffer->Unlock(); //バッファのロック解除
		}

		return MaxRadius;
	}
}
