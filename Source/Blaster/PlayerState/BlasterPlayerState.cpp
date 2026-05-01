// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
		
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if (BlasterPlayerController)
		{
			BlasterPlayerController->SetHudScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore()+ScoreAmount) ;

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if (BlasterPlayerController)
		{
			BlasterPlayerController->SetHudScore(GetScore());
		}
	}
}


void ABlasterPlayerState::OnRep_Defeats()
{
	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if (BlasterPlayerController)
		{
			BlasterPlayerController->SetHudDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if (BlasterPlayerController)
		{
			BlasterPlayerController->SetHudDefeats(Defeats);
		}
	}
}

