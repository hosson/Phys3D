#include "MultiThread.h"
#include "Common.h"
#include "Error.h"

namespace ho {

	//�R���X�g���N�^
	MultiThread::MultiThread(int Threads)
	{
		//�N���e�B�J���Z�N�V������������
		InitializeCriticalSection(&csTask); 

		hQueueEmpty = CreateEvent(NULL, true, true, NULL); //�V�O�i����ԂŃC�x���g�I�u�W�F�N�g���쐬
		hQueueExist = CreateEvent(NULL, true, false, NULL); //��V�O�i����ԂŃC�x���g�I�u�W�F�N�g���쐬

		if (Threads <= 0) //�X���b�h�����w�肳��Ă��Ȃ��ꍇ
		{
			SYSTEM_INFO SystemInfo;
			GetSystemInfo(&SystemInfo);
			Threads = SystemInfo.dwNumberOfProcessors;
		}
		this->Threads = Threads;

		//�e�X���b�h�ɑΉ������C�x���g�I�u�W�F�N�g���쐬
		vectorhEvent1.resize(Threads);
		vectorhEvent2.resize(Threads);
		for (int i = 0; i < Threads; i++)
		{
			vectorhEvent1.at(i) = CreateEvent(NULL, true, false, NULL); //��V�O�i����ԂŃC�x���g�I�u�W�F�N�g���쐬
			vectorhEvent2.at(i) = CreateEvent(NULL, true, true, NULL); //�V�O�i����ԂŃC�x���g�I�u�W�F�N�g���쐬
		}

		//�X���b�h�I�u�W�F�N�g���쐬
		vectorpThreadObj.resize(Threads);
		for (int i = 0; i < Threads; i++)
			vectorpThreadObj.at(i) = new Thread(vectorhEvent1.at(i), vectorhEvent2.at(i));

		//�^�X�N�X�P�W���[���X���b�h�̍쐬
		ThreadState = false;
		hTaskSchedulerThread = (HANDLE)_beginthreadex(NULL, 0, TaskSchedulerThread, this, 0, NULL); //�X���b�h�쐬
	}

	//�f�X�g���N�^
	MultiThread::~MultiThread()
	{
		ThreadState = true; //�^�X�N�X�P�W���[���̃X���b�h�֐����I��
		SetEvent(hQueueExist); //�C�x���g���V�O�i����Ԃɂ���
		WaitForSingleObject(hTaskSchedulerThread, INFINITE); //�^�X�N�X�P�W���[���X���b�h���I������̂�҂�

		//�X���b�h�I�u�W�F�N�g���폜
		for (int i = 0; i < Threads; i++)
			SDELETE(vectorpThreadObj.at(i));

		//�e�X���b�h�ɑΉ������C�x���g�I�u�W�F�N�g�����
		for (int i = 0; i < Threads; i++)
		{
			CloseHandle(vectorhEvent1.at(i));
			CloseHandle(vectorhEvent2.at(i));
		}

		CloseHandle(hQueueExist); //�C�x���g�����
		CloseHandle(hQueueEmpty); //�C�x���g�����

		//�N���e�B�J���Z�N�V���������
		DeleteCriticalSection(&csTask);

		if (!queuepThreadTaskObj.empty())
			ERR(false, TEXT("�L���[�̒��g���c���Ă��܂��B"), __WFILE__, __LINE__);
	}

	//�^�X�N�ǉ�
	void MultiThread::PushTask(const std::function<void(const void *p)> fTaskObj, const void *pArg)
	{
		ThreadTask *pThreadTaskObj = new ThreadTask(fTaskObj, pArg); //�X���b�h�^�X�N�I�u�W�F�N�g�𐶐�

		EnterCriticalSection(&csTask); //�N���e�B�J���Z�N�V�����ɓ���
		ResetEvent(hQueueEmpty); //�C�x���g���V�O�i����Ԃɂ���
		queuepThreadTaskObj.push(pThreadTaskObj);
		SetEvent(hQueueExist); //�C�x���g���V�O�i����Ԃɂ���
		LeaveCriticalSection(&csTask); //�N���e�B�J���Z�N�V��������o��

		return;
	}

