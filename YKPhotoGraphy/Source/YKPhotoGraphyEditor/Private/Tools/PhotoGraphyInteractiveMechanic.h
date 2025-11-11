#pragma once
#include "InteractionMechanic.h"
#include "PhotoGraphyComponent.h"
#include "ToolDataVisualizer.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
#include "PhotoGraphyInteractiveMechanic.generated.h"

class UPhotoGraphyComponent;
class UPhotoGraphyToolHandleBase;
class UPhotoGraphyToolSettings;
class UPhotoGraphyToolBase;
class UPhotoGraphyInteractiveTool;
class UCombinedTransformGizmo;
class UTransformProxy;

UCLASS(Transient)
class UPhotoGraphyInteractiveMechanic : public UInteractionMechanic, public IHoverBehaviorTarget, public IClickDragBehaviorTarget
{
	friend UPhotoGraphyToolHandleBase;
	GENERATED_BODY()
	
public:

	UPROPERTY()
	TObjectPtr<UPhotoGraphyToolHandleBase> ToolHandle = nullptr;
	
	// TWeakObjectPtr<UPhotoGraphyToolBase> PhotoGraphyTool;
	
	bool bIsEnabled = true;
	bool bDragging = false;
	
	virtual void Initialize();
	virtual void Setup(UInteractiveTool* InParentTool) override;
	virtual void Shutdown() override;
	FPhotoGraphyShapeModel GetShapeModel() const;
	void SetShapeModel(const FPhotoGraphyShapeModel &NewShapeModel);

	// UPhotoGraphyToolBase* GetPhotoGraphyTool();
	UPhotoGraphyToolSettings* GetToolSettings() const;
	UPhotoGraphyComponent* GetPhotoGraphyComponent() const;

	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
	virtual void DrawHUD(FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI);

	// IHoverBehavior
	virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
	virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
	virtual void OnEndHover() override;
	virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;

	// IClickDragBehavior
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnTerminateDragSequence() override;
	virtual void OnClickPress(const FInputDeviceRay& DragPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& DragPos) override;

	void InitialToolHandle(EPhotoGraphyShapeType NewShapeType);
	void InitialOrSwitchHandleType();
	void InitialOrSwitchHandleType(EPhotoGraphyShapeType NewShapeType);
	static bool GetCurrentMousePosition(FIntPoint& OutMousePos);


protected:
	FToolDataVisualizer ToolRenderer;
};
