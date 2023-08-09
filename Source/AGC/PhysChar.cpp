// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysChar.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APhysChar::APhysChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    check(SpringArm);
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bEnableCameraLag = true;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    check(Camera);
    Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);
    //Camera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APhysChar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    float Time = GetGameTimeSinceCreation();

    WalkTime = 2.0f * PI * GetGameTimeSinceCreation();
    LeftFootP = FVector(0.0f, 20.0f, 20.0f * FMath::Abs(FMath::Sin(WalkTime)));
    RightFootP = FVector(0.0f, -10.0f, 20.0f * FMath::Abs(FMath::Sin(WalkTime + 0.5f*PI)));

}

FVector APhysChar::GetLeftFootP() const
{
    return LeftFootP;
}

FVector APhysChar::GetRightFootP() const
{
    return RightFootP;
}

void APhysChar::MoveForward(float Value)
{
    AddMovementInput(Value * GetActorForwardVector());
}
void APhysChar::MoveRight(float Value)
{
    AddMovementInput(Value * GetActorRightVector());
}
void APhysChar::Turn(float Value)
{
    AddControllerYawInput(Value);
}
void APhysChar::LookUp(float Value)
{
    AddControllerPitchInput(-Value);
}

// Called to bind functionality to input
void APhysChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APhysChar::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APhysChar::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APhysChar::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APhysChar::LookUp);
}

