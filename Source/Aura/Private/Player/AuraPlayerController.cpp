// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true; //important for multiplayer
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext); //returns false if AuraContext is not set 

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>()) {
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y, false);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X, false);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/**
	* Line Trace trace from cursor. There are several scenarios:
	*	A. LastActor is null & ThisActor is null
	*		- Do nothing
	*	B. LastActor is null && ThisActor is valid
	*		- Highlight ThisActor
	*	C. LastActor is valid && ThisActor is null
	*		- UnHighlight LastActor
	*	D. Both actors are valid, but LastActor != ThisActor
	*		-UnHighlight LastActor
	*		-Highlight ThisActor
	*	E. Both actor are valid, and are the same actor
	*		- Do nothing
	*/

	if (!LastActor) {
		if (ThisActor) {
			//Case B
			ThisActor->HighlightActor();
		}
		else {
			//Case A - both are null, do nothing
		}
	}
	else { //LastActor is valid
		if (!ThisActor) {
			//Case C
			LastActor->UnHighlightActor();
		}
		else { // both actors are valid
			if (ThisActor != LastActor) {
				//Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else {
				//Case E - actors are the same, do nothing
			}
		}
	}
}
