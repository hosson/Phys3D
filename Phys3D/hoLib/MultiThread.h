//マルチスレッド処理クラス
#pragma once
#include <Windows.h>
#include <vector>
#include <queue>
#include <process.h>
#include <functional>

namespace ho {
	//前方宣言
	class Thread;
	class ThreadTask;

	class MultiThread
	{
	public:
		MultiThread(int Threads); //コンストラクタ
		~MultiThread(); //デストラクタ

		void PushTask(const std::function<void(const void *p)> fTaskObj, const void *pArg); //タスク追加
		void WaitForAllTask(int SleepTime = 0); //キューと実行中の全てのタスクが完了するまで待つブロック関数
		bool CheckCompleteAllTask(); //全てのタスクが完了してキューが空になったかどうかを調べる

		//アクセッサ
		int GetThreads() { return Threads; }
	private:
		int Threads; //作成するスレッド数
		std::vector<HANDLE> vectorhEvent1, vectorhEvent2; //各スレッドに対応したイベントオブジェクト配列
		std::vector<Thread *> vectorpThreadObj; //スレッドオブジェクトへのポインタの配列
		std::queue<ThreadTask *> queuepThreadTaskObj; //処理すべきスレッドタスクオブジェクトへのポインタを積むキュー
		CRITICAL_SECTION csTask; //タスクキューのクリティカルセクション
		HANDLE hTaskSchedulerThread; //タスクスケジューラスレッドのハンドル
		bool ThreadState; //TRUEにするとタスクスケジューラスレッドが終了
		HANDLE hQueueEmpty; //キューが空になるとシグナル状態となるイベントハンドル
		HANDLE hQueueExist; //キューに存在するとシグナル状態となるイベントハンドル

		static unsigned __stdcall TaskSchedulerThread(void *pArg); //タスクスケジューラのスレッド関数
		void TaskSchedulerThreadLocal();//タスクスケジューラのスレッド関数の内部関数
	};

	class Thread
	{
	public:
		Thread(HANDLE &hEvent1, HANDLE &hEvent2); //コンストラクタ
		~Thread(); //デストラクタ
		static unsigned __stdcall ThreadFunc(void *pArg); //スレッド関数
		bool SetNewTask(ThreadTask *pThreadTaskObj); //新しいタスクオブジェクトを設定

		//アクセッサ
		bool GetExit() { return Exit; }
		void SetExit(bool Exit) { this->Exit = Exit; }
		HANDLE GethThread() { return hThread; }
		HANDLE &GethEvent1() { return hEvent1; }
		HANDLE &GethEvent2() { return hEvent2; }
	private:
		void ThreadFuncLocal(); //内部スレッド関数
		HANDLE hThread; //スレッドハンドル
		HANDLE &hEvent1, &hEvent2; //イベントハンドル
		bool Exit; //スレッドを終了する場合はTRUEにする
		ThreadTask *pThreadTaskObj; //処理するスレッドタスクオブジェクトへのポインタ
	};

	class ThreadTask
	{
	public:
		ThreadTask(const std::function<void(const void *p)> fTaskObj, const void *pArg); //コンストラクタ
		~ThreadTask(); //デストラクタ
		virtual void RunTaskFunc(); //タスク関数を実行

		//アクセッサ
		bool GetComplete() { return Complete; }
	protected:
		const std::function<void(const void *p)> fTaskObj; //処理するタスク関数
		const void *pArg; //タスク関数内部で使用するデータへのポインタ

		//void (*pTaskFunc)(void *pArg); //処理するタスク関数へのポインタ

		bool Complete; //タスクが処理されたかどうか
	};
}

/*マルチスレッドを使って並列処理で高速化させる時などに使う。
使い方は、MultiThreadオブジェクトを1つ作成し、処理したい関数へのポインタを
書いたThreadTaskオブジェクトを作成し、MultiThread::PushTask()で
そのThreadTaskオブジェクトを登録する。すると、自動的に空いているスレッドで
タスクが実行される。

MultiThreadオブジェクトを削除すると、実行中のタスクやスレッドが終了するまで
待って、自動的に終了する。このとき、キューに残っているまだ実行されていない
タスクは実行されずに終了する。
*/
