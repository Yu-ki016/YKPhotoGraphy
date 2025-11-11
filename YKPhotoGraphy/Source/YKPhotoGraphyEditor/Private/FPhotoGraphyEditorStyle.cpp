#include "FPhotoGraphyEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FPhotoGraphyEditorStyle::StyleInstance = nullptr;

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

FName FPhotoGraphyEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("YKToolsEditorStyle"));
	return StyleSetName;
}

void FPhotoGraphyEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FPhotoGraphyEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FPhotoGraphyEditorStyle::Get()
{
	return *StyleInstance;
}

TSharedRef<FSlateStyleSet> FPhotoGraphyEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

	// 设置 Style 资源根目录，一般放在插件的 Resources 文件夹里
	// FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("YKTools"))->GetBaseDir() / TEXT("Resources");
	// Style->SetContentRoot(ContentDir);
	Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	// Style->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	// 定义图标
	const FVector2D Icon20x20(20.0f, 20.0f);
	Style->Set("YKToolsEditorMode.SelectPhotoGraphyTool", new IMAGE_BRUSH("Icons/GeneralTools/Select_40x", Icon20x20));
	Style->Set("YKToolsEditorMode.EditPhotoGraphyTool", new IMAGE_BRUSH("Icons/Landscape/Landscape_BrushPattern_x40", Icon20x20));

	return Style;
}