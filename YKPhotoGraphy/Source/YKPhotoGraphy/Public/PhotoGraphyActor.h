// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhotoGraphyActorBase.h"
#include "GameFramework/Actor.h"
#include "PhotoGraphyActor.generated.h"


UENUM(BlueprintType)
enum class EPhotoGraphyColorBlendMode : uint8
{
	Copy		UMETA(DisplayName = "Copy"),
	Add			UMETA(DisplayName = "Add"),
	Multiply	UMETA(DisplayName = "Multiply"),
	Overlay		UMETA(DisplayName = "Overlay"),
	Screen		UMETA(DisplayName = "Screen"),
};

UENUM(BlueprintType)
enum class EPhotoGraphyStencilType : uint8
{
	ExcludeStencil			UMETA(DisplayName = "Exclude Selected Actors"),
	IncludeStencil			UMETA(DisplayName = "Affect Only Selected Actors"),
	MAX
};

UCLASS(Abstract, Blueprintable, HideCategories=(Hide))
class YKPHOTOGRAPHY_API APhotoGraphyActor : public APhotoGraphyActorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Hide")
	bool bShowCategoryOrderProperty = false;
	
	// UE会先显示Actor上的属性分类再显示Component上的
	// 这个属性是无用的，但能够让UPhotoGraphyComponent上的PhotoGraphyShape排在更靠前的位置
	UPROPERTY(VisibleAnywhere, Category = "PhotoGraphyShape", meta=(EditCondition="bShowCategoryOrderProperty", EditConditionHides))
	bool OrderPhotoGraphyShape;
	
	/**
	 * PostProcess Material Target Actor(APostProcessVolume or ACameraActor)
	 * 后处理材质的目标Actor(后处理体积Actor或者摄像机Actor)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
	TSoftObjectPtr<AActor> PostProcessActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding")
	TObjectPtr<UMaterialInterface> PostProcessMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding", meta=(InlineEditConditionToggle))
	bool bOverrideBlendablePriority = false;

	/**
	 * The blending order of PostProcess Materials depends on BlendablePriority.
	 * The smaller the value, the earlier it is drawn. If the values are the same, UE will interpolate PostProcess Materials.
	 * 后处理材质的混合顺序依赖于BlendablePriority
	 * 数值越小，绘制顺序越前; 数值相同时，UE会对后处理材质进行插值
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding", meta=(EditCondition="bOverrideBlendablePriority"))
	int32 BlendablePriority = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding", AdvancedDisplay, meta=(InlineEditConditionToggle))
	bool bOverrideBlendableLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binding", AdvancedDisplay, meta=(EditCondition="bOverrideBlendableLocation"))
	TEnumAsByte<EBlendableLocation> BlendableLocation = BL_SceneColorAfterDOF;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, DuplicateTransient, Category = "Binding", AdvancedDisplay)
	TObjectPtr<UMaterialInstanceDynamic> DMI;

	/**
	 * To use Per Actor Picker, you need to enable Custom Depth and Stencil in Project Settings/Rendering/Custom Depth-Stencil Pass
	 * Per Actor Picker功能需要Custom Depth和Stencil，你可以在Project Settings/Rendering/Custom Depth-Stencil Pass中开启
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PerActorPicker")
	bool bEnablePerActorPicker = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PerActorPicker", meta = (EditCondition = "bEnablePerActorPicker", DisplayName = "Picker Mode"))
	EPhotoGraphyStencilType PerActorPickerMode = EPhotoGraphyStencilType::IncludeStencil;

	/** List of actors that get affected or ignored by Per actor CC. Effect depends on the above option. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PerActorPicker", meta = (editcondition = "bEnablePerActorPicker", DisplayName = "Actor Selection"))
	TSet<TSoftObjectPtr<AActor>> AffectedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PerActorPicker", meta = (EditCondition = "bEnablePerActorPicker", DisplayName = "Stencil Value", ClampMin=0, ClampMax=255))
	int32 ActorPickerStencil = 16;
	
	APhotoGraphyActor();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UPROPERTY(BlueprintReadOnly)
	TSet<TSoftObjectPtr<AActor>> CachedAffectedActors;
	
	UMaterialInstanceDynamic* GetOrCreateDMI();
	static FPostProcessSettings* GetPostProcessSettings(AActor* InPostProcessActor);
public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;
	
	UFUNCTION(BlueprintCallable, Category = "Binding")
	void BindingToPostProcessActor();

	UFUNCTION(BlueprintCallable, Category = "Binding")
	void UnbindingToPostProcessActor();

	UFUNCTION(BlueprintCallable, Category = "Binding")
	void UpdateBlendablePriority() const;

	UFUNCTION(BlueprintCallable, Category = "Binding")
	void UpdateBlendableLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Binding")
	void HandleAffectedActorsChangle();

};
