// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "PhotoGraphyActorBase.generated.h"

class UPhotoGraphyComponent;

UCLASS(Abstract, NotBlueprintable)
class YKPHOTOGRAPHY_API APhotoGraphyActorBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoGraphy")
	TObjectPtr<UPhotoGraphyComponent> PhotoGraphyComponent;
	APhotoGraphyActorBase();
};
