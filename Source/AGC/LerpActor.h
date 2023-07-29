// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LerpActor.generated.h"

UCLASS()
class AGC_API ALerpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALerpActor();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseLerpTowards = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    float Speed = 50.0f;

    FVector StartLocation;
    FVector TargetLocation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
