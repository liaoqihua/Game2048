// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct ChessBoard
{

public:
	ChessBoard()
		:num(16), total(16), IsRemoveing(false), score(0)
	{
		initChessBoard();
	}
	void initChessBoard();
	void randCreatePiece(int &x, int &y);
	void randCreatePiece();
	void Up();
	void Down();
	void Left();
	void Right();
	bool checkDeath();
	void RandomStartGame();
	void SetupData(const ChessBoard &cb);
public:
	int piece[4][4];
	int num;
	int total;
	bool IsRemoveing;
	int score;
};