#pragma once
#include "SceneViewExtension.h"
#include "ScreenPass.h"
#include "ShaderParameterStruct.h"

BEGIN_SHADER_PARAMETER_STRUCT(FViewCanvasParameters, )
	SHADER_PARAMETER(FVector2f, CanvasOffset)
	SHADER_PARAMETER(FVector2f, CanvasZoom)
	// Radians
	SHADER_PARAMETER(float, CanvasRotation)
END_SHADER_PARAMETER_STRUCT()

class FPhotoGraphySceneViewExtension : public FSceneViewExtensionBase
{
public:
	FPhotoGraphySceneViewExtension(const FAutoRegister& AutoRegister);

public:
	bool bEnabled = false;
	
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {};

	FViewCanvasParameters GetCanvasParameters() const{ return ViewCanvasParameters;}
	
	void SetCanvasParameters(const FViewCanvasParameters& InViewCanvasParameters, bool bEnabledIn)
	{
		bEnabled = bEnabledIn;
		ViewCanvasParameters = InViewCanvasParameters;
	}

	// See SceneViewExtension.h for hooks to different stages of rendering
	// f.ex. PrePostProcessPass_RenderThread happens just when rendering is finished but PostProcessing hasn't started yet

	// This is the method to hook into PostProcessing pass
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& View, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;

	// This is our actual processing function
	FScreenPassTexture CustomPostProcessing(FRDGBuilder& GraphBuilder, const FSceneView& SceneView, const FPostProcessMaterialInputs& Inputs) const;

private:
	FViewCanvasParameters ViewCanvasParameters;
};


BEGIN_SHADER_PARAMETER_STRUCT(FViewCanvasCSParameters, )
	SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, SceneColorViewport)
	SHADER_PARAMETER_STRUCT(FViewCanvasParameters, ViewCanvas)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, OriginalSceneColor)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, Output)
END_SHADER_PARAMETER_STRUCT()

// Custom Post Process Shader
class YKPHOTOGRAPHYEDITOR_API FViewCanvasShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FViewCanvasShader)
	using FParameters = FViewCanvasCSParameters;
	SHADER_USE_PARAMETER_STRUCT(FViewCanvasShader, FGlobalShader)
	
	// Basic shader initialization
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	// Define environment variables used by compute shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADS_X"), 8);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), 8);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);
	}
};