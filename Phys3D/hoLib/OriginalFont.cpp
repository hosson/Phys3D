#include "OrigianlFont.h"
#include "Direct3D.h"
#include "LibError.h"

namespace ho
{
	//コンストラクタ
	OriginalFontManager::OriginalFontManager()
	{
	}

	//デストラクタ
	OriginalFontManager::~OriginalFontManager()
	{
		for (DWORD i = 0; i < vectorpOriginalFontObj.size(); i++)
			SDELETE(vectorpOriginalFontObj.at(i));
	}

	//独自フォントオブジェクトを追加
	void OriginalFontManager::Add(OriginalFont *pOriginalFontObj, DWORD Index)
	{
		if (vectorpOriginalFontObj.size() <= Index)
			vectorpOriginalFontObj.resize(Index + 1);

		vectorpOriginalFontObj.at(Index) = pOriginalFontObj;

		return;
	}

	//独自フォントオブジェクトを取得
	OriginalFont *OriginalFontManager::GetpOriginalFontObj(DWORD Index)
	{
		if (vectorpOriginalFontObj.size() <= Index) //範囲外の場合
		{
			ER(TEXT("範囲外の独自フォントが選択されました。"), __WFILE__, __LINE__, false);
			return NULL;
		}

		return vectorpOriginalFontObj.at(Index);
	}





	//コンストラクタ
	OriginalFont::OriginalFont(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR *pArchiveName, const TCHAR *pTextureFilename, const TCHAR *pFontInfoFilename, bool Debug)
	{
		//フォントのテクスチャを読む
		ho::Archive ArchiveObj(pArchiveName, Debug); //アーカイブを開く

		void *p = ArchiveObj.GetFilePointer(pTextureFilename); //ファイルポインタを取得
		DWORD FileSize = ArchiveObj.GetFileSize(); //ファイルサイズ取得

		D3DXIMAGE_INFO d3dxii;
		try
		{
			HRESULT hr;

			if (FAILED(hr = D3DXGetImageInfoFromFileInMemory(p, FileSize, &d3dxii)))
			{
				ho::tstring str;
				ho::Direct3D::GetErrorStr(str, hr);
				str = TEXT("テクスチャ情報の取得に失敗しました。") + str;
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}

			if (FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(pD3DDevice,	p, FileSize, d3dxii.Width, d3dxii.Height, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &d3dxii, NULL, &pTexture))) //メモリからテクスチャを作成
			{
				ho::tstring str;
				ho::Direct3D::GetErrorStr(str, hr);
				str = TEXT("テクスチャを開くのに失敗しました。") + str;
				throw ho::Exception(str, __WFILE__, __LINE__, false);
			}
		}
		catch (ho::Exception &e)
		{
			ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		}
	
		ArchiveObj.EndFilePointer(); //ファイルポインタを終了


		//フォント情報ファイルから文字オブジェクトを作成
		ZeroMemory(pFontCharObj, sizeof(FontChar *) * 65536); //文字クラスへのポインタマップを初期化

		
		p = ArchiveObj.GetFilePointer(pFontInfoFilename); //フォント情報ファイルのポインタを取得

		ho::CSVReader CSVReaderObj(p); //CSVリーダオブジェクトを作成

		ho::tstring str;
		int x, y, w, h; //フォント情報ファイルに格納されていたテクスチャ上のピクセル座標
		float tu1, tu2, tv1, tv2;
		while (true)
		{
			CSVReaderObj.Read(&str); //文字を読む
			if (str.size() == 0) //文字が空白だった場合
				break; //ファイル終端に到達したのでループを抜ける

			CSVReaderObj.Read(&x);
			CSVReaderObj.Read(&y);
			CSVReaderObj.Read(&w);
			CSVReaderObj.Read(&h);

			//ピクセル座標からテクセル座標を計算
			tu1 = float((double)x / (double)d3dxii.Width);
			tv1 = float((double)y / (double)d3dxii.Height);
			tu2 = float(double(x + w) / (double)d3dxii.Width);
			tv2 = float(double(y + h) / (double)d3dxii.Height);

			pFontCharObj[str.at(0)] = new FontChar(w, h, tu1, tu2, tv1, tv2); //文字オブジェクトを作成してポインタマップに格納
		}

