#include "PhotoGraphyUtils.h"

#include "DynamicMeshBuilder.h"
#include "PhotoGraphyDetialPanel.h"
#include "ToolDataVisualizer.h"

FViewport* FPhotoGraphyUtils::GetActiveViewport()
{
	if (GEditor && GEditor->GetActiveViewport())
	{
		return GEditor->GetActiveViewport();
	}
	return nullptr;
}

FVector2D FPhotoGraphyUtils::Rotate2DPoint( const FVector2D& Point, float AngleDegrees, bool bClockwise,
                                           const FVector2D& Pivot )
{
	const float SignedAngle = bClockwise ? AngleDegrees : -AngleDegrees;
	const float Rad = FMath::DegreesToRadians(SignedAngle);
	const float CosA = FMath::Cos(Rad);
	const float SinA = FMath::Sin(Rad);

	const FVector2D Local = Point - Pivot;
	return FVector2D(
		Local.X * CosA - Local.Y * SinA,
		Local.X * SinA + Local.Y * CosA
	) + Pivot;
}

FVector2D FPhotoGraphyUtils::Rotate2DPoint( const FVector2D& Point, float AngleDegrees, float Aspect, bool bClockwise,
	const FVector2D& Pivot )
{
	const float SignedAngle = bClockwise ? AngleDegrees : -AngleDegrees;
	const float Rad = FMath::DegreesToRadians(SignedAngle);
	const float CosA = FMath::Cos(Rad);
	const float SinA = FMath::Sin(Rad);

	FVector2D Local = Point - Pivot;
	Local.X *= Aspect;
	Local = FVector2D(
		Local.X * CosA - Local.Y * SinA,
		Local.X * SinA + Local.Y * CosA
	);
	
	Local.X /= Aspect;
	return Local + Pivot;
}

FIntPoint FPhotoGraphyUtils::Rotate2DPointInt( const FIntPoint& Point, float AngleDegrees, bool bClockwise,
                                               const FIntPoint& Pivot )
{
	const float SignedAngle = bClockwise ? AngleDegrees : -AngleDegrees;
	// 转为弧度
	const float AngleRad = FMath::DegreesToRadians(SignedAngle);

	// 计算 cos/sin
	const float CosA = FMath::Cos(AngleRad);
	const float SinA = FMath::Sin(AngleRad);

	// 平移到原点
	const float X = Point.X - Pivot.X;
	const float Y = Point.Y - Pivot.Y;

	// 旋转公式
	const float RotX = X * CosA - Y * SinA;
	const float RotY = X * SinA + Y * CosA;

	// 平移回去并四舍五入
	return FIntPoint(
		FMath::RoundToInt(RotX + Pivot.X),
		FMath::RoundToInt(RotY + Pivot.Y)
	);
}

FIntPoint FPhotoGraphyUtils::GetCanvasPivot( const FVector2D CanvasOffset, const FIntPoint InViewSize )
{
	FVector2D CanvasPivotUV = CanvasOffset * 2;
	CanvasPivotUV.Y *= -1.0f;
	return NormalizedUVToScreenPosition(CanvasPivotUV, InViewSize);
}

FVector FPhotoGraphyUtils::GetCanvasPivotWorldPosition( const FSceneView* InView, const FVector2D CanvasOffset)
{
	FVector2D CanvasPivotUV = CanvasOffset * 2;
	CanvasPivotUV.Y *= -1.0f;
	return NormalizedUVToWorldPosition(InView, CanvasPivotUV);
}

FVector2D FPhotoGraphyUtils::ScreenPositionToCanvasUV( const FIntPoint ScreenPosition, const FIntPoint InViewSize,
                                                       FVector2D CanvasOffset, FVector2D CanvasZoom, float CanvasRotation )
{
	FIntPoint CanvasPivot = GetCanvasPivot(CanvasOffset, InViewSize);
	FVector2D P = ScreenPosition - CanvasPivot;
	P = Rotate2DPointInt(FIntPoint(P.X, P.Y), -CanvasRotation, true);
	P /= CanvasZoom * InViewSize;
	return P;
}

