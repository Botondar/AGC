// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "ControlRig/Public/ControlRigComponent.h"
#include "Camera/CameraComponent.h"
#include "ClimbingComponent.h"
#include "AGCCharacter.generated.h"

UCLASS()
class AGC_API AAGCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAGCCharacter();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UControlRigComponent* ControlRigComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
    // Movement
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float RunSpeed = 600.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsRunning = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsAiming = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UClimbingComponent* ClimbingComponent;

    void LookUp(float Value);

    void MoveForward(float Value);
    void MoveRight(float Value);

    void ToggleCrouch();

    void StartRunning();
    void StopRunning();

    void StartJump();
    void StopJump();

    void Interact();

    void StartAttacking();
    void StopAttacking();

    void StartAiming();
    void StopAiming();

public:

    bool TraceForInteractables(FHitResult& HitResult);

public:
    UPROPERTY(BlueprintReadOnly)
    FRotator TorsoRotation;

    UPROPERTY(BlueprintReadOnly)
    FRotator HeadRotation;

    UPROPERTY(BlueprintReadOnly)
    FVector IKLeftFootLocation = FVector(0.0f);

    UPROPERTY(BlueprintReadOnly)
    FVector IKLeftFootNormal;

    UPROPERTY(BlueprintReadOnly)
    FVector IKRightFootLocation = FVector(0.0f);

    UPROPERTY(BlueprintReadOnly)
    FVector IKRightFootNormal;

    UFUNCTION(BlueprintCallable)
    void IKFootTrace(float TraceDistance, FName Socket, FVector& Offset, FVector& Normal);
public:	

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