		ArchiveObj.EndFilePointer(); //アーカイブのファイルポインタ使用を終了する
		
	}

	//デストラクタ
	OriginalFont::~OriginalFont()
	{
		for (int i = 0; i < 65536; i++)
			SDELETE(pFontCharObj[i]);

		RELEASE(pTexture);
	}

	//任意の文字列描画に必要な頂点情報の配列を生成する
	void OriginalFont::CreateDrawVt(TCHAR *pStr, float z, std::vector<D3DXVECTOR3> &vectorvecVt, std::vector<D3DXVECTOR2> &vectorvecUV, std::vector<WORD> &vectorIndex, int Height)
	{
		vectorvecVt.clear();
		vectorvecUV.clear();
		vectorIndex.clear();

		float x = 0, y = 0; //頂点位置
		D3DXVECTOR2 *pUV;
		WORD Index = 0;
		float w, h;
		double Ratio;
		while (*pStr) //文字列が NULL になるまで走査
		{
			//頂点座標を追加
			w = (float)pFontCharObj[*pStr]->GetptWH().x;
			h = (float)pFontCharObj[*pStr]->GetptWH().y;
			if (Height) //高さが指定されている場合
				Ratio = Height / h; //サイズの比率を計算
			else
				Ratio = 1.0;
			w = float(w * Ratio);
			h = float(h * Ratio);
			vectorvecVt.push_back(D3DXVECTOR3(x, y, z));
			vectorvecVt.push_back(D3DXVECTOR3(x + w, y, z));
			vectorvecVt.push_back(D3DXVECTOR3(x, y + h, z));
			vectorvecVt.push_back(D3DXVECTOR3(x + w, y + h, z));
			x += w; //頂点座標を次の文字の位置へ送る

			//UV座標を追加
			pUV = pFontCharObj[*pStr]->Getpuv(); //文字のテクスチャ内の4点の座標配列へのポインタを取得
			for (int i = 0; i < 4; i++)
				vectorvecUV.push_back(pUV[i]); //UV 座標を追加

			//頂点インデックスを追加
			vectorIndex.push_back(Index);
			vectorIndex.push_back(Index + 1);
			vectorIndex.push_back(Index + 2);
			vectorIndex.push_back(Index + 2);
			vectorIndex.push_back(Index + 1);
			vectorIndex.push_back(Index + 3);
			Index += 4;

			pStr++; //次の文字へ移動
		}

		return;
	}

	//文字列と高さから描画時の幅を計算する
	float OriginalFont::GetWidth(TCHAR *pStr, int Height) 
	{
		float x = 0; //頂点位置
		float w, h;
		double Ratio;

		while (*pStr) //文字列が NULL になるまで走査
		{
			//頂点座標を追加
			w = (float)pFontCharObj[*pStr]->GetptWH().x;
			h = (float)pFontCharObj[*pStr]->GetptWH().y;
			if (Height) //高さが指定されている場合
				Ratio = Height / h; //サイズの比率を計算
			else
				Ratio = 1.0;
			w = float(w * Ratio);
			h = float(h * Ratio);
			x += w; //頂点座標を次の文字の位置へ送る



			pStr++; //次の文字へ移動
		}

		return x;
	}




	//コンストラクタ
	FontChar::FontChar(int Width, int Height, float tu1, float tu2, float tv1, float tv2)
	{
		ptWH = ho::PT<int>(Width, Height);

		uv[0] = D3DXVECTOR2(tu1, tv1);
		uv[1] = D3DXVECTOR2(tu2, tv1);
		uv[2] = D3DXVECTOR2(tu1, tv2);
		uv[3] = D3DXVECTOR2(tu2, tv2);
	}

	//デストラクタ
	FontChar::~FontChar()
	{
	}
}
