#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IDetailCustomization.h"
#include "IDetailRootObjectCustomization.h"

#include "PhotoGraphyDetialPanel.generated.h"

class UPhotoGraphyEditorMode;

USTRUCT(BlueprintType)
struct FViewCanvas
{
	GENERATED_BODY()
	
	UPROPERTY(Category = "Canvas", EditAnywhere, meta=(ToolTip=""))
	bool bEditCanvas = true;
	
	UPROPERTY(Category = "Canvas", EditAnywhere, meta=(ToolTip="", EditCondition="bEditCanvas"))
	FVector2D CanvasOffset = FVector2D::ZeroVector;
	
	UPROPERTY(Category = "Canvas", EditAnywhere, meta=(ToolTip="", EditCondition="bEditCanvas"))
	FVector2D CanvasZoom = FVector2D(1.0f, 1.0f);

	// Degrees
	UPROPERTY(Category = "Canvas", EditAnywhere, meta=(ToolTip="", EditCondition="bEditCanvas"))
	float CanvasRotation = 0.0f;
};


UCLASS()
class UPhotoGraphyToolSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category = "Canvas", EditAnywhere, meta=(ToolTip="", ShowOnlyInnerProperties))
	FViewCanvas ViewCanvas;

	void UpdateViewCanvas() const;

	FViewCanvas GetViewCanvas() const;

	// virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	
};


class FPhotoGraphyGenerateSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	static bool FilterAsset(const FAssetData& AssetData);

	static void OnCreateButtonClick();
};

