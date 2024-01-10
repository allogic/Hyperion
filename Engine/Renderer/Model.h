#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <cassert>
#include <future>

#include <Engine/Forward.h>

#include <Engine/Animation/BoneHierarchy.h>

#include <Engine/Common/Types.h>

struct aiScene;
struct aiMaterial;

enum aiTextureType;

namespace hyperion
{
	struct AsyncImageResource
	{
		U32 Width;
		U32 Height;
		U32 Channels;
		U8* Data;
	};

	class Model
	{
	public:

		inline auto const& GetName() const { return mName; }
		inline auto const& GetMaterials() const { return mMaterials; }
		inline auto const& GetMeshes() const { return mMeshes; }
		inline auto const& GetAnimations() const { return mAnimations; }
		inline auto GetSkeleton() const { return mSkeleton; }

	public:

		static Model* Load(std::filesystem::path const& File);

	private:

		Model(std::string const& Name, aiScene const* Scene);

	public:

		virtual ~Model();

	private:

		void LoadMaterials(aiScene const* Scene);
		void LoadMeshes(aiScene const* Scene);
		void LoadAnimations(aiScene const* Scene);

	public:

		void PrintStats();

	private:

		AsyncImageResource LoadImageOfType(aiScene const* Scene, aiMaterial const* Material, aiTextureType Type);
		Image* ExchangeResourceForImageAndReleaseData(AsyncImageResource const& Resource);

	private:

		std::string mName = "";

	private:

		std::map<std::string, Material*> mMaterials = {};
		std::map<std::string, Mesh*> mMeshes = {};
		std::map<std::string, Animation*> mAnimations = {};

	private:

		Skeleton* mSkeleton = 0;

	private:

		std::vector<std::future<AsyncImageResource>> mAsyncImageResources = {};
	};
}