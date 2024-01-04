#include <Engine/Renderer/Material.h>

#include <Engine/Vulkan/Image.h>

namespace hyperion
{
	Material::Material()
	{

	}

	Material::~Material()
	{
		if (mDiffuseImage) delete mDiffuseImage;
		if (mSpecularImage) delete mSpecularImage;
		if (mAmbientImage) delete mAmbientImage;
		if (mEmissiveImage) delete mEmissiveImage;
		if (mHeightImage) delete mHeightImage;
		if (mNormalImage) delete mNormalImage;
		if (mShininessImage) delete mShininessImage;
		if (mOpacityImage) delete mOpacityImage;
		if (mDisplacementImage) delete mDisplacementImage;
		if (mLightMapImage) delete mLightMapImage;
		if (mReflectionImage) delete mReflectionImage;

		if (mBaseColorImage) delete mBaseColorImage;
		if (mNormalCameraImage) delete mNormalCameraImage;
		if (mEmissionColorImage) delete mEmissionColorImage;
		if (mMetalnessImage) delete mMetalnessImage;
		if (mDiffuseRoughnessImage) delete mDiffuseRoughnessImage;
		if (mAmbientOcclusionImage) delete mAmbientOcclusionImage;
	}
}