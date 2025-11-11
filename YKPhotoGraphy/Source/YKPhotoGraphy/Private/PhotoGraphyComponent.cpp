// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoGraphyComponent.h"


// Sets default values for this component's properties
UPhotoGraphyComponent::UPhotoGraphyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
	UActorComponent::SetComponentTickEnabled(false);
	// ...
}


// Called when the game starts
void UPhotoGraphyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPhotoGraphyComponent::TickComponent( float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction )
{
	if (OnPhotoGraphyComponentTick.IsBound())
	{
		FEditorScriptExecutionGuard ScriptGuard;
		OnPhotoGraphyComponentTick.Broadcast(DeltaTime);
	}
}

void UPhotoGraphyComponent::PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent )
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	// static const FName NAME_ShapeModel(TEXT("ShapeModel"));
	//
	// if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode())
	// {
	// 	const FProperty* MemberProperty = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue();
	// 	if (MemberProperty && MemberProperty->GetFName() == NAME_ShapeModel)
	// 	{
	// 		OnShapeModelUpdated.Broadcast(ShapeModel, EShapeModelUpdateSource::Default);
	// 	}
	// }
}

void UPhotoGraphyComponent::PostInitProperties()
{
	Super::PostInitProperties();
}

void UPhotoGraphyComponent::SetShapeModel( const FPhotoGraphyShapeModel& NewModel )
{
	if (ShapeModel == NewModel) return;
	
	ShapeModel = NewModel;
	
#if WITH_EDITOR
	Modify();
	MarkPackageDirty();
#endif
	
	if (OnPostShapeModelChanged.IsBound())
	{
		FEditorScriptExecutionGuard ScriptGuard;
		OnPostShapeModelChanged.Broadcast(NewModel);
	}
}