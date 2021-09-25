// Fill out your copyright notice in the Description page of Project Settings.

#include "NuitrackThread.h"
#include "HAL/PlatformProcess.h"
#include "NuitrackIO.h"

DEFINE_LOG_CATEGORY(NuitrackThreadLog);

FNuitrackThread::FNuitrackThread(UNuitrackIO* IO) :
	NuitrackIO(IO),
	Thread(nullptr),
	StopTaskCounter(0)
{
	Thread = FRunnableThread::Create(this, TEXT("FNuitrackThread"), 0, EThreadPriority::TPri_BelowNormal); // windows default = 8mb for thread, could specify more
	if (!Thread)
	{
		UE_LOG(NuitrackThreadLog, Error, TEXT("Failed to create Nuitrack thread."));
	}
}

FNuitrackThread::~FNuitrackThread()
{
	if (Thread)
	{
		delete Thread;
		Thread = nullptr;
	}
}

bool FNuitrackThread::Init()
{
	UE_LOG(NuitrackThreadLog, Verbose, TEXT("Nuitrack thread started."));
	return true;
}

uint32 FNuitrackThread::Run()
{
	if (!NuitrackIO)
	{
		UE_LOG(NuitrackThreadLog, Error, TEXT("Nuitrack is null, could not run the thread."));
		return 1;
	}

	while (StopTaskCounter.GetValue() == 0)
	{
		NuitrackIO->UpdateAsync();
	}

	return 0;
}

void FNuitrackThread::Stop()
{
	StopTaskCounter.Increment();
}

void FNuitrackThread::EnsureCompletion()
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

FCriticalSection* FNuitrackThread::GetCriticalSection()
{
	return &CriticalSection;
}
