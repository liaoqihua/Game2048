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
#include "Kismet/GameplayStatics.h"
#include "Button.h"
#include "SWidget.h"
#include <MessageDialog.h>
#include <GenericPlatformMisc.h>
#include <STextBlock.h>
#include <SlateApplication.h>
#include <SButton.h>
#include <SCanvas.h>
#include <SBox.h>
#include <SScrollBox.h>
#include <SEditableText.h>

AGame2048GameModeBase::AGame2048GameModeBase()
	:bIsSaving(false), bIsRestoring(false), FCancelNum(0), FRawScore(0)
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

	LoadLocalData();

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
		if (bIsRestoring) {
			UpdateGraphData();
			UpdateMaxScore();
			bIsRestoring = false;
		}
		else {
			if (BaseCB) BaseCB->IsRemoveing = true;
			UpdateGraph();
		}
		InitButtonsEvent();
	}
}

void AGame2048GameModeBase::BeginDestroy()
{
	SaveLocalData();

	Super::BeginDestroy();
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
					//UE_LOG(LogTemp, Warning, TEXT("Color:%s"), *Image->ColorAndOpacity.ToString());
				}
			}
		}
		Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGame2048GameModeBase::UpdateGraph()
{
	// 检测死亡逻辑
	if (BaseCB->checkDeath()) {
		if (!bIsSaving)
			CreateInputNameWidget();
		else
			CreateRestartWidget();
	}
	else {
		if (BaseCB->IsRemoveing) {
			AddHistory();

			UGameplayStatics::PlaySound2D(this, LoadObject<USoundBase>(this, TEXT("SoundWave'/Game/Resource/1.1'")));
			int x=-1, y=-1;
			BaseCB->randCreatePiece(x, y);
			if (x != -1 && y != -1) {
				UUserWidget *widget = Cast<UUserWidget>(DisplayCB->piece[x][y]);
				PlayAnimation(widget);
			}
		}
		UpdateGraphData();
	}
}

void AGame2048GameModeBase::UpdateGraphData()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			UpdateOnePiece(BaseCB->piece[i][j], DisplayCB->piece[i][j]);
		}
	}

	UpdateScore();
}

void AGame2048GameModeBase::UpdateScore()
{
	if (DisplayCB->score) {
		UTextBlock* ScoreBlock = Cast<UTextBlock>(DisplayCB->score);
		if (ScoreBlock && BaseCB) {
			ScoreBlock->SetText(UKismetTextLibrary::Conv_IntToText(BaseCB->score));
		}

		if ((BaseCB->score - FRawScore) > 100) {
			++FCancelNum;
			FRawScore = BaseCB->score - (BaseCB->score % 100);
		}
		else if (BaseCB->score < FRawScore) {
			FRawScore = BaseCB->score - (BaseCB->score % 100);
		}
	}
	UpdateCancelTB();
}

void AGame2048GameModeBase::UpdateMaxScore()
{
	if (Rank.Num()) {
		Rank.Sort([](const FSaveDataStruct &a, const FSaveDataStruct &b) {return a.score > b.score; });

		if (DisplayCB) {
			UTextBlock *TextBlock = Cast<UTextBlock>(DisplayCB->maxScore);
			if (TextBlock) {
				TextBlock->SetText(UKismetTextLibrary::Conv_IntToText(Rank[0].score));
			}
		}
	}
}

void AGame2048GameModeBase::UpdateCancelTB()
{
	if (DisplayCB) {
		if (FCancelNum > 0) {
			DisplayCB->cancelButton->SetIsEnabled(true);
		}
		else DisplayCB->cancelButton->SetIsEnabled(false);
		UTextBlock *textBlock = Cast<UTextBlock>(DisplayCB->cancelText);
		textBlock->SetText(CancelButtonTextCallback());
	}
}

