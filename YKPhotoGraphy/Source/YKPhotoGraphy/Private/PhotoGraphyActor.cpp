// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoGraphyActor.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Library/YKPhotoGraphyBPLibrary.h"


// Sets default values
APhotoGraphyActor::APhotoGraphyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APhotoGraphyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhotoGraphyActor::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

void APhotoGraphyActor::Destroyed()
{
	UE_LOG(LogTemp, Display, TEXT("APhotoGraphyActorBase::Destroyed"));
	UnbindingToPostProcessActor();
	Super::Destroyed();
}


FPostProcessSettings* APhotoGraphyActor::GetPostProcessSettings( AActor* InPostProcessActor )
{
	FPostProcessSettings* PostProcessSetting = nullptr;
	if (APostProcessVolume* PPV = Cast<APostProcessVolume>(InPostProcessActor))
	{
		PostProcessSetting = &PPV->Settings;	
	}
	else if (ACameraActor* CA = Cast<ACameraActor>(InPostProcessActor))
	{
		PostProcessSetting = &CA->GetCameraComponent()->PostProcessSettings;
	}
	return PostProcessSetting;
}


void APhotoGraphyActor::BindingToPostProcessActor()
{
	// if (HasAnyFlags(RF_Transient)) return;;
	if (!PostProcessActor || !PostProcessActor.IsValid()) return;

	FPostProcessSettings* PostProcessSettingPtr = GetPostProcessSettings(PostProcessActor.Get());
	if (!PostProcessSettingPtr) return;
	FPostProcessSettings& PostProcessSetting = *PostProcessSettingPtr;

	DMI = UYKPhotoGraphyBPLibrary::GetOrCreateMID(DMI, TEXT("DMI_PhotoGraphy"), PostProcessMaterial, this);

	if (!DMI) return;
	
	bool bDMIInPostProcess = false;
	for (auto WeightedBlendable : PostProcessSetting.WeightedBlendables.Array)
	{
		if (WeightedBlendable.Object == DMI)
		{
			bDMIInPostProcess = true;
			break;
		}
	}

	if (!bDMIInPostProcess)
	{
		PostProcessSetting.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, DMI));
	}
	
}


void APhotoGraphyActor::UnbindingToPostProcessActor()
{
	if (DMI && PostProcessActor && PostProcessActor.IsValid())
	{
		FPostProcessSettings* PostProcessSettingPtr = GetPostProcessSettings(PostProcessActor.Get());
		if (!PostProcessSettingPtr) return;
		FPostProcessSettings& PostProcessSetting = *PostProcessSettingPtr;
		int PostMaterialCount = PostProcessSetting.WeightedBlendables.Array.Num();
		for (int i = PostMaterialCount - 1; i >= 0; --i)
		{
			if (PostProcessSetting.WeightedBlendables.Array[i].Object == DMI)
			{
				PostProcessSetting.WeightedBlendables.Array.RemoveAt(i);
			}
		}
	}
}

void APhotoGraphyActor::UpdateBlendablePriority() const
{
	if (!DMI) return;
	DMI->bOverrideBlendablePriority = bOverrideBlendablePriority;
	DMI->BlendablePriorityOverride = BlendablePriority;
}

void APhotoGraphyActor::UpdateBlendableLocation() const
{
	if (!DMI) return;
	DMI->bOverrideBlendableLocation = bOverrideBlendableLocation;
	DMI->BlendableLocationOverride	= BlendableLocation;
}

void APhotoGraphyActor::HandleAffectedActorsChangle()
{
	for (auto AffectedActor : AffectedActors)
	{
		if (!AffectedActor) continue;
		
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		AffectedActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (UPrimitiveComponent* Primitive : PrimitiveComponents)
		{
			Primitive->SetRenderCustomDepth(true);
			Primitive->CustomDepthStencilValue = ActorPickerStencil;
			Primitive->MarkRenderStateDirty();
		}
#if WITH_EDITOR
		AffectedActor->Modify();
#endif
	}
	
	for (auto CachedAffectedActor : CachedAffectedActors)
	{
		if (!CachedAffectedActor) continue;
		
		if (!AffectedActors.Find(CachedAffectedActor))
		{
			TArray<UPrimitiveComponent*> PrimitiveComponents;
			CachedAffectedActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
			for (UPrimitiveComponent* Primitive : PrimitiveComponents)
			{
				Primitive->SetRenderCustomDepth(false);
				Primitive->CustomDepthStencilValue = 0;
				Primitive->MarkRenderStateDirty();
			}
			
#if WITH_EDITOR
			CachedAffectedActor->Modify();
#endif
		}
	}

	CachedAffectedActors = AffectedActors;
}

void APhotoGraphyActor::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == TEXT("bOverrideBlendablePriority")
			|| PropertyChangedEvent.Property->GetFName() == TEXT("BlendablePriority"))
		{
			UpdateBlendablePriority();
		}
		if (PropertyChangedEvent.Property->GetFName() == TEXT("bOverrideBlendableLocation")
			|| PropertyChangedEvent.Property->GetFName() == TEXT("BlendableLocation"))
		{
			UpdateBlendableLocation();
		}
		if (PropertyChangedEvent.Property->GetFName() == TEXT("AffectedActors")
			|| PropertyChangedEvent.Property->GetFName() == TEXT("ActorPickerStencil"))
		{
			HandleAffectedActorsChangle();
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
