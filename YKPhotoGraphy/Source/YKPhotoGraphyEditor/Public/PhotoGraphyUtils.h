#pragma once

#include "InteractiveToolManager.h"

struct FViewCanvas;
class FToolDataVisualizer;

class FPhotoGraphyUtils
{
public:
	static FViewport* GetActiveViewport();

	static FVector2D Rotate2DPoint(const FVector2D& Point, float AngleDegrees, bool bClockwise = true, const FVector2D& Pivot = FVector2D::ZeroVector);
	static FVector2D Rotate2DPoint(const FVector2D& Point, float AngleDegrees, float Aspect, bool bClockwise = true, const FVector2D& Pivot = FVector2D::ZeroVector);
	static FIntPoint Rotate2DPointInt(const FIntPoint& Point, float AngleDegrees, bool bClockwise = true, const FIntPoint& Pivot = FIntPoint::ZeroValue);

	static FIntPoint GetCanvasPivot(const FVector2D CanvasOffset, const FIntPoint InViewSize);
	static FVector GetCanvasPivotWorldPosition( const FSceneView* InView, const FVector2D CanvasOffset);
	static FVector2D ScreenPositionToCanvasUV(const FIntPoint ScreenPosition, const FIntPoint InViewSize,
		FVector2D CanvasOffset, FVector2D CanvasZoom, float CanvasRotation);

	// Canvas空间我没有对长宽比进行缩放，屏幕中间为(0, 0)，屏幕左上角为(-0.5, -0.5)，屏幕右下角为(0.5, 0.5)
	static FIntPoint ApplyCanvasTransformForPoint(const FIntPoint ScreenPosition, const FIntPoint InViewSize, const FViewCanvas InViewCanvas);
	static FIntPoint RevertCanvasTransformForPoint(const FIntPoint ScreenPosition, const FIntPoint InViewSize, const FViewCanvas InViewCanvas);
	static FVector2D ApplyCanvasTransformForNormalizedUV(const FVector2D InUV, const FIntPoint InViewSize, const FViewCanvas InViewCanvas);

	// ScreenPosition以屏幕左上角为原点(0, 0)，屏幕右下角坐标为屏幕像素(ViewMaxX, ViewMaxY)
	// NormalizedUV原本应该是屏幕中间为原点(0, 0), 屏幕左上角(-1.0, -1.0), 屏幕右下角(1.0, 1.0)
	// 但是，我希望长宽比保持不变，比如当设置半径为(0.5，0.5)时，出现的是一个圆形而不是椭圆
	// 因此，真正的NormalizedUV屏幕左上角为(-ViewX/ViewY, -1.0), 屏幕右下角(ViewX/ViewY, 1.0)
	static FIntPoint NormalizedUVToScreenPosition(const FVector2D NormalizedUV, const FIntPoint InViewSize);
	static FVector2D ScreenPositionToNormalizedUV(const FVector2D ScreenPosition, const FIntPoint InViewSize);
	static FVector ScreenPositionToWorldPosition(const FSceneView* InView, FIntPoint ScreenPosition, FIntPoint InViewSize);
	static FVector NormalizedUVToWorldPosition( const FSceneView* InView, FVector2D NormalizedUV);
	static FVector NormalizedUVToWorldPosition( const FSceneView* InView, const FVector& NormalizedUV, bool bFixAspect);

	static FVector2D ScreenPositionToNormalizedUVFixRotation(const FVector2D ScreenPosition, const FVector2D PivotUV,
		const FIntPoint InViewSize, float Rotation);

	static void DrawEllipse(FToolDataVisualizer* Renderer, const FVector& Base, const FVector& X, const FVector& Y,
							const FLinearColor& Color, float Radius1, float Radius2, int32 NumSides, float Thickness, bool bDepthTestedIn);

	static FVector ScalePointByScreenAxis( FVector P, FVector O, FVector Forward, FVector AxisX, FVector AxisY, FVector2D Zoom );
	static void DrawEllipse(FToolDataVisualizer* Renderer, const FVector& Base, const FVector& X, const FVector& Y,
							const FLinearColor& Color, float Radius1, float Radius2, int32 NumSides, float Thickness,
							bool bDepthTestedIn, const FViewCanvas& InViewCanvas, const FSceneView* InView );

	static void DrawTriangle(FPrimitiveDrawInterface* PDI, const FVector& Center,
		const FVector& XAxis, const FVector& YAxis, float Radius,
		FColor Color, const FMaterialRenderProxy* MaterialRenderProxy, uint8 DepthPriority);

	static float ComputeRotationAngle(const FVector2D& Point, const FVector2D& RotatedPoint,
		bool bClockwise = true, const FVector2D& Pivot = FVector2D::ZeroVector);

	static float DistanceToLine(const FVector2D& P, const FVector2D& A, const FVector2D& B);
	static FVector2D  ProjectPointToLine(const FVector2D& P, const FVector2D& A, const FVector2D& B, float ProjectGap = 0.0f);

	static float GetFOVFixFactor(const FSceneView* InView, float ScaleFactor = 0.6f);

	static bool IsActiveViewport(IToolsContextRenderAPI* RenderAPI);
};
