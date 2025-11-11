#pragma once
#include "PhotoGraphyToolHandleBase.h"

#include "PhotoGraphyGradientToolHandle.generated.h"

struct FViewCanvas;

UCLASS()
class UPhotoGraphyGradientToolHandle : public UPhotoGraphyToolHandleBase
{
	GENERATED_BODY()
public:

	FPhotoGraphyHandlePoint Pivot;
	FPhotoGraphyHandlePoint RotationAndFade;
	TArray<FPhotoGraphyHandlePoint> Sides;

	virtual void Initialize(UPhotoGraphyInteractiveMechanic* Mechanic) override;
	virtual void UpdateHandlePosition(const FIntPoint InViewSize) override;
	virtual void Draw(FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI) override;
	virtual bool HitTest(const FInputDeviceRay& PressPos) override;
	virtual void ProcessDrag(const FInputDeviceRay& DragPos) override;

	void ProcessPivot(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessRotation(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	
protected:
	void DrawLine(FToolDataVisualizer* Renderer, const FSceneView* View, FIntPoint InViewSize) const;
};
