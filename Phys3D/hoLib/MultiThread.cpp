#include "MultiThread.h"
#include "Common.h"
#include "Error.h"

namespace ho {

	//コンストラクタ
	MultiThread::MultiThread(int Threads)
	{
		//クリティカルセクションを初期化
		InitializeCriticalSection(&csTask); 

		hQueueEmpty = CreateEvent(NULL, true, true, NULL); //シグナル状態でイベントオブジェクトを作成
		hQueueExist = CreateEvent(NULL, true, false, NULL); //非シグナル状態でイベントオブジェクトを作成

		if (Threads <= 0) //スレッド数が指定されていない場合
		{
			SYSTEM_INFO SystemInfo;
			GetSystemInfo(&SystemInfo);
			Threads = SystemInfo.dwNumberOfProcessors;
		}
		this->Threads = Threads;

		//各スレッドに対応したイベントオブジェクトを作成
		vectorhEvent1.resize(Threads);
		vectorhEvent2.resize(Threads);
		for (int i = 0; i < Threads; i++)
		{
			vectorhEvent1.at(i) = CreateEvent(NULL, true, false, NULL); //非シグナル状態でイベントオブジェクトを作成
			vectorhEvent2.at(i) = CreateEvent(NULL, true, true, NULL); //シグナル状態でイベントオブジェクトを作成
		}

		//スレッドオブジェクトを作成
		vectorpThreadObj.resize(Threads);
		for (int i = 0; i < Threads; i++)
			vectorpThreadObj.at(i) = new Thread(vectorhEvent1.at(i), vectorhEvent2.at(i));

		//タスクスケジューラスレッドの作成
		ThreadState = false;
		hTaskSchedulerThread = (HANDLE)_beginthreadex(NULL, 0, TaskSchedulerThread, this, 0, NULL); //スレッド作成
	}

	//デストラクタ
	MultiThread::~MultiThread()
	{
		ThreadState = true; //タスクスケジューラのスレッド関数を終了
		SetEvent(hQueueExist); //イベントをシグナル状態にする
		WaitForSingleObject(hTaskSchedulerThread, INFINITE); //タスクスケジューラスレッドが終了するのを待つ

		//スレッドオブジェクトを削除
		for (int i = 0; i < Threads; i++)
			SDELETE(vectorpThreadObj.at(i));

		//各スレッドに対応したイベントオブジェクトを解放
		for (int i = 0; i < Threads; i++)
		{
			CloseHandle(vectorhEvent1.at(i));
			CloseHandle(vectorhEvent2.at(i));
		}

		CloseHandle(hQueueExist); //イベントを解放
		CloseHandle(hQueueEmpty); //イベントを解放

		//クリティカルセクションを解放
		DeleteCriticalSection(&csTask);

		if (!queuepThreadTaskObj.empty())
			ERR(false, TEXT("キューの中身が残っています。"), __WFILE__, __LINE__);
	}

	//タスク追加
	void MultiThread::PushTask(const std::function<void(const void *p)> fTaskObj, const void *pArg)
	{
		ThreadTask *pThreadTaskObj = new ThreadTask(fTaskObj, pArg); //スレッドタスクオブジェクトを生成

		EnterCriticalSection(&csTask); //クリティカルセクションに入る
		ResetEvent(hQueueEmpty); //イベントを非シグナル状態にする
		queuepThreadTaskObj.push(pThreadTaskObj);
		SetEvent(hQueueExist); //イベントをシグナル状態にする
		LeaveCriticalSection(&csTask); //クリティカルセクションから出る

		return;
	}

	//キューが空になるまで待つブロック関数
	void MultiThread::WaitForAllTask(int SleepTime) 
	{
		//↓★これは現在つかっておらず、途中にあるWaitForMultipleObjects で止めている。
		/*引数の SleepTime は 負の値を入れると全くCPU時間を譲らない。
		0 を入れると、CPU 使用率は全開まで使うがタイムスライスを譲るため、他の忙しいスレッドの処理を優先させることができる。
		1 を入れると、CPU 使用率はかなり少なくなるが、キューを待つ精度が悪化して大きなオーバーヘッドが生じる。
		多くの場合は 0 を入れると最良な結果が得られる事が多いのでお勧め。*/

		WaitForSingleObject(hQueueEmpty, INFINITE); //キューが空になるまで待つ（イベントがシグナル状態になるまで待つ）
				
		//全てのスレッドのタスクが終了するまで待つ
		bool Complete; //全てのスレッドのタスクが終了しているかどうか
		while (true)
		{
			Complete = true;
			EnterCriticalSection(&csTask); //クリティカルセクションに入る
			WaitForMultipleObjects(Threads, &vectorhEvent2.at(0), TRUE, INFINITE); //全てのスレッドのイベント2がシグナル状態になるまで待機
			for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++) //全スレッドオブジェクトを走査
				if (WaitForSingleObject((*itr)->GethEvent1(), 0) == WAIT_OBJECT_0 || WaitForSingleObject((*itr)->GethEvent2(), 0) != WAIT_OBJECT_0) //イベントハンドルを取得し、イベントオブジェクトが非シグナル状態だった場合
					Complete = false;
			LeaveCriticalSection(&csTask); //クリティカルセクションから出る

			if (Complete) //全てのタスクが終了していた場合
				break;

			//if (SleepTime >= 0)
				//Sleep(SleepTime);
		}

