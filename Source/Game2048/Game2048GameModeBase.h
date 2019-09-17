// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <Widget.h>
#include <UserWidget.h>
#include "Kismet/KismetStringLibrary.h"
#include <SWindow.h>
#include <Reply.h>
#include "Game2048SaveGame.h"
#include "Game2048GameModeBase.generated.h"

DECLARE_EVENT(AGame2048GameModeBase, FOnUpKeyPressEvent);
DECLARE_EVENT(AGame2048GameModeBase, FOnDownKeyPressEvent);
DECLARE_EVENT(AGame2048GameModeBase, FOnLeftKeyPressEvent);
DECLARE_EVENT(AGame2048GameModeBase, FOnRightKeyPressEvent);

class UWidgetChessBoard
{
public:
	UWidgetChessBoard(UUserWidget *UserWidget)
	{
		piece[0][0] = UserWidget->GetWidgetFromName("Block11");
		piece[0][1] = UserWidget->GetWidgetFromName("Block12");
		piece[0][2] = UserWidget->GetWidgetFromName("Block13");
		piece[0][3] = UserWidget->GetWidgetFromName("Block14");

		piece[1][0] = UserWidget->GetWidgetFromName("Block21");
		piece[1][1] = UserWidget->GetWidgetFromName("Block22");
		piece[1][2] = UserWidget->GetWidgetFromName("Block23");
		piece[1][3] = UserWidget->GetWidgetFromName("Block24");

		piece[2][0] = UserWidget->GetWidgetFromName("Block31");
		piece[2][1] = UserWidget->GetWidgetFromName("Block32");
		piece[2][2] = UserWidget->GetWidgetFromName("Block33");
		piece[2][3] = UserWidget->GetWidgetFromName("Block34");

		piece[3][0] = UserWidget->GetWidgetFromName("Block41");
		piece[3][1] = UserWidget->GetWidgetFromName("Block42");
		piece[3][2] = UserWidget->GetWidgetFromName("Block43");
		piece[3][3] = UserWidget->GetWidgetFromName("Block44");

		score = UserWidget->GetWidgetFromName("ScoreTextBlock");
	}

public:
	UWidget *piece[4][4];
	UWidget *score;
};

/**
 * 
 */
UCLASS()
class GAME2048_API AGame2048GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGame2048GameModeBase();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void BeginDestroy() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;
public:
	void UpdateOnePiece(int value, UWidget *Widget);
	void UpdateGraph();
	void UpdateScore();
	void PlayAnimation(UUserWidget *widget);
	FLinearColor CreateColor(int value);
	void InitButtonsEvent();

	void LoadData();
	void SaveData();
	void UpdateLastData();

	UFUNCTION()
		void UpKeyHandle();
	UFUNCTION()
		void DownKeyHandle();
	UFUNCTION()
		void LeftKeyHandle();
	UFUNCTION()
		void RightKeyHandle();
	UFUNCTION()
		void OnClickedButton0Callback();
	UFUNCTION()
		void OnClickedButton1Callback();
	UFUNCTION()
		void OnClickedButton2Callback();
	UFUNCTION()
		void OnClickedButton3Callback();
	UFUNCTION()
		void OnClickedButton4Callback();

	FReply OnClickedButton0YesCallback();
	FReply OnClickedButton0NoCallback();

public:
	UPROPERTY(EditAnywhere, Category = "WBPClass")
		TSubclassOf<UUserWidget> MainWidgetClass;
	UPROPERTY(EditAnywhere, Category = "WBPClass")
		TSubclassOf<UUserWidget> BlockWidgetClass;
	UPROPERTY()
		UUserWidget *MainWidgetObject;
	FOnUpKeyPressEvent OnUpKeyPressEvent;
	FOnDownKeyPressEvent OnDownKeyPressEvent;
	FOnLeftKeyPressEvent OnLeftKeyPressEvent;
	FOnRightKeyPressEvent OnRightKeyPressEvent;


	TSharedPtr<ChessBoard> BaseCB;
	TSharedPtr<UWidgetChessBoard> DisplayCB;
	TSharedPtr<UGame2048SaveGame> SaveGamePtr;

	TSharedPtr<SWindow> newGameWindowPtr;
	TSharedPtr<SWindow> RankWindowPtr;
};
