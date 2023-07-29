// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "PhysChar.generated.h"

UCLASS()
class AGC_API APhysChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APhysChar();

protected:
    
    UPROPERTY(EditAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(EditAnywhere)
    UCameraComponent* Camera;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void MoveForward(float Value);
    void MoveRight(float Value);

    void Turn(float Value);
    void LookUp(float Value);

protected:
    //
    // Animation states
    //

    float WalkTime = 0.0f;

    FVector LeftFootP;
    FVector RightFootP;
public:
    UFUNCTION(BlueprintCallable)
    FVector GetLeftFootP() const;

    UFUNCTION(BlueprintCallable)
    FVector GetRightFootP() const;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
