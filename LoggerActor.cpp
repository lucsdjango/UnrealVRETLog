// Fill out your copyright notice in the Description page of Project Settings.


#include "LoggerActor.h"


// Sets default values
ALoggerActor::ALoggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork; // as late as possible

	LogColumns = TMap<FString, FString>();
	ResetColumns = TMap<FString, FString>();
	Logging = false;
}

// Called when the game starts or when spawned
void ALoggerActor::BeginPlay()
{
	Super::BeginPlay();
	startSeconds = FPlatformTime::Seconds();
	Logging = false;
	AddEntry("time");
	AddEntry("test", "initVal", true);

	//StartLogging("test.log");
	

}

// Called every frame
void ALoggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FString s = (Logging ? "true" : "false");
	//UE_LOG(LogTemp, Warning, TEXT("hej"));
	if (Logging == true)
	{
		UpdateEntry("time", FString::SanitizeFloat(FPlatformTime::Seconds() - startSeconds,4).Left(6)); // Make helper function
		if(FMath::RandRange(0, 100) > 90)
			UpdateEntry("test", "hej");
		if (!Async)
		{
			Log();
		}
		for (auto& Elem : ResetColumns)
		{
			UpdateEntry(Elem.Key, Elem.Value);
		}
	}

}

void ALoggerActor::AddEntry(FString k, FString v, bool resetEachFrame)
{
	LogColumns.Add(k,v);
	if (resetEachFrame)
		ResetColumns.Add(k, v);
}

void ALoggerActor::UpdateEntry(FString k, FString v)
{
	if (LogColumns.Contains(k))
		LogColumns[k] = v + "\t";
	else
		UE_LOG(LogTemp, Warning, TEXT("Attempted update of missing log-key: "));
}

void ALoggerActor::StartLogging(FString id, bool async)
{
	UE_LOG(LogTemp, Warning, TEXT("StartLog called"));
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	const TCHAR* directory = *id;
	fileHandle = file.OpenWrite(directory);
	LogLabels();
	Async = async;
	Logging = true;

}


void ALoggerActor::LogLabels()
{
	if (!fileHandle)
	{
		return;
	}
	FString line = "";
	for (auto& Elem : LogColumns)
	{
		line.Append(Elem.Key + "\t");
	}
	line.Append("\n");
	fileHandle->Write((const uint8*)TCHAR_TO_ANSI(*line), line.Len());
	//UE_LOG(LogTemp, Warning, TEXT("Labels written"));
}


void ALoggerActor::Log()
{
	//UE_LOG(LogTemp, Warning, TEXT("Log called"));
	if (!Logging || !fileHandle)
	{
		return;
	}
	FString line = "";
	for (auto& Elem : LogColumns)
	{
		line.Append(Elem.Value + "\t");
	}
	line.Append("\n");
	fileHandle->Write((const uint8*)TCHAR_TO_ANSI(*line), line.Len());
	//UE_LOG(LogTemp, Warning, TEXT("Values written"));


}

void ALoggerActor::AsyncLog()
{
	if (Async)
	{
		Log();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted Async log when in sync mode! "));
	
	}

}


void ALoggerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	fileHandle->Flush(true);
	delete fileHandle;
	UE_LOG(LogTemp, Warning, TEXT("file closed"));

	Super::EndPlay(EndPlayReason);

}

