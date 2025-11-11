#include "PhotoGraphySceneViewExtension.h"
#include "ScreenPass.h"
#include "PostProcess/PostProcessMaterialInputs.h"

IMPLEMENT_GLOBAL_SHADER(FViewCanvasShader, "/Plugins/YKPhotoGraphy/PhotoGraphyCanvasCS.usf", "MainCS", SF_Compute);

FPhotoGraphySceneViewExtension::FPhotoGraphySceneViewExtension( const FAutoRegister& AutoRegister ) : FSceneViewExtensionBase(AutoRegister)
{
}

void FPhotoGraphySceneViewExtension::SubscribeToPostProcessingPass( EPostProcessingPass PassId, const FSceneView& View,
	FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled )
{
	// Define to what Post Processing stage to hook the SceneViewExtension into. See SceneViewExtension.h and PostProcessing.cpp for more info
	if (PassId == EPostProcessingPass::VisualizeDepthOfField)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FPhotoGraphySceneViewExtension::CustomPostProcessing));
	}
}

FScreenPassTexture FPhotoGraphySceneViewExtension::CustomPostProcessing( FRDGBuilder& GraphBuilder,
	const FSceneView& SceneView, const FPostProcessMaterialInputs& Inputs ) const
{
	const FSceneViewFamily& ViewFamily = *SceneView.Family;
	
	FScreenPassRenderTarget OverrideOutput = Inputs.OverrideOutput;
	if (!bEnabled)
	{
		FScreenPassTextureSlice& SceneColorSlice = const_cast<FScreenPassTextureSlice&>(Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor]);
		return FScreenPassTexture::CopyFromSlice(GraphBuilder, SceneColorSlice, static_cast<FScreenPassTexture>(OverrideOutput));
	}
	
	const FScreenPassTexture& SceneColor = FScreenPassTexture::CopyFromSlice(GraphBuilder, Inputs.GetInput(EPostProcessMaterialInput::SceneColor));
	if (!OverrideOutput.IsValid())
	{
		OverrideOutput = FScreenPassRenderTarget::CreateFromInput(GraphBuilder, SceneColor, SceneView.GetOverwriteLoadAction(), TEXT("PhotoGraphyRenderTarget"));
	}
	
	const FScreenPassTextureViewport SceneColorViewport(SceneColor);
	
	// Here starts the RDG stuff
	RDG_EVENT_SCOPE(GraphBuilder, "YK Photo Graphy Canvas");
	{
		// Accesspoint to our Shaders
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(ViewFamily.GetFeatureLevel());
	
		// Setup all the descriptors to create a target texture
		FRDGTextureDesc OutputDesc;
		{
			OutputDesc = OverrideOutput.Texture->Desc;
	
			// OutputDesc.Reset();
			OutputDesc.Flags |= TexCreate_UAV;
			OutputDesc.Flags &= ~(TexCreate_RenderTargetable | TexCreate_FastVRAM);
	
			FLinearColor ClearColor(0., 0., 0., 0.);
			OutputDesc.ClearValue = FClearValueBinding(ClearColor);
		}
	
		// Create target texture
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputDesc, TEXT("PhotoGraphyCanvasTexture"));
	
		// Set the shader parameters
		FViewCanvasShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FViewCanvasShader::FParameters>();
	
		// Input is the SceneColor from PostProcess Material Inputs
		PassParameters->OriginalSceneColor = SceneColor.Texture;
	
		// Use ScreenPassTextureViewportParameters so we don't need to calculate these ourselves
		PassParameters->SceneColorViewport = GetScreenPassTextureViewportParameters(SceneColorViewport);
		PassParameters->ViewCanvas = this->GetCanvasParameters();
	
		FIntPoint PassViewSize = SceneColor.ViewRect.Size();
		
		// Create UAV from Target Texture
		PassParameters->Output = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputTexture));
	
		// Set Compute Shader and execute
		FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(PassViewSize, FComputeShaderUtils::kGolden2DGroupSize);
	
		TShaderMapRef<FViewCanvasShader> ComputeShader(GlobalShaderMap);
	
		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("PhotoGraphyCanvas CS Shader %dx%d", PassViewSize.X, PassViewSize.Y),
			ComputeShader,
			PassParameters,
			GroupCount);
	
		// Copy the output texture back to SceneColor
		// Returning the new texture as ScreenPassTexture doesn't work, so this is pretty fast alternative
		// Also with f.ex 'PrePostProcessPass_RenderThread' you get only input and something similar needs to be implemented then
		AddCopyTexturePass(GraphBuilder, OutputTexture, OverrideOutput.Texture);
	}
	
	// The call expects ScreenPassTexture as a return, we return with the same texture as we started with, see AddCopyTexturePass above 
	return static_cast<FScreenPassTexture>(OverrideOutput);
}

