//スマートポインタクラス
//未完成

#pragma once
#include "Common.h"

#include "DetectMemoryLeak.h"

namespace ho
{
	template <class T> class SmartPtr
	{
	public:
		//コンストラクタ
		SmartPtr(const T *pSrc = NULL) //通常
		{
			this->p = pSrc;
			this->pRefCount = NULL;
			if (p) //ポインタが渡された場合
				AddRef(); //参照カウンタを増加
		}
		SmartPtr(const SmartPtr<T> &src) //コピーコンストラクタ
		{
			this->p = src.p; //ポインタをコピー
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
				pRefCount = new ULONG(1); //参照カウンタを作成

			return;
		}
		//参照カウンタを減少
		void SubRef()
		{
			if (pRefCount) //参照カウンタが存在する場合
			{
				(*pRefCount)--;
				if (*pRefCount == 0) //参照カウンタが0になった場合
				{
					SDELETE(pRefCount); //参照カウンタを削除
					SDELETE(*p); //オブジェクトの実体を削除
				}
			}

			return;
		}

		//アクセッサ
		T Getp() { return p; } //ポインタを取得

		//演算子
		SmartPtr &operator =(const SmartPtr<T> &src) //代入演算子のオーバーロード
		{
			if (this->p == src.p) //自分自身への代入の場合
				return *this; //何もしない

			SubRef(); //自分自身の参照カウンタを減少

			this->p = src.p; //ポインタをコピー
			this->pRefCount = src.pRefCount; //参照カウンタへのポインタをコピー

			AddRef(); //代入元の参照カウンタを増加

			return *this;
		}

		T operator ->() //オブジェクトのメンバへのアクセス用
		{
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return *p;
			//★エラー（エラー処理を記述した方がいい）
			return NULL; //COMポインタがいずれのインターフェースも指していない場合
		} 

		operator T() const //インターフェースへのアクセス用のキャスト
		{
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return *p;
			//★エラー（エラー処理を記述した方がいい）
			return NULL; //COMポインタがいずれのインターフェースも指していない場合
		}

		operator T*() const //インターフェースへのダブルポインタ取得用キャスト
		{
			if (pRefCount) //参照カウンタが存在する（1以上の）場合
				return p;
			//★エラー（エラー処理を記述した方がいい）
			return NULL; //COMポインタがいずれのインターフェースも指していない場合
		} 
	private:
		T *p; //オブジェクトへのポインタ
		ULONG *pRefCount; //参照カウンタへのポインタ
	};
}
