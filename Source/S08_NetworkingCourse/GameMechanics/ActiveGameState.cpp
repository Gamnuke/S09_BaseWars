// Fill out your copyright notice in the Description page of Project Settings.

#include "ActiveGameState.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

AActiveGameState::AActiveGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bReplicates = true;
}
void AActiveGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AActiveGameState, RoundData);
	DOREPLIFETIME(AActiveGameState, Points);
}
void AActiveGameState::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (HasAuthority()) { 
		APlayingGameMode *GM = Cast<APlayingGameMode>(GetWorld()->GetAuthGameMode());
		RoundData = GM->RoundData;
	}
	GetGameInstance()->GetEngine()->AddOnScreenDebugMessage(10, 100, FColor::Blue, FString("Enemies Left: ") + FString::FromInt(RoundData.EnemiesLeft));
}

