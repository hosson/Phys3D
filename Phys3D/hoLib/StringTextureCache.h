//文字列テクスチャのキャッシュクラス
#pragma once
#include <list>
#include <d3d9.h>
#include "PT.h"
#include "ComPtr.hpp"
#include "tstring.h"

namespace ho
{
	class DrawString;
	class StringTextureCache;

	class StringTextureCacheManager
	{
	public:
		StringTextureCacheManager(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj); //コンストラクタ
		~StringTextureCacheManager(); //デストラクタ

		StringTextureCache *GetpStringTextureCache(TCHAR *pStr, int Height, int LimitiWidth); //任意の文字列のテクスチャを取得
		void Optimize(); //最適化
		void Clear(); //全てのキャッシュを削除

		//アクセッサ
		DrawString *GetpDrawStringObj() { return pDrawStringObj; }
		DWORD GetTime() { return Time; }
	private:
		LPDIRECT3DDEVICE9 pD3DDevice;
		DrawString *pDrawStringObj;
		std::list<StringTextureCache *> listpStringTextureCacheObj;
		DWORD Time; //Optimize() が呼ばれた回数
	};

	class StringTextureCache
	{
	public:
		StringTextureCache(TCHAR *pStr, int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj); //コンストラクタ
		~StringTextureCache(); //デストラクタ

		BOOL Compare(TCHAR *pStr); //引数の文字列とテクスチャの文字列が同じかどうかを比較する
		void Use(DWORD Time) { this->LastUseTime = Time; } //使用する（最後に使用された時間が更新される）

		//アクセッサ
		ho::ComPtr<LPDIRECT3DTEXTURE9> GetcpTexture() { return cpTexture; }
		ho::PT<int> GetptTextureSize() { return ptTextureSize; }
		int GetCharNum() { return CharNum; }
		DWORD GetLastUseTime() { return LastUseTime; }
	private:
		StringTextureCacheManager *pStringTextureCacheManagerObj;
		ho::ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
		tstring str;
		ho::PT<int> ptTextureSize; //文字列テクスチャのサイズ
		int CharNum; //文字数
		DWORD LastUseTime; //最後に使用された時間

		void CreateCacheTexture(int Height, int LimitWidth, LPDIRECT3DDEVICE9 pD3DDevice, StringTextureCacheManager *pStringTextureCacheManagerObj, LOGFONT *pLF); //キャッシュとなるテクスチャを作成する
		void CalcPtStringSize(ho::PT<int> *pptSize, int *pCharNum, TCHAR *pStr, int Height, int LimitWidth, LOGFONT *pLF); //文字列のサイズを計算する
	};
}

/*DrawString クラスのコンストラクタで StringTextureCacheManager を1つだけ作成する。
文字列描画時に、StringTextureCacheManager::GetpStringTextureCache() を呼び出し、
キャッシュされた文字列テクスチャのオブジェクトを取得する。この時、まだキャッシュされていない
場合は自動でキャッシュが作成される。
そして、StringTextureCache::GetptTextureSize() で文字列テクスチャのサイズを取得して頂点情報を設定したら、
StiringTextureCache::GetcpTexture() でテクスチャを得てデバイスに設定して描画する。

★キャッシュの削除について
キャッシュされたテクスチャを削除するためには、StringTextureCacheManager::Optimize() を
定期的に実行して使われていないと思われるテクスチャを削除する必要がある。
他にも改造次第で、テクスチャの容量を計算して一定以上になったら未使用なものを削除などの方法もある。
*/




