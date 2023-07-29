// Fill out your copyright notice in the Description page of Project Settings.


#include "LerpActor.h"

// Sets default values
ALerpActor::ALerpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALerpActor::BeginPlay()
{
	Super::BeginPlay();

    StartLocation = GetActorLocation();
    TargetLocation = StartLocation + FVector(300.0f, 0.0f, 0.0f);
}

// Called every frame
void ALerpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = CurrentLocation;

    float t = 0.0f;

    if (bUseLerpTowards)
    {
        if ((TargetLocation - CurrentLocation).Size() > 1e-1f)
        {
            NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, 1.0f - FMath::Exp(-10.0f * DeltaTime));
        }
        else
        {
            FVector Temp = TargetLocation;
            TargetLocation = StartLocation;
            StartLocation = Temp;
        }
    }
    else
    {
        float MaxDistance = (TargetLocation - StartLocation).Size();
        float CurrentDistance = (TargetLocation - CurrentLocation).Size();

        float dt = DeltaTime * (Speed / MaxDistance);
        t = (MaxDistance - CurrentDistance) / MaxDistance;
        if (t + dt > 1.0f)
        {
            float dtBack = t - dt;
            t = 1.0f - dtBack;

            FVector Temp = TargetLocation;
            TargetLocation = StartLocation;
            StartLocation = Temp;
        }

        t += dt;

        t = FMath::Clamp(t, 0.0f, 1.0f);

        NewLocation = FMath::Lerp(StartLocation, TargetLocation, t);
    }

    SetActorLocation(NewLocation, true);
}