void AGame2048GameModeBase::AddHistory()
{
	History.Add(*BaseCB);
	if (History.Num() >= 10){
		History.RemoveAt(0);
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
	FLinearColor Ret = FLinearColor::Black;
	switch (value)
	{
	case 2:Ret = FLinearColor(FColor(238, 228, 218)); break;
	case 4:Ret = FLinearColor(FColor(237, 224, 200)); break;
	case 8:Ret = FLinearColor(FColor(242, 177, 121)); break;
	case 16:Ret = FLinearColor(FColor(245, 149, 99)); break;
	case 32:Ret = FLinearColor(FColor(246, 124, 95)); break;
	case 64:Ret = FLinearColor(FColor(246, 94, 59)); break;
	case 128:Ret = FLinearColor(FColor(237, 204, 97)); break;
	case 256:Ret = FLinearColor(FColor(237, 204, 97)); break;
	case 512:Ret = FLinearColor(FColor(237, 204, 97)); break;
	case 1024:Ret = FLinearColor(FColor(237, 204, 97)); break;
	case 2048:Ret = FLinearColor(FColor(237, 194, 46)); break;
	default:
		Ret = FLinearColor(FColor(255, 0, 0));
		break;
	}
	return Ret;
}

void AGame2048GameModeBase::InitButtonsEvent()
{
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UWidget *ButtonObject0 = MainWidgetObject->GetWidgetFromName("NewGameButton");
		if (ButtonObject0) {
			UButton *Button0 = Cast<UButton>(ButtonObject0);
			Button0->OnClicked.AddDynamic(this, &AGame2048GameModeBase::OnClickedButton0Callback);
		}
		UWidget *ButtonObject1 = MainWidgetObject->GetWidgetFromName("RandomButton");
		if (ButtonObject1) {
			UButton *Button1 = Cast<UButton>(ButtonObject1);
			Button1->OnClicked.AddDynamic(this, &AGame2048GameModeBase::OnClickedButton1Callback);
		}
		UWidget *ButtonObject2 = MainWidgetObject->GetWidgetFromName("CancelButton");
		if (ButtonObject2) {
			UButton *Button2 = Cast<UButton>(ButtonObject2);
			Button2->OnClicked.AddDynamic(this, &AGame2048GameModeBase::OnClickedButton2Callback);
		}
		UWidget *ButtonObject3 = MainWidgetObject->GetWidgetFromName("RankButton");
		if (ButtonObject3) {
			UButton *Button3 = Cast<UButton>(ButtonObject3);
			Button3->OnClicked.AddDynamic(this, &AGame2048GameModeBase::OnClickedButton3Callback);
		}
		UWidget *ButtonObject4 = MainWidgetObject->GetWidgetFromName("DescriptionButton");
		if (ButtonObject4) {
			UButton *Button4 = Cast<UButton>(ButtonObject4);
			Button4->OnClicked.AddDynamic(this, &AGame2048GameModeBase::OnClickedButton4Callback);
		}
	}
}

void AGame2048GameModeBase::CreateRestartWidget()
{
	FText MessageText = FText::Format(FText::FromString(TEXT("没有可移动的块了\n是否开始新游戏?\n(你还有{0}次撤销机会)")), 0);
	FText TitleText = FText::FromString(TEXT("游戏结束"));

	if (!newGameWindowPtr.IsValid()) {
		newGameWindowPtr = SNew(SWindow).Title(TitleText).ClientSize(FVector2D(200.0f, 100.f)).IsTopmostWindow(true);
		TSharedPtr<SVerticalBox> mainWidget = SNew(SVerticalBox) + SVerticalBox::Slot().HAlign(HAlign_Fill)[
			SNew(STextBlock).Text(MessageText)
		] + SVerticalBox::Slot().HAlign(HAlign_Right)[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth()[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString(TEXT("是(Yes)")))
				].OnClicked(FOnClicked::CreateUObject(this, &AGame2048GameModeBase::OnClickedButton0YesCallback))
			] + SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth()[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString(TEXT("否(No)")))
				].OnClicked(FOnClicked::CreateUObject(this, &AGame2048GameModeBase::OnClickedButton0NoCallback))
			]
		];
			newGameWindowPtr->SetOnWindowClosed(FOnWindowClosed::CreateLambda([&](TSharedRef<SWindow> window) {
				newGameWindowPtr.Reset();
			}));
			newGameWindowPtr->SetContent(mainWidget.ToSharedRef());
			FSlateApplication::Get().AddWindow(newGameWindowPtr.ToSharedRef());
	}
}

