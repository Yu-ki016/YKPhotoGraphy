// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhotoGraphyEditorModeToolkit.h"
#include "PhotoGraphyEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"
#include "PhotoGraphyEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "YKToolsEditorModeToolkit"

FPhotoGraphyEditorModeToolkit::FPhotoGraphyEditorModeToolkit()
{
}

void FPhotoGraphyEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FPhotoGraphyEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(UPhotoGraphyEditorMode::PhotoGraphyCategoryName);
}

FText FPhotoGraphyEditorModeToolkit::GetToolPaletteDisplayName( FName Palette ) const
{
	if (Palette == UPhotoGraphyEditorMode::PhotoGraphyCategoryName)
	{
		return LOCTEXT("PhotoGraphyPalette", "PhotoGraphy");
	}
	return FText();
}

void FPhotoGraphyEditorModeToolkit::BuildToolPalette( FName Palette, class FToolBarBuilder& ToolbarBuilder )
{
	FModeToolkit::BuildToolPalette(Palette, ToolbarBuilder);
}

FName FPhotoGraphyEditorModeToolkit::GetToolkitFName() const
{
	return FName("YKToolsEditorMode");
}

FText FPhotoGraphyEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "YKToolsEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
