// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tools/UEdMode.h"
#include "PhotoGraphyEditorMode.generated.h"

class UPhotoGraphyInteractiveToolProperties;
class UPhotoGraphyToolSettings;
class FPhotoGraphySceneViewExtension;
/**
 * This class provides an example of how to extend a UEdMode to add some simple tools
 * using the InteractiveTools framework. The various UEdMode input event handlers (see UEdMode.h)
 * forward events to a UEdModeInteractiveToolsContext instance, which
 * has all the logic for interacting with the InputRouter, ToolManager, etc.
 * The functions provided here are the minimum to get started inserting some custom behavior.
 * Take a look at the UEdMode markup for more extensibility options.
 */
UCLASS()
class UPhotoGraphyEditorMode : public UEdMode
{
	GENERATED_BODY()

public:
	const static FEditorModeID EM_PhotoGraphyEditorModeId;

	static FName PhotoGraphyCategoryName;
	
	static FString InteractiveToolName;
	static FString PhotoGraphyGenerateToolName;
	static FString PhotoGraphyInteractiveToolName;

	UPhotoGraphyEditorMode();
	virtual ~UPhotoGraphyEditorMode();

	/** UEdMode interface */
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void ActorSelectionChangeNotify() override;
	virtual void CreateToolkit() override;
	virtual void OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool) override;
	virtual void OnToolEnded(UInteractiveToolManager* Manager, UInteractiveTool* Tool) override;
	virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override;
	virtual void ActivateDefaultTool() override;
	

	static UPhotoGraphyEditorMode* GetPhotoGraphyEditorMode();
	static UInteractiveTool* GetActiveTool();
	static UPhotoGraphyInteractiveToolProperties* GetPhotoGraphyToolProperties();
	static FPhotoGraphySceneViewExtension* GetPhotoGraphySceneViewExtension();
	UPhotoGraphyToolSettings* GetPhotoGraphySettings() const;
	
	void SetCurrentTool(UInteractiveTool* InTool);
	UInteractiveTool* GetCurrentTool() const;
	
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<UPhotoGraphyToolSettings> ModeSettings;
	
	TWeakObjectPtr<UInteractiveTool> CurrentTool;
};
