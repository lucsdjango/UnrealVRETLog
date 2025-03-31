// Fill out your copyright notice in the Description page of Project Settings.


#include "TSVLogger.h"

namespace {

	FString NumStringCapped(float f)
	{
		//return FString::SanitizeFloat(f, 4).Left(6);
		return FString::SanitizeFloat(f);
	}

}

// Sets default values
ATSVLogger::ATSVLogger()
{
	
	PrimaryActorTick.bCanEverTick = true; // Set this actor to call Tick() every frame. 
	PrimaryActorTick.TickGroup = TG_PostUpdateWork; // as late as possible

	// Initialize TMaps
	LogColumns = TMap<FString, FString>();
	ResetColumns = TMap<FString, FString>();
	Logging = false;


}

// Called when the game starts or when spawned
void ATSVLogger::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->FixedFrameRate = 150.0;
	//GEngine->bUseFixedFrameRate = true;
	startSeconds = FPlatformTime::Seconds();
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
}

// Called every frame
void ATSVLogger::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	FString s = (Logging ? "true" : "false");
	//UE_LOG(LogTemp, Warning, TEXT("hej"));
	if (Logging == true)
	{
		UpdateEntry("time", NumStringCapped(FPlatformTime::Seconds() - startSeconds)); // Make helper function
		if (FMath::RandRange(0, 100) > 90)
			UpdateEntry("test", "hej");
		if (!Async && Logging && fileHandle)
		{
			Log();
		}
		for (auto& Elem : ResetColumns)
		{
			UpdateEntry(Elem.Key, Elem.Value);
		}
	}

}

void ATSVLogger::LogLabels()
{
	if (!fileHandle)
	{
		return;
	}
	FString Line = "";
	for (auto& Elem : LogColumns)
	{
		Line.Append(Elem.Key + "\t");
	}
	Line.Append("\n");
	fileHandle->Write((const uint8*)TCHAR_TO_ANSI(*Line), Line.Len());
	UE_LOG(LogTemp, Warning, TEXT("Labels written"));
}

// Called once every tick after logging started, unless async logging mode enabled
void ATSVLogger::Log()
{
	//UE_LOG(LogTemp, Warning, TEXT("Log called"));

	FString line = "";
	for (auto& Elem : LogColumns)
	{
		line.Append(Elem.Value + "\t");
	}
	line.Append("\n");
	fileHandle->Write((const uint8*)TCHAR_TO_ANSI(*line), line.Len());
	//UE_LOG(LogTemp, Warning, TEXT("Values written"));
}

void ATSVLogger::AsyncLog()
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

void ATSVLogger::AddEntry(FString k, FString v, bool resetEachFrame)
{
	LogColumns.Add(k, v);

	if (resetEachFrame)
		ResetColumns.Add(k, v);
}

void ATSVLogger::UpdateEntry(FString k, FString v)
{
	if (LogColumns.Contains(k))
		LogColumns[k] = v;
	else
		UE_LOG(LogTemp, Warning, TEXT("Attempted update of missing log-key: "));
}

void ATSVLogger::UpdateVector(FString k, FVector v)
{
	UpdateEntry(k, NumStringCapped(v.X) + "," + NumStringCapped(v.Y) + "," + NumStringCapped(v.Z));
}



void ATSVLogger::UpdateDouble(FString k, double v)
{
	UpdateEntry(k, NumStringCapped(v));
}

void ATSVLogger::UpdateFloat(FString k, float v)
{
	UpdateEntry(k, NumStringCapped(v));
}

void ATSVLogger::UpdateInt(FString k, int v)
{
	UpdateEntry(k, FString::FromInt(v));
}

void ATSVLogger::InitOrReset()
{
	LogColumns = TMap<FString, FString>();
	ResetColumns = TMap<FString, FString>();
	Logging = false;
	AddEntry("time");
	AddEntry("test", "default", true);
}


void ATSVLogger::StartLogging(FString id, bool async)
{
	UE_LOG(LogTemp, Warning, TEXT("StartLog called"));
	
	FString ProjectSavedDirectory = FPaths::ProjectContentDir() + "/VRETLogs";
	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!platformFile.DirectoryExists(*ProjectSavedDirectory))
	{
		platformFile.CreateDirectory(*ProjectSavedDirectory);
	}
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: ProjectSavedDir: %s"), *ProjectSavedDirectory);
	
	
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	const TCHAR* directory = *ProjectSavedDirectory.Append("//").Append(*id);
	fileHandle = file.OpenWrite(directory);
	LogLabels();
	Async = async;
	Logging = true;
}



void ATSVLogger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("EndPlay"));
	if (fileHandle) // close file writer after writing buffer
	{
		fileHandle->Flush(true);
		delete fileHandle;
		UE_LOG(LogTemp, Warning, TEXT("file closed"));
	}

	Super::EndPlay(EndPlayReason);
}




 