void AGame2048GameModeBase::CreateRankWidget()
{
	if (!RankWindowPtr.IsValid()) {
		RankWindowPtr = SNew(SWindow).ClientSize(FVector2D(200.0f, 350.0f)).Title(FText::FromString(TEXT("排行榜")));

		TSharedPtr<SScrollBox> ScrollWidget;
		TSharedPtr<SWidget> CanvasPanel = SNew(SCanvas) + SCanvas::Slot().Position(FVector2D(0.0f, 0.0f)).Size(FVector2D(200.0f, 50.0f))[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center).HAlign(HAlign_Center)[
				SNew(STextBlock).Text(FText::FromString(TEXT("---排行榜---")))
			]
		] + SCanvas::Slot().Position(FVector2D(0.0f, 50.0f)).Size(FVector2D(200.0f, 245.0f))[
			SAssignNew(ScrollWidget, SScrollBox) + SScrollBox::Slot().VAlign(VAlign_Fill)[
				SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Center)[
					SNew(STextBlock).Text(FText::FromString(TEXT("排名")))
				] + SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Center)[
					SNew(STextBlock).Text(FText::FromString(TEXT("姓名")))
				] + SHorizontalBox::Slot().FillWidth(3.0f).HAlign(HAlign_Center)[
					SNew(STextBlock).Text(FText::FromString(TEXT("得分")))
				]
			]
		] + SCanvas::Slot().Position(FVector2D(0.0f, 295.0f)).Size(FVector2D(200.0f, 55.0f))[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center).HAlign(HAlign_Right).Padding(FMargin(10.0f, 0.0f))[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString(TEXT("确定")))
				].OnClicked(FOnClicked::CreateLambda([&]() -> FReply {
						FSlateApplication::Get().DestroyWindowImmediately(RankWindowPtr.ToSharedRef());
						RankWindowPtr.Reset();
						FSlateApplication::Get().SetUserFocusToGameViewport(0);
						return FReply::Handled();
					}))
			]
		];
					//更新最大值
					UpdateMaxScore();

					//添加数据
					int i = 0;
					for (auto &item : Rank) {
						ScrollWidget->AddSlot().VAlign(VAlign_Fill)[
							SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Center)[
								SNew(STextBlock).Text(UKismetTextLibrary::Conv_IntToText(i + 1))
							] + SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Center)[
								SNew(STextBlock).Text(FText::FromString(item.name))
							] + SHorizontalBox::Slot().FillWidth(3.0f).HAlign(HAlign_Center)[
								SNew(STextBlock).Text(UKismetTextLibrary::Conv_IntToText(item.score))
							]
						];

						++i;
						if (i >= 10) break;
					}

					RankWindowPtr->SetContent(CanvasPanel.ToSharedRef());
					RankWindowPtr->SetOnWindowClosed(FOnWindowClosed::CreateLambda([&](TSharedRef<SWindow> window) {
						RankWindowPtr.Reset();
					}));
					FSlateApplication::Get().AddWindow(RankWindowPtr.ToSharedRef());
	}
}

void AGame2048GameModeBase::CreateInputNameWidget()
{
	FText MessageText = FText::FromString(TEXT("留下大名:"));
	FText TitleText = FText::FromString(TEXT("登上榜单"));

	if (!NameWindowPtr.IsValid()) {
		NameWindowPtr = SNew(SWindow).Title(TitleText).ClientSize(FVector2D(200.0f, 100.f)).IsTopmostWindow(true);
		TSharedPtr<SVerticalBox> mainWidget = SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(2.0f).HAlign(HAlign_Fill)[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center)[
				SNew(STextBlock).Text(MessageText)
			] + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center)[
				SAssignNew(EditTextPtr, SEditableText)
			]
		] + SVerticalBox::Slot().FillHeight(1.0f).HAlign(HAlign_Right)[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth()[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString(TEXT("确定(Yes)")))
				].OnClicked(FOnClicked::CreateUObject(this, &AGame2048GameModeBase::OnClickedNameYesCallback))
			] + SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth()[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString(TEXT("取消(No)")))
				].OnClicked(FOnClicked::CreateUObject(this, &AGame2048GameModeBase::OnClickedNameNoCallback))
			]
		];
			NameWindowPtr->SetOnWindowClosed(FOnWindowClosed::CreateUObject(this, &AGame2048GameModeBase::NameCloseCallback));
			NameWindowPtr->SetContent(mainWidget.ToSharedRef());
			FSlateApplication::Get().AddWindow(NameWindowPtr.ToSharedRef());
	}
}

