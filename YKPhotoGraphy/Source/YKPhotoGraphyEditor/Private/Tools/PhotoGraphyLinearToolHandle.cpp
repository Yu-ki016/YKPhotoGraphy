#include "PhotoGraphyLinearToolHandle.h"

#include "PhotoGraphyDetialPanel.h"
#include "PhotoGraphyInteractiveMechanic.h"
#include "PhotoGraphyUtils.h"

void UPhotoGraphyLinearToolHandle::Initialize( UPhotoGraphyInteractiveMechanic* Mechanic )
{
	Super::Initialize(Mechanic);

	Pivot.Type = EPGHandle::PivotPoint;
	Pivot.Dir = EPGHandle::None;

	Rotation.Type = EPGHandle::RotateDir;
	Rotation.Dir = EPGHandle::None;

	Rects.SetNum(8);
	OuterRects.SetNum(8);
	InnerRects.SetNum(4);
	
	for ( int i = 0; i < 8; i++ )
	{
		EPGHandle::EHandleDirection RectDir = static_cast<EPGHandle::EHandleDirection>(EPGHandle::Up + i);
		Rects[i].Type = EPGHandle::Rect;
		Rects[i].Dir = RectDir;

		OuterRects[i].Type = EPGHandle::OuterRect;
		OuterRects[i].Dir = RectDir;
		if (i < 4)
		{
			EPGHandle::EHandleDirection InnerRectDir = static_cast<EPGHandle::EHandleDirection>(EPGHandle::UpLeft + i);
			InnerRects[i].Type = EPGHandle::InnerRect;
			InnerRects[i].Dir = InnerRectDir;
			InnerRects[i].Visible = false;
		}
		else
		{
			OuterRects[i].Visible = false;
		}
	}

	AllHandles.Empty();
	AllHandles.Add(&Pivot);
	AllHandles.Add(&Rotation);

	for (FPhotoGraphyHandlePoint& Handle : Rects)
	{
		AllHandles.Add(&Handle);
	}
	for (FPhotoGraphyHandlePoint& Handle : OuterRects)
	{
		AllHandles.Add(&Handle);
	}
	for (FPhotoGraphyHandlePoint& Handle : InnerRects)
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

FVector2D UPhotoGraphyLinearToolHandle::GetRectsOffset( EPGHandle::EHandleDirection Direction) const
{
	const FVector2D A = ShapeModel.LinearShape.A;
	const FVector2D B = ShapeModel.LinearShape.B;
	const FVector2D C = ShapeModel.LinearShape.C;
	const FVector2D D = ShapeModel.LinearShape.D;

	switch (Direction)
	{
		case EPGHandle::UpLeft:		return A;
		case EPGHandle::UpRight:	return B;
		case EPGHandle::DownRight:	return C;
		case EPGHandle::DownLeft:	return D;
		case EPGHandle::Up:			return (A + B) * 0.5f;
		case EPGHandle::Down:		return (C + D) * 0.5f;
		case EPGHandle::Left:		return (D + A) * 0.5f;
		case EPGHandle::Right:		return (B + C) * 0.5f;
		default: return FVector2D::ZeroVector;
	}
}

FColor UPhotoGraphyLinearToolHandle::GetHandleColor( EPGHandle::EHandleType InType )
{
	switch (InType)
	{
		case EPGHandle::Rect:		return FColor(30, 127, 255);
		case EPGHandle::OuterRect:	return FColor(220, 76, 255);
		case EPGHandle::RotateDir:	return FColor(30, 127, 255);
		case EPGHandle::PivotPoint:	return  FColor(50, 50, 50);
		default: return FColor::White;
	}
}

bool UPhotoGraphyLinearToolHandle::EnsureRectNormals( const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D )
{
	FVector2D AB = B - A;
	FVector2D BC = C - B;
	FVector2D CD = D - C;
	FVector2D DA = A - D;

	float Cross1 = FVector2D::CrossProduct(AB, BC);
	float Cross2 = FVector2D::CrossProduct(BC, CD);
	float Cross3 = FVector2D::CrossProduct(CD, DA);
	float Cross4 = FVector2D::CrossProduct(DA, AB);

	constexpr float EPS = 1e-6f;

	bool AllPositive = (Cross1 > EPS) && (Cross2 > EPS) && (Cross3 > EPS) && (Cross4 > EPS);
	bool AllNegative = (Cross1 < -EPS) && (Cross2 < -EPS) && (Cross3 < -EPS) && (Cross4 < -EPS);

	return AllPositive;
}

bool UPhotoGraphyLinearToolHandle::SignPointSide( const FVector2D& A, const FVector2D& B, const FVector2D& C, float& OutSign, const float EPS )
{
	FVector2D AB = (B - A).GetSafeNormal();
	FVector2D BC = (C - B).GetSafeNormal();

	float Cross = FVector2D::CrossProduct(AB, BC);

	OutSign = Cross > 0.0f ? 1.0f : -1.0f;
	return Cross > EPS;
}

bool UPhotoGraphyLinearToolHandle::ClampRectCorner( const FVector2D& A, const FVector2D& B, const FVector2D& C, const FVector2D& D, const FVector2D& InBeginA, FVector2D& OutOffset )
{
	constexpr float ProjectGap = 3;
	constexpr float EPS = 1e-3f;
	OutOffset = FVector2D::ZeroVector;
	float OutSign = 1.0f;
	if(!SignPointSide(A, B, C, OutSign, EPS))
	{
		FVector2D ProjectedPoint = FPhotoGraphyUtils::ProjectPointToLine(A, B, C, ProjectGap * OutSign);
		OutOffset = ProjectedPoint - A;
		if (!SignPointSide(A, C, D, OutSign, EPS))
		{
			FVector2D DirAC = (InBeginA - C).GetSafeNormal() * OutSign;
			OutOffset = (C - DirAC * ProjectGap) - A;
			// OutOffset = FPhotoGraphyUtils::ProjectPointToLine(ProjectedPoint, C, D, ProjectGap * OutSign) - A;
		}
		UE_LOG(LogTemp, Log, TEXT("OutA:%f %f"), A.X + OutOffset.X, A.Y + OutOffset.Y)
		UE_LOG(LogTemp, Log, TEXT("C:%f %f"), C.X, C.Y)
		return true;
	}

	if (!SignPointSide(A, C, D, OutSign, EPS))
	{
		FVector2D ProjectedPoint = FPhotoGraphyUtils::ProjectPointToLine(A, C, D, ProjectGap * OutSign);
		OutOffset = ProjectedPoint - A;
		if (!SignPointSide(A, B, C, OutSign, EPS))
		{
			FVector2D DirAC = (InBeginA - C).GetSafeNormal() * OutSign;
			OutOffset = (C - DirAC * ProjectGap) - A;
			// OutOffset = FPhotoGraphyUtils::ProjectPointToLine(ProjectedPoint, B, C, ProjectGap * OutSign) - A;
		}
		UE_LOG(LogTemp, Log, TEXT("OutA:%f %f"), A.X + OutOffset.X, A.Y + OutOffset.Y)
		UE_LOG(LogTemp, Log, TEXT("C:%f %f"), C.X, C.Y)
		return true;
	}
	return false;
}

bool UPhotoGraphyLinearToolHandle::GetIntersection2D( const FVector2D& A, const FVector2D& B, const FVector2D& C,
                                                      const FVector2D& D, FVector2D& OutIntersection )
{
	FVector2D AB = B - A;
	FVector2D CD = D - C;
	float Denominator = FVector2D::CrossProduct(AB, CD);
	
	if (FMath::IsNearlyZero(Denominator, KINDA_SMALL_NUMBER))
	{
		return false; // 平行或重合
	}

	const FVector2D AC = C - A;
	const float t = (AC.X * CD.Y - AC.Y * CD.X) / Denominator;

	OutIntersection = A + t * AB;
	return true;
}

FVector2D UPhotoGraphyLinearToolHandle::GetOutRectCornerOffset( const FVector2D& A, const FVector2D& B, const FVector2D& C,
	const FVector2D& NormalAB, const FVector2D& NormalBC, float FadeAB, float FadeBC )
{
	FVector2D OffsetA = A + NormalAB * FadeAB;
	FVector2D OffsetB1 = B + NormalAB * FadeAB;
	FVector2D OffsetB2 = B + NormalBC * FadeBC;
	FVector2D OffsetC = C + NormalBC * FadeBC;
	FVector2D OutCorner;
	if(GetIntersection2D(OffsetA, OffsetB1, OffsetB2, OffsetC, OutCorner)) return OutCorner;
	return FVector2D::ZeroVector;
}

void UPhotoGraphyLinearToolHandle::UpdateHandlePosition( const FIntPoint InViewSize )
{
	FIntPoint HalfViewSize = InViewSize / 2;
	const float AspectRationInv = static_cast<float>(InViewSize.Y) / InViewSize.X;
	
	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();

	const FVector2D PivotPos = ShapeModel.LinearShape.PivotPos;
	const float RotationAngle = ShapeModel.LinearShape.RotationAngle;
	
	const FVector2D A = ShapeModel.LinearShape.A;
	const FVector2D B = ShapeModel.LinearShape.B;
	const FVector2D C = ShapeModel.LinearShape.C;
	const FVector2D D = ShapeModel.LinearShape.D;
	FVector2D MiddleAB = (A + B) * 0.5f;
	FVector2D MiddleBC = (B + C) * 0.5f;
	FVector2D MiddleCD = (C + D) * 0.5f;
	FVector2D MiddleDA = (D + A) * 0.5f;
	FVector2D PivotOffset;
	GetIntersection2D(
		MiddleAB, MiddleCD,
		MiddleBC, MiddleDA,
		PivotOffset);
	FVector2D VisualizePivot = PivotPos + PivotOffset;
	Pivot.Position = FPhotoGraphyUtils::NormalizedUVToScreenPosition(VisualizePivot, InViewSize);
	
	FVector2D RotationVector = FPhotoGraphyUtils::Rotate2DPoint(FVector2D(0, -RotationDistance), RotationAngle);
	Rotation.Position = Pivot.Position + FIntPoint(RotationVector.X * HalfViewSize.Y, RotationVector.Y * HalfViewSize.Y);

	for (FPhotoGraphyHandlePoint& Handle : Rects)
	{
		FVector2D Offset = GetRectsOffset(Handle.Dir) - PivotOffset;
		Offset = FPhotoGraphyUtils::Rotate2DPoint(Offset, RotationAngle);
		Handle.Position = FPhotoGraphyUtils::NormalizedUVToScreenPosition(VisualizePivot + Offset, InViewSize);
	}
	
	const float FadeAB = ShapeModel.LinearShape.FadeAB;
	const float FadeBC = ShapeModel.LinearShape.FadeBC;
	const float FadeCD = ShapeModel.LinearShape.FadeCD;
	const float FadeDA = ShapeModel.LinearShape.FadeDA;
	const FVector2D DirAB = (B - A).GetSafeNormal();
	const FVector2D DirBC = (C - B).GetSafeNormal();
	const FVector2D DirCD = (D - C).GetSafeNormal();
	const FVector2D DirDA = (A - D).GetSafeNormal();
	const FVector2D NormalAB{DirAB.Y, -DirAB.X};
	const FVector2D NormalBC{DirBC.Y, -DirBC.X};
	const FVector2D NormalCD{DirCD.Y, -DirCD.X};
	const FVector2D NormalDA{DirDA.Y, -DirDA.X};
	
	for (FPhotoGraphyHandlePoint& Handle : OuterRects)
	{
		FVector2D Offset = FVector2D::ZeroVector;
		switch (Handle.Dir)
		{
			case EPGHandle::UpLeft:
				Offset = GetOutRectCornerOffset(D, A, B, NormalDA, NormalAB, FadeDA, FadeAB);
				break;
			case EPGHandle::UpRight:
				Offset = GetOutRectCornerOffset(A, B, C, NormalAB, NormalBC, FadeAB, FadeBC);
				break;
			case EPGHandle::DownRight:
				Offset = GetOutRectCornerOffset(B, C, D, NormalBC, NormalCD, FadeBC, FadeCD);
				break;
			case EPGHandle::DownLeft:
				Offset = GetOutRectCornerOffset(C, D, A, NormalCD, NormalDA, FadeCD, FadeDA);
				break;
			default: continue;
		}
		Offset -= PivotOffset;
		Handle.Position = Pivot.Position +
			FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(Offset.X * HalfViewSize.X * AspectRationInv, Offset.Y * HalfViewSize.Y), RotationAngle);
	}

	for (FPhotoGraphyHandlePoint& Handle : OuterRects)
	{
		switch (Handle.Dir)
		{
		case EPGHandle::Up:
			Handle.Position = FindHandleByDir(OuterRects, EPGHandle::UpLeft)->Position + FindHandleByDir(OuterRects, EPGHandle::UpRight)->Position;
			break;
		case EPGHandle::Down:
			Handle.Position = FindHandleByDir(OuterRects, EPGHandle::DownLeft)->Position + FindHandleByDir(OuterRects, EPGHandle::DownRight)->Position;
			break;
		case EPGHandle::Left:
			Handle.Position = FindHandleByDir(OuterRects, EPGHandle::UpLeft)->Position + FindHandleByDir(OuterRects, EPGHandle::DownLeft)->Position;
			break;
		case EPGHandle::Right:
			Handle.Position = FindHandleByDir(OuterRects, EPGHandle::UpRight)->Position + FindHandleByDir(OuterRects, EPGHandle::DownRight)->Position;
			break;
		default: continue;
		}
		Handle.Position = FIntPoint(Handle.Position.X * 0.5f, Handle.Position.Y * 0.5f);
	}

	for (FPhotoGraphyHandlePoint& Handle : InnerRects)
	{
		FVector2D Offset = FVector2D::ZeroVector;
		switch (Handle.Dir)
		{
		case EPGHandle::UpLeft:
			Offset = GetOutRectCornerOffset(D, A, B, NormalDA, NormalAB, -FadeDA, -FadeAB);
			break;
		case EPGHandle::UpRight:
			Offset = GetOutRectCornerOffset(A, B, C, NormalAB, NormalBC, -FadeAB, -FadeBC);
			break;
		case EPGHandle::DownRight:
			Offset = GetOutRectCornerOffset(B, C, D, NormalBC, NormalCD, -FadeBC, -FadeCD);
			break;
		case EPGHandle::DownLeft:
			Offset = GetOutRectCornerOffset(C, D, A, NormalCD, NormalDA, -FadeCD, -FadeDA);
			break;
		default: continue;
		}
		Offset -= PivotOffset;
		Handle.Position = Pivot.Position +
			FPhotoGraphyUtils::Rotate2DPointInt(FIntPoint(Offset.X * HalfViewSize.X * AspectRationInv, Offset.Y * HalfViewSize.Y), RotationAngle);
	}

	for (auto Handle : AllHandles)
	{
		Handle->Position = FPhotoGraphyUtils::ApplyCanvasTransformForPoint(Handle->Position, InViewSize, ViewCanvas);
	}
}

