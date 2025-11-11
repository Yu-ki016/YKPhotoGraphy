// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhotoGraphyEditorMode.h"
#include "PhotoGraphyEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "PhotoGraphyEditorModeCommands.h"

#include "EditorModeManager.h"
#include "EdMode.h"
#include "PhotoGraphyDetialPanel.h"
#include "BaseGizmos/TransformGizmoUtil.h"
#include "Tools/PhotoGraphyInteractiveTool.h"
#include "Tools/PhotoGraphyGenerateTool.h"
#include "ViewExtension/PhotoGraphySceneViewExtension.h"
#include "ViewExtension/PhotoGraphySubsystem.h"

#define LOCTEXT_NAMESPACE "YKToolsEditorMode"

const FEditorModeID UPhotoGraphyEditorMode::EM_PhotoGraphyEditorModeId = TEXT("EM_PhotoGraphyEditorMode");

FName  UPhotoGraphyEditorMode::PhotoGraphyCategoryName = FName(TEXT("PhotoGraphy"));

FString UPhotoGraphyEditorMode::PhotoGraphyGenerateToolName = TEXT("YKPhotoGraphy_GenerateTool");
FString UPhotoGraphyEditorMode::PhotoGraphyInteractiveToolName = TEXT("YKPhotoGraphy_InteractiveTool");


UPhotoGraphyEditorMode::UPhotoGraphyEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	SettingsClass = UPhotoGraphyToolSettings::StaticClass();
	
	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UPhotoGraphyEditorMode::EM_PhotoGraphyEditorModeId,
		LOCTEXT("ModeName", "YKPhotoGraphy"),
		FSlateIcon(),
		true);
}

void UPhotoGraphyEditorMode::OnToolStarted( UInteractiveToolManager* Manager, UInteractiveTool* Tool )
{
	SetCurrentTool(Tool);
	Super::OnToolStarted(Manager, Tool);
}

void UPhotoGraphyEditorMode::OnToolEnded( UInteractiveToolManager* Manager, UInteractiveTool* Tool )
{
	SetCurrentTool(nullptr);
	Super::OnToolEnded(Manager, Tool);
}

UPhotoGraphyEditorMode::~UPhotoGraphyEditorMode()
{
}


void UPhotoGraphyEditorMode::ActorSelectionChangeNotify()
{
}

void UPhotoGraphyEditorMode::Enter()
{
	UEdMode::Enter();

	ModeSettings = Cast<UPhotoGraphyToolSettings>(SettingsObject);
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FPhotoGraphyEditorModeCommands& SampleToolCommands = FPhotoGraphyEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SelectPhotoGraphyTool, PhotoGraphyGenerateToolName, NewObject<UPhotoGraphyGenerateToolBuilder>(this));
	RegisterTool(SampleToolCommands.EditPhotoGraphyTool, PhotoGraphyInteractiveToolName, NewObject<UToonPhotoGraphyInteractiveToolBuilder>(this));

	UE::TransformGizmoUtil::RegisterTransformGizmoContextObject(GetInteractiveToolsContext());
	GetInteractiveToolsContext()->SetForceCombinedGizmoMode(true);
	
	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, PhotoGraphyGenerateToolName);

	if (ModeSettings.Get())
	{
		ModeSettings->ViewCanvas.bEditCanvas = true;
		ModeSettings->UpdateViewCanvas();
	}

	ActivateDefaultTool();
}

void UPhotoGraphyEditorMode::Exit()
{
	if (ModeSettings.Get())
	{
		ModeSettings->ViewCanvas.bEditCanvas = false;
		ModeSettings->UpdateViewCanvas();
	}
	
	Super::Exit();
}

void UPhotoGraphyEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FPhotoGraphyEditorModeToolkit);
}

UPhotoGraphyEditorMode* UPhotoGraphyEditorMode::GetPhotoGraphyEditorMode()
{
	return Cast<UPhotoGraphyEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode("EM_PhotoGraphyEditorMode"));
}

UInteractiveTool* UPhotoGraphyEditorMode::GetActiveTool()
{
	UPhotoGraphyEditorMode* ToolEdMode = GetPhotoGraphyEditorMode();
	UInteractiveToolManager* ToolManager = ToolEdMode != nullptr ? ToolEdMode->GetToolManager() : nullptr;
	return ToolManager != nullptr ? ToolManager->GetActiveTool(EToolSide::Mouse) : nullptr;
}

template<typename T>
static T* GetTypedToolProperties()
{
	UPhotoGraphyEditorMode* ToolEdMode = UPhotoGraphyEditorMode::GetPhotoGraphyEditorMode();
	UInteractiveToolManager* ToolManager = ToolEdMode != nullptr ? ToolEdMode->GetToolManager() : nullptr;
	UInteractiveTool* Tool = ToolManager != nullptr ? ToolManager->GetActiveTool(EToolSide::Mouse) : nullptr;
	if (Tool != nullptr)
	{
		const TArray<UObject*> PropertyArray = Tool->GetToolProperties();
		for (UObject* Property : PropertyArray)
		{
			if (T* FoundProperty = Cast<T>(Property))
			{
				return FoundProperty;
			}
		}
	}
	return nullptr;
}

UPhotoGraphyInteractiveToolProperties* UPhotoGraphyEditorMode::GetPhotoGraphyToolProperties()
{
	return GetTypedToolProperties<UPhotoGraphyInteractiveToolProperties>();
}

UPhotoGraphyToolSettings* UPhotoGraphyEditorMode::GetPhotoGraphySettings() const
{
	return ModeSettings.Get();
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UPhotoGraphyEditorMode::GetModeCommands() const
{
	return FPhotoGraphyEditorModeCommands::Get().GetCommands();
}


FPhotoGraphySceneViewExtension* UPhotoGraphyEditorMode::GetPhotoGraphySceneViewExtension()
{
	if (GEngine)
		if (const UPhotoGraphySubsystem* Subsystem = GEngine->GetEngineSubsystem<UPhotoGraphySubsystem>())
			return Subsystem->GetPhotoGraphySceneViewExtension();
	return nullptr;
}

void UPhotoGraphyEditorMode::SetCurrentTool(UInteractiveTool* InTool)
{
	CurrentTool = InTool;
}

UInteractiveTool* UPhotoGraphyEditorMode::GetCurrentTool() const
{
	if (CurrentTool.IsValid()) return CurrentTool.Get();
	return GetActiveTool();
}

bool UPhotoGraphyEditorMode::GetCursor( EMouseCursor::Type& OutCursor ) const
{
	if (UPhotoGraphyToolBase* PhotoGraphyTool = Cast<UPhotoGraphyToolBase>(GetCurrentTool()))
	{
		return PhotoGraphyTool->GetCursor(OutCursor);
	}
	return false;
}

void UPhotoGraphyEditorMode::ActivateDefaultTool()
{
	GetInteractiveToolsContext()->StartTool(PhotoGraphyGenerateToolName);
	Super::ActivateDefaultTool();
}

#undef LOCTEXT_NAMESPACE
