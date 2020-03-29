//�}���`�X���b�h�����N���X
#pragma once
#include <Windows.h>
#include <vector>
#include <queue>
#include <process.h>
#include <functional>

namespace ho {
	//�O���錾
	class Thread;
	class ThreadTask;

	class MultiThread
	{
	public:
		MultiThread(int Threads); //�R���X�g���N�^
		~MultiThread(); //�f�X�g���N�^

		void PushTask(const std::function<void(const void *p)> fTaskObj, const void *pArg); //�^�X�N�ǉ�
		void WaitForAllTask(int SleepTime = 0); //�L���[�Ǝ��s���̑S�Ẵ^�X�N����������܂ő҂u���b�N�֐�
		bool CheckCompleteAllTask(); //�S�Ẵ^�X�N���������ăL���[����ɂȂ������ǂ����𒲂ׂ�

		//�A�N�Z�b�T
		int GetThreads() { return Threads; }
	private:
		int Threads; //�쐬����X���b�h��
		std::vector<HANDLE> vectorhEvent1, vectorhEvent2; //�e�X���b�h�ɑΉ������C�x���g�I�u�W�F�N�g�z��
		std::vector<Thread *> vectorpThreadObj; //�X���b�h�I�u�W�F�N�g�ւ̃|�C���^�̔z��
		std::queue<ThreadTask *> queuepThreadTaskObj; //�������ׂ��X���b�h�^�X�N�I�u�W�F�N�g�ւ̃|�C���^��ςރL���[
		CRITICAL_SECTION csTask; //�^�X�N�L���[�̃N���e�B�J���Z�N�V����
		HANDLE hTaskSchedulerThread; //�^�X�N�X�P�W���[���X���b�h�̃n���h��
		bool ThreadState; //TRUE�ɂ���ƃ^�X�N�X�P�W���[���X���b�h���I��
		HANDLE hQueueEmpty; //�L���[����ɂȂ�ƃV�O�i����ԂƂȂ�C�x���g�n���h��
		HANDLE hQueueExist; //�L���[�ɑ��݂���ƃV�O�i����ԂƂȂ�C�x���g�n���h��

		static unsigned __stdcall TaskSchedulerThread(void *pArg); //�^�X�N�X�P�W���[���̃X���b�h�֐�
		void TaskSchedulerThreadLocal();//�^�X�N�X�P�W���[���̃X���b�h�֐��̓����֐�
	};

	class Thread
	{
	public:
		Thread(HANDLE &hEvent1, HANDLE &hEvent2); //�R���X�g���N�^
		~Thread(); //�f�X�g���N�^
		static unsigned __stdcall ThreadFunc(void *pArg); //�X���b�h�֐�
		bool SetNewTask(ThreadTask *pThreadTaskObj); //�V�����^�X�N�I�u�W�F�N�g��ݒ�

		//�A�N�Z�b�T
		bool GetExit() { return Exit; }
		void SetExit(bool Exit) { this->Exit = Exit; }
		HANDLE GethThread() { return hThread; }
		HANDLE &GethEvent1() { return hEvent1; }
		HANDLE &GethEvent2() { return hEvent2; }
	private:
		void ThreadFuncLocal(); //�����X���b�h�֐�
		HANDLE hThread; //�X���b�h�n���h��
		HANDLE &hEvent1, &hEvent2; //�C�x���g�n���h��
		bool Exit; //�X���b�h���I������ꍇ��TRUE�ɂ���
		ThreadTask *pThreadTaskObj; //��������X���b�h�^�X�N�I�u�W�F�N�g�ւ̃|�C���^
	};

	class ThreadTask
	{
	public:
		ThreadTask(const std::function<void(const void *p)> fTaskObj, const void *pArg); //�R���X�g���N�^
		~ThreadTask(); //�f�X�g���N�^
		virtual void RunTaskFunc(); //�^�X�N�֐������s

		//�A�N�Z�b�T
		bool GetComplete() { return Complete; }
	protected:
		const std::function<void(const void *p)> fTaskObj; //��������^�X�N�֐�
		const void *pArg; //�^�X�N�֐������Ŏg�p����f�[�^�ւ̃|�C���^

		//void (*pTaskFunc)(void *pArg); //��������^�X�N�֐��ւ̃|�C���^

		bool Complete; //�^�X�N���������ꂽ���ǂ���
	};
}

/*�}���`�X���b�h���g���ĕ��񏈗��ō����������鎞�ȂǂɎg���B
�g�����́AMultiThread�I�u�W�F�N�g��1�쐬���A�����������֐��ւ̃|�C���^��
������ThreadTask�I�u�W�F�N�g���쐬���AMultiThread::PushTask()��
����ThreadTask�I�u�W�F�N�g��o�^����B����ƁA�����I�ɋ󂢂Ă���X���b�h��
�^�X�N�����s�����B

MultiThread�I�u�W�F�N�g���폜����ƁA���s���̃^�X�N��X���b�h���I������܂�
�҂��āA�����I�ɏI������B���̂Ƃ��A�L���[�Ɏc���Ă���܂����s����Ă��Ȃ�
�^�X�N�͎��s���ꂸ�ɏI������B
*/
