// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

DECLARE_LOG_CATEGORY_EXTERN(NuitrackThreadLog, Log, All);

class UNuitrackIO;

class FNuitrackThread : public FRunnable
{
public:
	FNuitrackThread(UNuitrackIO* IO);
	virtual ~FNuitrackThread();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	/** Stops the threadand waits for its completion. */
	void EnsureCompletion();

	FCriticalSection* GetCriticalSection();

private:
	/** Thread handle.Control the thread using this, with operators like Killand Suspend */
	FRunnableThread* Thread;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	/** The device that starts this thread. */
	UNuitrackIO* NuitrackIO;

	/** To be used for UScopeLock */
	FCriticalSection CriticalSection;

};
