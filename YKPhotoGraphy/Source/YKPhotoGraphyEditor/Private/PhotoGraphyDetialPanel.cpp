#include "PhotoGraphyDetialPanel.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PhotoGraphyEditorMode.h"
#include "YKCustomActorPickerNodeBuilder.h"
#include "Tools/PhotoGraphyGenerateTool.h"
#include "ViewExtension/PhotoGraphySceneViewExtension.h"
#include "Runtime/Engine/Classes/Camera/CameraActor.h"

#define LOCTEXT_NAMESPACE "UYKToolsEditorMode"

void UPhotoGraphyToolSettings::UpdateViewCanvas() const
{
	if(FPhotoGraphySceneViewExtension* ViewExtension = UPhotoGraphyEditorMode::GetPhotoGraphySceneViewExtension())
	{
		FViewCanvasParameters ViewCanvasParameters;
		ViewCanvasParameters.CanvasOffset = FVector2f(ViewCanvas.CanvasOffset);
		ViewCanvasParameters.CanvasZoom = FVector2f(ViewCanvas.CanvasZoom);
		ViewCanvasParameters.CanvasRotation = FMath::DegreesToRadians(ViewCanvas.CanvasRotation);
		ViewExtension->SetCanvasParameters(ViewCanvasParameters, ViewCanvas.bEditCanvas);
	}
}

FViewCanvas UPhotoGraphyToolSettings::GetViewCanvas() const
{
	if (!ViewCanvas.bEditCanvas) return FViewCanvas();
	return ViewCanvas;
}

void UPhotoGraphyToolSettings::PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent )
{
	if (PropertyChangedEvent.PropertyChain.GetActiveMemberNode())
	{
		const FProperty* MemberProperty = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue();
		if (MemberProperty && MemberProperty->GetFName() == TEXT("ViewCanvas"))
		{
			UpdateViewCanvas();
		}
	}

	UObject::PostEditChangeChainProperty(PropertyChangedEvent);
}

TSharedRef<IDetailCustomization> FPhotoGraphyGenerateSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FPhotoGraphyGenerateSettingsCustomization());
}

bool FPhotoGraphyGenerateSettingsCustomization::FilterAsset( const FAssetData& AssetData )
{
	UE_LOG(LogTemp, Log, TEXT("Filtering asset"));
	UClass* Class = AssetData.GetClass();
	if (!Class) return true;

	UE_LOG(LogTemp, Log, TEXT("Filtering asset,Class%1s"), *Class->StaticClass()->GetName());
	return !(Class->IsChildOf(ACameraActor::StaticClass()) ||
			 Class->IsChildOf(APostProcessVolume::StaticClass()));
}

void FPhotoGraphyGenerateSettingsCustomization::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
	IDetailCategoryBuilder& PhotoGraphyCategory = DetailLayout.EditCategory(TEXT("Create"));
	PhotoGraphyCategory.SetSortOrder(0);
	
	TSharedRef<IPropertyHandle> ActorClassHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPhotoGraphyGenerateToolProperties, ActorClass));
	TSharedRef<IPropertyHandle> PostProcessActorHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPhotoGraphyGenerateToolProperties, PostProcessActor));
	
	PhotoGraphyCategory.AddProperty(ActorClassHandle);
	
	DetailLayout.HideProperty(PostProcessActorHandle);
	FOnShouldFilterActor Filter = FOnShouldFilterActor::CreateLambda([](const AActor* InActor)
	{
		if (!InActor) return false;
		return InActor->IsA<ACameraActor>() || InActor->IsA<APostProcessVolume>();
	});

	PhotoGraphyCategory.AddCustomBuilder(MakeShared<FYKCustomActorPickerNodeBuilder>(PostProcessActorHandle, Filter));
	
	PhotoGraphyCategory.AddCustomRow(FText::FromString("Create"))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(150.0f)
			.HeightOverride(30.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Text(FText::FromString("Create"))
				.ToolTipText(FText::FromString("Create PhotoGraphy Actor"))
				.OnClicked_Lambda([this]()
				{
					OnCreateButtonClick();
					return FReply::Handled();
				})
			]
		]
	];
	
}

void FPhotoGraphyGenerateSettingsCustomization::OnCreateButtonClick()
{
	if (UPhotoGraphyGenerateTool* GenerateTool = Cast<UPhotoGraphyGenerateTool>(UPhotoGraphyEditorMode::GetActiveTool()))
	{
		GenerateTool->GeneratePhotoGraphyActor();
	}
}

#undef LOCTEXT_NAMESPACE