void AGame2048GameModeBase::CreateNewGameWidget()
{
	FText MessageText = FText::FromString(TEXT("是否开始新游戏?"));
	FText TitleText = FText::FromString(TEXT("新游戏"));

	if (!newGameWindowPtr.IsValid()) {
		newGameWindowPtr = SNew(SWindow).Title(TitleText).ClientSize(FVector2D(200.0f, 100.f)).IsTopmostWindow(true);
		TSharedPtr<SVerticalBox> mainWidget = SNew(SVerticalBox) + SVerticalBox::Slot().HAlign(HAlign_Fill)[
			SNew(STextBlock).Text(MessageText)
		] + SVerticalBox::Slot().HAlign(HAlign_Right)[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth()[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString("Yes"))
				].OnClicked(FOnClicked::CreateUObject(this, &AGame2048GameModeBase::OnClickedButton0YesCallback))
			] + SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth()[
				SNew(SButton).Content()[
					SNew(STextBlock).Text(FText::FromString("No"))
				].OnClicked(FOnClicked::CreateUObject(this, &AGame2048GameModeBase::OnClickedButton0NoCallback))
			]
		];
			newGameWindowPtr->SetOnWindowClosed(FOnWindowClosed::CreateLambda([&](TSharedRef<SWindow> window) {
				newGameWindowPtr.Reset();
			}));
			newGameWindowPtr->SetContent(mainWidget.ToSharedRef());
			FSlateApplication::Get().AddWindow(newGameWindowPtr.ToSharedRef());
	}
}

void AGame2048GameModeBase::LoadLocalData()
{
	bool bRet = UGameplayStatics::DoesSaveGameExist("SaveData", 0);
	if (bRet) {
		TSharedPtr<UGame2048SaveGame> SaveGamePtr = MakeShareable(Cast<UGame2048SaveGame>(UGameplayStatics::LoadGameFromSlot("SaveData", 0)));
		if (SaveGamePtr.IsValid()) {
			UE_LOG(LogTemp, Warning, TEXT("LoadLocalData"));
			if (BaseCB) {
				*BaseCB = UGame2048SaveGame::TransDataToChessBoard(SaveGamePtr->LastData);
				FCancelNum = BaseCB->cancelNum;
				FRawScore = SaveGamePtr->LastData.rawScore;
			}
			SetupRank(SaveGamePtr->Rank);
			bIsSaving = SaveGamePtr->bIsSaving;
			bIsRestoring = true;

			bool bEmpty = false;
			for (auto &item : SaveGamePtr->LastData.piece) {
				if (item != -1) {
					bEmpty = true;
					break;
				}
			}
			if (!bEmpty) bIsRestoring = false;
		}
	}
}

void AGame2048GameModeBase::SaveLocalData()
{
	TSharedPtr<UGame2048SaveGame> SaveGamePtr = MakeShareable(Cast<UGame2048SaveGame>(UGameplayStatics::CreateSaveGameObject(UGame2048SaveGame::StaticClass())));

	if (SaveGamePtr && BaseCB) {
		BaseCB->cancelNum = FCancelNum;
		FSaveDataStruct SaveData = UGame2048SaveGame::InitDataFromChessBoard(*BaseCB);
		SaveData.rawScore = FRawScore;
		SaveGamePtr->SetupLastData(SaveData);
		SaveGamePtr->SetupRank(Rank);
		SaveGamePtr->bIsSaving = bIsSaving;
		UGameplayStatics::SaveGameToSlot(SaveGamePtr.Get(), "SaveData", 0);
	}
}

void AGame2048GameModeBase::SetupRank(const TArray<FSaveDataStruct> &r)
{
	Rank.RemoveAll([](const FSaveDataStruct &sds) -> bool {return true; });

	for (auto &item : r) {
		Rank.Add(item);
	}
}

void AGame2048GameModeBase::UpKeyHandle()
{
	//UE_LOG(LogTemp, Warning, TEXT("UpKeyHandle"));
	BaseCB->Up();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::DownKeyHandle()
{
	//UE_LOG(LogTemp, Warning, TEXT("DownKeyHandle"));
	BaseCB->Down();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::LeftKeyHandle()
{
	//UE_LOG(LogTemp, Warning, TEXT("LeftKeyHandle"));
	BaseCB->Left();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::RightKeyHandle()
{
	//UE_LOG(LogTemp, Warning, TEXT("RightKeyHandle"));
	BaseCB->Right();
	if (MainWidgetObject && BaseCB && DisplayCB) {
		UpdateGraph();
	}
}

void AGame2048GameModeBase::OnClickedButton0Callback()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnClickedButton0Callback"));

	CreateNewGameWidget();
}

void AGame2048GameModeBase::OnClickedButton1Callback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickedButton1Callback"));

	if (BaseCB) {
		History.RemoveAll([](const ChessBoard &cb) {return true; });

		BaseCB->RandomStartGame();
		FCancelNum = 0;
		FRawScore = BaseCB->score - BaseCB->score % 100;
		BaseCB->IsRemoveing = true;
		UpdateGraph();
		bIsSaving = false;
	}
}

void AGame2048GameModeBase::OnClickedButton2Callback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickedButton2Callback:%d"), FCancelNum);

	if (FCancelNum) {
		if (History.Num() > 1)
			*BaseCB = History.Pop();
		--FCancelNum;
		UpdateGraphData();
	}
}

