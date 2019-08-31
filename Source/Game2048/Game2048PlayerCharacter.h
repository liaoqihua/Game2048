// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Game2048GameModeBase.h"
#include "Game2048PlayerCharacter.generated.h"

UCLASS()
class GAME2048_API AGame2048PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGame2048PlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION()
		void OnUpKeyPressCallback();
	UFUNCTION()
		void OnDownKeyPressCallback();
	UFUNCTION()
		void OnLeftKeyPressCallback();
	UFUNCTION()
		void OnRightKeyPressCallback();

public:
	UPROPERTY()
		AGame2048GameModeBase *GameModeBase;
};
