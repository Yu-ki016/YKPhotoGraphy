#include "PhotoGraphyEllipseToolHandle.h"

#include "PhotoGraphyInteractiveMechanic.h"
#include "PhotoGraphyUtils.h"
#include "ToolContextInterfaces.h"
#include "ToolDataVisualizer.h"
#include "PhotoGraphyComponent.h"
#include "PhotoGraphyDetialPanel.h"

FColor UPhotoGraphyEllipseToolHandle::GetHandleColor( EPGHandle::EHandleType Type )
{
	switch (Type)
	{
	case EPGHandle::Ellipse:		return FColor(30, 127, 255);
	case EPGHandle::OuterEllipse:	return FColor(220, 76, 255);
	case EPGHandle::Rect:			return FColor(30, 127, 255);
	case EPGHandle::OuterRect:		return FColor(220, 76, 255);
	case EPGHandle::RotateDir:		return FColor(30, 127, 255);
	case EPGHandle::PivotPoint:		return  FColor(50, 50, 50);
	default:						return FColor::Orange;
	}
}

FColor GetHandleOffsetColor( EPGHandle::EHandleDirection Type )
{
	switch (Type)
	{
	case EPGHandle::Up:				return FColor::Black;
	case EPGHandle::Down:			return FColor::Red;
	case EPGHandle::Left:			return FColor::Green;
	case EPGHandle::Right:			return FColor::Blue;
	case EPGHandle::UpLeft:			return FColor::Yellow;
	case EPGHandle::UpRight:		return FColor::Cyan;
	case EPGHandle::DownLeft:		return FColor::Magenta;
	case EPGHandle::DownRight:		return FColor::Orange;
	default:						return FColor::White;
	}
}

void UPhotoGraphyEllipseToolHandle::Initialize(UPhotoGraphyInteractiveMechanic* Mechanic)
{
	Super::Initialize(Mechanic);
	
	Pivot.Type = EPGHandle::PivotPoint;
	Pivot.Dir = EPGHandle::None;

	Rotation.Type = EPGHandle::RotateDir;
	Rotation.Dir = EPGHandle::None;
	
	Ellipses.SetNum(4);
	OuterEllipses.SetNum(4);
	Rects.SetNum(4);
	for (int i = 0; i < 4; i++)
	{
		EPGHandle::EHandleDirection EllipseDir = static_cast<EPGHandle::EHandleDirection>(EPGHandle::Up + i);
		
		Ellipses[i].Type = EPGHandle::Ellipse;
		Ellipses[i].Dir = EllipseDir;

		OuterEllipses[i].Type = EPGHandle::OuterEllipse;
		OuterEllipses[i].Dir = EllipseDir;

		EPGHandle::EHandleDirection RectType = static_cast<EPGHandle::EHandleDirection>(EPGHandle::UpLeft + i);
		Rects[i].Type = EPGHandle::Rect;
		Rects[i].Dir = RectType;
	}

	AllHandles.Empty();
	AllHandles.Add(&Pivot);
	AllHandles.Add(&Rotation);
	for (FPhotoGraphyHandlePoint& Handle : Ellipses)
	{
		AllHandles.Add(&Handle);
	}
	for (FPhotoGraphyHandlePoint& Handle : OuterEllipses)
	{
		AllHandles.Add(&Handle);
	}
	for (FPhotoGraphyHandlePoint& Handle : Rects)
	{
		AllHandles.Add(&Handle);
	}
	
	FIntPoint ViewSize(1920, 1080);
	if (GEditor && GEditor->GetActiveViewport())
	{
		ViewSize = GEditor->GetActiveViewport()->GetSizeXY();
	}
	UpdateHandlePosition(ViewSize);
	
}

