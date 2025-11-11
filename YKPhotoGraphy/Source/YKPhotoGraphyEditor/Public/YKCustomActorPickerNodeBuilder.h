// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActorPickerMode.h"
#include "IDetailCustomNodeBuilder.h"
#include "Templates/SharedPointer.h"

class IPropertyHandle;
class IPropertyUtilities;
class SComboButton;
class SWidget;

/** Used to customize an actor picker to filter items based on a delegate */
class FYKCustomActorPickerNodeBuilder : public IDetailCustomNodeBuilder, public TSharedFromThis<FYKCustomActorPickerNodeBuilder> 
{
public:
	explicit FYKCustomActorPickerNodeBuilder(const TSharedRef<IPropertyHandle>& InPropertyHandle, FOnShouldFilterActor InActorFilterDelegate)
		: PropertyHandle(InPropertyHandle)
		, ActorFilterDelegate(InActorFilterDelegate)
	{}

private:
	static TSharedPtr<SWidget> FindWidgetType(TSharedPtr<SWidget> InSearchWidget, FName InTypeName, bool bInReverseSearch);

	//~ Begin IDetailCustomNodeBuilder
	virtual FName GetName() const override;
	virtual void GenerateHeaderRowContent(FDetailWidgetRow& InNodeRow) override;
	virtual void GenerateChildContent(IDetailChildrenBuilder& InChildrenBuilder) override;
	virtual TSharedPtr<IPropertyHandle> GetPropertyHandle() const override;
	//~ End IDetailCustomNodeBuilder

	FText GetPickerLabelText() const;
	FText GetSelectTooltipText() const;
	FText GetUseSelectTooltipText() const;
	TSharedRef<SWidget> GetActorPickerWidget() const;

	void OnActorSelected(AActor* InSelection) const;
	void OnSelectActor() const;
	void OnUseSelectedActor() const;

	AActor* GetPropertyActor() const;
	AActor* GetSelectedActor() const;

	TSharedRef<IPropertyHandle> PropertyHandle;
	TSharedPtr<SComboButton> ComboButton;
	FOnShouldFilterActor ActorFilterDelegate;
};
