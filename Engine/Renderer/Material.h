#pragma once

#include <string>

#include <Engine/Forward.h>

namespace hyperion
{
	class Material
	{
	public:

		inline auto const& GetName() const { return mName; }
		inline auto GetDiffuseImage() const { return mDiffuseImage; }
		inline auto GetSpecularImage() const { return mSpecularImage; }
		inline auto GetAmbientImage() const { return mAmbientImage; }
		inline auto GetEmissiveImage() const { return mEmissiveImage; }
		inline auto GetHeightImage() const { return mHeightImage; }
		inline auto GetNormalImage() const { return mNormalImage; }
		inline auto GetShininessImage() const { return mShininessImage; }
		inline auto GetOpacityImage() const { return mOpacityImage; }
		inline auto GetDisplacementImage() const { return mDisplacementImage; }
		inline auto GetLightMapImage() const { return mLightMapImage; }
		inline auto GetReflectionImage() const { return mReflectionImage; }
		inline auto GetBaseColorImage() const { return mBaseColorImage; }
		inline auto GetNormalCameraImage() const { return mNormalCameraImage; }
		inline auto GetEmissionColorImage() const { return mEmissionColorImage; }
		inline auto GetMetallnessImage() const { return mMetalnessImage; }
		inline auto GetDiffuseRoughnessImage() const { return mDiffuseRoughnessImage; }
		inline auto GetAmbientOcclusionImage() const { return mAmbientOcclusionImage; }

	public:

		inline void SetName(std::string const& Value) { mName = Value; }
		inline void SetDiffuseImage(Image* Value) { mDiffuseImage = Value; }
		inline void SetSpecularImage(Image* Value) { mSpecularImage = Value; }
		inline void SetAmbientImage(Image* Value) { mAmbientImage = Value; }
		inline void SetEmissiveImage(Image* Value) { mEmissiveImage = Value; }
		inline void SetHeightImage(Image* Value) { mHeightImage = Value; }
		inline void SetNormalImage(Image* Value) { mNormalImage = Value; }
		inline void SetShininessImage(Image* Value) { mShininessImage = Value; }
		inline void SetOpacityImage(Image* Value) { mOpacityImage = Value; }
		inline void SetDisplacementImage(Image* Value) { mDisplacementImage = Value; }
		inline void SetLightMapImage(Image* Value) { mLightMapImage = Value; }
		inline void SetReflectionImage(Image* Value) { mReflectionImage = Value; }
		inline void SetBaseColorImage(Image* Value) { mBaseColorImage = Value; }
		inline void SetNormalCameraImage(Image* Value) { mNormalCameraImage = Value; }
		inline void SetEmissionColorImage(Image* Value) { mEmissionColorImage = Value; }
		inline void SetMetalnessImage(Image* Value) { mMetalnessImage = Value; }
		inline void SetDiffuseRoughnessImage(Image* Value) { mDiffuseRoughnessImage = Value; }
		inline void SetAmbientOcclusionImage(Image* Value) { mAmbientOcclusionImage = Value; }

	public:

		Material();
		virtual ~Material();

	private:

		std::string mName = "";

		Image* mDiffuseImage = 0;
		Image* mSpecularImage = 0;
		Image* mAmbientImage = 0;
		Image* mEmissiveImage = 0;
		Image* mHeightImage = 0;
		Image* mNormalImage = 0;
		Image* mShininessImage = 0;
		Image* mOpacityImage = 0;
		Image* mDisplacementImage = 0;
		Image* mLightMapImage = 0;
		Image* mReflectionImage = 0;

		Image* mBaseColorImage = 0;
		Image* mNormalCameraImage = 0;
		Image* mEmissionColorImage = 0;
		Image* mMetalnessImage = 0;
		Image* mDiffuseRoughnessImage = 0;
		Image* mAmbientOcclusionImage = 0;
	};
}