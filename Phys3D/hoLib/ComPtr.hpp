//COMポインタクラス
#pragma once
#include "Common.h"
#include <Windows.h>

namespace ho
{
	template <class T> class ComPtr
	{
	public:
		//コンストラクタ
		ComPtr(T *ppSrcInterface = NULL) //通常
		{
			this->ppInterface = ppSrcInterface;
			this->pRefCount = NULL;
			AddRef(); //参照カウンタを増加
		}
		ComPtr(const ComPtr<T> &src) //コピーコンストラクタ
		{
			this->ppInterface = src.ppInterface; //インターフェースへのダブルポインタをコピー
			this->pRefCount = src.pRefCount; //参照カウンタへのポインタをコピー

			AddRef(); //参照カウンタを増加
		}

		//デストラクタ
		~ComPtr()
		{
			SubRef(); //参照カウンタを減少
		}

		//参照カウンタを増加
		void AddRef()
		{
			if (pRefCount) //参照カウンタが存在する場合
				(*pRefCount)++;
			else
				pRefCount = new unsigned long(1); //参照カウンタを作成

			if (ppInterface)
				if (*ppInterface)
					(*ppInterface)->AddRef(); //インターフェース内の参照カウンタを増加

			return;
		}
		//参照カウンタを減少
		void SubRef()
		{
			if (pRefCount) //参照カウンタが存在する場合
			{
				(*pRefCount)--;
				if (ppInterface)
					if (*ppInterface)
						(*ppInterface)->Release(); //インターフェースを解放

				if (*pRefCount == 0) //参照カウンタが0になった場合
				{
					SDELETE(pRefCount); //参照カウンタを削除
					SDELETE(this->ppInterface); //インターフェースへのダブルポインタを削除
				}
			}


			return;
		}

		//Create系関数用
		T *ToCreator()
		{
			SubRef(); //参照カウンタを減少
			this->ppInterface = NULL; //カウンタが減ったのでインターフェースへのダブルポインタも消去

			this->ppInterface = new T; //インターフェースへのポインタを作成
			*this->ppInterface = NULL;
			this->pRefCount = NULL; //参照カウンタを初期化
			AddRef(); //参照カウンタを増加

			return this->ppInterface; //インターフェースへのダブルポインタを渡す
		}

		//ポインタの中身を入れ替える
		void Swap(ComPtr<T> &src)
		{
			//双方のポインタの参照カウンタの値を取得
			unsigned long SrcRef = src.GetRefCount();
			unsigned long MyRef = this->GetRefCount();

			//双方のポインタの参照カウンタの差分を計算
			BOOL SrcDecriment = FALSE; //引数の方が参照数が多いかどうか
			unsigned long SubRef = MyRef - SrcRef;
			if (SrcRef > MyRef) //引数の方が参照数が多い場合
			{
				SubRef = SrcRef - MyRef;
				SrcDecriment = TRUE;
			}

			//参照カウンタの増加側と減少側を決める
			T pReleaseObj, pAddObj;
			if (SrcDecriment)
			{
				pReleaseObj = src.GetpInterface();
				pAddObj = *this->ppInterface;
			} else {
				pReleaseObj = *this->ppInterface;
				pAddObj = src.GetpInterface();
			}

			//互いの参照カウントを交換するように増減
			unsigned long i;
			if (pReleaseObj && pAddObj) //双方が有効なインターフェースの場合
			{
				for (i = 0; i < SubRef; i++)
				{
					pReleaseObj->Release();
					pAddObj->AddRef();
				}
			} else if (pReleaseObj && !pAddObj) //減少側だけが有効の場合
			{
				for (i = 0; i < SubRef; i++)
					pReleaseObj->Release();
			} else if (!pReleaseObj && pAddObj) //増加側だけが有効の場合
			{
				for (i = 0; i < SubRef; i++)
					pAddObj->AddRef();
			}

			//COMポインタ内のインターフェースへのポインタを交換
			T pTemp = src.GetpInterface();
			src.SetpInterface(this->GetpInterface());
			this->SetpInterface(pTemp);

			return;
		}


		//演算子
		ComPtr &operator =(const ComPtr<T> &src) //代入演算子のオーバーロード
		{
			if (this->ppInterface == src.ppInterface) //自分自身への代入の場合
				return *this; //何もしない

			SubRef(); //自分自身の参照カウンタを減少

			this->ppInterface = src.ppInterface; //インターフェースへのダブルポインタをコピー
			this->pRefCount = src.pRefCount; //参照カウンタへのポインタをコピー

			AddRef(); //代入元の参照カウンタを増加

			return *this;
		}
		ComPtr &operator =(T* ppSrcInterface) //インターフェースへのダブルポインタの代入
		{
			if (this->ppInterface == ppSrcInterface) //自分自身への代入の場合
				return *this; //何もしない

			SubRef(); //自分自身の参照カウンタを減少

			this->pRefCount = NULL; //参照カウンタを初期化
			this->ppInterface = ppSrcInterface; //インターフェースへのダブルポインタをコピー
			AddRef(); //参照カウンタを増加

			return *this;
		}

		//インターフェースへのポインタを取得
		T GetpInterface() const
		{
			if (ppInterface)
				return *ppInterface;
			return NULL;
		} 

		T *GetppInterface() //インタフェースへのダブルポインタを取得
		{
			return ppInterface;
		}

		void SetpInterface(T p)
		{
			if (!ppInterface)
				ppInterface = new T;
			*ppInterface = p;
			return;
		}

		unsigned long *GetpRefCount() { return pRefCount; } //参照カウンタへのポインタを取得
		unsigned long GetRefCount() //参照カウンタの値を取得
		{
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return *pRefCount;
			return 0; 
		}


		T operator ->() //インタフェースへのアクセス用
		{
			if (!ppInterface)
				return NULL;
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return *ppInterface;
			return NULL; //COMポインタがいずれのインターフェースも指していない場合
		} 

		operator T() const //インターフェースへのアクセス用のキャスト
		{
			if (!ppInterface)
				return NULL;
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return *ppInterface;
			return NULL; //COMポインタがいずれのインターフェースも指していない場合
		}

		operator T*() const //インターフェースへのダブルポインタ取得用キャスト
		{
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return ppInterface;
			return NULL; //COMポインタがいずれのインターフェースも指していない場合
		} 
	private:
		T *ppInterface; //インターフェースへのダブルポインタ
		unsigned long *pRefCount; //参照カウンタへのポインタ
	};
}

