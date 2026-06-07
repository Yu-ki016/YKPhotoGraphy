#include "PhotoGraphyGradientToolHandle.h"

#include "PhotoGraphyDetialPanel.h"
#include "PhotoGraphyInteractiveMechanic.h"
#include "PhotoGraphyUtils.h"

void UPhotoGraphyGradientToolHandle::Initialize( UPhotoGraphyInteractiveMechanic* Mechanic )
{
	Super::Initialize(Mechanic);
	
	Pivot.Type = EPGHandle::PivotPoint;
	Pivot.Dir = EPGHandle::None;

	RotationAndFade.Type = EPGHandle::RotateDir;
	RotationAndFade.Dir = EPGHandle::None;

	Sides.SetNum(2);
	for ( int i = 0; i < 2; i++)
	{
		EPGHandle::EHandleDirection SideDir = static_cast<EPGHandle::EHandleDirection>(EPGHandle::Left + i);
		Sides[i].Type = EPGHandle::Side;
		Sides[i].Dir = SideDir;
		Sides[i].Visible = false;
	}

	AllHandles.Empty();
	AllHandles.Add(&Pivot);
	AllHandles.Add(&RotationAndFade);
	for (FPhotoGraphyHandlePoint& Handle : Sides)
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

void UPhotoGraphyGradientToolHandle::UpdateHandlePosition( const FIntPoint InViewSize )
{
	FIntPoint HalfViewSize = InViewSize / 2;
	// const float AspectRationInv = static_cast<float>(InViewSize.Y) / InViewSize.X;

	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();

	const FVector2D PivotPos = ShapeModel.GradientShape.PivotPos;
	const float RotationAngle = ShapeModel.GradientShape.RotationAngle;
	const float Fade = ShapeModel.GradientShape.Fade;

	const FIntPoint CenterPosInt = FPhotoGraphyUtils::NormalizedUVToScreenPosition(PivotPos, InViewSize);
	Pivot.Position = CenterPosInt;

	RotationAndFade.Position = Pivot.Position +
		FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(0, -Fade * HalfViewSize.Y), RotationAngle);

	for (FPhotoGraphyHandlePoint& Handle : Sides)
	{
		FVector2D Offset = FVector2D::ZeroVector;
		switch (Handle.Dir)
		{
		case EPGHandle::Left:	Offset = FVector2D(-0.5f, 0.0f); break;
		case EPGHandle::Right:	Offset = FVector2D(0.5f, 0.0f); break;
		default: ;
		}
		Offset = FPhotoGraphyUtils::Rotate2DPoint(Offset, RotationAngle);
		Handle.Position = FPhotoGraphyUtils::NormalizedUVToScreenPosition(PivotPos + Offset, InViewSize);
	}
	
	for (auto Handle : AllHandles)
	{
		Handle->Position = FPhotoGraphyUtils::ApplyCanvasTransformForPoint(Handle->Position, InViewSize, ViewCanvas);
	}
}

void UPhotoGraphyGradientToolHandle::DrawLine( FToolDataVisualizer* Renderer, const FSceneView* View, FIntPoint InViewSize ) const
{
	float K = FPhotoGraphyUtils::GetFOVFixFactor(View);
	float ThickLineWidth = K * ThickLine;
	FVector PivotPosition = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Pivot.Position, InViewSize);
	FVector RotationPosition = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, RotationAndFade.Position, InViewSize);
	Renderer->DrawLine(PivotPosition, RotationPosition, DefaultLineColor, ThickLineWidth, false);

	FVector SideLeft = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Sides[0].Position, InViewSize);
	FVector SideRight= FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Sides[1].Position, InViewSize);
	Renderer->DrawLine(SideLeft, SideRight, DefaultLineColor, ThickLineWidth, false);
}

