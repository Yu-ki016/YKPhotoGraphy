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

#if WITH_EDITOR

	static const FName NAME_ShapeModel(TEXT("ShapeModel"));
	UClass* ComponentClass = GetClass();
	FProperty* ShapeModelProperty = ComponentClass->FindPropertyByName(NAME_ShapeModel);
	
	if (ShapeModelProperty)
	{
		Modify();
		
		FEditPropertyChain PropertyChain;
		PropertyChain.AddTail(ShapeModelProperty);
		PropertyChain.SetActivePropertyNode(ShapeModelProperty);
		
		// 广播OnPreObjectPropertyChanged委托，Sequencer会监听这个委托来实现自动k帧
		FCoreUObjectDelegates::OnPreObjectPropertyChanged.Broadcast(this, PropertyChain);
	}
#endif
	
	ShapeModel = NewModel;
	
#if WITH_EDITOR
	if (ShapeModelProperty)
	{
		FPropertyChangedEvent PropertyChangedEvent(ShapeModelProperty, EPropertyChangeType::ValueSet);
		
		// 广播OnObjectPropertyChanged委托，Sequencer会监听这个委托来实现自动k帧
		FCoreUObjectDelegates::OnObjectPropertyChanged.Broadcast(this, PropertyChangedEvent);
		
		MarkPackageDirty();
	}
#endif
	
	if (OnPostShapeModelChanged.IsBound())
	{
		FEditorScriptExecutionGuard ScriptGuard;
		OnPostShapeModelChanged.Broadcast(NewModel);
	}
}