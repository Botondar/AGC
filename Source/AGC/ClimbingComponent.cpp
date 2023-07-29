// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"

#include "AGCCharacter.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/CapsuleComponent.h>

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();

    CurrentHangingTime = 0.0f;

    bIsHanging = false;
    bIsTryingToClimb = false;

    Char = GetOwner<AAGCCharacter>();
    if (!Char)
    {
        UE_LOG(LogTemp, Error, TEXT("Owning actor is not AAGCCharacter"));
    }
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const float CapsuleHalfHeight = Char->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
    const float CapsuleRadius = Char->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	if (bIsTryingToClimb)
    {
        if (bIsHanging)
        {
            // Climb up onto grabbed ledge
            FVector TeleportTarget = HangLedgeLocation + FVector(0.0f, 0.0f, CapsuleHalfHeight);
            Char->TeleportTo(TeleportTarget, HangActorTargetRotation);

            Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

            bIsHanging = false;
        }
        else
        {
            // Find ledge and grab onto it
            FHitResult WallHit, LedgeHit;
            if (TraceForLedge(WallHit, LedgeHit))
            {
                HangLedgeLocation = LedgeHit.ImpactPoint;
                HangWallNormal = WallHit.ImpactNormal;

                HangActorStartLocation = Char->GetActorLocation();
                HangActorStartRotation = Char->GetActorRotation();

                FVector HangZOffset = FVector(0.0f, 0.0f, 30.0f - CapsuleHalfHeight);
                FVector HangWallOffset = HangWallNormal * (CapsuleRadius + 5.0f);
                HangActorTargetLocation = HangLedgeLocation + HangZOffset + HangWallOffset;

                float Yaw, Pitch;
                UKismetMathLibrary::GetYawPitchFromVector(HangWallNormal, Yaw, Pitch);
                HangActorTargetRotation = FRotator(0.0f, Yaw + 180.0f, 0.0f);

                Char->GetCharacterMovement()->StopMovementImmediately();
                Char->GetCharacterMovement()->DisableMovement();

                bIsHanging = true;

                // Turn off trying to climb here so that we don't immediately climb up the ledge
                bIsTryingToClimb = false;

                CurrentHangingTime = 0.0f;
            }
        }
    }

    if (bIsHanging)
    {
        MoveActorToHangTarget(DeltaTime);
    }
}

void UClimbingComponent::MoveActorToHangTarget(float dt)
{
    CurrentHangingTime += dt;

    const float Alpha = FMath::Clamp(CurrentHangingTime / HangAttachTime, 0.0f, 1.0f);

    FVector NewLocation = FMath::Lerp(HangActorStartLocation, HangActorTargetLocation, Alpha);
    FRotator NewRotation = FMath::Lerp(HangActorStartRotation, HangActorTargetRotation, Alpha);

    Char->SetActorLocationAndRotation(NewLocation, NewRotation);
}

void UClimbingComponent::StartClimbing()
{
    bIsTryingToClimb = true;
}

void UClimbingComponent::StopClimbing()
{
    bIsTryingToClimb = false;
}

void UClimbingComponent::LetGoOfLedge()
{
    if (bIsHanging)
    {
        Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        bIsHanging = false;
    }
}

bool UClimbingComponent::TraceForLedge(FHitResult& WallHit, FHitResult& LedgeHit, bool bEnableDebug /*= false*/)
{
    bool bResult = false;

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Cast<AActor>(Char));

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

    const FVector ActorP = Char->GetActorLocation() + FVector(0.0f, 0.0f, Char->BaseEyeHeight);
    const FVector ActorV = Char->GetActorForwardVector();
    
    constexpr float TraceLength = 50.0f;
    FVector Start = ActorP;
    FVector End = Start + TraceLength * ActorV;
    constexpr float Radius = 10.0f;
    
    EDrawDebugTrace::Type TraceMode = bEnableDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;

    if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),
        Start, End, Radius,
        ObjectTypes, false, 
        ActorsToIgnore,
        TraceMode,
        WallHit,
        true,
        FLinearColor(0.0f, 1.0f, 0.0f, 1.0f),
        FLinearColor(1.0f, 0.0f, 0.0f, 1.0f),
        1.0f))
    {
        FVector ClimbableEnd = WallHit.ImpactPoint;
        FVector ClimbableStart = 
            ClimbableEnd + 
            FVector(0.0f, 0.0f, Char->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()) +
            FVector(0.0f, 0.0f, 2.0f * Char->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 10.0f);

        if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),
            ClimbableStart, ClimbableEnd, Radius,
            ObjectTypes, false, 
            ActorsToIgnore,
            TraceMode,
            LedgeHit,
            true,
            FLinearColor(0.0f, 1.0f, 0.0f, 1.0f),
            FLinearColor(1.0f, 0.0f, 0.0f, 1.0f),
            1.0f))
        {
            if ((LedgeHit.ImpactPoint.Z - ActorP.Z) <= ClimbHeight)
            {
                bResult = true;
            }
        }
        
    }

    return bResult;
}