void UPhotoGraphyEllipseToolHandle::UpdateHandlePosition(const FIntPoint InViewSize)
{
	FIntPoint HalfViewSize = InViewSize / 2;
	const float AspectRationInv = static_cast<float>(InViewSize.Y) / InViewSize.X;
	
	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();

	const FVector2D PivotPos = ShapeModel.EllipseShape.PivotPos;
	const float RotationAngle = ShapeModel.EllipseShape.RotationAngle;
	const float RadiusX = ShapeModel.EllipseShape.RadiusX;
	const float RadiusY = ShapeModel.EllipseShape.RadiusY;
	const float Fade = ShapeModel.EllipseShape.Fade;
	
	const FIntPoint CenterPosInt = FPhotoGraphyUtils::NormalizedUVToScreenPosition(PivotPos, InViewSize);
	Pivot.Position = CenterPosInt;
	
	FVector2D RotationVector = FPhotoGraphyUtils::Rotate2DPoint(FVector2D(0, -RotationDistance), RotationAngle);
	Rotation.Position = Pivot.Position + FIntPoint(RotationVector.X * HalfViewSize.Y, RotationVector.Y * HalfViewSize.Y);
	
	for (int i = 0; i < 4; i++)
	{
		EPGHandle::EHandleDirection EllipseDir = static_cast<EPGHandle::EHandleDirection>(EPGHandle::Up + i);

		FVector2D InnerOffset = GetHandleOffsetDir(EllipseDir) * FVector2D(RadiusX, RadiusY);
		Ellipses[i].Position = Pivot.Position +
			FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(InnerOffset.X * HalfViewSize.X * AspectRationInv, InnerOffset.Y * HalfViewSize.Y), RotationAngle);

		FVector2D OuterOffset = InnerOffset * (1.0f + Fade);
		OuterEllipses[i].Position = Pivot.Position +
			FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(OuterOffset.X * HalfViewSize.X * AspectRationInv, OuterOffset.Y * HalfViewSize.Y), RotationAngle);

		EPGHandle::EHandleDirection RectType = static_cast<EPGHandle::EHandleDirection>(EPGHandle::UpLeft + i);
		FVector2D RectOffset = GetHandleOffsetDir(RectType) * FVector2D(RadiusX, RadiusY);
		Rects[i].Position = Pivot.Position +
			FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(RectOffset.X * HalfViewSize.X * AspectRationInv, RectOffset.Y * HalfViewSize.Y), RotationAngle);
	}

	for (auto Handle : AllHandles)
	{
		Handle->Position = FPhotoGraphyUtils::ApplyCanvasTransformForPoint(Handle->Position, InViewSize, ViewCanvas);
	}
}

void UPhotoGraphyEllipseToolHandle::DrawEllipses( FToolDataVisualizer* Renderer , const FSceneView* View, const FViewCanvas& InViewCanvas) const
{
	if (!View) return;
	
	FVector2D PivotPos = ShapeModel.EllipseShape.PivotPos;
	// PivotPos.Y *= -1;
	const float RotationAngle = ShapeModel.EllipseShape.RotationAngle;
	const float RadiusX = ShapeModel.EllipseShape.RadiusX;
	const float RadiusY = ShapeModel.EllipseShape.RadiusY;
	const float Fade = ShapeModel.EllipseShape.Fade;
	
	FIntPoint ViewSize = View->UnconstrainedViewRect.Size();
	FVector2D PivotPosInCanvas = FPhotoGraphyUtils::ApplyCanvasTransformForNormalizedUV(PivotPos, ViewSize, InViewCanvas);
	FVector PivotWorldPos_Canvas = FPhotoGraphyUtils::NormalizedUVToWorldPosition(View, PivotPosInCanvas);
	FVector PivotWorldPos_Origin = FPhotoGraphyUtils::NormalizedUVToWorldPosition(View, PivotPos);

	const FVector ViewDirectionInv = -View->GetViewDirection();
	FVector EllipseWorldDirX = View->GetViewRight().RotateAngleAxis(RotationAngle + InViewCanvas.CanvasRotation, ViewDirectionInv);
	EllipseWorldDirX.Normalize();
	FVector EllipseWorldDirY = View->GetViewUp().RotateAngleAxis(RotationAngle + InViewCanvas.CanvasRotation, ViewDirectionInv);
	EllipseWorldDirY.Normalize();
	// const float AspectRationInv = static_cast<float>(ViewSize.Y) / ViewSize.X;

	const FVector EllipseEdgeX = FPhotoGraphyUtils::NormalizedUVToWorldPosition(View, PivotPos + FVector2D(RadiusX, 0));
	const FVector EllipseEdgeY = FPhotoGraphyUtils::NormalizedUVToWorldPosition(View, PivotPos + FVector2D(0, RadiusY));
	const float EllipseRadiusX = FVector::Distance(PivotWorldPos_Origin, EllipseEdgeX);
	const float EllipseRadiusY = FVector::Distance(PivotWorldPos_Origin, EllipseEdgeY);

	float K = FPhotoGraphyUtils::GetFOVFixFactor(View);
	float ThickLineWidth = K * ThickLine;
	float ThinLineWidth = K * ThinLine;
	
	FPhotoGraphyUtils::DrawEllipse(Renderer, PivotWorldPos_Canvas, EllipseWorldDirX, EllipseWorldDirY,
		FLinearColor::White, EllipseRadiusX, EllipseRadiusY, 64, ThickLineWidth, false, InViewCanvas, View);
	
	const float OutEllipseRadiusX = EllipseRadiusX * FMath::Clamp(1.0f + Fade, 0.0f, 2.0f);
	const float OutEllipseRadiusY = EllipseRadiusY * FMath::Clamp(1.0f + Fade, 0.0f, 2.0f);
	FPhotoGraphyUtils::DrawEllipse(Renderer, PivotWorldPos_Canvas, EllipseWorldDirX, EllipseWorldDirY,
		FLinearColor::White, OutEllipseRadiusX, OutEllipseRadiusY, 64, ThinLineWidth, false, InViewCanvas, View);
	
	const float InEllipseRadiusX = EllipseRadiusX * FMath::Clamp(1.0f - Fade, 0.0f, 2.0f);
	const float InEllipseRadiusY = EllipseRadiusY * FMath::Clamp(1.0f - Fade, 0.0f, 2.0f);
	FPhotoGraphyUtils::DrawEllipse(Renderer, PivotWorldPos_Canvas, EllipseWorldDirX, EllipseWorldDirY,
		FLinearColor::White, InEllipseRadiusX, InEllipseRadiusY, 64, ThinLineWidth, false, InViewCanvas, View);

}