FIntPoint FPhotoGraphyUtils::ApplyCanvasTransformForPoint( const FIntPoint ScreenPosition, const FIntPoint InViewSize, const FViewCanvas InViewCanvas )
{
	FIntPoint ViewCenter = FIntPoint(InViewSize.X * 0.5, InViewSize.Y * 0.5);
	FIntPoint P = ScreenPosition - ViewCenter;
	P.X *= InViewCanvas.CanvasZoom.X;
	P.Y *= InViewCanvas.CanvasZoom.Y;
	P = Rotate2DPointInt(P, InViewCanvas.CanvasRotation, true);
	P += FIntPoint(InViewSize.X * InViewCanvas.CanvasOffset.X, InViewSize.Y * InViewCanvas.CanvasOffset.Y);
	P += ViewCenter;
	return P;
}

FIntPoint FPhotoGraphyUtils::RevertCanvasTransformForPoint( const FIntPoint ScreenPosition, const FIntPoint InViewSize,
	const FViewCanvas InViewCanvas )
{
	FIntPoint ViewCenter = FIntPoint(InViewSize.X * 0.5, InViewSize.Y * 0.5);
	FIntPoint P = ScreenPosition - ViewCenter;
	P -= FIntPoint(InViewSize.X * InViewCanvas.CanvasOffset.X, InViewSize.Y * InViewCanvas.CanvasOffset.Y);
	P = Rotate2DPointInt(P, -InViewCanvas.CanvasRotation, true);
	P.X /= InViewCanvas.CanvasZoom.X;
	P.Y /= InViewCanvas.CanvasZoom.Y;
	P += ViewCenter;
	return P;
}

FVector2D FPhotoGraphyUtils::ApplyCanvasTransformForNormalizedUV( const FVector2D InUV, const FIntPoint InViewSize,
                                                                  const FViewCanvas InViewCanvas )
{
	float AspectRatio = static_cast<float>(InViewSize.X) / InViewSize.Y;
	// AspectRatio *=InViewCanvas.CanvasZoom.X / InViewCanvas.CanvasZoom.Y;

	FVector2D UV = InUV * InViewCanvas.CanvasZoom;
	UV = Rotate2DPoint(UV, InViewCanvas.CanvasRotation, 1.0f, true);
	UV += InViewCanvas.CanvasOffset * 2.0f * FVector2D(AspectRatio, 1.0f);// * FVector2D(1.0f, -1.0f);
	return UV;
}

FIntPoint FPhotoGraphyUtils::NormalizedUVToScreenPosition( const FVector2D NormalizedUV, const FIntPoint InViewSize )
{
	const float AspectRationInv = static_cast<float>(InViewSize.Y) / InViewSize.X;
	FVector2D ScreenUV = NormalizedUV * FVector2D(AspectRationInv, 1.0f);
	ScreenUV = ScreenUV * 0.5f + FVector2D(0.5f, 0.5f);
	return FIntPoint(
		FMath::RoundToInt(ScreenUV.X * InViewSize.X),
		FMath::RoundToInt(ScreenUV.Y * InViewSize.Y));
}

FVector2D FPhotoGraphyUtils::ScreenPositionToNormalizedUV( const FVector2D ScreenPosition, const FIntPoint InViewSize )
{
	const float AspectRation = static_cast<float>(InViewSize.X) / InViewSize.Y;
	FVector2D ScreenUV = ScreenPosition / InViewSize;
	ScreenUV = ScreenUV * 2.0f - FVector2D(1.0f, 1.0f);
	ScreenUV.X *= AspectRation;
	return ScreenUV;
}

FVector FPhotoGraphyUtils::ScreenPositionToWorldPosition( const FSceneView* InView, FIntPoint ScreenPosition, FIntPoint InViewSize )
{
	return NormalizedUVToWorldPosition(InView, ScreenPositionToNormalizedUV(ScreenPosition, InViewSize));
}

FVector FPhotoGraphyUtils::NormalizedUVToWorldPosition( const FSceneView* InView, FVector2D NormalizedUV )
{
	// when z = 0.0f, we get a point on the far plane
	// when z = 1.0f, we get a point on the near plane
	return NormalizedUVToWorldPosition(InView, FVector(NormalizedUV.X, NormalizedUV.Y, 0.9f), true);
}

