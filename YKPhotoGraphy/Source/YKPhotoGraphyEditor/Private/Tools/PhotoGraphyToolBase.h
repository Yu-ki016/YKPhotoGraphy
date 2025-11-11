// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveTool.h"
#include "InteractiveToolBuilder.h"
#include "PhotoGraphyToolBase.generated.h"


class UPhotoGraphyToolSettings;
class UPhotoGraphyCanvasMechanic;

UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyToolBaseToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()
	
public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * 
 */
UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyToolBase : public UInteractiveTool
{
	GENERATED_BODY()
	
public:
	TWeakObjectPtr<UPhotoGraphyToolSettings> PhotoGraphySettings;
	
	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	virtual bool GetCursor(EMouseCursor::Type& OutCursor);
	UPhotoGraphyToolSettings* GetToolSettings();
	
protected:
	UPROPERTY()
	TObjectPtr<UPhotoGraphyCanvasMechanic> CanvasMechanic;
};
