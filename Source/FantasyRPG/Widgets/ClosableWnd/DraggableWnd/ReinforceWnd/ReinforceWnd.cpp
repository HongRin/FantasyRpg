#include "ReinforceWnd.h"
#include "Widgets/ClosableWnd/DraggableWnd/ReinforceWnd/ReinforceWndRow.h"

void UReinforceWnd::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateWndSize(600.0f, 600.0f);

	Cast<UReinforceWndRow>(GetWidgetFromName(TEXT("BP_ReinforceRow_Weapon")))->SetReinforceWnd(this);
	Cast<UReinforceWndRow>(GetWidgetFromName(TEXT("BP_ReinforceRow_Armor")))->SetReinforceWnd(this);
}