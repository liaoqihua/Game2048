// Fill out your copyright notice in the Description page of Project Settings.

#include "Game2048SaveGame.h"


void UGame2048SaveGame::SetupLastData(const FSaveDataStruct &sds)
{
	LastData.piece = sds.piece;
	LastData.score = sds.score;
	LastData.num = sds.num;
	LastData.name = sds.name;
}

void UGame2048SaveGame::SetupRank(const TArray<FSaveDataStruct> &r)
{
	Rank.RemoveAll([](const FSaveDataStruct &sds) -> bool {return true; });

	for (auto &item : r) {
		Rank.Add(item);
	}
}

FSaveDataStruct UGame2048SaveGame::InitDataFromChessBoard(const ChessBoard &cb)
{
	FSaveDataStruct ret;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ret.piece[i * 4 + j] = cb.piece[i][j];
		}
	}
	ret.score = cb.score;
	ret.num = cb.num;

	return ret;
}

ChessBoard UGame2048SaveGame::TransDataToChessBoard(const FSaveDataStruct &sds)
{
	ChessBoard cb;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			cb.piece[i][j] = sds.piece[i * 4 + j];
		}
	}
	cb.score = sds.score;
	cb.num = sds.num;

	return cb;
}
