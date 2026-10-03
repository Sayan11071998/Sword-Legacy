#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "SL_TabListWidgetBase.generated.h"

class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_TabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
	
public:
	void RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName);
	
private:
	// ~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	// ~ End UWidget Interface
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game UI Tab List Settings", meta = (AllowPrivateAccess = "true", ClampMin = 1, ClampMax = 10))
	int32 DebugEditorPreviewTabCount = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game UI Tab List Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USL_CommonButtonBase> TabButtonEntryWidgetClass;
};