// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"

void AAuraEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsHighlighted) {
		DrawDebugSphere(GetWorld(), GetActorLocation(), 50.f, 10, FColor::Cyan);
	}
}

void AAuraEnemy::HighlightActor()
{
	bIsHighlighted = true;
}

void AAuraEnemy::UnHighlightActor()
{
	bIsHighlighted = false;
}
