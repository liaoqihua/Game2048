// Fill out your copyright notice in the Description page of Project Settings.

#include "Game2048GameModeBase.h"
#include <ConstructorHelpers.h>
#include <UserWidget.h>
#include <TextBlock.h>
#include <Kismet/KismetTextLibrary.h>
#include "Game2048PlayerCharacter.h"
#include <cstdlib>
#include <WidgetBlueprintGeneratedClass.h>
#include <WidgetAnimation.h>
#include <Image.h>

void ChessBoard::initChessBoard()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			piece[i][j] = -1;
		}
	}
}


void ChessBoard::randCreatePiece(int &x, int &y)
{
	UE_LOG(LogTemp, Warning, TEXT("randCreatePiece:Num:%d"), num);
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
					piece[i][j] = 2;
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

AGame2048GameModeBase::AGame2048GameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = AGame2048PlayerCharacter::StaticClass();

	ConstructorHelpers::FClassFinder<UUserWidget> MainWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/WBP_MainWidget.WBP_MainWidget_C'"));
	if (MainWidgetAsset.Succeeded()) {
		MainWidgetClass = MainWidgetAsset.Class;
	}
	ConstructorHelpers::FClassFinder<UUserWidget> BlockWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/WBP_Block.WBP_Block_C'"));
	if (BlockWidgetAsset.Succeeded()) {
		BlockWidgetClass = BlockWidgetAsset.Class;
	}

	BaseCB = MakeShareable(new ChessBoard());

	OnUpKeyPressEvent.AddUFunction(this, "UpKeyHandle");
	OnDownKeyPressEvent.AddUFunction(this, "DownKeyHandle");
	OnLeftKeyPressEvent.AddUFunction(this, "LeftKeyHandle");
	OnRightKeyPressEvent.AddUFunction(this, "RightKeyHandle");
}

void AGame2048GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld *World = GetWorld();
	if (World) {
		MainWidgetObject = CreateWidget<UUserWidget>(World, MainWidgetClass);
		if (MainWidgetObject) {
			MainWidgetObject->AddToViewport(0);
			DisplayCB = MakeShareable(new UWidgetChessBoard(MainWidgetObject));
		}

		World->GetFirstPlayerController()->bShowMouseCursor = true;
	}

	if (MainWidgetObject && BaseCB && DisplayCB) {
		if (BaseCB) BaseCB->IsRemoveing = true;
		UpdateGraph();
	}
}

void AGame2048GameModeBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void AGame2048GameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AGame2048GameModeBase::UpdateOnePiece(int value, UWidget *Widget)
{
	if (value == -1) {
		Widget->SetVisibility(ESlateVisibility::Hidden);
	}
	else {
		Widget->SetVisibility(ESlateVisibility::Visible);
		if (Widget->GetClass() == BlockWidgetClass) {

			UWidget *TextObject= Cast<UUserWidget>(Widget)->GetWidgetFromName("Text_Number");
			UWidget *ImageObject = Cast<UUserWidget>(Widget)->GetWidgetFromName("Image_0");
			if (TextObject) {
				UTextBlock *Text = Cast<UTextBlock>(TextObject);
				if (Text) {
					Text->SetText(UKismetTextLibrary::Conv_IntToText(value));
				}
			}
			if (ImageObject) {
				UImage *Image = Cast<UImage>(ImageObject);
				if (Image) {
					Image->SetColorAndOpacity(CreateColor(value));
				}
			}
		}
	}
}

void AGame2048GameModeBase::UpdateGraph()
{
	if (BaseCB->IsRemoveing) {
		int x=-1, y=-1;
		BaseCB->randCreatePiece(x, y);
		if (x != -1 && y != -1) {
			UUserWidget *widget = Cast<UUserWidget>(DisplayCB->piece[x][y]);
			PlayAnimation(widget);
			UE_LOG(LogTemp, Warning, TEXT("Create Piece:(%d,%d)"), x, y);
		}

	}
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			UpdateOnePiece(BaseCB->piece[i][j], DisplayCB->piece[i][j]);
		}
	}
}

void AGame2048GameModeBase::PlayAnimation(UUserWidget *widget)
{
	UProperty *prop = widget->GetClass()->PropertyLink;
	while (prop != nullptr) {
		if (prop->GetClass() == UObjectProperty::StaticClass()) {
			UObjectProperty *ObjectProp = Cast<UObjectProperty>(prop);
			if (ObjectProp->PropertyClass == UWidgetAnimation::StaticClass()) {
				UObject *object = ObjectProp->GetObjectPropertyValue_InContainer(widget);
				UWidgetAnimation *widgetAnim = Cast<UWidgetAnimation>(object);
				if (widgetAnim) {
					widget->PlayAnimation(widgetAnim);
					break;
				}
			}
		}
		prop = prop->PropertyLinkNext;
	}
}

FLinearColor AGame2048GameModeBase::CreateColor(int value)
{
	FLinearColor Ret = FLinearColor::White;
	switch (value)
	{
	case 2:break;
	case 4:break;
	default:
		break;
	}
	return Ret;
}

void AGame2048GameModeBase::UpKeyHandle()
{
	UE_LOG(LogTemp, Warning, TEXT("UpKeyHandle"));
	BaseCB->Up();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::DownKeyHandle()
{
	UE_LOG(LogTemp, Warning, TEXT("DownKeyHandle"));
	BaseCB->Down();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::LeftKeyHandle()
{
	UE_LOG(LogTemp, Warning, TEXT("LeftKeyHandle"));
	BaseCB->Left();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::RightKeyHandle()
{
	UE_LOG(LogTemp, Warning, TEXT("RightKeyHandle"));
	BaseCB->Right();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

