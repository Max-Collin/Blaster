// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"),*Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	APlayerState* PlayerState = InPawn->GetPlayerState();
	FString PlayerName;
	if(PlayerState == nullptr)
	{
		PlayerName = FString::Printf(TEXT("Unkown"));
	}
	else
	{
		PlayerName =FString::Printf(TEXT("%s"),*PlayerState->GetPlayerName()) ;
	}
	SetDisplayText(PlayerName);
}
