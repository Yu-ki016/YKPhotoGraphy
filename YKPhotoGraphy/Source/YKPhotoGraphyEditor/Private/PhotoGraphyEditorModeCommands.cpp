// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhotoGraphyEditorModeCommands.h"
#include "PhotoGraphyEditorMode.h"
#include "EditorStyleSet.h"
#include "FPhotoGraphyEditorStyle.h"

#define LOCTEXT_NAMESPACE "YKToolsEditorModeCommands"

FPhotoGraphyEditorModeCommands::FPhotoGraphyEditorModeCommands()
	: TCommands<FPhotoGraphyEditorModeCommands>("YKToolsEditorMode",
		NSLOCTEXT("YKToolsEditorMode", "YKToolsEditorModeCommands", "YKTools Editor Mode"),
		NAME_None,
		FPhotoGraphyEditorStyle::GetStyleSetName())
{
}

void FPhotoGraphyEditorModeCommands::RegisterCommands()
{
	UI_COMMAND(SelectPhotoGraphyTool, "Create", "Create a PhotoGraphy Actor", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(EditPhotoGraphyTool, "Edit", "Edit Selected PhotoGraphy Actor", EUserInterfaceActionType::ToggleButton, FInputChord());
	
	TArray<TSharedPtr<FUICommandInfo>> PhotoGraphyCommands = {
		SelectPhotoGraphyTool, EditPhotoGraphyTool,
	};
	Commands.Add(UPhotoGraphyEditorMode::PhotoGraphyCategoryName, PhotoGraphyCommands);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FPhotoGraphyEditorModeCommands::GetCommands()
{
	return FPhotoGraphyEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
