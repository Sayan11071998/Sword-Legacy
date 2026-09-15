#include "Widgets/Components/SL_TabListWidgetBase.h"
#include "Editor/WidgetCompilerLog.h"
#include "Widgets/Components/SL_CommonButtonBase.h"

#if WITH_EDITOR	
void USL_TabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	
	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(
			TEXT("The variable TabButtonEntryWidgetClass has not valid entry specified. ") +
			GetClass()->GetName() +
			TEXT(" needs a valid entry widget class to function properly")
		));
	}
}
#endif