void UPhotoGraphyLinearToolHandle::DrawRect( FToolDataVisualizer* Renderer, const FSceneView* View,
	FIntPoint InViewSize ) const
{
	
	float K = FPhotoGraphyUtils::GetFOVFixFactor(View);
	float ThickLineWidth = K * ThickLine;
	float ThinLineWidth = K * ThinLine;
	{
		FIntPoint A = FindHandleByDir(Rects, EPGHandle::UpLeft)->Position;
		FIntPoint B = FindHandleByDir(Rects, EPGHandle::UpRight)->Position;
		FIntPoint C = FindHandleByDir(Rects, EPGHandle::DownRight)->Position;
		FIntPoint D = FindHandleByDir(Rects, EPGHandle::DownLeft)->Position;
		FVector WorldA = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, A, InViewSize);
		FVector WorldB = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, B, InViewSize);
		FVector WorldC = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, C, InViewSize);
		FVector WorldD = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, D, InViewSize);
		Renderer->DrawLine(WorldA, WorldB, DefaultLineColor, ThickLineWidth, false);
		Renderer->DrawLine(WorldB, WorldC, DefaultLineColor, ThickLineWidth, false);
		Renderer->DrawLine(WorldC, WorldD, DefaultLineColor, ThickLineWidth, false);
		Renderer->DrawLine(WorldD, WorldA, DefaultLineColor, ThickLineWidth, false);
	}

	{
		FIntPoint A = FindHandleByDir(OuterRects, EPGHandle::UpLeft)->Position;
		FIntPoint B = FindHandleByDir(OuterRects, EPGHandle::UpRight)->Position;
		FIntPoint C = FindHandleByDir(OuterRects, EPGHandle::DownRight)->Position;
		FIntPoint D = FindHandleByDir(OuterRects, EPGHandle::DownLeft)->Position;
		FVector WorldA = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, A, InViewSize);
		FVector WorldB = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, B, InViewSize);
		FVector WorldC = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, C, InViewSize);
		FVector WorldD = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, D, InViewSize);
		Renderer->DrawLine(WorldA, WorldB, DefaultLineColor, ThinLineWidth, false);
		Renderer->DrawLine(WorldB, WorldC, DefaultLineColor, ThinLineWidth, false);
		Renderer->DrawLine(WorldC, WorldD, DefaultLineColor, ThinLineWidth, false);
		Renderer->DrawLine(WorldD, WorldA, DefaultLineColor, ThinLineWidth, false);
	}

	if (EnsureRectNormals(InnerRects[0].Position, InnerRects[1].Position, InnerRects[3].Position, InnerRects[2].Position))
	{
		FIntPoint A = FindHandleByDir(InnerRects, EPGHandle::UpLeft)->Position;
		FIntPoint B = FindHandleByDir(InnerRects, EPGHandle::UpRight)->Position;
		FIntPoint C = FindHandleByDir(InnerRects, EPGHandle::DownRight)->Position;
		FIntPoint D = FindHandleByDir(InnerRects, EPGHandle::DownLeft)->Position;
		FVector WorldA = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, A, InViewSize);
		FVector WorldB = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, B, InViewSize);
		FVector WorldC = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, C, InViewSize);
		FVector WorldD = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, D, InViewSize);
		Renderer->DrawLine(WorldA, WorldB, DefaultLineColor, ThinLineWidth, false);
		Renderer->DrawLine(WorldB, WorldC, DefaultLineColor, ThinLineWidth, false);
		Renderer->DrawLine(WorldC, WorldD, DefaultLineColor, ThinLineWidth, false);
		Renderer->DrawLine(WorldD, WorldA, DefaultLineColor, ThinLineWidth, false);
	}
	
	FVector PivotPosition = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Pivot.Position, InViewSize);
	FVector RotationPosition = FPhotoGraphyUtils::ScreenPositionToWorldPosition(View, Rotation.Position, InViewSize);
	Renderer->DrawLine(PivotPosition, RotationPosition, FLinearColor::White, ThickLine, false);
}

