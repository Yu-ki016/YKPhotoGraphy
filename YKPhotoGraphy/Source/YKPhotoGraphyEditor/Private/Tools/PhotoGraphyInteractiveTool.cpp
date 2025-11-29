#include "PhotoGraphyInteractiveTool.h"
#include "InteractiveToolManager.h"
#include "PhotoGraphyCanvasMechanic.h"
#include "Selection.h"
#include "PhotoGraphyComponent.h"

#define LOCTEXT_NAMESPACE "UToonPhotographyTool"

UInteractiveTool* UToonPhotoGraphyInteractiveToolBuilder::BuildTool( const FToolBuilderState& SceneState ) const
{
	UPhotoGraphyInteractiveTool* NewTool = NewObject<UPhotoGraphyInteractiveTool>(SceneState.ToolManager);
	return NewTool;
}

UPhotoGraphyInteractiveTool::UPhotoGraphyInteractiveTool()
{
	PropertyClass = UPhotoGraphyInteractiveToolProperties::StaticClass();
}

void UPhotoGraphyInteractiveTool::Setup()
{
	InteractiveMechanic = NewObject<UPhotoGraphyInteractiveMechanic>(this);
	InteractiveMechanic->Setup(this);
	InteractiveMechanic->Initialize();

	Super::Setup();

	ToolProperties = NewObject<UPhotoGraphyInteractiveToolProperties>(this, PropertyClass.Get(), TEXT("PhotoGraphy"));
	AddToolPropertySource(ToolProperties);

	USelection::SelectionChangedEvent.AddUObject(this, &UPhotoGraphyInteractiveTool::OnLevelSelectionChanged);
	
	if (GEditor && ToolProperties)
	{
		OnLevelSelectionChanged(GEditor->GetSelectedActors());
	}

	if (GetPhotoGraphyComponent())
	{
		InteractiveMechanic->bIsEnabled = true;
		// GetPhotoGraphyComponent()->OnShapeModelUpdated.AddUObject(this, &UPhotoGraphyInteractiveTool::UpdateShapeModelFromComponent);
	}
	else
	{
		InteractiveMechanic->bIsEnabled = false;
	}
}

void UPhotoGraphyInteractiveTool::Shutdown( EToolShutdownType ShutdownType )
{
	InteractiveMechanic->Shutdown();
	CanvasMechanic->Shutdown();
	USelection::SelectionChangedEvent.RemoveAll(this);
	// if (UPhotoGraphyInteractiveToolProperties* Properties = GetPhotoGraphyProperties())
	// {
	// 	if (Properties->TargetComponent.IsValid())
	// 		Properties->TargetComponent->OnShapeModelUpdated.RemoveAll(this);
	// }
	Super::Shutdown(ShutdownType);
}

void UPhotoGraphyInteractiveTool::Render( IToolsContextRenderAPI* RenderAPI )
{
	Super::Render(RenderAPI);
	InteractiveMechanic->Render(RenderAPI);
}

void UPhotoGraphyInteractiveTool::DrawHUD( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
	Super::DrawHUD(Canvas, RenderAPI);
}

void UPhotoGraphyInteractiveTool::OnTick( float DeltaTime )
{
	CheckShapeModelDirty();
}

UPhotoGraphyComponent* UPhotoGraphyInteractiveTool::GetPhotoGraphyComponent()
{
	if (!GetPhotoGraphyProperties()) return nullptr;
	return GetPhotoGraphyProperties()->TargetComponent.Get();
}

float UPhotoGraphyInteractiveTool::GetHandleSize()
{
	return ToolProperties ? ToolProperties->HandleSize : 1.0f;
}

void UPhotoGraphyInteractiveTool::OnLevelSelectionChanged( UObject* NewSelection ) const
{
	UPhotoGraphyInteractiveToolProperties* Properties = GetPhotoGraphyProperties();
	if (!ensure(Properties)) return;
	if (Properties->LockTarget && Properties->TargetComponent.IsValid()) return;
	
	USelection* Selection = Cast<USelection>(NewSelection);
	if (!Selection || Selection->Num() == 0) return;

	// 尽可能保持当前选择的 Component
	UPhotoGraphyComponent* NewComponent = nullptr;
	for (FSelectionIterator It(*Selection); It; ++It)
	{
		if (AActor* SelectedActor = Cast<AActor>(*It))
		{
			if (UPhotoGraphyComponent* Component = SelectedActor->FindComponentByClass<UPhotoGraphyComponent>())
			{
				if (Properties->TargetComponent.IsValid() && Properties->TargetComponent == Component)
				{
					// TargetComponent Not Change
					return;
				}
				NewComponent = Component;
			}
		}
	}

	if (Properties->TargetComponent.IsValid())
	{
		// Properties->TargetComponent->OnShapeModelUpdated.RemoveAll(this);
	}
	
	Properties->TargetComponent = NewComponent;
	SelectionChanged.Broadcast();
	
	if (!InteractiveMechanic) return;
	
	if (NewComponent)
	{
		// Properties->TargetComponent->OnShapeModelUpdated.AddUObject(this, &UPhotoGraphyInteractiveTool::UpdateShapeModelFromComponent);
		InteractiveMechanic->bIsEnabled = true;
		InteractiveMechanic->SetShapeModel(NewComponent->ShapeModel);
	}
	else
	{
		InteractiveMechanic->bIsEnabled = false;
	}
}

void UPhotoGraphyInteractiveTool::UpdateShapeModelFromEditor( )
{
	if (!InteractiveMechanic) return;

	if (!GetPhotoGraphyComponent()) return;
	GetPhotoGraphyComponent()->SetShapeModel(InteractiveMechanic->GetShapeModel());
}

void UPhotoGraphyInteractiveTool::UpdateShapeModelFromComponent(const FPhotoGraphyShapeModel& InShapeModel) const
{
	if (!InteractiveMechanic) return;
	InteractiveMechanic->SetShapeModel(InShapeModel);
	
}

void UPhotoGraphyInteractiveTool::CheckShapeModelDirty()
{
	if (!InteractiveMechanic->bIsEnabled || InteractiveMechanic->bDragging) return;
	if (!InteractiveMechanic) return;
	UPhotoGraphyComponent* PhotoGraphyComponent = GetPhotoGraphyComponent();
	if (!PhotoGraphyComponent) return;

	if (PhotoGraphyComponent->ShapeModel != InteractiveMechanic->GetShapeModel())
	{
		UpdateShapeModelFromComponent(PhotoGraphyComponent->ShapeModel);
	}
}

#undef LOCTEXT_NAMESPACE