void UPhotoGraphyGradientToolHandle::Draw( FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI )
{
	const FSceneView* View = RenderAPI->GetSceneView();
	if (!View) return;
	const FIntPoint ViewSizeInt = View->UnscaledViewRect.Size();
	const FVector ViewDirection = View->GetViewDirection();
	UpdateHandlePosition(ViewSizeInt);

	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();

	DrawLine(Renderer, View, ViewSizeInt);

	float K = FPhotoGraphyUtils::GetFOVFixFactor(View) * HandleSize;
	float TriSize = 0.2f * K;

	for (const FPhotoGraphyHandlePoint* Handle : AllHandles)
	{
		if (!Handle) continue;
		if (!Handle->Visible) continue;
		float Size = DiscSize;
		FColor Color = FColor::White;
		if (Handle->bSelected)
		{
			Size = HighlightDiscSize;
			Color = HighlightColor;
		}
		Size *= K;

		if (Handle->Type == EPGHandle::RotateDir)
		{
			const float RotationAngle = ShapeModel.GradientShape.RotationAngle;
			const FVector ViewDirectionInv = -View->GetViewDirection();
			FVector EllipseWorldDirX = View->GetViewRight().RotateAngleAxis(RotationAngle + ViewCanvas.CanvasRotation, ViewDirectionInv);
			EllipseWorldDirX.Normalize();
			FVector EllipseWorldDirY = View->GetViewUp().RotateAngleAxis(RotationAngle + ViewCanvas.CanvasRotation, ViewDirectionInv);
			EllipseWorldDirY.Normalize();
			
			FPhotoGraphyUtils::DrawTriangle(RenderAPI->GetPrimitiveDrawInterface(),
				FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Handle->Position, ViewSizeInt),
				EllipseWorldDirY, EllipseWorldDirX, TriSize,
				Color, EditorDisc->GetRenderProxy(), SDPG_Foreground);
		}
		else
		{
			Renderer->DrawDisc(FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Handle->Position, ViewSizeInt),
			ViewDirection, Size, 16, Color, EditorDisc->GetRenderProxy(), false);
		}
	}

}

bool UPhotoGraphyGradientToolHandle::HitTest( const FInputDeviceRay& PressPos )
{
	return Super::HitTest(PressPos);
}

void UPhotoGraphyGradientToolHandle::ProcessPivot( FVector2D MousePos, FIntPoint InViewSize,
	const FViewCanvas& InViewCanvas )
{
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);
	ShapeModel.GradientShape.PivotPos = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedMousePos, InViewSize);
}

void UPhotoGraphyGradientToolHandle::ProcessRotation( FVector2D MousePos, FIntPoint InViewSize,
	const FViewCanvas& InViewCanvas )
{
	const float AspectRatio = static_cast<float>(InViewSize.X) / InViewSize.Y;
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);
	FVector2D RotatedMouseUV = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedMousePos, InViewSize) - ShapeModel.GradientShape.PivotPos;
	// RotatedMouseUV.X *= AspectRatio;
	ShapeModel.GradientShape.RotationAngle = FPhotoGraphyUtils::ComputeRotationAngle(FVector2D(0, -1), RotatedMouseUV);
	ShapeModel.GradientShape.Fade = FMath::Max(RotatedMouseUV.Size(), 0);
}

void UPhotoGraphyGradientToolHandle::ProcessDrag( const FInputDeviceRay& DragPos )
{
	const FSceneView* View = OwnerMechanic.IsValid() ? OwnerMechanic->LastActiveSceneView : nullptr;
	if (!View) return ;
	const FIntPoint ViewSize = View->UnscaledViewRect.Size();

	if (!SelectedHandle) return;
	FVector2D MousePos2D = DragPos.ScreenPosition;
	MousePos2D -= View->UnscaledViewRect.Min;

	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();

	switch (SelectedHandle->Type)
	{
		case EPGHandle::PivotPoint: ProcessPivot(MousePos2D, ViewSize, ViewCanvas); break;
		case EPGHandle::RotateDir: ProcessRotation(MousePos2D, ViewSize, ViewCanvas); break;
		default: return;
	}
	
	bShapeModified = true;
	UpdateHandlePosition(ViewSize);
}
