// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoGraphyToolHandleBase.h"

#include "PhotoGraphyInteractiveMechanic.h"


void UPhotoGraphyToolHandleBase::Initialize( UPhotoGraphyInteractiveMechanic* Mechanic )
{
	checkf(Mechanic, TEXT("UPhotoGraphyToolHandleBase::Initialize Mechanic is null"));
	
	EditorDisc = static_cast<UMaterial*>(StaticLoadObject(UMaterial::StaticClass(), nullptr,
	TEXT("/YKPhotoGraphy/Material/Editor/EditorDisc.EditorDisc"), nullptr, LOAD_None, nullptr));

	OwnerMechanic = Mechanic;
}

FPhotoGraphyShapeModel UPhotoGraphyToolHandleBase::GetShapeModel() const
{
	return ShapeModel;
}

void UPhotoGraphyToolHandleBase::SetShapeModel( const FPhotoGraphyShapeModel& NewShapeModel )
{
	ShapeModel = NewShapeModel;
}

FVector2D UPhotoGraphyToolHandleBase::GetHandleOffsetDir( EPGHandle::EHandleDirection Type )
{
	switch (Type)
	{
	case EPGHandle::Up:				return FVector2D(0, -1);
	case EPGHandle::Down:			return FVector2D(0, 1);
	case EPGHandle::Left:			return FVector2D(-1, 0);
	case EPGHandle::Right:			return FVector2D(1, 0);
	case EPGHandle::UpLeft:			return FVector2D(-1, -1);
	case EPGHandle::UpRight:		return FVector2D(1, -1);
	case EPGHandle::DownLeft:		return FVector2D(-1, 1);
	case EPGHandle::DownRight:		return FVector2D(1, 1);
	default:						return FVector2D::ZeroVector;
	}
}

const FPhotoGraphyHandlePoint* UPhotoGraphyToolHandleBase::FindHandleByDir(
	const TArray<FPhotoGraphyHandlePoint>& InHandles,
	EPGHandle::EHandleDirection Dir)
{
	return Algo::FindByPredicate(InHandles, [Dir](const FPhotoGraphyHandlePoint& Handle)
	{
		return Handle.Dir == Dir;
	});
}

bool UPhotoGraphyToolHandleBase::HitTest( const FInputDeviceRay& PressPos )
{
	bool bHit = false;
	const FSceneView* View = OwnerMechanic.IsValid() ? OwnerMechanic->LastActiveSceneView : nullptr;
	if (!View) return  false;
	
	FVector2D MousePos2D = PressPos.ScreenPosition;
	MousePos2D -= View->UnscaledViewRect.Min;

	if (SelectedHandle)
	{
		if (FVector2D::Distance(MousePos2D, FVector2D(SelectedHandle->Position.X, SelectedHandle->Position.Y)) < 10.0f)
		{
			bHit = true;
			return bHit;
		}
		SelectedHandle->bSelected = false;
		SelectedHandle = nullptr;
	}
	
	for (FPhotoGraphyHandlePoint* Handle : AllHandles)
	{
		if (!Handle) continue;
		if (!Handle->Visible) continue;
		if (FVector2D::Distance(MousePos2D, FVector2D(Handle->Position.X, Handle->Position.Y)) < 10.0f)
		{
			SelectedHandle = Handle;
			Handle->bSelected = true;
			bHit = true;
			break;
		}
	}
	return bHit;
}
