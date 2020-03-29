//メッセージループクラス

#pragma once
#include <functional>

namespace ho
{
	class MessageLoop
	{
	public:
		MessageLoop(const std::function<bool()> &fFrameControlObj); //コンストラクタ
		~MessageLoop() {} //デストラクタ

		int Run(); //メッセージループ実行
	private:
		const std::function<bool()> fFrameControlObj; //フレーム制御関数
	};
}

/*
WinMain内のメッセージループを処理するクラス

通常アプリケーションの場合はコンストラクタに
std::function<bool()>()
を渡す。

フレーム制御関数を挟むアプリケーションの場合は、
コンストラクタにフレーム制御関数のオブジェクトを渡す。
*/