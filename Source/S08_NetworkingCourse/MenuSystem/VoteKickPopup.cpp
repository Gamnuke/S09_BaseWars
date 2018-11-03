// Fill out your copyright notice in the Description page of Project Settings.

#include "VoteKickPopup.h"

void UVoteKickPopup::RecievePlayerInput(bool Input) {

}

void UVoteKickPopup::SetInformation(FKickInformation Info) {
	KickingPlayer = Info.KickingPlayerController;
	if (Player != nullptr) {
		Player->SetText(Info.PlayerName);
	}
	if (Reason != nullptr) {
		Reason->SetText(Info.Reason);
	}
}



