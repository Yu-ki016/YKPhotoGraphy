// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoGraphyToolBase.h"

#include "InteractiveToolManager.h"
#include "PhotoGraphyCanvasMechanic.h"
#include "PhotoGraphyDetialPanel.h"
#include "PhotoGraphyEditorMode.h"

UInteractiveTool* UPhotoGraphyToolBaseToolBuilder::BuildTool( const FToolBuilderState& SceneState ) const
{
	UPhotoGraphyToolBase* NewTool = NewObject<UPhotoGraphyToolBase>(SceneState.ToolManager);
	return NewTool;
}

void UPhotoGraphyToolBase::Setup()
{
	Super::Setup();
	
	CanvasMechanic = NewObject<UPhotoGraphyCanvasMechanic>(this);
	CanvasMechanic->Setup(this);
	
	GetToolSettings();
	
}
void UPhotoGraphyToolBase::Shutdown( EToolShutdownType ShutdownType )
{
	CanvasMechanic->Shutdown();
	PhotoGraphySettings = nullptr;
	Super::Shutdown(ShutdownType);
}

bool UPhotoGraphyToolBase::GetCursor( EMouseCursor::Type& OutCursor )
{
	if (CanvasMechanic)
	{
		return CanvasMechanic->GetCursor(OutCursor);
	}
	return false;
}

UPhotoGraphyToolSettings* UPhotoGraphyToolBase::GetToolSettings()
{
	if (PhotoGraphySettings.IsValid()) return PhotoGraphySettings.Get();
	
	if (const UPhotoGraphyEditorMode* EdMode = UPhotoGraphyEditorMode::GetPhotoGraphyEditorMode())
	{
		PhotoGraphySettings = EdMode->GetPhotoGraphySettings();
	}
	
	return PhotoGraphySettings.IsValid() ? PhotoGraphySettings.Get() : nullptr;
}