	//�L���[����ɂȂ�܂ő҂u���b�N�֐�
	void MultiThread::WaitForAllTask(int SleepTime) 
	{
		//��������͌��݂����Ă��炸�A�r���ɂ���WaitForMultipleObjects �Ŏ~�߂Ă���B
		/*������ SleepTime �� ���̒l������ƑS��CPU���Ԃ�����Ȃ��B
		0 ������ƁACPU �g�p���͑S�J�܂Ŏg�����^�C���X���C�X�����邽�߁A���̖Z�����X���b�h�̏�����D�悳���邱�Ƃ��ł���B
		1 ������ƁACPU �g�p���͂��Ȃ菭�Ȃ��Ȃ邪�A�L���[��҂��x���������đ傫�ȃI�[�o�[�w�b�h��������B
		�����̏ꍇ�� 0 ������ƍŗǂȌ��ʂ������鎖�������̂ł����߁B*/

		WaitForSingleObject(hQueueEmpty, INFINITE); //�L���[����ɂȂ�܂ő҂i�C�x���g���V�O�i����ԂɂȂ�܂ő҂j
				
		//�S�ẴX���b�h�̃^�X�N���I������܂ő҂�
		bool Complete; //�S�ẴX���b�h�̃^�X�N���I�����Ă��邩�ǂ���
		while (true)
		{
			Complete = true;
			EnterCriticalSection(&csTask); //�N���e�B�J���Z�N�V�����ɓ���
			WaitForMultipleObjects(Threads, &vectorhEvent2.at(0), TRUE, INFINITE); //�S�ẴX���b�h�̃C�x���g2���V�O�i����ԂɂȂ�܂őҋ@
			for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++) //�S�X���b�h�I�u�W�F�N�g�𑖍�
				if (WaitForSingleObject((*itr)->GethEvent1(), 0) == WAIT_OBJECT_0 || WaitForSingleObject((*itr)->GethEvent2(), 0) != WAIT_OBJECT_0) //�C�x���g�n���h�����擾���A�C�x���g�I�u�W�F�N�g����V�O�i����Ԃ������ꍇ
					Complete = false;
			LeaveCriticalSection(&csTask); //�N���e�B�J���Z�N�V��������o��

			if (Complete) //�S�Ẵ^�X�N���I�����Ă����ꍇ
				break;

			//if (SleepTime >= 0)
				//Sleep(SleepTime);
		}

