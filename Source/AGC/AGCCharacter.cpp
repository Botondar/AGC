// Fill out your copyright notice in the Description page of Project Settings.


#include "AGCCharacter.h"

#include "Interactable.h"

#include <GameFramework/CharacterMovementComponent.h>
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AAGCCharacter::AAGCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->AirControl = 0.6f;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 300.0f;
    SpringArmComponent->bEnableCameraLag = true;
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform);

    ClimbingComponent = CreateDefaultSubobject<UClimbingComponent>(TEXT("ClimbingComponent"));
    check(ClimbingComponent != nullptr);
    ClimbingComponent->RegisterComponent();

    ControlRigComponent = CreateDefaultSubobject<UControlRigComponent>(TEXT("ControlRig"));
    check(ControlRigComponent != nullptr);
    ControlRigComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAGCCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAGCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    float TraceDistance = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 40.0f;
    FName LeftFootSocket = GetMesh()->GetSocketBoneName(TEXT("foot_l"));
    FName RightFootSocket = GetMesh()->GetSocketBoneName(TEXT("foot_r"));

    IKFootTrace(TraceDistance, LeftFootSocket, IKLeftFootLocation, IKLeftFootNormal);
    IKFootTrace(TraceDistance, RightFootSocket, IKRightFootLocation, IKRightFootNormal);

    // Update torso rotation
    {
        FRotator MeshRotation = GetMesh()->GetComponentRotation();
        FRotator MeshRelativeRotation = GetMesh()->GetRelativeRotation();
        FRotator ControlRotation = GetControlRotation();

        // Global rotation of the controller in the space of the mesh
        float Yaw = -MeshRelativeRotation.Yaw - ControlRotation.Yaw + MeshRotation.Yaw;
        float Pitch = MeshRelativeRotation.Pitch - ControlRotation.Pitch;
        
        // NOTE(boti): swizzle is intentional, this seems to be the correct order for the control rig space
        HeadRotation = FRotator(0.0f, FMath::ClampAngle(Pitch, -60.0f, +60.0f), FMath::ClampAngle(Yaw, -85.0f, +85.0f));
        TorsoRotation = HeadRotation;
    }
}

void AAGCCharacter::IKFootTrace(float TraceDistance, FName Socket, FVector& Offset, FVector& Normal)
{
    Offset = FVector(0.0f, 0.0f, 0.0f);
    Normal = FVector(0.0f, 0.0f, 1.0f);

    FVector RootLocation = GetMesh()->GetSocketLocation(FName(TEXT("pelvis")));
    FVector SocketLocation = GetMesh()->GetSocketLocation(Socket);
    FVector ActorLocation = GetActorLocation();
    
    FVector TraceStart = FVector(SocketLocation.X, SocketLocation.Y, ActorLocation.Z);
    FVector TraceEnd = FVector(SocketLocation.X, SocketLocation.Y, ActorLocation.Z - TraceDistance);

    constexpr float TraceRadius = 15.0f;

    TArray<AActor*> IgnoredActors;
    FHitResult HitResult;
    if (UKismetSystemLibrary::SphereTraceSingle(
            GetWorld(),
            TraceStart, TraceEnd, TraceRadius,
            UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, 
            IgnoredActors,
            EDrawDebugTrace::None,
            HitResult,
            true,
            FLinearColor(0.0f, 1.0f, 0.0f, 1.0f),
            FLinearColor(1.0f, 0.0f, 1.0f, 1.0f)))
    {
        constexpr float NormalOffset = TraceRadius;
        Normal = HitResult.ImpactNormal;
        Offset = HitResult.Location - SocketLocation;
    }
}

void AAGCCharacter::LookUp(float Value)
{
    AddControllerPitchInput(-Value);
}

void AAGCCharacter::MoveForward(float Value)
{
    FVector Forward = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
    AddMovementInput(Value * Forward);
}

void AAGCCharacter::MoveRight(float Value)
{
    FVector Right = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
    AddMovementInput(10.0f * Value * Right);
}

void AAGCCharacter::ToggleCrouch()
{
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Crouch toggle"));
    if (ACharacter::bIsCrouched)
    {
        ACharacter::UnCrouch();
    }
    else
    {
        GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
        ACharacter::Crouch();
    }
}

void AAGCCharacter::StartRunning()
{
    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AAGCCharacter::StopRunning()
{
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AAGCCharacter::StartJump()
{
    ACharacter::UnCrouch();
    
    Super::Jump();
    ClimbingComponent->StartClimbing();
}

void AAGCCharacter::StopJump()
{
    Super::StopJumping();
    ClimbingComponent->StopClimbing();
}

void AAGCCharacter::Interact()
{
    if (ClimbingComponent->GetIsHanging())
    {
        ClimbingComponent->LetGoOfLedge();
    }
    else
    {
        FHitResult InteractableResult;
        if (TraceForInteractables(InteractableResult))
        {
            AInteractable* Interactable = Cast<AInteractable>(InteractableResult.GetActor());
            if (Interactable)
            {
                Interactable->InteractWith(this);
            }
        }
    }
}

void AAGCCharacter::StartAttacking()
{
}

void AAGCCharacter::StopAttacking()
{
}

void AAGCCharacter::StartAiming()
{
    bIsAiming = true;

    SpringArmComponent->SetRelativeLocation(FVector(40.0f, 0.0f, 0.0f));
}

void AAGCCharacter::StopAiming()
{
    bIsAiming = false;

    SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

// Called to bind functionality to input
void AAGCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AAGCCharacter::LookUp);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AAGCCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AAGCCharacter::MoveRight);

    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AAGCCharacter::StartJump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AAGCCharacter::StopJump);

    PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AAGCCharacter::StartRunning);
    PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AAGCCharacter::StopRunning);

    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AAGCCharacter::Interact);

    PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AAGCCharacter::StartAttacking);
    PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &AAGCCharacter::StopAttacking);

    PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &AAGCCharacter::StartAiming);
    PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AAGCCharacter::StopAiming);

    PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AAGCCharacter::ToggleCrouch);
}

bool AAGCCharacter::TraceForInteractables(FHitResult& HitResult)
{
    const float TraceLength = 300.0f;

    FVector Direction = CameraComponent->GetForwardVector();
    FVector Start = CameraComponent->GetComponentLocation() + SpringArmComponent->TargetArmLength * Direction;
    FVector End = Start + TraceLength * Direction;

    TArray<AActor*> ActorsToIgnore;

    bool Result = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, 
        UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, 
        EDrawDebugTrace::None,
        HitResult, true);

    return Result;
}