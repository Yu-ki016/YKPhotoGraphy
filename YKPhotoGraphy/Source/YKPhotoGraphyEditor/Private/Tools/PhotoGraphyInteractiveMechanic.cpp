#include "PhotoGraphyInteractiveMechanic.h"

#include "InteractiveToolManager.h"
#include "PhotoGraphyEllipseToolHandle.h"
#include "PhotoGraphyGradientToolHandle.h"
#include "ToolDataVisualizer.h"
#include "PhotoGraphyInteractiveTool.h"
#include "PhotoGraphyLinearToolHandle.h"
#include "PhotoGraphyUtils.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"


void UPhotoGraphyInteractiveMechanic::Setup( UInteractiveTool* InParentTool )
{
	Super::Setup(InParentTool);

	UMouseHoverBehavior* HoverBehavior = NewObject<UMouseHoverBehavior>(this);
	HoverBehavior->Initialize(this);
	HoverBehavior->SetDefaultPriority(0);
	InParentTool->AddInputBehavior(HoverBehavior);
	
	// 注册拖拽行为
	UClickDragInputBehavior* DragBehavior = NewObject<UClickDragInputBehavior>(this);
	DragBehavior->Initialize(this);
	DragBehavior->SetDefaultPriority(1);
	InParentTool->AddInputBehavior(DragBehavior);

	ToolRenderer = FToolDataVisualizer();

	InitialOrSwitchHandleType();
	
}

void UPhotoGraphyInteractiveMechanic::InitialToolHandle(EPhotoGraphyShapeType NewShapeType)
{
	switch (NewShapeType)
	{
		case EPhotoGraphyShapeType::Ellipse: ToolHandle = NewObject<UPhotoGraphyEllipseToolHandle>(this); break;
		case EPhotoGraphyShapeType::Gradient: ToolHandle = NewObject<UPhotoGraphyGradientToolHandle>(this); break;
		case EPhotoGraphyShapeType::Linear: ToolHandle = NewObject<UPhotoGraphyLinearToolHandle>(this); break;
		default: return;
	}

	if (!ToolHandle) return;
	ToolHandle->Initialize(this);
}

void UPhotoGraphyInteractiveMechanic::InitialOrSwitchHandleType()
{
	UPhotoGraphyComponent* TargetComponent = GetPhotoGraphyComponent();
	if (!TargetComponent) return;

	EPhotoGraphyShapeType NewShapeType = TargetComponent->ShapeModel.ShapeType;
	InitialOrSwitchHandleType(NewShapeType);
}

void UPhotoGraphyInteractiveMechanic::InitialOrSwitchHandleType( EPhotoGraphyShapeType NewShapeType )
{
	if (!ToolHandle) InitialToolHandle(NewShapeType);
	if (!ToolHandle) return;
	
	if (ToolHandle->GetShapeModel().ShapeType == NewShapeType) return;

	ToolHandle->ConditionalBeginDestroy();
	InitialToolHandle(NewShapeType);
}

void UPhotoGraphyInteractiveMechanic::Shutdown()
{
	Super::Shutdown();
}

UPhotoGraphyToolSettings* UPhotoGraphyInteractiveMechanic::GetToolSettings() const
{
	if (UPhotoGraphyToolBase* Tool = Cast<UPhotoGraphyToolBase>(GetParentTool()))
	{
		return  Tool->GetToolSettings();
	}
	
	return nullptr;
}

UPhotoGraphyComponent* UPhotoGraphyInteractiveMechanic::GetPhotoGraphyComponent() const
{
	if (IPhotoGraphyInteractiveInterface* InteractiveTool = Cast<IPhotoGraphyInteractiveInterface>(GetParentTool()))
	{
		return InteractiveTool->GetPhotoGraphyComponent();
	}
	return nullptr;
}

float UPhotoGraphyInteractiveMechanic::GetHandleSize() const
{
	if (IPhotoGraphyInteractiveInterface* InteractiveTool = Cast<IPhotoGraphyInteractiveInterface>(GetParentTool()))
	{
		return InteractiveTool->GetHandleSize();
	}
	return 1.0f;
}

FPhotoGraphyShapeModel UPhotoGraphyInteractiveMechanic::GetShapeModel() const
{
	return ToolHandle->ShapeModel;
}

void UPhotoGraphyInteractiveMechanic::SetShapeModel(const FPhotoGraphyShapeModel &NewShapeModel)
{
	InitialOrSwitchHandleType(NewShapeModel.ShapeType);
	if (!ToolHandle) return;
	ToolHandle->SetShapeModel(NewShapeModel);
}

