// Copyright Epic Games, Inc. All Rights Reserved.

#include "YKPhotoGraphyEditorModule.h"
#include "FPhotoGraphyEditorStyle.h"
#include "PhotoGraphyDetialPanel.h"
#include "PhotoGraphyEditorModeCommands.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "YKPhotoGraphyEditorModule"

void FYKPhotoGraphyEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FYKPhotoGraphyEditorModule::OnPostEngineInit);

	// Set up the Shader Directories
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("YKPhotoGraphy"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/YKPhotoGraphy"), PluginShaderDir);

}

void FYKPhotoGraphyEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FPhotoGraphyEditorStyle::Shutdown();
	FPhotoGraphyEditorModeCommands::Unregister();
}

void FYKPhotoGraphyEditorModule::Register()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("PhotoGraphyGenerateToolProperties", FOnGetDetailCustomizationInstance::CreateStatic(&FPhotoGraphyGenerateSettingsCustomization::MakeInstance));
}

void FYKPhotoGraphyEditorModule::OnPostEngineInit()
{
	Register();
	FPhotoGraphyEditorStyle::Initialize();
	FPhotoGraphyEditorModeCommands::Register();
}
void FYKPhotoGraphyEditorModule::Unregister()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FYKPhotoGraphyEditorModule, YKPhotoGraphyEditor)