// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "PhotoGraphySubsystem.generated.h"

class FPhotoGraphySceneViewExtension;
/**
 * 
 */
UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FPhotoGraphySceneViewExtension* GetPhotoGraphySceneViewExtension() const { return PhotoGraphySceneViewExtension.Get(); }

private:
	TSharedPtr<class FPhotoGraphySceneViewExtension, ESPMode::ThreadSafe> PhotoGraphySceneViewExtension;
};
