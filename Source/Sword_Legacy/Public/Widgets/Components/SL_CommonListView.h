#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "SL_CommonListView.generated.h"

class USL_DataAsset_DataListEntryMapping;

UCLASS()
class SWORD_LEGACY_API USL_CommonListView : public UCommonListView
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UCommonListView Interface
	virtual UUserWidget& OnGenerateEntryWidgetInternal(
		UObject* Item,
		TSubclassOf<UUserWidget> DesiredEntryClass,
		const TSharedRef<STableViewBase>& OwnerTable	
	) override;
	
	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;
	// ~ End UCommonListView Interface
	
private:
	// ~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	// ~ End UWidget Interface
	
	UPROPERTY(EditAnywhere, Category = "Game List View Settings")
	TObjectPtr<USL_DataAsset_DataListEntryMapping> DataListEntryMapping;
};