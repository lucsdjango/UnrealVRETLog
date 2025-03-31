// Fill out your copyright notice in the Description page of Project Settings.


#include "VRLogger.h"

// Sets default values
AVRLogger::AVRLogger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Labels = { TEXT("Valid"),TEXT("GazeOrigin"), TEXT("GazeDir"), TEXT("Conf"), TEXT("FixPoint"), TEXT("LBlink"), TEXT("RBlink"), TEXT("LPupil"), TEXT("RPupil"), TEXT("LOrigin"),TEXT("LDir"),TEXT("ROrigin"),TEXT("RDir"),TEXT("HitPos"),TEXT("HitName") };
}

// Called when the game starts or when spawned
void AVRLogger::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->FixedFrameRate = 90.0;
	//GEngine->bUseFixedFrameRate = true;
	
}

// Called every frame
void AVRLogger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRLogger::StartLogging(FString id)
{
	Logger->InitOrReset();
	Logger->AddEntry("FPS");
	for (FString label : Labels) {
		Logger->AddEntry(label);
	}

	//Logger->AddEntry("GazeOrigin", "sdfdsf", false);
	//Logger->AddEntry("GazeDir", "sdfdsf", true);

	Logger->StartLogging(id, false);
}


void AVRLogger::SetDebugShape(UStaticMeshComponent* shape)
{
	DebugShape = shape;
}



void AVRLogger::UpdateETData(float deltaTime, FVector origin, FVector dir, float conf, FVector fixPoint, bool lBlink, bool rBlink, float lPupil, float rPupil, FVector lOrigin, FVector lDir, FVector rOrigin, FVector rDir, bool valid)
{
	Logger->UpdateFloat("FPS", 1.0f / deltaTime);

	if (!valid)
	{
		Logger->UpdateEntry("Valid", "F");
	}
	else
	{

		Logger->UpdateEntry("Valid", "T");
		Logger->UpdateVector("GazeOrigin", origin);
		Logger->UpdateVector("GazeDir", dir);
		Logger->UpdateFloat("Conf", conf);
		Logger->UpdateVector("FixPoint", fixPoint);
		Logger->UpdateEntry("LBlink", (lBlink ? "T" : "F"));
		Logger->UpdateEntry("RBlink", (rBlink ? "T" : "F"));
		Logger->UpdateFloat("LPupil", lPupil);
		Logger->UpdateFloat("RPupil", rPupil);
		Logger->UpdateVector("LOrigin", lOrigin);
		Logger->UpdateVector("LDir", lDir);
		Logger->UpdateVector("ROrigin", rOrigin);
		Logger->UpdateVector("RDir", rDir);

		CalculateAndUpdateHits(origin, dir, "");

		/*
		DebugShape->SetWorldLocation(origin + dir * 20.0f);
		
		TArray<FHitResult> Hits = {};
		//FHitResult HitData(ForceInit);
		//FCollisionObjectQueryParams ObjectList;
		

		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("ETTest")), false, this);

		GetWorld()->LineTraceMultiByChannel(Hits, origin, origin+dir*10000.0f, ECollisionChannel::ECC_Visibility, TraceParams);
		


		UE_LOG(LogTemp, Warning, TEXT("MyTrace Num Hits: %d"), Hits.Num());
		for (FHitResult& HitData : Hits)
		{

			if (HitData.GetActor()) // pointer check
			{
				if (HitData.GetActor()->Tags.Contains("ETTarget"))
				{
					UE_LOG(LogTemp, Warning, TEXT("ETTarget"));
				}
				//AEnemy* enemy = Cast<AEnemy>(HitData.GetActor()); // Type Check

			}
		}

		*/
	}
}

void AVRLogger::UpdateHits(FVector pos, FString name)
{
	Logger->UpdateVector("HitPos", pos);
	Logger->UpdateEntry("HitName", name);
}

void AVRLogger::CalculateAndUpdateHits(FVector pos, FVector dir, FString filterTag)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("ETTest")), false, this);
	TArray<FHitResult> Hits;
	FString HitsString = "";


	GetWorld()->LineTraceMultiByChannel(Hits, pos, dir * 9999999.9f, ECollisionChannel::ECC_Visibility, TraceParams);
	FVector FirstHitPos = FVector::Zero();

	// Collision responses -> visibility should be set to overlap not to block objects behind. TODO: Does not work with movable objects. 
	for (auto& Hit : Hits)
	{

		if (Hit.GetActor())
		{
			if (FirstHitPos.IsZero())
			{
				FirstHitPos = Hit.Location;
			}
			else
			{
				HitsString.Append(", ");
			}
			
			//UE_LOG(LogTemp, Warning, TEXT(" These are the hits : %s"), *Hit.GetActor()->GetName());
			HitsString.Append(*Hit.GetActor()->GetName());
			//TraceParams.AddIgnoredActor(Hit.GetActor());

		}
	}
	DebugShape->SetWorldLocation(FirstHitPos);
	UpdateHits(FirstHitPos, HitsString);
}