/*COM用のスマートポインタ

＜使い方＞
Create系関数へ渡すときは、ToCreator() 関数を使う。
	ComPtr<LPDIRECT3DTEXTURE9> cp;
	CreateTexture○○(…, cp.ToCreator());
cp がスコープを抜けるとデストラクタが呼ばれ、自動的にテクスチャ本体も解放される。

＜ダメな使い方1＞
コンストラクタや代入演算子を使って
ComPtr<LPDIRECT3DTEXTURE9> cp = new LPDIRECT3DTEXTURE9;
などとすると、インターフェースの実体はまだ作成されていないにもかかわらず
COMポインタ内部でインターフェースの AddRef() を呼ぼうとしてエラーが出る。

＜ダメな使い方2＞
	LPDIRECT3DTEXTURE9 pTex;
	CreateTexture○○（…, &pTex);
	ComPtr<LPDIRECT3DTEXTURE9> cp(pTex);
では AddRef() は正常に呼べるが、pTex を new で生成していないため、
pTex がスコープを抜けて破棄された瞬間に ppInterface がタンブリングポインタとなってしまう。

＜面倒な使い方1＞
	LPDIRECT3DTEXTURE9 *ppTex = new LPDIRECT3DTEXTURE9;
	*ppTex = NULL;
	ComPtr<LPDIRECT3DREXTURE9> cpTex = ppTex;
	CreateTexture○○(…, cpTex);
コンストラクタが呼ばれた時点で *ppTex == NULL 、つまりまだ実体が存在しないことが
示されていれば良い。このとき 最後に delete ppTex; を加えてはしてはいけない。


＜面倒な使い方2＞
ToCreator() を使わない実装では
	LPDIRECT3DTEXTURE9 *ppTex = new LPDIRECT3DTEXTURE9;
	CreateTexture○○(…, ppTex); //COM内部の参照カウンタが 0→1 になる
	ComPtr<LPDIRECT3DTEXTURE9> cpTex = ppTex; //COM内部の参照カウンタが 1→2 になる
	(*ppTex)->Release();  //COM内部の参照カウンタが 2→1 になる
	delete ppTex; //new で生成したので明示的に解放
となる。delete や Release() などを手動で呼ばねばならなくなり、面倒である。

*/