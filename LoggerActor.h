  // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"
#include "LoggerActor.generated.h"


UCLASS()
class QUICKSTART_API ALoggerActor : public AActor
{
	GENERATED_BODY()



public:	
	// Sets default values for this actor's properties
	ALoggerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	TMap<FString, FString> LogColumns;
	TMap<FString, FString> ResetColumns;
	IFileHandle* fileHandle;
	double startSeconds;
	bool Logging;
	void LogLabels();
	bool Async;
	void Log();
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void AddEntry(FString k, FString v = "", bool resetEachFrame = false);
	UFUNCTION(BlueprintCallable)
	void UpdateEntry(FString k, FString v = "");
	UFUNCTION(BlueprintCallable)
	void StartLogging(FString id, bool async = false);
	UFUNCTION(BlueprintCallable)
	void AsyncLog();

};