		return;
	}

	//�S�Ẵ^�X�N���������ăL���[����ɂȂ������ǂ����𒲂ׂ�
	bool MultiThread::CheckCompleteAllTask()
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(hQueueEmpty, 0)) //�L���[����ł͂Ȃ��ꍇ
			return false;

		bool Complete;
		EnterCriticalSection(&csTask); //�N���e�B�J���Z�N�V�����ɓ���
		{
			if (WAIT_OBJECT_0 == WaitForMultipleObjects(Threads, &vectorhEvent2.at(0), TRUE, 0)) //�S�ẴX���b�h�̃C�x���g2���V�O�i����Ԃ̏ꍇ
			{
				Complete = true;
				for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++) //�S�X���b�h�I�u�W�F�N�g�𑖍�
					if (WaitForSingleObject((*itr)->GethEvent1(), 0) == WAIT_OBJECT_0 || WaitForSingleObject((*itr)->GethEvent2(), 0) != WAIT_OBJECT_0) //�C�x���g�n���h�����擾���A�C�x���g�I�u�W�F�N�g����V�O�i����Ԃ������ꍇ
						Complete = false;

			} else {
				Complete = false;
			}
		}
		LeaveCriticalSection(&csTask); //�N���e�B�J���Z�N�V��������o��

		return Complete;
	}

	//�^�X�N�X�P�W���[���̃X���b�h�֐�
	unsigned __stdcall MultiThread::TaskSchedulerThread(void *pArg) 
	{
		((MultiThread *)pArg)->TaskSchedulerThreadLocal();

		return 0;
	}

	//�^�X�N�X�P�W���[���̃X���b�h�֐��̓����֐�
	void MultiThread::TaskSchedulerThreadLocal()
	{
		while (!ThreadState)
		{
			EnterCriticalSection(&csTask); //�N���e�B�J���Z�N�V�����ɓ���
			if (queuepThreadTaskObj.size()) //�L���[�ɗv�f�����݂���ꍇ
			{
				//�S�X���b�h�I�u�W�F�N�g�𑖍�
				for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++)
				{
					if (WaitForSingleObject((*itr)->GethEvent2(), 0) == WAIT_OBJECT_0) //�C�x���g�n���h�����擾���A�C�x���g�I�u�W�F�N�g���V�O�i����Ԃ������ꍇ
					{
						if (!queuepThreadTaskObj.empty()) //�L���[����ł͂Ȃ��ꍇ
						{
							if ((*itr)->SetNewTask(queuepThreadTaskObj.front())) //�L���[�̐擪�̃I�u�W�F�N�g���^�X�N�ɐݒ肵�A���������ꍇ
							{
								queuepThreadTaskObj.pop(); //�L���[�̐擪�̃^�X�N���폜
								if (queuepThreadTaskObj.empty()) //�L���[����ɂȂ����ꍇ
								{
									SetEvent(hQueueEmpty); //�C�x���g���V�O�i����Ԃɂ���
									ResetEvent(hQueueExist); //�C�x���g���V�O�i����Ԃɂ���
								}
	
								SetEvent((*itr)->GethEvent1()); //�C�x���g���V�O�i����Ԃɐݒ�
								break;
							}
						}
					}
				}
			}
			LeaveCriticalSection(&csTask); //�N���e�B�J���Z�N�V��������o��

			WaitForSingleObject(hQueueExist, INFINITE); //�C�x���g���V�O�i����ԁi�L���[�����݂����ԁj�ɂȂ�܂ő҂�
		}

		//�S�X���b�h�I�u�W�F�N�g�𑖍�
		for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++)
		{
			(*itr)->SetExit(true); //�I���t���O�𗧂Ă�
			WaitForSingleObject((*itr)->GethEvent2(), INFINITE); //�C�x���g���V�O�i����ԂɂȂ�܂őҋ@
			SetEvent((*itr)->GethEvent1()); //�C�x���g���V�O�i����Ԃɂ���
		}

		//�S�X���b�h�I�u�W�F�N�g�𑖍�
		for (std::vector<Thread *>::iterator itr = vectorpThreadObj.begin(); itr != vectorpThreadObj.end(); itr++)
			WaitForSingleObject(((*itr)->GethThread()), INFINITE); //�X���b�h���I������܂őҋ@

		return;
	}











	//�R���X�g���N�^
	Thread::Thread(HANDLE &hEvent1, HANDLE &hEvent2) : hEvent1(hEvent1), hEvent2(hEvent2)
	{
		pThreadTaskObj = NULL;
		Exit = false;
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, NULL); //�X���b�h�쐬
	}

	//�f�X�g���N�^
	Thread::~Thread() 
	{
	}

	//�X���b�h�֐�
	unsigned __stdcall Thread::ThreadFunc(void *pArg)
	{
		((Thread *)pArg)->ThreadFuncLocal(); //�����X���b�h�֐������s

		return 0;
	}

	//�����X���b�h�֐�
	void Thread::ThreadFuncLocal() 
	{
		while (!Exit)
		{
			ResetEvent(hEvent1); //�C�x���g���V�O�i����Ԃɂ���

			if (pThreadTaskObj) //�^�X�N�I�u�W�F�N�g�����݂���ꍇ
			{
				pThreadTaskObj->RunTaskFunc(); //�^�X�N�֐������s
				SDELETE(pThreadTaskObj); //���s�ς݂̃^�X�N�I�u�W�F�N�g���폜
			}

			SetEvent(hEvent2); //�C�x���g���V�O�i����Ԃɐݒ�
			WaitForSingleObject(hEvent1, INFINITE); //�C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂őҋ@
			ResetEvent(hEvent2); //�C�x���g���V�O�i����Ԃɂ���
		}

		return;
	}

	//�V�����^�X�N�I�u�W�F�N�g��ݒ�
	bool Thread::SetNewTask(ThreadTask *pThreadTaskObj) 
	{
		if (this->pThreadTaskObj) //�܂��X���b�h�^�X�N�I�u�W�F�N�g���ݒ肳��Ă���ꍇ
			return false; //���s

		this->pThreadTaskObj = pThreadTaskObj;
		SetEvent(hEvent1); //�C�x���g���V�O�i����Ԃɐݒ�

		return true;
	}











	//�R���X�g���N�^
	ThreadTask::ThreadTask(const std::function<void(const void *p)> fTaskObj, const void *pArg)
		: fTaskObj(fTaskObj), pArg(pArg)
	{
		this->Complete = false;
	}

	//�f�X�g���N�^
	ThreadTask::~ThreadTask()
	{
	}

	//�^�X�N�֐������s
	void ThreadTask::RunTaskFunc() 
	{
		fTaskObj(pArg); //�^�X�N�֐������s
		this->Complete = true;

		return;
	}
}
