﻿#include "TradeWnd.h"

#include "Actors/Characters/PlayerCharacter/PlayerCharacter.h"
#include "Actors/Controllers/PlayerController/RPGPlayerController.h"

#include "Components/PlayerInventory/PlayerInventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

#include "Structures/ItemInfo/ItemInfo.h"

#include "Single/GameInstance/FRGameInstance.h"
#include "Single/PlayerManager/PlayerManager.h"

void UTradeWnd::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateWndSize(500.0f, 300.0f);

	Text_ItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_ItemName")));
	Text_Q = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Q")));
	EditableTextBox_TradeCount = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("EditableTextBox_TradeCount")));
	Text_Costs = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Costs")));
	Button_Cancel = Cast<UButton>(GetWidgetFromName(TEXT("Button_Cancel")));
	Button_Ok = Cast<UButton>(GetWidgetFromName(TEXT("Button_Ok")));
	Button_Plus = Cast<UButton>(GetWidgetFromName(TEXT("Button_Plus")));
	Button_Minus = Cast<UButton>(GetWidgetFromName(TEXT("Button_Minus")));

	EditableTextBox_TradeCount->OnTextChanged.AddDynamic(this, &UTradeWnd::OnTradeCountChanged);
	Button_Cancel->OnClicked.AddDynamic(this, &UClosableWnd::OnCloseButtonClicked);
	Button_Ok->OnClicked.AddDynamic(this, &UTradeWnd::OnOkButtonClicked);
	Button_Plus->OnClicked.AddDynamic(this, &UTradeWnd::OnPlusButtonClicked);
	Button_Minus->OnClicked.AddDynamic(this, &UTradeWnd::OnMinusButtonClicked);
}

void UTradeWnd::InitializeTradeWnd(EShopItemType itemType, FItemInfo* itemInfo, int32 costs, int32 maxTradeCount)
{
	ItemType = itemType;
	Price = costs;
	ItemInfo = itemInfo;

	// 인벤토리 아이템이라면 
	if (itemType == EShopItemType::SI_InventoryItem)
		// 최대 판매 가능 개수를 소지중인 아이템 개수로 설정합니다.
		MaxTradeCount = maxTradeCount;
	// 상점 아이템이라면
	else
	{
		// 최대 구매 가능 개수를 소지금으로 구매 가능한 최대 개수로 설정합니다.
		MaxTradeCount =
			(GetManager(UPlayerManager)->GetPlayerInfo()->Money / costs);
	}

	Text_ItemName->SetText(itemInfo->ItemName);

	Text_Q->SetText(FText::FromString(
		FString(TEXT("(을)를 몇 개 ")) +
		(itemType == EShopItemType::SI_InventoryItem ? TEXT("판매") : TEXT("구매")) +
		FString(TEXT(" 할까요?"))
	));

}

void UTradeWnd::OnTradeCountChanged(const FText& text)
{
	// 입력되어있는 문자열이 숫자가 아니라면
	if (!text.IsNumeric())
	{
		EditableTextBox_TradeCount->SetText(FText::FromString(TEXT("0")));

		Text_Costs->SetText(FText::FromString(TEXT("합계 0 원")));

		return;
	}

	// 입력된 문자열을 숫자로 변경하여 저장합니다.
	TradeCount = FCString::Atoi(*text.ToString());
	/// - FCString : C Style 의 문자열과 함께 사용되는 함수들을 제공하는 구조체입니다.

	// 최대 교환 개수를 초과했다면 입력된 숫자를 최대 교환 가능 개수로 설정합니다.
	if (TradeCount > MaxTradeCount)
		TradeCount = MaxTradeCount;

	EditableTextBox_TradeCount->SetText(
		FText::FromString(FString::FromInt(TradeCount)));

	// 가격을 표시합니다.
	Text_Costs->SetText(FText::FromString(
		FString(TEXT("합계 ")) +
		FString::FromInt(TradeCount * Price) +
		FString(TEXT(" 원"))
	));
}

void UTradeWnd::OnOkButtonClicked()
{
	int32 inputCount = FCString::Atoi(*EditableTextBox_TradeCount->GetText().ToString());

	UPlayerInventoryComponent* playerInventory = Cast<APlayerCharacter>(GetManager(UPlayerManager)->GetPlayerController()->GetPawn())->
		GetPlayerInventory();
	if (ItemType == EShopItemType::SI_SaleItem)
		playerInventory->AddItem(ItemInfo->ItemCode, inputCount);
	else
		playerInventory->RemoveItem(playerInventory->GetItemIndex(FItemSlotInfo(ItemInfo->ItemCode, MaxTradeCount)), inputCount);;


	OnCloseButtonClicked();
}

void UTradeWnd::OnPlusButtonClicked()
{
		EditableTextBox_TradeCount->SetText(
			FText::FromString(FString::FromInt(++TradeCount)));
}

void UTradeWnd::OnMinusButtonClicked()
{
	EditableTextBox_TradeCount->SetText(
		FText::FromString(FString::FromInt(--TradeCount)));
}
