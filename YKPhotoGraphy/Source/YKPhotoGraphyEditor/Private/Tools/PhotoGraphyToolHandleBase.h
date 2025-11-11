// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhotoGraphyComponent.h"
#include "UObject/Object.h"
#include "PhotoGraphyToolHandleBase.generated.h"

struct FInputDeviceRay;
class IToolsContextRenderAPI;
class FToolDataVisualizer;
class UPhotoGraphyInteractiveMechanic;

constexpr float RotationDistance = 0.3f;
const FColor HighlightColor = FColor::Red;
const FColor DefaultLineColor = FColor::White;
const FLinearColor AdditionLineColor = FLinearColor::Blue;
constexpr float DiscSize = 0.1f;
constexpr float HighlightDiscSize = 0.12f;
constexpr float ThickLine = 3.0f;
constexpr float ThinLine = 1.5f;

namespace EPGHandle
{
	enum EHandleDirection
	{
		None			= 0,
		Up				= 1,
		Down			= 2,
		Left			= 3,
		Right			= 4,
		UpLeft			= 5,
		UpRight			= 6,
		DownLeft		= 7,
		DownRight		= 8,
	};

	enum EHandleType
	{
		PivotPoint		= 0,
		RotateDir		= 1,
		Ellipse			= 2,
		OuterEllipse	= 3,
		Rect			= 4,
		OuterRect		= 5,
		InnerRect		= 6,
		Side			= 7,
	};
}



struct FPhotoGraphyHandlePoint
{
	FIntPoint Position = FIntPoint::ZeroValue;
	EPGHandle::EHandleType Type;
	EPGHandle::EHandleDirection Dir = EPGHandle::None;
	bool bSelected = false;
	bool Visible = true;
};

UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyToolHandleBase : public UObject
{
	GENERATED_BODY()
	
public:
	TWeakObjectPtr<UPhotoGraphyInteractiveMechanic> OwnerMechanic;
	
	bool bShapeModified = false;
	FPhotoGraphyShapeModel ShapeModel;
	
	TArray<FPhotoGraphyHandlePoint*> AllHandles;
	FPhotoGraphyHandlePoint* SelectedHandle = nullptr;
	
	virtual void Initialize(UPhotoGraphyInteractiveMechanic* Mechanic);
	virtual void UpdateHandlePosition(const FIntPoint InViewSize){}
	virtual void Draw(FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI){}
	virtual void ProcessBeginDrag(const FInputDeviceRay& DragPos){}
	virtual void ProcessDrag(const FInputDeviceRay& DragPos){}
	virtual bool HitTest(const FInputDeviceRay& PressPos);
	
	FPhotoGraphyShapeModel GetShapeModel() const;
	void SetShapeModel( const FPhotoGraphyShapeModel &NewShapeModel);
	
	static FVector2D GetHandleOffsetDir( EPGHandle::EHandleDirection Type);
	static const FPhotoGraphyHandlePoint* FindHandleByDir(const TArray<FPhotoGraphyHandlePoint>& InHandles,  EPGHandle::EHandleDirection Dir);
	
protected:
	UPROPERTY()
	TObjectPtr<UMaterial> EditorDisc;
};