void UPhotoGraphyLinearToolHandle::Draw( FToolDataVisualizer* Renderer, IToolsContextRenderAPI* RenderAPI )
{
	const FSceneView* View = RenderAPI->GetSceneView();
	if (!View) return;
	const FIntPoint ViewSizeInt = View->UnconstrainedViewRect.Size();
	const FVector ViewDirection = View->GetViewDirection();
	UpdateHandlePosition(ViewSizeInt);

	DrawRect(Renderer, View, ViewSizeInt);

	float K = FPhotoGraphyUtils::GetFOVFixFactor(View);
	float Size = K * DiscSize;
	float HighlightSize = K * HighlightDiscSize;
	
	for (const FPhotoGraphyHandlePoint* Handle : AllHandles)
	{
		constexpr int Steps = 16;
		if (!Handle) continue;
		if (!Handle->Visible) continue;
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

bool UPhotoGraphyLinearToolHandle::HitTest( const FInputDeviceRay& PressPos )
{
	return Super::HitTest(PressPos);
}

void UPhotoGraphyLinearToolHandle::ProcessPivot( FVector2D MousePos, FIntPoint InViewSize,
	const FViewCanvas& InViewCanvas )
{
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);

	FVector2D RevertedPivotPosition = FPhotoGraphyUtils::RevertCanvasTransformForPoint(Pivot.Position, InViewSize, InViewCanvas);
	FVector2D VisualizePivot = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedPivotPosition, InViewSize);
	FVector2D PivotOffset = VisualizePivot - ShapeModel.LinearShape.PivotPos;
	
	ShapeModel.LinearShape.PivotPos = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedMousePos, InViewSize) - PivotOffset;
	UpdateHandlePosition(InViewSize);
}

