// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "YKPhotoGraphyBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class YKPHOTOGRAPHY_API UYKPhotoGraphyBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "YKTools|Material")
	static UMaterialInstanceDynamic* GetOrCreateTransientMID(UMaterialInstanceDynamic* InMID, FName InMIDName, UMaterialInterface* InMaterialInterface);
	
	static UMaterialInstanceDynamic* GetOrCreateTransientMID(UMaterialInstanceDynamic* InMID, FName InMIDName, UMaterialInterface* InMaterialInterface, EObjectFlags InAdditionalObjectFlags);

	static UMaterialInstanceDynamic* GetOrCreateMID(UMaterialInstanceDynamic* InMID, FName InMIDName, UMaterialInterface* InMaterialInterface, UObject* InOuter = GetTransientPackage());
};
