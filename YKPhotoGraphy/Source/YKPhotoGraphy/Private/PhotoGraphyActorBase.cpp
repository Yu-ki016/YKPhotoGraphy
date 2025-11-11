// Fill out your copyright notice in the Description page of Project Settings.

#include "PhotoGraphyActorBase.h"
#include "PhotoGraphyComponent.h"

// Sets default values
APhotoGraphyActorBase::APhotoGraphyActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	PhotoGraphyComponent = CreateDefaultSubobject<UPhotoGraphyComponent>(TEXT("PhotoGraphyComponent"));
}