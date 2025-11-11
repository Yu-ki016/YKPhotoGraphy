#pragma once
#include "InputState.h"
#include "PhotoGraphyToolHandleBase.h"

#include "PhotoGraphyEllipseToolHandle.generated.h"

struct FViewCanvas;
struct FInputDeviceRay;

UCLASS()
class UPhotoGraphyEllipseToolHandle : public UPhotoGraphyToolHandleBase
{
	GENERATED_BODY()

public:
	
	FPhotoGraphyHandlePoint Pivot;
	FPhotoGraphyHandlePoint Rotation;
	TArray<FPhotoGraphyHandlePoint> Ellipses;
	TArray<FPhotoGraphyHandlePoint> OuterEllipses;
	TArray<FPhotoGraphyHandlePoint> Rects;
	
	virtual void Initialize(UPhotoGraphyInteractiveMechanic* Mechanic) override;
	virtual void UpdateHandlePosition(const FIntPoint InViewSize) override;
	virtual void Draw(FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI) override;
	virtual bool HitTest(const FInputDeviceRay& PressPos) override;
	virtual void ProcessDrag(const FInputDeviceRay& DragPos) override;
	
	static FColor GetHandleColor(EPGHandle::EHandleType Type);
	
	void DrawEllipses(FToolDataVisualizer* Renderer, const FSceneView* View, const FViewCanvas& InViewCanvas) const;
	void DrawRect(FToolDataVisualizer* Renderer, const FSceneView* View, FIntPoint InViewSize) const;

	void ProcessPivot(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessRotation(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessScale(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessFade(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	
};