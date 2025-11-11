// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhotoGraphyComponent.generated.h"

UENUM(BlueprintType)
enum EPhotoGraphyShapeType : uint8
{
	Ellipse		= 0		UMETA(DisplayName="Ellipse"),
	Gradient	= 1		UMETA(DisplayName="Gradient"),
	Linear		= 2		UMETA(DisplayName="Linear"),
};

USTRUCT(BlueprintType)
struct FEllipseShape
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D PivotPos = FVector2D(0.0, 0.0);
	
	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	float RotationAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float RadiusX = 0.5f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float RadiusY = 0.5f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0", ClampMax = "1.0"))
	float Fade = 0.5f;

	bool operator==(const FEllipseShape& Other) const
	{
		return PivotPos == Other.PivotPos
			&& FMath::IsNearlyEqual(RotationAngle, Other.RotationAngle, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(RadiusX, Other.RadiusX, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(RadiusY, Other.RadiusY, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(Fade, Other.Fade, KINDA_SMALL_NUMBER);
	}
};

USTRUCT(BlueprintType)
struct FGradientShape
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D PivotPos = FVector2D(0.0, 0.0);
	
	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	float RotationAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float Fade = 0.5f;

	bool operator==(const FGradientShape& Other) const
	{
		return PivotPos == Other.PivotPos
			&& FMath::IsNearlyEqual(RotationAngle, Other.RotationAngle, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(Fade, Other.Fade, KINDA_SMALL_NUMBER);
	}
};

USTRUCT(BlueprintType)
struct FLinearShape
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D PivotPos = FVector2D(0.0, 0.0);
	
	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	float RotationAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D A = FVector2D( -0.5, -0.5 );

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D B = FVector2D( 0.5, -0.5 );

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D C = FVector2D( 0.5, 0.5 );

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	FVector2D D = FVector2D( -0.5, 0.5 );
	
	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float FadeAB = 0.2f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float FadeBC = 0.2f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float FadeCD = 0.2f;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(ClampMin = "0.0"))
	float FadeDA = 0.2f;

	bool operator==(const FLinearShape& Other) const
	{
		return PivotPos == Other.PivotPos
			&& FMath::IsNearlyEqual(RotationAngle, Other.RotationAngle, KINDA_SMALL_NUMBER)
			&& A == Other.A && B == Other.B && C == Other.C && D == Other.D
			&& FMath::IsNearlyEqual(FadeAB, Other.FadeAB, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(FadeBC, Other.FadeBC, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(FadeCD, Other.FadeCD, KINDA_SMALL_NUMBER)
			&& FMath::IsNearlyEqual(FadeDA, Other.FadeDA, KINDA_SMALL_NUMBER);
	}
};

USTRUCT(Blueprintable)
struct FPhotoGraphyShapeModel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp)
	TEnumAsByte<EPhotoGraphyShapeType> ShapeType = EPhotoGraphyShapeType::Ellipse;
	
	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(EditCondition = "ShapeType==EPhotoGraphyShapeType::Ellipse", EditConditionHides))
	FEllipseShape EllipseShape;

	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(EditCondition = "ShapeType==EPhotoGraphyShapeType::Gradient", EditConditionHides))
	FGradientShape GradientShape;
	
	UPROPERTY(EditAnywhere, Category="PhotoGraphyShape", Interp, meta=(EditCondition = "ShapeType==EPhotoGraphyShapeType::Linear", EditConditionHides))
	FLinearShape LinearShape;
	
	bool operator==( const FPhotoGraphyShapeModel& NewModel ) const = default;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShapeModelUpdated,const FPhotoGraphyShapeModel&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPostShapeModelChanged, const FPhotoGraphyShapeModel&, NewShapeModel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhotoGraphyComponentTick, float, DeltaTime);

UCLASS(ClassGroup=(PhotoGraphy), meta=(BlueprintSpawnableComponent))
class YKPHOTOGRAPHY_API UPhotoGraphyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPhotoGraphyComponent();

	UPROPERTY(EditAnywhere, Category="PhotoGraphy", Interp, Setter = "SetShapeModel", meta=(ShowOnlyInnerProperties))
	FPhotoGraphyShapeModel ShapeModel;

	UPROPERTY(BlueprintAssignable, Category = "PhotoGraphy", Meta = (DisplayName = "On PhotoGraphy Component Tick"))
	FOnPhotoGraphyComponentTick OnPhotoGraphyComponentTick;
	
	UPROPERTY(BlueprintAssignable, Category = "PhotoGraphy", Meta = (DisplayName = "Post ShapeModel Changed"))
	FPostShapeModelChanged OnPostShapeModelChanged;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType,
	                            FActorComponentTickFunction* ThisTickFunction ) override;

	// virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	virtual void PostInitProperties() override;
	
	UFUNCTION(BlueprintCallable, Category="PhotoGraphy")
	void SetShapeModel(const FPhotoGraphyShapeModel& NewModel);

	void PostShapeModelChanged(const FPhotoGraphyShapeModel& NewModel) const;
	//
	// UFUNCTION(BlueprintNativeEvent, Category="PhotoGraphy")
	// void PostShapeModelChangedBlueprint (const FPhotoGraphyShapeModel& NewModel);
};
