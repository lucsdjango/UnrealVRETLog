// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TSVLogger.h"
#include "VRLogger.generated.h"

UCLASS()
class VRSTART_API AVRLogger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRLogger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UStaticMeshComponent* DebugShape;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void StartLogging(FString id);
	UFUNCTION(BlueprintCallable)
	void SetDebugShape(UStaticMeshComponent* shape);
	UFUNCTION(BlueprintCallable)
	void UpdateETData(float deltaTime, FVector origin, FVector dir, float conf, FVector fixPoint, bool lBlink, bool rBlink, float lPupil, float rPupil, FVector lOrigin, FVector lDir, FVector rOrigin, FVector rDir, bool valid);
	UFUNCTION(BlueprintCallable)
	void UpdateHits(FVector pos, FString name);
	UFUNCTION(BlueprintCallable)
	void CalculateAndUpdateHits(FVector pos, FVector dir, FString filterTag);
	//void UpdateETValues(Vector GazeOrigin);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATSVLogger* Logger;
	TArray<FString> Labels;
	

};
