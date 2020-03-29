//フレームレート制御クラス
#include "FrameControl.h"
#include <Windows.h>

namespace ho
{
	//コンストラクタ
	FrameControl::FrameControl(const std::function<void()> &fUpdateObj, const std::function<void()> &fDrawObj)
		: fUpdateObj(fUpdateObj), fDrawObj(fDrawObj), ControlFirst(true)
	{
		if (!QueryPerformanceFrequency(&this->qpf)) //ハードウェアが QueryPerformanceCounter をサポートしていない場合
			this->qpf.QuadPart = 0;
	}

	//デストラクタ
	FrameControl::~FrameControl()
	{
	}

	//フレームコントロール
	bool FrameControl::Control() 
	{
		QueryPerformanceCounter(&qpcNow); //現在時間を取得

		if (this->ControlFirst) //フレームコントロール関数が初回呼び出しの場合
		{
			//次フレーム開始時間を計算
			this->qpcNext.QuadPart= this->qpcNow.QuadPart + LONGLONG(this->qpf.QuadPart / 60.0);

			this->ControlFirst = false;
		}

		if (this->qpcNow.QuadPart >= this->qpcNext.QuadPart) //次フレームの処理時間を過ぎていた場合
		{
			this->fUpdateObj(); //更新関数実行
			this->fDrawObj(); //描画関数実行

			this->qpcNext.QuadPart += LONGLONG(this->qpf.QuadPart / 60.0); //次フレームの時間を更新
		} else {
			Sleep(1);
		}

		return false;
	}
}