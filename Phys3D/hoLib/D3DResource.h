//Direct3D 上で扱うリソースのクラス
#pragma once
#include "Common.h"
#include "hoLib.h"
#include <list>
#include "Debug.h"
#include "Direct3D.h"
#include "ComPtr.hpp"
#include "LibError.h"

namespace ho
{
	class D3DResource;

	//リソースマネージャ
	class D3DResourceManager
	{
	public:
		D3DResourceManager(); //コンストラクタ
		~D3DResourceManager(); //デストラクタ
	
		void Regist(D3DResource *pD3DResourceObj); //リソースを登録する
		void Erase(void **ppInterface); //リソースを削除する
		void Optimize(); //最適化（未使用リソースを削除）
		void OnLostDevice(); //ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースを解放
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice); //リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成する
	private:
		std::list<D3DResource *> listpD3DResourceObj;
	};


	//各種リソースの基底となるクラス
	class D3DResource
	{
	public:
		D3DResource() {} //コンストラクタ
		virtual ~D3DResource() {} //デストラクタ

		virtual ULONG GetRefCount() { return 0; } //COMポインタの参照カウンタの値を得る
		virtual void OnLostDevice() {} //ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースを解放
		virtual void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice) {} //リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成する
		virtual void **Getpp() { return NULL; }
	};


	//テクスチャクラス
	class Texture : public D3DResource
	{
	public:
		Texture(ComPtr<LPDIRECT3DTEXTURE9> &cpTexture) //コンストラクタ
		{
			this->cpTexture = cpTexture;
			if (cpTexture.GetpInterface())
				(cpTexture)->GetLevelDesc(0, &Desc); //サーフェイス情報を取得
		}
		~Texture() //デストラクタ
		{
			if (this->cpTexture.GetRefCount() > 1) //参照数が2以上（このクラス以外にもCOMポインタが存在している場合）
				ER(TEXT("まだ解放されていないCOMポインタが存在します。"),  __WFILE__, __LINE__, true);
		} 

		ULONG GetRefCount() { return this->cpTexture.GetRefCount(); } //COMポインタの参照カウンタの値を得る
		
		//ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースなどを解放
		void OnLostDevice()
		{
			if (Desc.Pool == D3DPOOL_DEFAULT) //作成時に D3DPOOL_DEFAULT のフラグを伴っていた場合
			{
				ComPtr<LPDIRECT3DTEXTURE9> cpEmptyTexture; //空のテクスチャを作成
				this->cpTexture.Swap(cpEmptyTexture); //入れ替える
			}

			return;
		}

		//リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			if (Desc.Pool == D3DPOOL_DEFAULT)
			{
				ComPtr<LPDIRECT3DTEXTURE9> cpNewTexture;
				HRESULT hr = pD3DDevice->CreateTexture(Desc.Width, Desc.Height, 1, Desc.Usage, Desc.Format, Desc.Pool, cpNewTexture.ToCreator(), NULL);
				if (FAILED(hr))
				{
					tstring str;
					ho::Direct3D::GetErrorStr(str, hr);
					ER(str.c_str(),  __WFILE__, __LINE__, true);
				} else {
					this->cpTexture.Swap(cpNewTexture);
				}
			}

			return;
		}

		void **Getpp()
		{
			return (void **)cpTexture.GetppInterface();
		}

	private:
		ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
		D3DSURFACE_DESC Desc; //サーフェイス情報
	};


	//サーフェイスクラス
	class Surface : public D3DResource
	{
	public:
		Surface(ComPtr<LPDIRECT3DSURFACE9> &cpSurface) //コンストラクタ
		{
			this->cpSurface = cpSurface;
			if (cpSurface.GetpInterface())
				cpSurface->GetDesc(&Desc);
		}
		~Surface() //デストラクタ
		{
			if (this->cpSurface.GetRefCount() > 1) //参照数が2以上（このクラス以外にもCOMポインタが存在している場合）
				ER(TEXT("まだ解放されていないCOMポインタが存在します。"), __WFILE__, __LINE__, true);
		} 

		ULONG GetRefCount() { return this->cpSurface.GetRefCount(); } //COMポインタの参照カウンタの値を得る

		//ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースを解放
		void OnLostDevice()
		{
			if (Desc.Pool == D3DPOOL_DEFAULT) //作成時に D3DPOOL_DEFAULT のフラグを伴っていた場合
			{
				ComPtr<LPDIRECT3DSURFACE9> cpEmptySurface; //空のサーフェイスを作成
				this->cpSurface.Swap(cpEmptySurface); //入れ替える
			}

			return;
		} 

		//リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			if (Desc.Pool == D3DPOOL_DEFAULT) //作成時に D3DPOOL_DEFAULT のフラグを伴っていた場合
			{
				if (Desc.Usage == D3DUSAGE_DEPTHSTENCIL) //深度ステンシルバッファの場合
				{
					pD3DDevice->CreateDepthStencilSurface(Desc.Width, Desc.Height, Desc.Format, Desc.MultiSampleType, Desc.MultiSampleQuality, TRUE,  this->cpSurface.ToCreator(), NULL);
				} else if (Desc.Usage == D3DUSAGE_RENDERTARGET) //レンダーターゲットの場合
				{
					pD3DDevice->CreateRenderTarget(Desc.Width, Desc.Height, Desc.Format, Desc.MultiSampleType, Desc.MultiSampleQuality, Desc.Pool, this->cpSurface.ToCreator(), NULL);
				} else { //それ以外（通常のサーフェイス）の場合
					pD3DDevice->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, Desc.Pool, this->cpSurface.ToCreator(), NULL);
				}
			}

			return;
		}

	private:
		ComPtr<LPDIRECT3DSURFACE9> cpSurface;
		D3DSURFACE_DESC Desc; //サーフェイス情報
	};


	//スプライトクラス
	class Sprite : public D3DResource
	{
	public:
		Sprite(ComPtr<LPD3DXSPRITE> &cpSprite) { this->cpSprite = cpSprite;	} //コンストラクタ
		~Sprite() //デストラクタ
		{
			if (this->cpSprite.GetRefCount() > 1) //参照数が2以上（このクラス以外にもCOMポインタが存在している場合）
				ER(TEXT("まだ解放されていないCOMポインタが存在します。"), __WFILE__, __LINE__, true);
		} 

	
		//ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースなどを解放
		void OnLostDevice()
		{
			cpSprite->OnLostDevice();

			return;
		}

		//リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			cpSprite->OnResetDevice();

			return;
		}

		ULONG GetRefCount() { return this->cpSprite.GetRefCount(); } //COMポインタの参照カウンタの値を得る
		void **Getpp() { return (void **)cpSprite.GetppInterface(); }
	private:
		ComPtr<LPD3DXSPRITE> cpSprite;
	};

	//フォントクラス
	class Font : public D3DResource
	{
	public:
		Font(ComPtr<LPD3DXFONT> &cpFont) { this->cpFont = cpFont; } //コンストラクタ
		~Font() //デストラクタ
		{
			if (this->cpFont.GetRefCount() > 1) //参照数が2以上（このクラス以外にもCOMポインタが存在している場合）
				ER(TEXT("まだ解放されていないCOMポインタが存在します。"), __WFILE__, __LINE__, true);
		} 
	
		//ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースなどを解放
		void OnLostDevice()
		{
			cpFont->OnLostDevice();

			return;
		}

		//リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			cpFont->OnResetDevice();

			return;
		}

		ULONG GetRefCount() { return this->cpFont.GetRefCount(); } //COMポインタの参照カウンタの値を得る
		void **Getpp() { return (void **)cpFont.GetppInterface(); }
	private:
		ComPtr<LPD3DXFONT> cpFont;
	};

	//エフェクトクラス
	class Effect : public D3DResource
	{
	public:
		Effect(ComPtr<LPD3DXEFFECT> &cpEffect) { this->cpEffect = cpEffect; } //コンストラクタ
		~Effect() //デストラクタ
		{
			if (this->cpEffect.GetRefCount() > 1) //参照数が2以上（このクラス以外にもCOMポインタが存在している場合）
				ER(TEXT("まだ解放されていないCOMポインタが存在します。"), __WFILE__, __LINE__, true);
		} 
	
		//ロストデバイス時に D3DPOOL_DEFAULT フラグで作成されたリソースなどを解放
		void OnLostDevice()
		{
			cpEffect->OnLostDevice();

			return;
		}

		//リセットされたデバイス上で、 D3DPOOL_DEFAULT フラグで作成されたリソースを再作成
		void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice)
		{
			cpEffect->OnResetDevice();

			return;
		}

		ULONG GetRefCount() { return this->cpEffect.GetRefCount(); } //COMポインタの参照カウンタの値を得る
		void **Getpp() { return (void **)cpEffect.GetppInterface(); }
	private:
		ComPtr<LPD3DXEFFECT> cpEffect;
	};

}

/*使い方

■初期化
Direct3D クラスなどの Direct3D を管理するクラスなどに D3DResourceManager クラスの
オブジェクトを初期化時に1つだけ作成する。
プログラム終了のタイミングでこの D3DResourceManager クラスは削除する。

■リソース作成
テクスチャなどを作成するたびに Texture などの派生クラスを生成して
D3DResourceManager::Regist() メソッドで登録する。

■リソース削除
削除は D3DResourceManager::Optimize() メソッドを呼び出すことによって行われるので、
これを毎フレーム毎などで呼び出す。
このとき、 Texture などの作成時のCOMポインタが他の場所に残っていると削除されない。

*/
