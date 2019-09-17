// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessBoard.h"

void ChessBoard::initChessBoard()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			piece[i][j] = -1;
		}
	}
	num = 16;
	score = 0;
}


void ChessBoard::randCreatePiece(int &x, int &y)
{
	//随机在空位生成一个方块,(x,y)是方块的生成位置,(-1,-1)表示生成失败
	//UE_LOG(LogTemp, Warning, TEXT("randCreatePiece:Num:%d"), num);
	if (num) {
		int currentValue = rand() % num;
		int temp = 0;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (piece[i][j] == -1) {
					temp++;
					//UE_LOG(LogTemp, Warning, TEXT("randCreatePiece:row:%d,col:%d"), i, j);
				}
				if (temp == currentValue + 1) {
					piece[i][j] = rand() % 2 ? 2 : 4;
					x = i;
					y = j;
					num--;
					//UE_LOG(LogTemp, Warning, TEXT("randCreatePiece:CurrentNum:%d"), temp);
					return;
				}
			}
		}
	}
}

void ChessBoard::randCreatePiece()
{
	//无参数形式的版本是用于随机开始游戏的版本
	if (num) {
		int currentValue = rand() % num;
		int temp = 0;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (piece[i][j] == -1) {
					temp++;
					//UE_LOG(LogTemp, Warning, TEXT("randCreatePiece:row:%d,col:%d"), i, j);
				}
				if (temp == currentValue + 1) {
					int randNum = rand() % 1000 + 1;
					if (randNum > 999) {
						piece[i][j] = 512;
					}
					else if (randNum > 997) {
						piece[i][j] = 256;
					}
					else if (randNum > 993) {
						piece[i][j] = 128;
					}
					else if (randNum > 985) {
						piece[i][j] = 64;
					}
					else if (randNum > 969) {
						piece[i][j] = 32;
					}
					else if (randNum > 727) {
						piece[i][j] = 16;
					}
					else if (randNum > 485) {
						piece[i][j] = 8;
					}
					else if (randNum > 243) {
						piece[i][j] = 4;
					}
					else {
						piece[i][j] = 2;
					}
					num--;
					//UE_LOG(LogTemp, Warning, TEXT("randCreatePiece:CurrentNum:%d"), temp);
					return;
				}
			}
		}
	}
}

void ChessBoard::Up()
{
	IsRemoveing = false;

	for (int row = 1; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			if (piece[row][col] == -1) {
				continue;
			}

			int j = row;
			while (j > 0) {
				//UE_LOG(LogTemp, Warning, TEXT("IsRemoving %s"), IsRemoveing ? TEXT("true") : TEXT("false"));
				if (piece[j - 1][col] == -1) {
					piece[j - 1][col] = piece[j][col];
					piece[j][col] = -1;
					j = j - 1;
					if (!IsRemoveing) IsRemoveing = true;
				}
				else if (piece[j - 1][col] == piece[j][col]) {
					piece[j - 1][col] = 2 * piece[j][col];
					score += 2 * piece[j][col];
					piece[j][col] = -1;
					j = j - 1;
					num++;
					if (!IsRemoveing) IsRemoveing = true;
					break;//中途结束移动
				}
				else {
					break;//移动到底部
				}
			}
		}
	}
}

void ChessBoard::Down()
{
	IsRemoveing = false;

	for (int row = 2; row >= 0; --row) {
		for (int col = 0; col < 4; ++col) {
			if (piece[row][col] == -1) {
				continue;
			}

			int j = row;
			while (j < 3) {
				//UE_LOG(LogTemp, Warning, TEXT("IsRemoving %s"), IsRemoveing?TEXT("true"):TEXT("false"));
				if (piece[j + 1][col] == -1) {
					piece[j + 1][col] = piece[j][col];
					piece[j][col] = -1;
					j = j + 1;
					if (!IsRemoveing) IsRemoveing = true;
				}
				else if (piece[j + 1][col] == piece[j][col]) {
					piece[j + 1][col] = 2 * piece[j][col];
					score += 2 * piece[j][col];
					piece[j][col] = -1;
					j = j + 1;
					num++;
					if (!IsRemoveing) IsRemoveing = true;
					break;
				}
				else {
					break;
				}
			}
		}
	}
}

void ChessBoard::Left()
{
	IsRemoveing = false;

	for (int col = 1; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			if (piece[row][col] == -1) {
				continue;
			}

			int j = col;
			while (j > 0) {
				//UE_LOG(LogTemp, Warning, TEXT("IsRemoving %s"), IsRemoveing ? TEXT("true") : TEXT("false"));
				//UE_LOG(LogTemp, Warning, TEXT("row:%d,J:%d"), row, j);
				if (piece[row][j - 1] == -1) {
					piece[row][j - 1] = piece[row][j];
					piece[row][j] = -1;
					j = j - 1;
					if (!IsRemoveing) IsRemoveing = true;
				}
				else if (piece[row][j - 1] == piece[row][j]) {
					piece[row][j - 1] = 2 * piece[row][j];
					score += 2 * piece[row][j];
					piece[row][j] = -1;
					j = j - 1;
					num++;
					if (!IsRemoveing) IsRemoveing = true;
					break;
				}
				else {
					break;
				}
			}
		}
	}
}

void ChessBoard::Right()
{
	IsRemoveing = false;

	for (int col = 2; col >= 0; --col) {
		for (int row = 0; row < 4; ++row) {
			if (piece[row][col] == -1) {
				continue;
			}

			int j = col;
			while (j < 3) {
				//UE_LOG(LogTemp, Warning, TEXT("IsRemoving %s"), IsRemoveing ? TEXT("true") : TEXT("false"));
				//UE_LOG(LogTemp, Warning, TEXT("row:%d,J:%d"), row, j);
				if (piece[row][j + 1] == -1) {
					piece[row][j + 1] = piece[row][j];
					piece[row][j] = -1;
					j = j + 1;
					if (!IsRemoveing) IsRemoveing = true;
				}
				else if (piece[row][j + 1] == piece[row][j]) {
					piece[row][j + 1] = 2 * piece[row][j];
					score += 2 * piece[row][j];
					piece[row][j] = -1;
					j = j + 1;
					num++;
					if (!IsRemoveing) IsRemoveing = true;
					break;
				}
				else {
					break;
				}
			}
		}
	}
}

bool ChessBoard::checkDeath()
{
	if (num == 0) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (i - 1 >= 0 && piece[i - 1][j] == piece[i][j]) {
					return false;
				}
				if (i + 1 <= 3 && piece[i + 1][j] == piece[i][j]) {
					return false;
				}
				if (j - 1 >= 0 && piece[i][j - 1] == piece[i][j]) {
					return false;
				}
				if (j + 1 <= 3 && piece[i][j + 1] == piece[i][j]) {
					return false;
				}
			}
		}
	}
	else {
		return false;
	}
	return true;
}

void ChessBoard::RandomStartGame()
{
	initChessBoard();
	int createBlockNumber = rand() % 16 + 1;
	do {
		for (int i = 0; i < createBlockNumber; ++i) {
			randCreatePiece();
		}
	} while (checkDeath());

	score = rand() % 1000 + 1;
}

void ChessBoard::SetupData(const ChessBoard &cb)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			piece[i][j] = cb.piece[i][j];
		}
	}
	num = cb.num;
	score = cb.score;
}
