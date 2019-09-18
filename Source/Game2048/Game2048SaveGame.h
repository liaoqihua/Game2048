// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ChessBoard.h"
#include "Game2048SaveGame.generated.h"

USTRUCT()
struct FSaveDataStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<int> piece;
	UPROPERTY()
		int num;
	UPROPERTY()
		int score;
	UPROPERTY()
		FString name;
	UPROPERTY()
		int cancelNum;
	UPROPERTY()
		int rawScore;

	FSaveDataStruct() {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				piece.Add(-1);
			}
		}
		num = 16;
		score = 0;
		cancelNum = 0;
		rawScore = 0;
	}
};

/**
 * 
 */
UCLASS()
class GAME2048_API UGame2048SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UGame2048SaveGame() {}
	UGame2048SaveGame(const FSaveDataStruct &ds, const TArray<FSaveDataStruct> &r)
		:LastData(ds), Rank(r), bIsSaving(false)
	{
	}

public:
	UPROPERTY()
		FSaveDataStruct LastData;
	UPROPERTY()
		TArray<FSaveDataStruct> Rank;
	UPROPERTY()
		bool bIsSaving;
public:
	void SetupLastData(const FSaveDataStruct &sds);
	void SetupRank(const TArray<FSaveDataStruct> &r);
	static FSaveDataStruct InitDataFromChessBoard(const ChessBoard &cb);
	static ChessBoard TransDataToChessBoard(const FSaveDataStruct &sds);
};