FVector FPhotoGraphyUtils::NormalizedUVToWorldPosition( const FSceneView* InView, const FVector& NormalizedUV, bool bFixAspect )
{
	FVector OutPosition = FVector::ZeroVector;

	if (!InView) return OutPosition;
	const FIntPoint ViewSizeInt = InView->UnscaledViewRect.Size();
	const float AspectRationInv = bFixAspect ? static_cast<float>(ViewSizeInt.Y) / ViewSizeInt.X : 1.0f;
	const FMatrix InvProjectionMatrix	= InView->ViewMatrices.GetInvProjectionMatrix();
	const FMatrix InvViewMatrix			= InView->ViewMatrices.GetInvViewMatrix();

	const FVector4 ProjectPos = FVector4(NormalizedUV.X * AspectRationInv, -NormalizedUV.Y, NormalizedUV.Z, 1.0f);
	const FVector4 HScreenViewSpacePos = InvProjectionMatrix.TransformFVector4(ProjectPos);
	FVector ScreenViewSpacePos(HScreenViewSpacePos.X, HScreenViewSpacePos.Y, HScreenViewSpacePos.Z);
	if (HScreenViewSpacePos.W != 0.0f)
	{
		// 应用透视除法
		ScreenViewSpacePos /= HScreenViewSpacePos.W;
	}
	
	OutPosition = InvViewMatrix.TransformPosition(ScreenViewSpacePos);
	return OutPosition;
}

FVector2D FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation( const FVector2D ScreenPosition,
	const FVector2D PivotUV, const FIntPoint InViewSize, const float Rotation)
{
	FVector2D UV = ScreenPositionToNormalizedUV(ScreenPosition, InViewSize);
	UV = Rotate2DPoint(UV, -Rotation, true, PivotUV);
	return UV;
}

void FPhotoGraphyUtils::DrawEllipse( FToolDataVisualizer* Renderer, const FVector& Base, const FVector& X,
                                     const FVector& Y, const FLinearColor& Color, float Radius1, float Radius2, int32 NumSides, float Thickness,
                                     bool bDepthTestedIn )
{
	const float	AngleDelta = 2.0f * PI / NumSides;
	FVector	LastVertex = Base + X * Radius1;

	for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
	{
		const FVector Vertex = Base + (X * FMath::Cos(AngleDelta * (SideIndex + 1)) * Radius1 + Y * FMath::Sin(AngleDelta * (SideIndex + 1)) * Radius2);
		Renderer->DrawLine(LastVertex, Vertex, Color, Thickness, bDepthTestedIn);
		LastVertex = Vertex;
	}
}

FVector FPhotoGraphyUtils::ScalePointByScreenAxis( FVector P, FVector O, FVector Forward, FVector AxisX, FVector AxisY, FVector2D Zoom )
{
	FVector O2P = P - O;
	
	float Z = FVector::DotProduct(O2P, Forward);
	float X = FVector::DotProduct(O2P, AxisX);
	float Y = FVector::DotProduct(O2P, AxisY);

	X *= Zoom.X;
	Y *= Zoom.Y;

	return O + Forward * Z + AxisX * X + AxisY * Y;
}

void FPhotoGraphyUtils::DrawEllipse( FToolDataVisualizer* Renderer, const FVector& Base, const FVector& X,
                                     const FVector& Y, const FLinearColor& Color, float Radius1, float Radius2, int32 NumSides, float Thickness,
                                     bool bDepthTestedIn, const FViewCanvas& InViewCanvas, const FSceneView* InView )
{
	if (!Renderer) return;
	if (!InView) return;

	FVector ScreenDir = InView->GetViewDirection();
	FVector ScreenX = InView->GetViewRight().RotateAngleAxis(InViewCanvas.CanvasRotation, -ScreenDir);
	FVector ScreenY = InView->GetViewUp().RotateAngleAxis(InViewCanvas.CanvasRotation, -ScreenDir);
	// FVector ViewCenter = InView->ViewLocation;
	// FVector2D ViewSize = InView->UnconstrainedViewRect.Size();
	FVector CanvasCenter = GetCanvasPivotWorldPosition(InView, InViewCanvas.CanvasOffset);
	
	const float	AngleDelta = 2.0f * PI / NumSides;
	FVector	LastVertex = Base + X * Radius1;
	LastVertex = ScalePointByScreenAxis(LastVertex, Base, ScreenDir, ScreenX, ScreenY, InViewCanvas.CanvasZoom);

	for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
	{
		float Angle = AngleDelta * (SideIndex + 1);
		
		// 原始局部坐标
		float x = FMath::Cos(Angle) * Radius1;
		float y = FMath::Sin(Angle) * Radius2;

		FVector Vertex = Base + X * x  + Y * y;
		Vertex = ScalePointByScreenAxis(Vertex, Base, ScreenDir, ScreenX, ScreenY, InViewCanvas.CanvasZoom);

		Renderer->DrawLine(LastVertex, Vertex, Color, Thickness, bDepthTestedIn);
		LastVertex = Vertex;
	}
}