void AGame2048GameModeBase::OnClickedButton3Callback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickedButton3Callback"));

	CreateRankWidget();
}

void AGame2048GameModeBase::OnClickedButton4Callback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickedButton4Callback"));

	if (!ExplanationWindowPtr.IsValid()) {
		ExplanationWindowPtr = SNew(SWindow).ClientSize(FVector2D(200.0f, 250.0f)).Title(FText::FromString(TEXT("说明")));
		
		TSharedPtr<SWidget> mainWidget = SNew(SVerticalBox) + SVerticalBox::Slot().VAlign(VAlign_Center)[
			SNew(STextBlock).Text(FText::FromString(TEXT("这是说明，键盘上下左右可移动方块\n作者：廖其华")))
		] + SVerticalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)[
			SNew(SButton).HAlign(HAlign_Center).Content()[
				SNew(STextBlock).Text(FText::FromString(TEXT("确定")))
			].OnClicked(FOnClicked::CreateLambda([&]() -> FReply {
					ExplanationWindowPtr->RequestDestroyWindow();

					return FReply::Handled();
				}))
		];

		ExplanationWindowPtr->SetContent(mainWidget.ToSharedRef());
		ExplanationWindowPtr->SetOnWindowClosed(FOnWindowClosed::CreateLambda([&](TSharedRef<SWindow> window) {
			ExplanationWindowPtr.Reset();
		}));
		FSlateApplication::Get().AddWindow(ExplanationWindowPtr.ToSharedRef());
	}
}

FText AGame2048GameModeBase::CancelButtonTextCallback()
{
	return FText::Format(FTextFormat::FromString(TEXT("撤销({0})")), FCancelNum);
}

FReply AGame2048GameModeBase::OnClickedButton0YesCallback()
{
	History.RemoveAll([](const ChessBoard &cb) {return true; });

	if (BaseCB) {
		BaseCB->initChessBoard();
		BaseCB->IsRemoveing = true;
		FCancelNum = 0;
		FRawScore = 0;
		UpdateGraph();
		UpdateMaxScore();
		bIsSaving = false;
	}

	newGameWindowPtr->RequestDestroyWindow();
	newGameWindowPtr.Reset();
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
	return FReply::Handled();
}

FReply AGame2048GameModeBase::OnClickedButton0NoCallback()
{
	newGameWindowPtr->RequestDestroyWindow();
	newGameWindowPtr.Reset();
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
	return FReply::Handled();
}

FReply AGame2048GameModeBase::OnClickedNameYesCallback()
{
	if (EditTextPtr.IsValid()) {
		FSaveDataStruct sds = UGame2048SaveGame::InitDataFromChessBoard(*BaseCB);
		FString name = EditTextPtr->GetText().ToString();
		if (name.IsEmpty()) {
			sds.name = TEXT("无名氏");
		}
		else sds.name = name;

		Rank.Add(sds);
	}

	NameWindowPtr->RequestDestroyWindow();
	NameWindowPtr.Reset();

	return FReply::Handled();
}

FReply AGame2048GameModeBase::OnClickedNameNoCallback()
{
	FSaveDataStruct sds = UGame2048SaveGame::InitDataFromChessBoard(*BaseCB);
	sds.name = TEXT("无名氏");

	Rank.Add(sds);

	NameWindowPtr->RequestDestroyWindow();
	NameWindowPtr.Reset();

	return FReply::Handled();
}

void AGame2048GameModeBase::NameCloseCallback(const TSharedRef<SWindow>& window)
{
	RankWindowPtr.Reset();
	bIsSaving = true;
	CreateRankWidget();
}

