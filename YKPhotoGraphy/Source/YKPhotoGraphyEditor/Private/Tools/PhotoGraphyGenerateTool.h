// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PhotoGraphyToolBase.h"
#include "PhotoGraphyGenerateTool.generated.h"

class APhotoGraphyActorBase;
/**
 * Builder for UYKToolsSimpleTool
 */
UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyGenerateToolBuilder : public UPhotoGraphyToolBaseToolBuilder
{
	GENERATED_BODY()

public:
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};



/**
 * Settings UObject for UYKToolsSimpleTool. This UClass inherits from UInteractiveToolPropertySet,
 * which provides an OnModified delegate that the Tool will listen to for changes in property values.
 */
UCLASS(Transient)
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyGenerateToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UPhotoGraphyGenerateToolProperties();

	UPROPERTY(EditAnywhere, Category = "Create")
	TSubclassOf<APhotoGraphyActorBase> ActorClass;
	
	/** The object to bind PostProcess Material to, which can be PostProcessVolume or CameraActor.
	 *  绑定PostProcess Material的对象，可以是PostProcessVolume或者CameraActor
	 */
	UPROPERTY(EditAnywhere, Category = "Create", meta = (DisplayName = "PostProcess Actor"))
	TWeakObjectPtr<AActor> PostProcessActor;

};


UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyGenerateTool : public UPhotoGraphyToolBase
{
	GENERATED_BODY()

public:
	UPhotoGraphyGenerateTool();

	virtual void SetWorld(UWorld* World);
	void GeneratePhotoGraphyActor() const;

	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UPhotoGraphyGenerateToolProperties> ToolProperties;


protected:
	UPROPERTY()
	TObjectPtr<UWorld> TargetWorld = nullptr;
};