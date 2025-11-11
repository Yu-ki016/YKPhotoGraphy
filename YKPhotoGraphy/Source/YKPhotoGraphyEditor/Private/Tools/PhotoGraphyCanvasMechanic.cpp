// Fill out your copyright notice in the Description page of Project Settings.

#include "PhotoGraphyCanvasMechanic.h"

#include "PhotoGraphyDetialPanel.h"
#include "PhotoGraphyEditorMode.h"
#include "PhotoGraphyToolBase.h"
#include "PhotoGraphyUtils.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseBehaviors/KeyInputBehavior.h"
#include "BaseBehaviors/MouseWheelBehavior.h"

void UPhotoGraphyCanvasMechanic::Setup( UInteractiveTool* InParentTool )
{
	Super::Setup(InParentTool);

	UKeyInputBehavior* KeyBehavior = NewObject<UKeyInputBehavior>(this);
	TArray<FKey> SupportedKeys;
	SupportedKeys.Add(EKeys::SpaceBar);
	KeyBehavior->Initialize(this, SupportedKeys);
	KeyBehavior->bRequireAllKeys = false;
	KeyBehavior->SetDefaultPriority(0);
	InParentTool->AddInputBehavior(KeyBehavior);

	UClickDragInputBehavior* DragBehavior = NewObject<UClickDragInputBehavior>(this);
	DragBehavior->Initialize(this);
	DragBehavior->SetDefaultPriority(1);
	InParentTool->AddInputBehavior(DragBehavior);

	UMouseWheelInputBehavior* MouseWheelBehavior = NewObject<UMouseWheelInputBehavior>(this);
	MouseWheelBehavior->Initialize(this);
	MouseWheelBehavior->SetDefaultPriority(2);
	InParentTool->AddInputBehavior(MouseWheelBehavior);

	GetPhotoGraphyTool();
}

void UPhotoGraphyCanvasMechanic::Shutdown()
{
	PhotoGraphyTool = nullptr;
	Super::Shutdown();
}

UPhotoGraphyToolBase* UPhotoGraphyCanvasMechanic::GetPhotoGraphyTool()
{
	if (PhotoGraphyTool.IsValid()) return PhotoGraphyTool.Get();

	if (UInteractiveTool* CurrentTool = UPhotoGraphyEditorMode::GetPhotoGraphyEditorMode()->GetCurrentTool())
	{
		PhotoGraphyTool = Cast<UPhotoGraphyToolBase>(CurrentTool);
	}

	return PhotoGraphyTool.IsValid() ? PhotoGraphyTool.Get() : nullptr;
}

UPhotoGraphyToolSettings* UPhotoGraphyCanvasMechanic::GetToolSettings()
{
	if (UPhotoGraphyToolBase* Tool = GetPhotoGraphyTool())
	{
		return  Tool->GetToolSettings();
	}
	
	return nullptr;
}

bool UPhotoGraphyCanvasMechanic::IsMoveMode() const
{
	FModifierKeysState Mods = FSlateApplication::Get().GetModifierKeys();
	return bSpaceBarPressed && !Mods.IsControlDown() && !Mods.IsShiftDown();
}

bool UPhotoGraphyCanvasMechanic::IsZoomMode() const
{
	FModifierKeysState Mods = FSlateApplication::Get().GetModifierKeys();
	return bSpaceBarPressed && Mods.IsControlDown() && !Mods.IsShiftDown();
}

bool UPhotoGraphyCanvasMechanic::IsRotateMode() const
{
	FModifierKeysState Mods = FSlateApplication::Get().GetModifierKeys();
	return bSpaceBarPressed && !Mods.IsControlDown() && Mods.IsShiftDown();
}

bool UPhotoGraphyCanvasMechanic::GetCursor( EMouseCursor::Type& OutCursor ) const
{
	if (bSpaceBarPressed)
	{
		if (IsMoveMode())    
		{
			OutCursor = EMouseCursor::Type::CardinalCross;
			return true;
		}
		if (IsZoomMode())
		{
			OutCursor = EMouseCursor::Type::ResizeUpDown;
			return true;
		}
		if (IsRotateMode())
		{
			OutCursor = EMouseCursor::Type::Hand;
			return true;
		}
	}
	return false;
}

FInputRayHit UPhotoGraphyCanvasMechanic::CanBeginClickDragSequence( const FInputDeviceRay& PressPos )
{
	FInputRayHit Hit;
	Hit.bHit = false;
	if (bSpaceBarPressed)
	{
		Hit.bHit = true;
	}
	return Hit;
}

void UPhotoGraphyCanvasMechanic::ProcessMoveCanvas( const FVector2D MousePos, const FIntPoint ViewPortSize )
{
	UPhotoGraphyToolSettings* ToolSettings = GetToolSettings();
	if (!ToolSettings) return;
	
	FVector2D MouseOffset = MousePos - BeginDragPos;
	FVector2D CanvasOffset = BeginCanvasOffset;
	CanvasOffset += MouseOffset / ViewPortSize;
	
	ToolSettings->ViewCanvas.CanvasOffset = CanvasOffset; 
	ToolSettings->UpdateViewCanvas();
}

