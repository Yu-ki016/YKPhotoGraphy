// Fill out your copyright notice in the Description page of Project Settings.

#include "Library/YKPhotoGraphyBPLibrary.h"


UMaterialInstanceDynamic* UYKPhotoGraphyBPLibrary::GetOrCreateTransientMID( UMaterialInstanceDynamic* InMID, FName InMIDName,
                                                                            UMaterialInterface* InMaterialInterface, EObjectFlags InAdditionalObjectFlags )
{
	if (!IsValid(InMaterialInterface))
	{
		return nullptr;
	}

	UMaterialInstanceDynamic* ResultMID = InMID;

	// If there's no MID yet or if the MID's parent material interface (could be a Material or a MIC) doesn't match the requested material interface (could be a Material, MIC or MID), create a new one : 
	if (!IsValid(InMID) || (InMID->Parent != InMaterialInterface))
	{
		// If the requested material is already a UMaterialInstanceDynamic, we can use it as is :
		ResultMID = Cast<UMaterialInstanceDynamic>(InMaterialInterface);

		if (ResultMID != nullptr)
		{
			ensure(EnumHasAnyFlags(InMaterialInterface->GetFlags(), EObjectFlags::RF_Transient)); // The name of the function implies we're dealing with transient MIDs
		}
		else
		{
			// If it's not a UMaterialInstanceDynamic, it's a UMaterialInstanceConstant or a UMaterial, both of which can be used to create a MID : 
			ResultMID = UMaterialInstanceDynamic::Create(InMaterialInterface, nullptr, MakeUniqueObjectName(GetTransientPackage(), UMaterialInstanceDynamic::StaticClass(), InMIDName));
			ResultMID->SetFlags(InAdditionalObjectFlags);
		}
	}

	check(ResultMID != nullptr);
	return ResultMID;
}

UMaterialInstanceDynamic* UYKPhotoGraphyBPLibrary::GetOrCreateTransientMID( UMaterialInstanceDynamic* InMID, FName InMIDName,
	UMaterialInterface* InMaterialInterface )
{
	return GetOrCreateTransientMID(InMID, InMIDName, InMaterialInterface, RF_NoFlags);
}

UMaterialInstanceDynamic* UYKPhotoGraphyBPLibrary::GetOrCreateMID( UMaterialInstanceDynamic* InMID, FName InMIDName,
	UMaterialInterface* InMaterialInterface, UObject* InOuter )
{
	if (!IsValid(InMaterialInterface))
	{
		return nullptr;
	}

	UMaterialInstanceDynamic* ResultMID = InMID;
	
	if (!IsValid(InMID) || (InMID->Parent != InMaterialInterface))
	{
		// If the requested material is already a UMaterialInstanceDynamic, we can use it as is :
		ResultMID = Cast<UMaterialInstanceDynamic>(InMaterialInterface);

		if (ResultMID == nullptr)
		{
			ResultMID = UMaterialInstanceDynamic::Create(InMaterialInterface, InOuter, MakeUniqueObjectName(InOuter, UMaterialInstanceDynamic::StaticClass(), InMIDName));
		}
	}
	
	check(ResultMID != nullptr);
	return ResultMID;
}
