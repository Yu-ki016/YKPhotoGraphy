#pragma once

#include "PhotoGraphyComponent.h"
#include "PhotoGraphyInteractiveMechanic.h"
#include "PhotoGraphyToolBase.h"
#include "PhotoGraphyInteractiveTool.generated.h"


class UPhotoGraphyCanvasMechanic;
class UPhotoGraphyComponent;
class FToolDataVisualizer;

UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyInteractiveToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Target", VisibleAnywhere)
	TWeakObjectPtr<UPhotoGraphyComponent> TargetComponent;

	// 当选中了一个可用的Target，不会因为选中其他物体而切换Target
	UPROPERTY(Category = "Target", EditAnywhere, meta=(DisplayName= "LockTarget"))
	bool LockTarget = true;
	
	// 控制点大小
	UPROPERTY(Category="Setting", EditAnywhere, meta = (ClampMin = "0", UIMax = "10.0"))
	float HandleSize = 1.0f;
};


UCLASS()
class YKPHOTOGRAPHYEDITOR_API UToonPhotoGraphyInteractiveToolBuilder : public UPhotoGraphyToolBaseToolBuilder
{
	GENERATED_BODY()
	
public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	
};


UINTERFACE()
class UPhotoGraphyInteractiveInterface : public UInterface
{
	GENERATED_BODY()
};

class IPhotoGraphyInteractiveInterface
{
	GENERATED_BODY()

public:
	virtual void UpdateShapeModelFromEditor() = 0;
	virtual UPhotoGraphyComponent* GetPhotoGraphyComponent() = 0;
	virtual float GetHandleSize() {return 1.0f;}
};

UCLASS()
class YKPHOTOGRAPHYEDITOR_API UPhotoGraphyInteractiveTool : public UPhotoGraphyToolBase, public IPhotoGraphyInteractiveInterface
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FOnSelectionChanged);
	
	UPhotoGraphyInteractiveTool();
	
	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
	virtual void DrawHUD(FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI) override;
	virtual void OnTick(float DeltaTime) override;
	
	/** IPhotoGraphyInteractiveInterface overrides */
	virtual void UpdateShapeModelFromEditor() override;
	virtual UPhotoGraphyComponent* GetPhotoGraphyComponent() override;
	virtual float GetHandleSize() override;

	void UpdateShapeModelFromComponent(const FPhotoGraphyShapeModel& InShapeModel) const;
	
	UPhotoGraphyInteractiveToolProperties* GetPhotoGraphyProperties() const { return ToolProperties; }
	FOnSelectionChanged GetSelectionChanged() const { return SelectionChanged; }
	void CheckShapeModelDirty();
	
protected:
	
	UPROPERTY()
	TObjectPtr<UPhotoGraphyInteractiveMechanic> InteractiveMechanic;

	TSoftClassPtr<UPhotoGraphyInteractiveToolProperties> PropertyClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UPhotoGraphyInteractiveToolProperties> ToolProperties;
	
	void OnLevelSelectionChanged(UObject* NewSelection) const;

private:
	FOnSelectionChanged SelectionChanged;
	
};
