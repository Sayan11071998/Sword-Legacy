#include "Widgets/Options/DataObjects/SL_ListDataObject_Scalar.h"

FCommonNumberFormattingOptions USL_ListDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	
	Options.MaximumFractionalDigits = 0;
	
	return Options;
}

FCommonNumberFormattingOptions USL_ListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	
	Options.MaximumFractionalDigits = NumFracDigit;
	
	return Options;
}