void UPhotoGraphyLinearToolHandle::ProcessRotation( FVector2D MousePos, FIntPoint InViewSize,
	const FViewCanvas& InViewCanvas )
{
	const float AspectRatio = static_cast<float>(InViewSize.X) / InViewSize.Y;
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);
	FVector2D RevertedPivotPosition = FPhotoGraphyUtils::RevertCanvasTransformForPoint(Pivot.Position, InViewSize, InViewCanvas);
	FVector2D RotatedMouseUV = RevertedMousePos - RevertedPivotPosition;
	RotatedMouseUV /= InViewSize;
	RotatedMouseUV.X *= AspectRatio;
	ShapeModel.LinearShape.RotationAngle = FPhotoGraphyUtils::ComputeRotationAngle(FVector2D(0, -1), RotatedMouseUV);
}

void UPhotoGraphyLinearToolHandle::ProcessRect( FVector2D MousePos, FIntPoint InViewSize,
	const FViewCanvas& InViewCanvas )
{
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(
		FIntPoint(MousePos.X, MousePos.Y), InViewSize, InViewCanvas);

	FIntPoint IntA = FindHandleByDir(Rects, EPGHandle::UpLeft)->Position;
	FIntPoint IntB = FindHandleByDir(Rects, EPGHandle::UpRight)->Position;
	FIntPoint IntC = FindHandleByDir(Rects, EPGHandle::DownRight)->Position;
	FIntPoint IntD = FindHandleByDir(Rects, EPGHandle::DownLeft)->Position;
	IntA = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntA, InViewSize, InViewCanvas);
	IntB = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntB, InViewSize, InViewCanvas);
	IntC = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntC, InViewSize, InViewCanvas);
	IntD = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntD, InViewSize, InViewCanvas);

	FVector2D MouseOffset = RevertedMousePos - BeginMousePos;
	switch (SelectedHandle->Dir)
	{
	case EPGHandle::Up:
		IntA = FIntPoint(BeginA.X + MouseOffset.X, BeginA.Y + MouseOffset.Y);
		IntB = FIntPoint(BeginB.X + MouseOffset.X, BeginB.Y + MouseOffset.Y);
		break;
	case EPGHandle::Down:
		IntC = FIntPoint(BeginC.X + MouseOffset.X, BeginC.Y + MouseOffset.Y);
		IntD = FIntPoint(BeginD.X + MouseOffset.X, BeginD.Y + MouseOffset.Y);
		break;
	case EPGHandle::Left:
		IntA = FIntPoint(BeginA.X + MouseOffset.X, BeginA.Y + MouseOffset.Y);
		IntD = FIntPoint(BeginD.X + MouseOffset.X, BeginD.Y + MouseOffset.Y);
		break;
	case EPGHandle::Right:
		IntB = FIntPoint(BeginB.X + MouseOffset.X, BeginB.Y + MouseOffset.Y);
		IntC = FIntPoint(BeginC.X + MouseOffset.X, BeginC.Y + MouseOffset.Y);
		break;
	case EPGHandle::UpLeft:		IntA = FIntPoint(RevertedMousePos.X, RevertedMousePos.Y); break;
	case EPGHandle::UpRight:	IntB = FIntPoint(RevertedMousePos.X, RevertedMousePos.Y); break;
	case EPGHandle::DownRight:	IntC = FIntPoint(RevertedMousePos.X, RevertedMousePos.Y); break;
	case EPGHandle::DownLeft:	IntD = FIntPoint(RevertedMousePos.X, RevertedMousePos.Y); break;
	default: ;
	}

	FVector2D A = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(IntA, InViewSize);
	FVector2D B = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(IntB, InViewSize);
	FVector2D C = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(IntC, InViewSize);
	FVector2D D = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(IntD, InViewSize);
	FVector2D NewMiddleAB = (A + B) * 0.5f;
	FVector2D NewMiddleBC = (B + C) * 0.5f;
	FVector2D NewMiddleCD = (C + D) * 0.5f;
	FVector2D NewMiddleDA = (D + A) * 0.5f;
	
	FVector2D NewVisualizePivot;
	GetIntersection2D(NewMiddleAB, NewMiddleCD, NewMiddleBC, NewMiddleDA, NewVisualizePivot);

	FVector2D NewA = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(
			IntA, NewVisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	FVector2D NewB = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(
			IntB, NewVisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	FVector2D NewC = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(
			IntC, NewVisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	FVector2D NewD = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(
			IntD, NewVisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);

	FVector2D NewMiddleABUV = (NewA + NewB) * 0.5f;
	FVector2D NewMiddleBCUV = (NewB + NewC) * 0.5f;
	FVector2D NewMiddleCDUV = (NewC + NewD) * 0.5f;
	FVector2D NewMiddleDAUV = (NewD + NewA) * 0.5f;
	
	FVector2D NewPivotOffset;
	GetIntersection2D(
		NewMiddleABUV, NewMiddleCDUV,
		NewMiddleBCUV, NewMiddleDAUV,
		NewPivotOffset);

	FVector2D NewPivot = NewVisualizePivot - NewPivotOffset;
	ShapeModel.LinearShape.A = NewA;
	ShapeModel.LinearShape.B = NewB;
	ShapeModel.LinearShape.C = NewC;
	ShapeModel.LinearShape.D = NewD;
	ShapeModel.LinearShape.PivotPos = NewPivot;
	UpdateHandlePosition(InViewSize);
}

void UPhotoGraphyLinearToolHandle::ProcessFade( FVector2D MousePos, FIntPoint InViewSize,
	const FViewCanvas& InViewCanvas )
{
	FVector2D RevertedMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(MousePos.X, MousePos.Y),
		InViewSize, InViewCanvas);
	FVector2D RevertedPivotPosition = FPhotoGraphyUtils::RevertCanvasTransformForPoint(Pivot.Position, InViewSize, InViewCanvas);
	FVector2D VisualizePivot = FPhotoGraphyUtils::ScreenPositionToNormalizedUV(RevertedPivotPosition, InViewSize);
	FVector2D MouseUV = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(RevertedMousePos, VisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	
	FIntPoint IntA = FindHandleByDir(Rects, EPGHandle::UpLeft)->Position;
	FIntPoint IntB = FindHandleByDir(Rects, EPGHandle::UpRight)->Position;
	FIntPoint IntC = FindHandleByDir(Rects, EPGHandle::DownRight)->Position;
	FIntPoint IntD = FindHandleByDir(Rects, EPGHandle::DownLeft)->Position;
	IntA = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntA, InViewSize, InViewCanvas);
	IntB = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntB, InViewSize, InViewCanvas);
	IntC = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntC, InViewSize, InViewCanvas);
	IntD = FPhotoGraphyUtils::RevertCanvasTransformForPoint(IntD, InViewSize, InViewCanvas);
	FVector2D A = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(IntA, VisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	FVector2D B = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(IntB, VisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	FVector2D C = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(IntC, VisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	FVector2D D = FPhotoGraphyUtils::ScreenPositionToNormalizedUVFixRotation(IntD, VisualizePivot, InViewSize, ShapeModel.LinearShape.RotationAngle);
	
	switch (SelectedHandle->Dir)
	{
	case EPGHandle::Up:		ShapeModel.LinearShape.FadeAB = FPhotoGraphyUtils::DistanceToLine(MouseUV, A, B);break;
	case EPGHandle::Down:	ShapeModel.LinearShape.FadeCD = FPhotoGraphyUtils::DistanceToLine(MouseUV, C, D); break;
	case EPGHandle::Left:	ShapeModel.LinearShape.FadeDA = FPhotoGraphyUtils::DistanceToLine(MouseUV, D, A); break;
	case EPGHandle::Right:	ShapeModel.LinearShape.FadeBC = FPhotoGraphyUtils::DistanceToLine(MouseUV, B, C); break;
	default: return;
	}

}

void UPhotoGraphyLinearToolHandle::ProcessBeginDrag( const FInputDeviceRay& DragPos )
{
	const FViewport* Viewport = FPhotoGraphyUtils::GetActiveViewport();
	if (!Viewport) return;
	
	const FIntPoint ViewSize = Viewport->GetSizeXY();
	FViewCanvas ViewCanvas = FViewCanvas();
	if (OwnerMechanic.IsValid()) ViewCanvas = OwnerMechanic->GetToolSettings()->GetViewCanvas();
	
	BeginMousePos = FPhotoGraphyUtils::RevertCanvasTransformForPoint(FIntPoint(DragPos.ScreenPosition.X, DragPos.ScreenPosition.Y),ViewSize, ViewCanvas);
	BeginLinearShape = ShapeModel.LinearShape;
	BeginA = FindHandleByDir(Rects, EPGHandle::UpLeft)->Position;
	BeginB = FindHandleByDir(Rects, EPGHandle::UpRight)->Position;
	BeginC = FindHandleByDir(Rects, EPGHandle::DownRight)->Position;
	BeginD = FindHandleByDir(Rects, EPGHandle::DownLeft)->Position;
	BeginA = FPhotoGraphyUtils::RevertCanvasTransformForPoint(BeginA, ViewSize, ViewCanvas);
	BeginB = FPhotoGraphyUtils::RevertCanvasTransformForPoint(BeginB, ViewSize, ViewCanvas);
	BeginC = FPhotoGraphyUtils::RevertCanvasTransformForPoint(BeginC, ViewSize, ViewCanvas);
	BeginD = FPhotoGraphyUtils::RevertCanvasTransformForPoint(BeginD, ViewSize, ViewCanvas);
}

void UPhotoGraphyLinearToolHandle::ProcessDrag( const FInputDeviceRay& DragPos )
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
	case EPGHandle::Rect:			ProcessRect(MousePos2D, ViewSize, ViewCanvas); break;
	case EPGHandle::OuterRect:		ProcessFade(MousePos2D, ViewSize, ViewCanvas); break;
	default: return;
	}
	bShapeModified = true;
}
