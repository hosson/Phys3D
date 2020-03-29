//フレームレート制御クラス
#pragma once
#include <functional>
#include <Windows.h>

namespace ho
{
	class FrameControl
	{
	public:
		FrameControl(const std::function<void()> &fUpdateObj, const std::function<void()> &fDrawObj); //コンストラクタ
		virtual ~FrameControl(); //デストラクタ

		virtual bool Control(); //フレーム制御処理
	protected:
	private:
		const std::function<void()> fUpdateObj; //更新関数
		const std::function<void()> fDrawObj; //描画関数
		bool ControlFirst; //Control 関数が初回呼び出しかどうか
		LARGE_INTEGER qpf; //QueryPerformanceCounter の 1 秒あたりのカウント数
		LARGE_INTEGER qpcNow; //現在の時間
		LARGE_INTEGER qpcNext; //次のフレームを開始する時間
	};
}