void UPhotoGraphyEllipseToolHandle::DrawRect( FToolDataVisualizer* Renderer, const FSceneView* View, FIntPoint InViewSize) const
{
	float K = FPhotoGraphyUtils::GetFOVFixFactor(View);
	float ThinLineWidth = K * ThinLine;
	
	FVector C00 = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Rects[0].Position, InViewSize);
	FVector C01 = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Rects[1].Position, InViewSize);
	FVector C10 = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Rects[2].Position, InViewSize);
	FVector C11 = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Rects[3].Position, InViewSize);
	Renderer->DrawLine(C00, C10, AdditionLineColor, ThinLineWidth, false);
	Renderer->DrawLine(C10, C11, AdditionLineColor, ThinLineWidth, false);
	Renderer->DrawLine(C11, C01, AdditionLineColor, ThinLineWidth, false);
	Renderer->DrawLine(C01, C00, AdditionLineColor, ThinLineWidth, false);

	FVector PivotPosition = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Pivot.Position, InViewSize);
	FVector RotationPosition = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Rotation.Position, InViewSize);
	Renderer->DrawLine(PivotPosition, RotationPosition, FLinearColor::White, ThinLineWidth, false);
}

void UPhotoGraphyEllipseToolHandle::Draw( FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI)
{
	const FSceneView* View = RenderAPI->GetSceneView();
	if (!View) return;
	const FIntPoint ViewSizeInt = View->UnconstrainedViewRect.Size();
	const FVector ViewDirection = View->GetViewDirection();
	UpdateHandlePosition(ViewSizeInt);

	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();
	
	DrawEllipses(Renderer, View, ViewCanvas);
	DrawRect(Renderer, View, ViewSizeInt);

	float K = FPhotoGraphyUtils::GetFOVFixFactor(View);
	float Size = K * DiscSize;
	float HighlightSize = K * HighlightDiscSize;
	
	for (const FPhotoGraphyHandlePoint* Handle : AllHandles)
	{
		constexpr int Steps = 16;
		if (!Handle) continue;
		if (Handle->bSelected)
		{
			Renderer->DrawDisc(FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Handle->Position, ViewSizeInt),
			ViewDirection, HighlightSize, Steps, HighlightColor, EditorDisc->GetRenderProxy(), false);
		}
		else
		{
			Renderer->DrawDisc(FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Handle->Position, ViewSizeInt),
			ViewDirection, Size, Steps, DefaultLineColor, EditorDisc->GetRenderProxy(), false);
			Renderer->DrawCircle(FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Handle->Position, ViewSizeInt),
				ViewDirection, HighlightSize, Steps, GetHandleColor(Handle->Type), ThinLine, false);
		}
	}
}

bool UPhotoGraphyEllipseToolHandle::HitTest( const FInputDeviceRay& PressPos )
{
	return Super::HitTest(PressPos);
}

