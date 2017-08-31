#pragma once

#define LOADER_WORKER_THREAD_STACK_SIZE	1024
#define LOADER_WORKER_THREAD_PRIORITY   (OS_THREAD_LAUNCHER_PRIORITY - 1)

class Loader
{
private:
	enum LoaderState
	{
		LOADER_STATE_FADE_IN,
		LOADER_STATE_LOOP,
		LOADER_STATE_FADE_OUT,
		LOADER_STATE_FINISHED
	};
	LoaderState mState;
	int mStateCounter;

	bool mShouldFinish;

	OSThread mLoaderThread;
	u32 mLoaderThreadStack[LOADER_WORKER_THREAD_STACK_SIZE / sizeof(u32)];

	void LoaderThreadMain();
public:
	Loader()
		: mShouldFinish(false)
	{ }

	void BeginLoad();
	void FinishLoad();

	bool HasFinished() const
	{
		return mState == LOADER_STATE_FINISHED;
	}

	void WaitFinish()
	{
		if(HasFinished())
			return;
		OS_JoinThread(&mLoaderThread);
	}

	static void LoaderThreadMain(void* arg)
	{
		((Loader*)arg)->LoaderThreadMain();
	}
};