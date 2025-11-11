#pragma once
#include "PhotoGraphyToolHandleBase.h"

#include "PhotoGraphyLinearToolHandle.generated.h"

struct FViewCanvas;

UCLASS()
class UPhotoGraphyLinearToolHandle : public UPhotoGraphyToolHandleBase
{
	GENERATED_BODY()
public:
	
	FPhotoGraphyHandlePoint Pivot;
	FPhotoGraphyHandlePoint Rotation;

	TArray<FPhotoGraphyHandlePoint> InnerRects;
	TArray<FPhotoGraphyHandlePoint> Rects;
	TArray<FPhotoGraphyHandlePoint> OuterRects;

	FVector2D BeginMousePos;
	FLinearShape BeginLinearShape;
	FIntPoint BeginA;
	FIntPoint BeginB;
	FIntPoint BeginC;
	FIntPoint BeginD;
	
	virtual void Initialize(UPhotoGraphyInteractiveMechanic* Mechanic) override;
	virtual void UpdateHandlePosition(const FIntPoint InViewSize) override;
	virtual void Draw(FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI) override;
	virtual bool HitTest(const FInputDeviceRay& PressPos) override;
	virtual void ProcessBeginDrag(const FInputDeviceRay& DragPos) override;
	virtual void ProcessDrag(const FInputDeviceRay& DragPos) override;

	void DrawRect(FToolDataVisualizer* Renderer, const FSceneView* View, FIntPoint InViewSize) const;
	
	FVector2D GetRectsOffset(EPGHandle::EHandleDirection Direction) const;
	static bool GetIntersection2D(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D, FVector2D& OutIntersection);
	static FVector2D GetOutRectCornerOffset(const FVector2D& A, const FVector2D& B, const FVector2D& C,
	                                        const FVector2D& NormalAB, const FVector2D& NormalBC, float FadeAB, float FadeBC);

	void ProcessPivot(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessRotation(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessRect(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	void ProcessFade(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas);
	
	static FColor GetHandleColor(EPGHandle::EHandleType InType);
	static bool EnsureRectNormals(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D);
	static bool SignPointSide(const FVector2D& A, const FVector2D& B, const FVector2D& C, float& OutSign, const float EPS = 1e-4);
	static bool ClampRectCorner(const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D, const FVector2D& InBeginA, FVector2D& OutOffset);
};
