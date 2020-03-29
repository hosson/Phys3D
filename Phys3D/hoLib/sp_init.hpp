//std::shared_ptr の初期化を支援するクラス
#pragma once
#include <memory>

namespace ho
{
	//スマートポインタ初期化用関数
	template <typename U> std::shared_ptr<U> sp_init(U *p)
	{
		std::shared_ptr<U> sp = ((sp_base<U> *)p)->_get(); //基底クラスのスマートポインタを取得
		((sp_base<U> *)p)->_reset(); //基底クラスのスマートポインタを解放

		return sp; //スマートポインタを返すと、参照カウントは 1 に戻る
	}

	//基底クラス
	template <typename U> class sp_base
	{
		friend std::shared_ptr<U> sp_init<U>(U *p); //_get() と _reset() を private にするためのフレンド指定
	public:
		sp_base() //基底クラスのコンストラクタが先に実行される
		{
			this->sp_this = std::shared_ptr<U>((U *)this);
		}
	private:
		std::shared_ptr<U> _get() { return sp_this; }
		void _reset() { sp_this.reset(); }
	protected:
		std::shared_ptr<U> sp_this;
	};
}

