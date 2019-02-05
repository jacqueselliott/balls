// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ballsBall.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"

AballsBall::AballsBall()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMesh(TEXT("/Game/Rolling/Meshes/BallMesh.BallMesh"));

	// Create mesh component for the ball
	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball0"));
	Ball->SetStaticMesh(BallMesh.Object);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);
	Ball->SetAngularDamping(0.1f);
	Ball->SetLinearDamping(0.1f);
	Ball->BodyInstance.MassScale = 3.5f;
	Ball->BodyInstance.MaxAngularVelocity = 800.0f;
	Ball->SetNotifyRigidBodyCollision(true);
	RootComponent = Ball;

	// Create a camera boom attached to the root (ball)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bAbsoluteRotation = true; // Rotation of the ball should not affect rotation of boom
	SpringArm->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArm->TargetArmLength = 200.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 3.f;

	// TODO: hack
	SpringArmPtr = SpringArm;

	// Create a camera and attach to boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Set up forces
	RollTorque = 50000000.0f;
	JumpImpulse = 350000.0f;
	bCanJump = true; // Start being able to jump
}


void AballsBall::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAxis("MoveRight", this, &AballsBall::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AballsBall::MoveForward);
	PlayerInputComponent->BindAxis("CameraYaw", this, &AballsBall::CameraYaw);
	PlayerInputComponent->BindAxis("CameraPitch", this, &AballsBall::CameraPitch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AballsBall::Jump);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AballsBall::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AballsBall::TouchStopped);
	
}

void AballsBall::CameraYaw(float Val)
{
	SpringArmPtr->AddRelativeRotation(FRotator(0.f, Val, 0.f));
}

void AballsBall::CameraPitch(float Val)
{
	SpringArmPtr->AddRelativeRotation(FRotator(Val, 0.f, 0.f));
}

void AballsBall::MoveRight(float Val)
{
	FVector forwardVector = SpringArmPtr->GetForwardVector();
	const FVector Torque = FVector(-1.f * forwardVector.X * Val * RollTorque, -1.f * forwardVector.Y * Val * RollTorque, 0.f);
	Ball->AddTorqueInRadians(Torque);
}

void AballsBall::MoveForward(float Val)
{
	FVector rightVector = SpringArmPtr->GetRightVector();
	const FVector Torque = FVector(rightVector.X * Val * RollTorque, rightVector.Y * Val * RollTorque, 0.f);
	Ball->AddTorqueInRadians(Torque);	
}

void AballsBall::Jump()
{
	if(bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}
}

void AballsBall::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	bCanJump = true;
}

void AballsBall::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}

}

void AballsBall::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}
}

