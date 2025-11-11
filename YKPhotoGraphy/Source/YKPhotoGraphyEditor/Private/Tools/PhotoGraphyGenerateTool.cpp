// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhotoGraphyGenerateTool.h"
#include "InteractiveToolManager.h"
#include "CollisionQueryParams.h"
#include "PhotoGraphyActor.h"
#include "PhotoGraphyActorBase.h"
#include "Engine/World.h"

// localization namespace
#define LOCTEXT_NAMESPACE "YKToolsSimpleTool"

/*
 * ToolBuilder implementation
 */

UInteractiveTool* UPhotoGraphyGenerateToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPhotoGraphyGenerateTool* NewTool = NewObject<UPhotoGraphyGenerateTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}



/*
 * ToolProperties implementation
 */

UPhotoGraphyGenerateToolProperties::UPhotoGraphyGenerateToolProperties()
{
}


/*
 * Tool implementation
 */

UPhotoGraphyGenerateTool::UPhotoGraphyGenerateTool()
{
}


void UPhotoGraphyGenerateTool::SetWorld(UWorld* World)
{
	this->TargetWorld = World;
}

void UPhotoGraphyGenerateTool::GeneratePhotoGraphyActor() const
{
	if (!ToolProperties) return;
	if (!ToolProperties->PostProcessActor.IsValid() || !ToolProperties->ActorClass.Get()) return;
	AActor* NewActor = TargetWorld->SpawnActor<AActor>(ToolProperties->ActorClass.Get(), FTransform::Identity);
	if (APhotoGraphyActor* PhotoGraphyActor = Cast<APhotoGraphyActor>(NewActor))
	{
		PhotoGraphyActor->PostProcessActor = ToolProperties->PostProcessActor.Get();
		PhotoGraphyActor->BindingToPostProcessActor();
	}
	
	UE_LOG(LogTemp, Log, TEXT("Spawned actor: %s"), *NewActor->GetName());
	
}

void UPhotoGraphyGenerateTool::Setup()
{
	Super::Setup();

	ToolProperties = NewObject<UPhotoGraphyGenerateToolProperties>(this);
	AddToolPropertySource(ToolProperties);
}

void UPhotoGraphyGenerateTool::Shutdown( EToolShutdownType ShutdownType )
{
	TargetWorld = nullptr;
	Super::Shutdown(ShutdownType);
}


#undef LOCTEXT_NAMESPACE