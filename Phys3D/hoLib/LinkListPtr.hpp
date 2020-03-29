//リンクリスト方式のスマートポインタ
#pragma once

namespace ho
{
	class LinkListPtr_Base
	{
	public:
	protected:
		LinkListPtr_Base(LinkListPtr_Base *pPrev, LinkListPtr_Base *pNext) //コンストラクタ
		{
			this->pPrev = pPrev;
			this->pNext = pNext;
		}

		//リンクリストから外れる処理
		bool Remove()
		{
			if (!pPrev && !pNext) //前も次も接続されていない場合
				return true; //オブジェクトは削除される

			if (pNext) //次が接続されている場合
				pNext->pPrev = pPrev;
			if (pPrev) //前が接続されている場合
				pPrev->pNext = pNext;

			return false; //オブジェクトは削除されない
		}

		//引数のオブジェクトをリストの次方向に追加する
		void Join(LinkListPtr_Base &obj) const
		{
			if (pNext) //すでに次方向に存在する場合
			{
				obj.pPrev = (LinkListPtr_Base *)this;
				obj.pNext = pNext;
				pNext->pPrev = &obj;
				pNext = &obj;
			} else {
				obj.pPrev = (LinkListPtr_Base *)this;
				pNext = &obj;
			}

			return;
		}

		mutable LinkListPtr_Base *pPrev, *pNext; //隣接するオブジェクトへのポインタ
	};

	template <class T> class LinkListPtr : public LinkListPtr_Base
	{
	public:
		//コンストラクタ
		LinkListPtr(T *p = NULL) : LinkListPtr_Base(NULL, NULL)
		{
			this->p = p;
		}

		//コピーコンストラクタ
		LinkListPtr(const LinkListPtr<T> &obj) : LinkListPtr_Base(NULL, NULL)
		{
			this->p = NULL;
			Delete(); //リストから外れ、参照されなくなったオブジェクトを破棄する

			obj.Join(*this);
			this->p = obj.p;
		}

		//デストラクタ
		~LinkListPtr()
		{
			Delete(); //リストから外れ、参照されなくなったオブジェクトを破棄する
		}

		//リストから外れ、参照されなくなったオブジェクトを破棄する
		void Delete()
		{
			if (Remove()) //リンクリストから外れる処理をし、オブジェクトが削除される場合
			{
				if (p)
				{
					delete p;
					p = NULL;
				}
			}
		}

		//オブジェクトへのポインタを取得
		T Getp()
		{
			return p;
		}

		//代入
		LinkListPtr<T> &operator =(const LinkListPtr<T> &obj) {
			if (&obj != this) //同じポインタを参照するものではない場合
			{
				Delete(); //リストから外れ、参照されなくなったオブジェクトを破棄する

				obj.Join(*this);
				this->p = obj.p;
			}
			return *this;
		}

		//値取得
		const T &operator *() const { return *p; }
		T &operator *() { return *p; }

		//オブジェクトのメンバアクセス用
		const T *operator ->() const { return p; }
		T *operator ->() { return p; }

	private:
		T *p; //オブジェクトへのポインタ
	};
}

/*最低限の動作はするが、まだ未実装な機能もある。*/