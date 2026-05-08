// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameModes/BlasterGameMode.h"
#include "Blaster/HUD/Announcement.h"
#include "Blaster/HUD/BlasterHud.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/HUD/Announcement.h"
#include "Components/ProgressBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameStates/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	
	BlasterHud = Cast<ABlasterHud>(GetHUD());
	ServerCheckMatchState();
	
}



void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();

	CheckTimeSync(DeltaSeconds);

	PollInit();
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController,MatchState)
}


void ABlasterPlayerController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(PossessedPawn);
	if (BlasterCharacter)
	{
		SetHudHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
	}
}


void ABlasterPlayerController::SetHudHealth(float Health, float MaxHealth)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->HealthBar && BlasterHud->CharacterOverlay->HealthText)
	{
		const float HealthPercentage = Health / MaxHealth;
		BlasterHud->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);

		FString HealthText =  FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Health),FMath::CeilToInt(MaxHealth));
		BlasterHud->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->ScoreAmount )
	{
		FString ScoreText =  FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
		BlasterHud->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->DefeatsAmount )
	{
		FString DefeatsText =  FString::Printf(TEXT("%d"),Defeats);
		BlasterHud->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
		ClearHUDIcon();
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->WeaponAmmoAmount )
	{
		FString AmmoText =  FString::Printf(TEXT("%d"),Ammo);
		BlasterHud->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->CarriedAmmoAmount )
	{
		FString AmmoText =  FString::Printf(TEXT("%d"),Ammo);
		BlasterHud->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDWeaponIcon(UTexture2D* Icon)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->WeaponTypeImage )
	{
		BlasterHud->CharacterOverlay->WeaponTypeImage->SetBrushFromTexture(Icon,true);
		BlasterHud->CharacterOverlay->WeaponTypeImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float MatchCountdownTime)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->MatchCountdownText )
	{
		if (MatchCountdownTime <0.f)
		{
			BlasterHud->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(MatchCountdownTime / 60.f);
		int32 Seconds = MatchCountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHud->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float AnnouncementCountdownTime)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->Announcement && BlasterHud->Announcement->WarmupTime )
	{
		if (AnnouncementCountdownTime <0.f)
		{
			BlasterHud->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(AnnouncementCountdownTime / 60.f);
		int32 Seconds = AnnouncementCountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHud->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::ClearHUDIcon()
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->WeaponTypeImage )
	{
		
		BlasterHud->CharacterOverlay->WeaponTypeImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = WarmupTime + MatchTime + CooldownTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			 SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime());
		}
	}
	
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart ||MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	

	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHud && BlasterHud->CharacterOverlay)
		{
			CharacterOverlay = BlasterHud->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHudHealth(HUDHealth,HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}


float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	
	
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}



void ABlasterPlayerController::OnRep_MatchState()
{
	
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}
void ABlasterPlayerController::HandleMatchStarted()
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if (BlasterHud)
	{
		if (BlasterHud->CharacterOverlay == nullptr) BlasterHud->AddCharacterOverlay();
		if (BlasterHud->Announcement)
		{
			BlasterHud->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{

	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if (BlasterHud)
	{
		BlasterHud->CharacterOverlay->RemoveFromParent();
		if (BlasterHud->Announcement)
		{
			BlasterHud->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("Game Over");
			BlasterHud->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("No winners this time");
				}
				else if (TopPlayers.Num()==1 && TopPlayers[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You are the winner");
				}
				else if (TopPlayers.Num()==1)
				{
					InfoTextString = FString::Printf(TEXT("%s is the winner"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num()>1)
				{
					InfoTextString = FString("Draw \n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"),*TiedPlayer->GetPlayerName()));
					}
				}
				BlasterHud->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			
			
		}
	}

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter)
	{
		BlasterCharacter->bDisableGameplay=true;
		if (BlasterCharacter->GetCombat())
		{
			BlasterCharacter->GetCombat()->FireButtonPressed(false);
		}
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest,ServerTimeOfReceipt);
	
}
void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5 * RoundTripTime);

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}



void ABlasterPlayerController::CheckTimeSync(float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}



void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		MatchState = GameMode->GetMatchState();
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		ClientJoinMidGame(MatchState,WarmupTime,MatchTime,CooldownTime,LevelStartingTime);
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match,float Cooldown, float StartingTime)
{
	MatchState = StateOfMatch;
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;

	OnMatchStateSet(MatchState);

	if (BlasterHud && MatchState == MatchState::WaitingToStart)
	{
		BlasterHud->AddAnnouncement();
	}
}