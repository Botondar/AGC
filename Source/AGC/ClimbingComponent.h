// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGC_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbingComponent();

protected:
    // Owning character
    class AAGCCharacter* Char;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ClimbHeight = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float HangAttachTime = 0.2f;

    float CurrentHangingTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsHanging;

    FVector HangLedgeLocation;
    FVector HangWallNormal;

    FVector HangActorStartLocation;
    FRotator HangActorStartRotation;

    FVector HangActorTargetLocation;
    FRotator HangActorTargetRotation;

    bool bIsTryingToClimb;

    void MoveActorToHangTarget(float dt);

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    bool TraceForLedge(FHitResult& WallHit, FHitResult& LedgeHit, bool bEnableDebug = false);

    void StartClimbing();
    void StopClimbing();

    void LetGoOfLedge();

    bool GetIsHanging() const { return bIsHanging; }
};