void UPhotoGraphyCanvasMechanic::ProcessZoomCanvas( const FVector2D MousePos, const FIntPoint ViewPortSize )
{
	UPhotoGraphyToolSettings* ToolSettings = GetToolSettings();
	if (!ToolSettings) return;
	
	const float BeginZoomRatio = FMath::Abs(BeginCanvasZoom.Y) > 0.0001f ? BeginCanvasZoom.X / BeginCanvasZoom.Y : 0;
	
	FVector2D MouseOffset = (MousePos - BeginDragPos) / ViewPortSize;
	FVector2D CanvasZoom = BeginCanvasZoom;
	FVector2D ZoomFactor = FVector2D(-MouseOffset.Y * 2);
	ZoomFactor *= FVector2D(BeginZoomRatio, 1.0f);
	CanvasZoom += ZoomFactor;

	const FVector2D BeginMouseCanvasUV = FPhotoGraphyUtils::ScreenPositionToCanvasUV(
		FIntPoint(BeginDragPos.X, BeginDragPos.Y), ViewPortSize, BeginCanvasOffset, BeginCanvasZoom, BeginCanvasRotation);
	FVector2D MouseToCanvasPivot = -BeginMouseCanvasUV;
	MouseToCanvasPivot *= ZoomFactor;
	FVector2D CanvasOffset = MouseToCanvasPivot + BeginCanvasOffset;

	ToolSettings->ViewCanvas.CanvasZoom = CanvasZoom;
	ToolSettings->ViewCanvas.CanvasOffset = CanvasOffset;
	ToolSettings->UpdateViewCanvas();
}

void UPhotoGraphyCanvasMechanic::ProcessRotationCanvas( const FVector2D MousePos, const FIntPoint ViewPortSize )
{
	UPhotoGraphyToolSettings* ToolSettings = GetToolSettings();
	if (!ToolSettings) return;

	const FVector2D BeginMouseCanvasUV = FPhotoGraphyUtils::ScreenPositionToCanvasUV(
		FIntPoint(BeginDragPos.X, BeginDragPos.Y), ViewPortSize, BeginCanvasOffset, BeginCanvasZoom, BeginCanvasRotation);
	const FVector2D MouseCanvasUV = FPhotoGraphyUtils::ScreenPositionToCanvasUV(
		FIntPoint(MousePos.X, MousePos.Y), ViewPortSize, BeginCanvasOffset, BeginCanvasZoom, BeginCanvasRotation);

	float BeginRotationAngle = FPhotoGraphyUtils::ComputeRotationAngle(FVector2D(0, -1), BeginMouseCanvasUV);
	float RotationAngle = FPhotoGraphyUtils::ComputeRotationAngle(FVector2D(0, -1), MouseCanvasUV);
	float CanvasRotation = BeginCanvasRotation;
	CanvasRotation += RotationAngle - BeginRotationAngle;
	ToolSettings->ViewCanvas.CanvasRotation = CanvasRotation;
	ToolSettings->UpdateViewCanvas();
}

void UPhotoGraphyCanvasMechanic::OnClickPress( const FInputDeviceRay& PressPos )
{
	BeginDragPos = PressPos.ScreenPosition;

	UPhotoGraphyToolSettings* ToolSettings = GetToolSettings();
	if (!ToolSettings) return;
	ToolSettings->Modify();
	FViewCanvas ViewCanvas = ToolSettings->GetViewCanvas();
	BeginCanvasOffset = ViewCanvas.CanvasOffset;
	BeginCanvasZoom = ViewCanvas.CanvasZoom;
	BeginCanvasRotation = ViewCanvas.CanvasRotation;
}

void UPhotoGraphyCanvasMechanic::OnClickDrag( const FInputDeviceRay& DragPos )
{
	if (!bSpaceBarPressed) return;
	
	const FViewport* Viewport = FPhotoGraphyUtils::GetActiveViewport();
	if (!Viewport) return;
	const FIntPoint ViewSize = Viewport->GetSizeXY();

	FVector2D MousePos = DragPos.ScreenPosition;
	
	if (IsMoveMode())
	{
		ProcessMoveCanvas(MousePos, ViewSize);
		return;
	}
	if (IsZoomMode())
	{
		ProcessZoomCanvas(MousePos, ViewSize);
		return;
	}
	if (IsRotateMode())
	{
		ProcessRotationCanvas(MousePos, ViewSize);
	}
	
}

void UPhotoGraphyCanvasMechanic::OnClickRelease( const FInputDeviceRay& ReleasePos )
{
	
}

void UPhotoGraphyCanvasMechanic::OnTerminateDragSequence()
{
}

FInputRayHit UPhotoGraphyCanvasMechanic::ShouldRespondToMouseWheel( const FInputDeviceRay& CurrentPos )
{
	FInputRayHit Hit;
	Hit.bHit = false;
	return Hit;
}

void UPhotoGraphyCanvasMechanic::OnMouseWheelScrollUp( const FInputDeviceRay& CurrentPos )
{
}

void UPhotoGraphyCanvasMechanic::OnMouseWheelScrollDown( const FInputDeviceRay& CurrentPos )
{
}

void UPhotoGraphyCanvasMechanic::OnKeyPressed( const FKey& KeyID )
{
	if (KeyID == EKeys::SpaceBar) bSpaceBarPressed = true;
	FSlateApplication::Get().QueryCursor();
	UE_LOG(LogTemp, Log, TEXT("OnKeyPressed:%1s"), *KeyID.GetDisplayName().ToString());
	IKeyInputBehaviorTarget::OnKeyPressed(KeyID);
}

void UPhotoGraphyCanvasMechanic::OnKeyReleased( const FKey& KeyID )
{
	if (KeyID == EKeys::SpaceBar) bSpaceBarPressed = false;
	FSlateApplication::Get().QueryCursor();
	UE_LOG(LogTemp, Log, TEXT("OnKeyReleased:%1s"), *KeyID.GetDisplayName().ToString());
	IKeyInputBehaviorTarget::OnKeyReleased(KeyID);
}