void FPhotoGraphyUtils::DrawTriangle( FPrimitiveDrawInterface* PDI, const FVector& Center, const FVector& XAxis,
	const FVector& YAxis, const float Radius, const FColor Color, const FMaterialRenderProxy* MaterialRenderProxy,
	uint8 DepthPriority )
{
	if (!PDI) return;
	if (!MaterialRenderProxy) return;
	
	constexpr int32 NumSides = 3;
	constexpr float AngleDelta = 2.0f * UE_PI / NumSides;

	FVector ZAxis = (XAxis ^ YAxis).GetSafeNormal();

	FDynamicMeshBuilder MeshBuilder(PDI->View->GetFeatureLevel());

	TArray<int32> VertexIndices;
	for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
	{
		float Angle = AngleDelta * SideIndex;
		FVector VertexPos = Center + (XAxis * FMath::Cos(Angle) + YAxis * FMath::Sin(Angle)) * Radius;

		FDynamicMeshVertex MeshVertex;
		MeshVertex.Position = static_cast<FVector3f>(VertexPos);
		MeshVertex.Color = Color;
		MeshVertex.TextureCoordinate[0] = FVector2f(
			0.5f + 0.5f * FMath::Cos(Angle),
			0.5f + 0.5f * FMath::Sin(Angle)
		);
		MeshVertex.SetTangents(
			static_cast<FVector3f>(-ZAxis),
			FVector3f((XAxis ^ ZAxis).GetSafeNormal()),
			static_cast<FVector3f>(ZAxis)
		);

		VertexIndices.Add(MeshBuilder.AddVertex(MeshVertex));
	}

	MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
	MeshBuilder.Draw(PDI, FMatrix::Identity, MaterialRenderProxy, DepthPriority, 0.f);
}

float FPhotoGraphyUtils::ComputeRotationAngle( const FVector2D& Point, const FVector2D& RotatedPoint, bool bClockwise,
                                               const FVector2D& Pivot )
{
	const FVector2D V1 = Point - Pivot;
	const FVector2D V2 = RotatedPoint - Pivot;

	// 计算叉积和点积
	const float Cross = V1.X * V2.Y - V1.Y * V2.X;
	const float Dot   = V1.X * V2.X + V1.Y * V2.Y;

	// atan2 返回 [-pi, pi]，转为角度
	float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(Cross, Dot));

	if (!bClockwise)
	{
		AngleDeg = -AngleDeg;
	}

	return AngleDeg;
}

float FPhotoGraphyUtils::DistanceToLine( const FVector2D& P, const FVector2D& A, const FVector2D& B )
{
	FVector2D AB = B - A;
	FVector2D AP = P - A;

	float Cross = AB.X * AP.Y - AB.Y * AP.X;

	// 距离 = |叉积| / |AB|
	float ABLength = AB.Size();
	if (ABLength < KINDA_SMALL_NUMBER)
	{
		// 避免除0，退化为点
		return (P - A).Size();
	}

	return FMath::Abs(Cross) / ABLength;
}

FVector2D  FPhotoGraphyUtils::ProjectPointToLine( const FVector2D& P, const FVector2D& A, const FVector2D& B, float ProjectGap )
{
	FVector2D AP = P - A;
	FVector2D AB = B - A;

	float AB2 = FVector2D::DotProduct(AB, AB);
	if (AB2 < KINDA_SMALL_NUMBER)
		return A; // avoid divide by zero

	float t = FVector2D::DotProduct(AP, AB) / AB2;
	FVector2D ProjectedPoint = A + AB * t;
	FVector2D ProjectDir = (ProjectedPoint - P).GetSafeNormal();
	
	return ProjectedPoint - ProjectDir * ProjectGap;
}

float FPhotoGraphyUtils::GetFOVFixFactor( const FSceneView* InView, float ScaleFactor )
{
	const float HFOV = FMath::Atan(1.0f / InView->ViewMatrices.GetProjectionMatrix().M[0][0]) * 2.0f;
	return ScaleFactor * (2.0f * FMath::Tan(HFOV * 0.5f));
}
bool FPhotoGraphyUtils::IsActiveViewport( IToolsContextRenderAPI* RenderAPI )
{
	return EnumHasAllFlags(RenderAPI->GetViewInteractionState(), EViewInteractionState::Focused);
}