		return;
	}

	//全てのタスクが完了してキューが空になったかどうかを調べる
	bool MultiThread::CheckCompleteAllTask()
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(hQueueEmpty, 0)) //キューが空ではない場合
			return false;

		bool Complete;
		EnterCriticalSection(&csTask); //クリティカルセクションに入る
		{
			if (WAIT_OBJECT_0 == WaitForMultipleObjects(Threads, &vectorhEvent2.at(0), TRUE, 0)) //全てのスレッドのイベント2がシグナル状態の場合
			{
				Complete = true;
				for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++) //全スレッドオブジェクトを走査
					if (WaitForSingleObject((*itr)->GethEvent1(), 0) == WAIT_OBJECT_0 || WaitForSingleObject((*itr)->GethEvent2(), 0) != WAIT_OBJECT_0) //イベントハンドルを取得し、イベントオブジェクトが非シグナル状態だった場合
						Complete = false;

			} else {
				Complete = false;
			}
		}
		LeaveCriticalSection(&csTask); //クリティカルセクションから出る

		return Complete;
	}

	//タスクスケジューラのスレッド関数
	unsigned __stdcall MultiThread::TaskSchedulerThread(void *pArg) 
	{
		((MultiThread *)pArg)->TaskSchedulerThreadLocal();

		return 0;
	}

	//タスクスケジューラのスレッド関数の内部関数
	void MultiThread::TaskSchedulerThreadLocal()
	{
		while (!ThreadState)
		{
			EnterCriticalSection(&csTask); //クリティカルセクションに入る
			if (queuepThreadTaskObj.size()) //キューに要素が存在する場合
			{
				//全スレッドオブジェクトを走査
				for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++)
				{
					if (WaitForSingleObject((*itr)->GethEvent2(), 0) == WAIT_OBJECT_0) //イベントハンドルを取得し、イベントオブジェクトがシグナル状態だった場合
					{
						if (!queuepThreadTaskObj.empty()) //キューが空ではない場合
						{
							if ((*itr)->SetNewTask(queuepThreadTaskObj.front())) //キューの先頭のオブジェクトをタスクに設定し、成功した場合
							{
								queuepThreadTaskObj.pop(); //キューの先頭のタスクを削除
								if (queuepThreadTaskObj.empty()) //キューが空になった場合
								{
									SetEvent(hQueueEmpty); //イベントをシグナル状態にする
									ResetEvent(hQueueExist); //イベントを非シグナル状態にする
								}
	
								SetEvent((*itr)->GethEvent1()); //イベントをシグナル状態に設定
								break;
							}
						}
					}
				}
			}
			LeaveCriticalSection(&csTask); //クリティカルセクションから出る

			WaitForSingleObject(hQueueExist, INFINITE); //イベントがシグナル状態（キューが存在する状態）になるまで待つ
		}

		//全スレッドオブジェクトを走査
		for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++)
		{
			(*itr)->SetExit(true); //終了フラグを立てる
			WaitForSingleObject((*itr)->GethEvent2(), INFINITE); //イベントがシグナル状態になるまで待機
			SetEvent((*itr)->GethEvent1()); //イベントをシグナル状態にする
		}

		//全スレッドオブジェクトを走査
		for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++)
			WaitForSingleObject(((*itr)->GethThread()), INFINITE); //スレッドが終了するまで待機

		return;
	}











	//コンストラクタ
	Thread::Thread(HANDLE &hEvent1, HANDLE &hEvent2) : hEvent1(hEvent1), hEvent2(hEvent2)
	{
		pThreadTaskObj = NULL;
		Exit = false;
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, NULL); //スレッド作成
	}

	//デストラクタ
	Thread::~Thread() 
	{
	}

	//スレッド関数
	unsigned __stdcall Thread::ThreadFunc(void *pArg)
	{
		((Thread *)pArg)->ThreadFuncLocal(); //内部スレッド関数を実行

		return 0;
	}

	//内部スレッド関数
	void Thread::ThreadFuncLocal() 
	{
		while (!Exit)
		{
			ResetEvent(hEvent1); //イベントを非シグナル状態にする

			if (pThreadTaskObj) //タスクオブジェクトが存在する場合
			{
				pThreadTaskObj->RunTaskFunc(); //タスク関数を実行
				SDELETE(pThreadTaskObj); //実行済みのタスクオブジェクトを削除
			}

			SetEvent(hEvent2); //イベントをシグナル状態に設定
			WaitForSingleObject(hEvent1, INFINITE); //イベントオブジェクトがシグナル状態になるまで待機
			ResetEvent(hEvent2); //イベントを非シグナル状態にする
		}

		return;
	}

	//新しいタスクオブジェクトを設定
	bool Thread::SetNewTask(ThreadTask *pThreadTaskObj) 
	{
		if (this->pThreadTaskObj) //まだスレッドタスクオブジェクトが設定されている場合
			return false; //失敗

		this->pThreadTaskObj = pThreadTaskObj;
		SetEvent(hEvent1); //イベントをシグナル状態に設定

		return true;
	}











	//コンストラクタ
	ThreadTask::ThreadTask(const std::function<void(const void *p)> fTaskObj, const void *pArg)
		: fTaskObj(fTaskObj), pArg(pArg)
	{
		this->Complete = false;
	}

	//デストラクタ
	ThreadTask::~ThreadTask()
	{
	}

	//タスク関数を実行
	void ThreadTask::RunTaskFunc() 
	{
		fTaskObj(pArg); //タスク関数を実行
		this->Complete = true;

		return;
	}
}
