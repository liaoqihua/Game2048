// Fill out your copyright notice in the Description page of Project Settings.

#include "Game2048PlayerCharacter.h"
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/PlayerInput.h>


// Sets default values
AGame2048PlayerCharacter::AGame2048PlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGame2048PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase *ModeBase = GetWorld()->GetAuthGameMode();
	if (ModeBase && ModeBase->GetClass() == AGame2048GameModeBase::StaticClass()) {
		GameModeBase = Cast<AGame2048GameModeBase>(ModeBase);
	}
}

// Called every frame
void AGame2048PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGame2048PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	FInputActionKeyMapping UpKey("Up", EKeys::Up);
	FInputActionKeyMapping DownKey("Down", EKeys::Down);
	FInputActionKeyMapping LeftKey("Left", EKeys::Left);
	FInputActionKeyMapping RightKey("Right", EKeys::Right);
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(UpKey);
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(DownKey);
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(LeftKey);
	GetWorld()->GetFirstPlayerController()->PlayerInput->AddActionMapping(RightKey);

	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &AGame2048PlayerCharacter::OnUpKeyPressCallback);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &AGame2048PlayerCharacter::OnDownKeyPressCallback);
	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &AGame2048PlayerCharacter::OnLeftKeyPressCallback);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &AGame2048PlayerCharacter::OnRightKeyPressCallback);
}

void AGame2048PlayerCharacter::OnUpKeyPressCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnUpKeyPressCallback"));
	if (GameModeBase) {
		GameModeBase->OnUpKeyPressEvent.Broadcast();
	}
}

void AGame2048PlayerCharacter::OnDownKeyPressCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDownKeyPressCallback"));
	if (GameModeBase) {
		GameModeBase->OnDownKeyPressEvent.Broadcast();
	}
}

void AGame2048PlayerCharacter::OnLeftKeyPressCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnLeftKeyPressCallback"));
	if (GameModeBase) {
		GameModeBase->OnLeftKeyPressEvent.Broadcast();
	}
}

void AGame2048PlayerCharacter::OnRightKeyPressCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRightKeyPressCallback"));
	if (GameModeBase) {
		GameModeBase->OnRightKeyPressEvent.Broadcast();
	}
}