void UPhotoGraphyEllipseToolHandle::ProcessPivot(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas)
{
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);
	ShapeModel.EllipseShape.PivotPos = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedMousePos, InViewSize);
}

void UPhotoGraphyEllipseToolHandle::ProcessRotation( FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas )
{
	// const float AspectRatio = static_cast<float>(InViewSize.X) / InViewSize.Y;
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);
	FVector2D RotatedMouseUV = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedMousePos, InViewSize) - ShapeModel.EllipseShape.PivotPos;
	// RotatedMouseUV.X *= AspectRatio;
	ShapeModel.EllipseShape.RotationAngle = FPhotoGraphyUtils::ComputeRotationAngle(FVector2D(0, -1), RotatedMouseUV);
}

void UPhotoGraphyEllipseToolHandle::ProcessScale(FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas)
{
	const float AspectRatio = static_cast<float>(InViewSize.X) / InViewSize.Y;
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
			InViewSize, InViewCanvas);
	FIntPoint CenterPosInt = FPhotoGraphyUtils::NormalizedUVToScreenPosition(ShapeModel.EllipseShape.PivotPos, InViewSize);
	FVector2D MouseUV = RevertedMousePos - CenterPosInt ;
	MouseUV = FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(MouseUV.X, MouseUV.Y), -ShapeModel.EllipseShape.RotationAngle);
	MouseUV = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(MouseUV + CenterPosInt, InViewSize);
	MouseUV -= ShapeModel.EllipseShape.PivotPos;
	
	FVector2D OffsetDir = GetHandleOffsetDir(SelectedHandle->Dir);

	if (OffsetDir.X != 0) ShapeModel.EllipseShape.RadiusX = FMath::Abs(MouseUV.X);
	if (OffsetDir.Y != 0) ShapeModel.EllipseShape.RadiusY = FMath::Abs(MouseUV.Y);
}


void UPhotoGraphyEllipseToolHandle::ProcessFade( FVector2D MousePos, FIntPoint InViewSize, const FViewCanvas& InViewCanvas)
{
	const float AspectRatio = static_cast<float>(InViewSize.X) / InViewSize.Y;
	FIntPoint CenterPosInt = FPhotoGraphyUtils::NormalizedUVToScreenPosition(ShapeModel.EllipseShape.PivotPos, InViewSize);
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
			InViewSize, InViewCanvas);
	FVector2D MouseUV = RevertedMousePos - CenterPosInt ;
	MouseUV = FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(MouseUV.X, MouseUV.Y), -ShapeModel.EllipseShape.RotationAngle);
	MouseUV = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(MouseUV + CenterPosInt, InViewSize);
	MouseUV -= ShapeModel.EllipseShape.PivotPos;
	FVector2D OffsetDir = GetHandleOffsetDir(SelectedHandle->Dir);
	if (OffsetDir.X != 0) ShapeModel.EllipseShape.Fade = FMath::Clamp(FMath::Abs(MouseUV.X) / ShapeModel.EllipseShape.RadiusX - 1.0f, 0.0f, 1.0f);
	if (OffsetDir.Y != 0) ShapeModel.EllipseShape.Fade = FMath::Clamp(FMath::Abs(MouseUV.Y) / ShapeModel.EllipseShape.RadiusY - 1.0f, 0.0f, 1.0f);
}

void UPhotoGraphyEllipseToolHandle::ProcessDrag( const FInputDeviceRay& DragPos )
{
	const FViewport* Viewport = FPhotoGraphyUtils::GetActiveViewport();
	if (!Viewport) return;
	const FIntPoint ViewSize = Viewport->GetSizeXY();

	if (!SelectedHandle) return;
	const FVector2D MousePos2D = DragPos.ScreenPosition;

	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();

	switch (SelectedHandle->Type)
	{
		case EPGHandle::PivotPoint:		ProcessPivot(MousePos2D, ViewSize, ViewCanvas); break;
		case EPGHandle::RotateDir:		ProcessRotation(MousePos2D, ViewSize, ViewCanvas); break;
		case EPGHandle::Ellipse:		ProcessScale(MousePos2D, ViewSize, ViewCanvas); break;
		case EPGHandle::Rect:			ProcessScale(MousePos2D, ViewSize, ViewCanvas); break;
		case EPGHandle::OuterEllipse:	ProcessFade(MousePos2D, ViewSize, ViewCanvas); break;
		default: return;
	}
	
	bShapeModified = true;
	UpdateHandlePosition(ViewSize);
}
