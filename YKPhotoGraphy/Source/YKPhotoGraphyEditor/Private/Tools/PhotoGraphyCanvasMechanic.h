// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionMechanic.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
#include "PhotoGraphyCanvasMechanic.generated.h"

class UPhotoGraphyToolBase;
class UPhotoGraphyToolSettings;
/**
 * 
 */
UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyCanvasMechanic
	: public UInteractionMechanic
	, public IKeyInputBehaviorTarget
	, public IClickDragBehaviorTarget
	, public IMouseWheelBehaviorTarget
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<UPhotoGraphyToolBase> PhotoGraphyTool;
	
	bool bSpaceBarPressed = false;
	// bool bCtrlPressed = false;
	// bool bShiftPressed = false;

	FVector2D BeginCanvasOffset;
	FVector2D BeginCanvasZoom;
	float BeginCanvasRotation;
	
	FVector2D BeginDragPos;
	
	UPhotoGraphyToolBase* GetPhotoGraphyTool();
	UPhotoGraphyToolSettings* GetToolSettings();

	bool IsMoveMode() const;
	bool IsZoomMode() const;
	bool IsRotateMode() const;

	bool GetCursor(EMouseCursor::Type& OutCursor) const;

	void ProcessMoveCanvas(const FVector2D MousePos, const FIntPoint ViewPortSize);
	void ProcessZoomCanvas(const FVector2D MousePos, const FIntPoint ViewPortSize);
	void ProcessRotationCanvas(const FVector2D MousePos, const FIntPoint ViewPortSize);
	
	/** UInteractionMechanic overrides */
	virtual void Setup(UInteractiveTool* InParentTool) override;
	virtual void Shutdown() override;
	
	/** IClickDragBehaviorTarget overrides */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
	virtual void OnTerminateDragSequence() override;

	/** IMouseWheelBehaviorTarget overrides */
	virtual FInputRayHit ShouldRespondToMouseWheel(const FInputDeviceRay& CurrentPos) override;
	virtual void OnMouseWheelScrollUp(const FInputDeviceRay& CurrentPos) override;
	virtual void OnMouseWheelScrollDown(const FInputDeviceRay& CurrentPos) override;

	/** IKeyInputBehaviorTarget overrides */
	virtual void OnKeyPressed(const FKey& KeyID) override;
	virtual void OnKeyReleased(const FKey& KeyID) override;
	
};
