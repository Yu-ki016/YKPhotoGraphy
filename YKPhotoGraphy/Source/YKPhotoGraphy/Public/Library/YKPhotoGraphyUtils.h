#pragma once

struct YKPhotoGraphyUtils
{
	// Our transient MIDs are per-object and shall not survive duplicating nor be exported to text when copy-pasting : 
	static EObjectFlags GetTransientMIDFlags();
};
