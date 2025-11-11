// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoGraphySubsystem.h"

#include "PhotoGraphySceneViewExtension.h"
#include "SceneViewExtension.h"

void UPhotoGraphySubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	PhotoGraphySceneViewExtension = FSceneViewExtensions::NewExtension<FPhotoGraphySceneViewExtension>();
	UE_LOG(LogTemp, Log, TEXT("UPhotoGraphySubsystem: Subsystem initialized & SceneViewExtension created"));

}

void UPhotoGraphySubsystem::Deinitialize()
{
	{
		PhotoGraphySceneViewExtension->IsActiveThisFrameFunctions.Empty();

		FSceneViewExtensionIsActiveFunctor IsActiveFunctor;

		IsActiveFunctor.IsActiveFunction = [](const ISceneViewExtension* SceneViewExtension, const FSceneViewExtensionContext& Context)
		{
			return TOptional<bool>(false);
		};

		PhotoGraphySceneViewExtension->IsActiveThisFrameFunctions.Add(IsActiveFunctor);
	}

	PhotoGraphySceneViewExtension.Reset();
	PhotoGraphySceneViewExtension = nullptr;
}
