// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"

/**
 * This class contains info about the full set of commands used in this editor mode.
 */
class FPhotoGraphyEditorModeCommands : public TCommands<FPhotoGraphyEditorModeCommands>
{
public:
	FPhotoGraphyEditorModeCommands();

	virtual void RegisterCommands() override;
	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	TSharedPtr<FUICommandInfo> SelectPhotoGraphyTool;
	TSharedPtr<FUICommandInfo> EditPhotoGraphyTool;

protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
