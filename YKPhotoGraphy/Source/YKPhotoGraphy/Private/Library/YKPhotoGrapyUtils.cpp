#include "Library/YKPhotoGraphyUtils.h"

EObjectFlags YKPhotoGraphyUtils::GetTransientMIDFlags()
{
	return RF_Transient | RF_NonPIEDuplicateTransient | RF_TextExportTransient;
}