void UPhotoGraphyInteractiveMechanic::Initialize()
{
	
}

void UPhotoGraphyInteractiveMechanic::Render( IToolsContextRenderAPI* RenderAPI )
{
	if (!bIsEnabled || !ToolHandle) return;

	if (!LastActiveSceneView)
	{
		if (EnumHasAnyFlags(RenderAPI->GetViewInteractionState(), EViewInteractionState::Hovered))
		{
			LastActiveSceneView = RenderAPI->GetSceneView();
		}
	}
	
	if (EnumHasAnyFlags(RenderAPI->GetViewInteractionState(), EViewInteractionState::Focused))
	{
		LastActiveSceneView = RenderAPI->GetSceneView();
	}

	if (!LastActiveSceneView || RenderAPI->GetSceneView() != LastActiveSceneView) return;
	
	ToolRenderer.BeginFrame(RenderAPI);
	ToolHandle->HandleSize = GetHandleSize();
	ToolHandle->Draw(&ToolRenderer, RenderAPI);
	ToolRenderer.EndFrame();
	Super::Render(RenderAPI);
}

void UPhotoGraphyInteractiveMechanic::DrawHUD( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
}

void UPhotoGraphyInteractiveMechanic::OnClickPress( const FInputDeviceRay& DragPos )
{
	bDragging = true;
	if (!ToolHandle) return;
	
	UPhotoGraphyComponent* TargetComponent = GetPhotoGraphyComponent();
	if (!TargetComponent) return;

	if (GEditor)
	{
		GEditor->BeginTransaction(TEXT(""), FText::FromString(TEXT("Modify PhotoGraphy Shape Model")), nullptr);	
	}
	ToolHandle->ProcessBeginDrag(DragPos);
	TargetComponent->Modify();
	
}
void UPhotoGraphyInteractiveMechanic::OnClickDrag( const FInputDeviceRay& DragPos )
{
	if (!ToolHandle) return;
	
	if (bDragging)
	{
		ToolHandle->ProcessDrag(DragPos);
	}

	if (ToolHandle->bShapeModified)
	{
		IPhotoGraphyInteractiveInterface* PhotoGraphyInteractiveTool = Cast<IPhotoGraphyInteractiveInterface>(GetParentTool());
		if (!PhotoGraphyInteractiveTool) return;
		PhotoGraphyInteractiveTool->UpdateShapeModelFromEditor();
	}
}
void UPhotoGraphyInteractiveMechanic::OnClickRelease( const FInputDeviceRay& DragPos )
{
	bDragging = false;
	if (GEditor)
	{
		GEditor->EndTransaction();
	}
}

FInputRayHit UPhotoGraphyInteractiveMechanic::CanBeginClickDragSequence( const FInputDeviceRay& PressPos )
{
	FInputRayHit Hit;
	if (ToolHandle && ToolHandle->HitTest(PressPos))
	{
		Hit.bHit = true;
		Hit.HitDepth = 0.f;
	}
	return Hit;
}
void UPhotoGraphyInteractiveMechanic::OnTerminateDragSequence()
{
}

FInputRayHit UPhotoGraphyInteractiveMechanic::BeginHoverSequenceHitTest( const FInputDeviceRay& PressPos )
{
	FInputRayHit Hit;
	if (ToolHandle && ToolHandle->HitTest(PressPos))
	{
		Hit.bHit = true;
		Hit.HitDepth = 0.f;
	}
	return Hit;
}
void UPhotoGraphyInteractiveMechanic::OnBeginHover( const FInputDeviceRay& DevicePos )
{
}
void UPhotoGraphyInteractiveMechanic::OnEndHover()
{
}
bool UPhotoGraphyInteractiveMechanic::OnUpdateHover( const FInputDeviceRay& DevicePos )
{
	return false;
}

bool UPhotoGraphyInteractiveMechanic::GetCurrentMousePosition(FIntPoint& OutMousePos)
{
	if (GEditor && GEditor->GetActiveViewport())
	{
		FViewport* Viewport = GEditor->GetActiveViewport();
		FIntPoint ViewSizeInt = Viewport->GetSizeXY();
		if (Viewport)
		{
			Viewport->GetMousePos(OutMousePos);
			// 转换为相对于视口左上角的坐标
			OutMousePos.X = FMath::Clamp(OutMousePos.X, 0, ViewSizeInt.X);
			OutMousePos.Y = FMath::Clamp(OutMousePos.Y, 0, ViewSizeInt.Y);
			return true;
		}
	}
	return false;
}


