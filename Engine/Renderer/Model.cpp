#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Bone.h>
#include <Engine/Animation/BoneInfo.h>
#include <Engine/Animation/Skeleton.h>

#include <Engine/Assimp/Importer.hpp>
#include <Engine/Assimp/Exporter.hpp>
#include <Engine/Assimp/postprocess.h>
#include <Engine/Assimp/scene.h>

#include <Engine/Common/Macros.h>

#include <Engine/Renderer/Model.h>
#include <Engine/Renderer/Material.h>
#include <Engine/Renderer/Mesh.h>
#include <Engine/Renderer/Vertex.h>

#include <Engine/Stbimage/stb_image.h>

#include <Engine/Utility/AssimpUtility.h>
#include <Engine/Utility/ImageUtility.h>

#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/ImageVariance.h>
#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/BufferVariance.h>

namespace hyperion
{
	Model* Model::Load(std::filesystem::path const& File)
	{
		Assimp::Importer importer = {};

		std::string filePath = File.string();
		std::string fileName = File.stem().string();

		U32 importFlags = aiProcess_ValidateDataStructure | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData;

		aiScene const* scene = importer.ReadFile(filePath, importFlags);

		if (scene && (scene->mFlags & ~(AI_SCENE_FLAGS_INCOMPLETE)) && scene->mRootNode)
		{
			return new Model{ fileName, scene };
		}

		return 0;
	}

	Model::Model(std::string const& Name, aiScene const* Scene)
	{
		mName = Name;

		mSkeleton = new Skeleton;

		LoadMaterials(Scene);
		LoadMeshes(Scene);
		LoadAnimations(Scene);

		mSkeleton->BuildBoneHierarchy();
	}

	Model::~Model()
	{
		delete mSkeleton;

		for (auto const& [name, material] : mMaterials)
		{
			delete material;
		}

		for (auto const& [name, mesh] : mMeshes)
		{
			delete mesh;
		}

		for (auto const& [name, animation] : mAnimations)
		{
			delete animation;
		}
	}

	void Model::LoadMaterials(aiScene const* Scene)
	{
		mAsyncImageResources.resize(Scene->mNumMaterials * 17);

		for (U32 materialIndex = 0, resourceIndex = 0; materialIndex < Scene->mNumMaterials; ++materialIndex, resourceIndex += 17)
		{
			aiMaterial const* assimpMaterial = Scene->mMaterials[materialIndex];

			mAsyncImageResources[resourceIndex + 0] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_DIFFUSE); });
			mAsyncImageResources[resourceIndex + 1] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_SPECULAR); });
			mAsyncImageResources[resourceIndex + 2] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_AMBIENT); });
			mAsyncImageResources[resourceIndex + 3] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_EMISSIVE); });
			mAsyncImageResources[resourceIndex + 4] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_HEIGHT); });
			mAsyncImageResources[resourceIndex + 5] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_NORMALS); });
			mAsyncImageResources[resourceIndex + 6] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_SHININESS); });
			mAsyncImageResources[resourceIndex + 7] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_OPACITY); });
			mAsyncImageResources[resourceIndex + 8] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_DISPLACEMENT); });
			mAsyncImageResources[resourceIndex + 9] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_LIGHTMAP); });
			mAsyncImageResources[resourceIndex + 10] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_REFLECTION); });
			mAsyncImageResources[resourceIndex + 11] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_BASE_COLOR); });
			mAsyncImageResources[resourceIndex + 12] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_NORMAL_CAMERA); });
			mAsyncImageResources[resourceIndex + 13] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_EMISSIVE); }); // TODO
			mAsyncImageResources[resourceIndex + 14] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_METALNESS); });
			mAsyncImageResources[resourceIndex + 15] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS); });
			mAsyncImageResources[resourceIndex + 16] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, assimpMaterial, aiTextureType_AMBIENT_OCCLUSION); });
		}

		for (auto const& asyncImageResource : mAsyncImageResources)
		{
			asyncImageResource.wait();
		}

		for (U32 materialIndex = 0, resourceIndex = 0; materialIndex < Scene->mNumMaterials; ++materialIndex, resourceIndex += 17)
		{
			aiMaterial const* assimpMaterial = Scene->mMaterials[materialIndex];

			char const* materialName = assimpMaterial->GetName().C_Str();

			Image* diffuseImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 0].get());
			Image* specularImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 1].get());
			Image* ambientImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 2].get());
			Image* emissiveImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 3].get());
			Image* heightImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 4].get());
			Image* normalImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 5].get());
			Image* shininessImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 6].get());
			Image* opacityImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 7].get());
			Image* displacementImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 8].get());
			Image* lightMapImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 9].get());
			Image* reflectionImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 10].get());
			Image* baseColorImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 11].get());
			Image* normalCameraImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 12].get());
			Image* emissionColorImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 13].get());
			Image* metalnessImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 14].get());
			Image* diffuseRoughnessImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 15].get());
			Image* ambientOcclusionImage = ExchangeResourceForImageAndReleaseData(mAsyncImageResources[resourceIndex + 16].get());

			if (mMaterials.find(materialName) == mMaterials.end())
			{
				mMaterials[materialName] = new Material;

				mMaterials[materialName]->SetName(materialName);

				mMaterials[materialName]->SetDiffuseImage(diffuseImage);
				mMaterials[materialName]->SetSpecularImage(specularImage);
				mMaterials[materialName]->SetAmbientImage(ambientImage);
				mMaterials[materialName]->SetEmissiveImage(emissiveImage);
				mMaterials[materialName]->SetHeightImage(heightImage);
				mMaterials[materialName]->SetNormalImage(normalImage);
				mMaterials[materialName]->SetShininessImage(shininessImage);
				mMaterials[materialName]->SetOpacityImage(opacityImage);
				mMaterials[materialName]->SetDisplacementImage(displacementImage);
				mMaterials[materialName]->SetLightMapImage(lightMapImage);
				mMaterials[materialName]->SetReflectionImage(reflectionImage);
				mMaterials[materialName]->SetBaseColorImage(baseColorImage);
				mMaterials[materialName]->SetNormalCameraImage(normalCameraImage);
				mMaterials[materialName]->SetEmissionColorImage(emissionColorImage);
				mMaterials[materialName]->SetMetalnessImage(metalnessImage);
				mMaterials[materialName]->SetDiffuseRoughnessImage(diffuseRoughnessImage);
				mMaterials[materialName]->SetAmbientOcclusionImage(ambientOcclusionImage);
			}
			else
			{
				assert(0);
			}
		}

		mAsyncImageResources.clear();
	}

	void Model::LoadMeshes(aiScene const* Scene)
	{
		std::vector<PhysicallyBasedVertex> vertices = {};
		std::vector<U32> indices = {};

		for (U32 meshIndex = 0; meshIndex < Scene->mNumMeshes; ++meshIndex)
		{
			aiMesh const* assimpMesh = Scene->mMeshes[meshIndex];

			char const* meshName = assimpMesh->mName.C_Str();

			Mesh* mesh = new Mesh;

			assert(assimpMesh->mNumVertices > 0);

			vertices.resize(assimpMesh->mNumVertices);

			for (U32 vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; ++vertexIndex)
			{
				if (assimpMesh->HasPositions())
				{
					vertices[vertexIndex].Position = AssimpUtility::ToGlmVector3(assimpMesh->mVertices[vertexIndex]);;
				}

				if (assimpMesh->HasNormals())
				{
					vertices[vertexIndex].Normal = AssimpUtility::ToGlmVector3(assimpMesh->mNormals[vertexIndex]);
				}

				if (assimpMesh->HasTangentsAndBitangents())
				{
					vertices[vertexIndex].Tangent = AssimpUtility::ToGlmVector3(assimpMesh->mTangents[vertexIndex]);
					vertices[vertexIndex].BiTangent = AssimpUtility::ToGlmVector3(assimpMesh->mBitangents[vertexIndex]);
				}

#pragma warning (push)
#pragma warning (disable : 26819)
				switch (assimpMesh->GetNumColorChannels())
				{
					case 8: vertices[vertexIndex].ColorChannel7 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[7][vertexIndex]);
					case 7: vertices[vertexIndex].ColorChannel6 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[6][vertexIndex]);
					case 6: vertices[vertexIndex].ColorChannel5 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[5][vertexIndex]);
					case 5: vertices[vertexIndex].ColorChannel4 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[4][vertexIndex]);
					case 4: vertices[vertexIndex].ColorChannel3 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[3][vertexIndex]);
					case 3: vertices[vertexIndex].ColorChannel2 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[2][vertexIndex]);
					case 2: vertices[vertexIndex].ColorChannel1 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[1][vertexIndex]);
					case 1: vertices[vertexIndex].ColorChannel0 = AssimpUtility::ToGlmVector4(assimpMesh->mColors[0][vertexIndex]);
					case 0: break;
					default: assert(0); break;
				}

				switch (assimpMesh->GetNumUVChannels())
				{
					case 8: vertices[vertexIndex].TexCoordChannel7 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[7][vertexIndex]);
					case 7: vertices[vertexIndex].TexCoordChannel6 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[6][vertexIndex]);
					case 6: vertices[vertexIndex].TexCoordChannel5 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[5][vertexIndex]);
					case 5: vertices[vertexIndex].TexCoordChannel4 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[4][vertexIndex]);
					case 4: vertices[vertexIndex].TexCoordChannel3 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[3][vertexIndex]);
					case 3: vertices[vertexIndex].TexCoordChannel2 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[2][vertexIndex]);
					case 2: vertices[vertexIndex].TexCoordChannel1 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[1][vertexIndex]);
					case 1: vertices[vertexIndex].TexCoordChannel0 = AssimpUtility::ToGlmVector3(assimpMesh->mTextureCoords[0][vertexIndex]);
					case 0: break;
					default: assert(0); break;
				}
#pragma warning (pop)

				vertices[vertexIndex].BoneIndices = I32V4{ -1, -1, -1, -1 };
				vertices[vertexIndex].BoneWeights = R32V4{ 0.0F };
			}

			for (U32 boneIndex = 0; boneIndex < assimpMesh->mNumBones; ++boneIndex)
			{
				aiBone const* assimpBone = assimpMesh->mBones[boneIndex];
				
				char const* parentBoneName = (assimpBone->mNode->mParent) ? assimpBone->mNode->mParent->mName.C_Str() : 0;
				char const* boneName = assimpBone->mName.C_Str();

				R32M4 boneTransform = AssimpUtility::ToGlmMatrix4(assimpBone->mNode->mTransformation);
				R32M4 boneOffset = AssimpUtility::ToGlmMatrix4(assimpBone->mOffsetMatrix);

				BoneInfo* boneInfo = mSkeleton->CreateBone(boneName, parentBoneName, boneTransform, boneOffset);

				aiVertexWeight const* weights = assimpBone->mWeights;

				for (U32 weightIndex = 0; weightIndex < assimpBone->mNumWeights; ++weightIndex)
				{
					U32 vertexId = weights[weightIndex].mVertexId;
					R32 weight = weights[weightIndex].mWeight;

					assert(vertexId <= assimpMesh->mNumVertices);

					for (U32 i = 0; i < 4; ++i)
					{
						if (vertices[vertexId].BoneIndices[i] < 0)
						{
							vertices[vertexId].BoneIndices[i] = boneInfo->GetBoneIndex();
							vertices[vertexId].BoneWeights[i] = weight;

							break;
						}
					}
				}
			}

			assert(assimpMesh->mNumFaces > 0);

			indices.resize(assimpMesh->mNumFaces * 3);

			for (U32 faceIndex = 0, indicesIndex = 0; faceIndex < assimpMesh->mNumFaces; ++faceIndex, indicesIndex += 3)
			{
				aiFace const assimpFace = assimpMesh->mFaces[faceIndex];

				assert(assimpFace.mNumIndices == 3);

				indices[indicesIndex + 0] = assimpFace.mIndices[0];
				indices[indicesIndex + 1] = assimpFace.mIndices[1];
				indices[indicesIndex + 2] = assimpFace.mIndices[2];
			}

			char const* materialName = Scene->mMaterials[assimpMesh->mMaterialIndex]->GetName().C_Str();

			assert(materialName);

			Material* material = mMaterials[materialName];

			assert(material);

			Buffer* vertexBuffer = BufferVariance::CreateVertex(vertices.data(), sizeof(PhysicallyBasedVertex) * vertices.size());
			Buffer* indexBuffer = BufferVariance::CreateIndex(indices.data(), sizeof(U32) * indices.size());

			mesh->SetMaterial(material);
			mesh->SetVertexBuffer(vertexBuffer);
			mesh->SetIndexBuffer(indexBuffer);

			if (mMeshes.find(meshName) == mMeshes.end())
			{
				mMeshes[meshName] = mesh;
			}
			else
			{
				assert(0);
			}
		}
	}

	void Model::LoadAnimations(aiScene const* Scene)
	{
		for (U32 animationIndex = 0; animationIndex < Scene->mNumAnimations; ++animationIndex)
		{
			aiAnimation const* assimpAnimation = Scene->mAnimations[animationIndex];

			char const* animationName = assimpAnimation->mName.C_Str();

			R32 duration = (R32)assimpAnimation->mDuration;
			R32 ticksPerSecond = (R32)assimpAnimation->mTicksPerSecond;

			std::vector<BoneChannelView> positionBoneChannelViews = {};
			std::vector<BoneChannelView> rotationBoneChannelViews = {};
			std::vector<BoneChannelView> scaleBoneChannelViews = {};

			std::vector<KeyFramePosition> positionKeyFrames = {};
			std::vector<KeyFrameRotation> rotationKeyFrames = {};
			std::vector<KeyFrameScale> scaleKeyFrames = {};

			U32 validBoneChannelCount = 0;

			for (U32 channelIndex = 0; channelIndex < assimpAnimation->mNumChannels; ++channelIndex)
			{
				aiNodeAnim const* assimpNodeAnimation = assimpAnimation->mChannels[channelIndex];

				char const* boneName = assimpNodeAnimation->mNodeName.C_Str();

				BoneInfo* boneInfo = mSkeleton->FindBone(boneName);

				if (!boneInfo)
				{
					aiNode const* assimpNode = AssimpUtility::FindNodeByNameRecursive(Scene->mRootNode, boneName);

					if (assimpNode)
					{
						char const* parentBoneName = (assimpNode->mParent) ? assimpNode->mParent->mName.C_Str() : 0;

						R32M4 boneTransform = AssimpUtility::ToGlmMatrix4(assimpNode->mTransformation);
						//R32M4 boneOffset = AssimpUtility::ToGlmMatrix4(assimpBone->mOffsetMatrix);

						//boneInfo = mSkeleton->CreateBone(boneName, parentBoneName, boneTransform, R32M4{ 1.0F }); // TODO
					}
				}

				if (boneInfo)
				{
					validBoneChannelCount++;
				}
			}

			positionBoneChannelViews.resize(validBoneChannelCount);
			rotationBoneChannelViews.resize(validBoneChannelCount);
			scaleBoneChannelViews.resize(validBoneChannelCount);

			U32 validBoneChannelIndex = 0;

			for (U32 channelIndex = 0; channelIndex < assimpAnimation->mNumChannels; ++channelIndex)
			{
				aiNodeAnim const* assimpNodeAnimation = assimpAnimation->mChannels[channelIndex];

				char const* boneName = assimpNodeAnimation->mNodeName.C_Str();

				BoneInfo* boneInfo = mSkeleton->FindBone(boneName);

				if (boneInfo)
				{
					Bone* bone = boneInfo->GetBone();

					bone->ChannelViewIndex = validBoneChannelIndex;

					U32 positionKeyFrameOffset = (U32)positionKeyFrames.size();
					U32 rotationKeyFrameOffset = (U32)rotationKeyFrames.size();
					U32 scaleKeyFrameOffset = (U32)scaleKeyFrames.size();

					U32 positionKeyFrameCount = assimpNodeAnimation->mNumPositionKeys;
					U32 rotationKeyFrameCount = assimpNodeAnimation->mNumRotationKeys;
					U32 scaleKeyFrameCount = assimpNodeAnimation->mNumScalingKeys;

					assert(positionKeyFrameCount > 0);
					assert(rotationKeyFrameCount > 0);
					assert(scaleKeyFrameCount > 0);

					positionBoneChannelViews[validBoneChannelIndex] = BoneChannelView{ positionKeyFrameOffset, positionKeyFrameCount };
					rotationBoneChannelViews[validBoneChannelIndex] = BoneChannelView{ rotationKeyFrameOffset, rotationKeyFrameCount };
					scaleBoneChannelViews[validBoneChannelIndex] = BoneChannelView{ scaleKeyFrameOffset, scaleKeyFrameCount };

					positionKeyFrames.resize(positionKeyFrameOffset + positionKeyFrameCount);
					rotationKeyFrames.resize(rotationKeyFrameOffset + rotationKeyFrameCount);
					scaleKeyFrames.resize(scaleKeyFrameOffset + scaleKeyFrameCount);

					for (U32 keyIndex = 0; keyIndex < positionKeyFrameCount; ++keyIndex)
					{
						aiVectorKey positionKey = assimpNodeAnimation->mPositionKeys[keyIndex];

						R32V3 position = AssimpUtility::ToGlmVector3(positionKey.mValue);
						R32 time = (R32)positionKey.mTime;

						positionKeyFrames[positionKeyFrameOffset + keyIndex] = KeyFramePosition{ position, time };
					}

					for (U32 keyIndex = 0; keyIndex < rotationKeyFrameCount; ++keyIndex)
					{
						aiQuatKey rotationKey = assimpNodeAnimation->mRotationKeys[keyIndex];

						R32V4 rotation = R32V4{ -rotationKey.mValue.x, -rotationKey.mValue.y, -rotationKey.mValue.z, rotationKey.mValue.w };
						R32 time = (R32)rotationKey.mTime;

						rotationKeyFrames[rotationKeyFrameOffset + keyIndex] = KeyFrameRotation{ rotation, time, 0, 0, 0 };
					}

					for (U32 keyIndex = 0; keyIndex < scaleKeyFrameCount; ++keyIndex)
					{
						aiVectorKey scaleKey = assimpNodeAnimation->mScalingKeys[keyIndex];

						R32V3 scale = AssimpUtility::ToGlmVector3(scaleKey.mValue);
						R32 time = (R32)scaleKey.mTime;

						scaleKeyFrames[scaleKeyFrameOffset + keyIndex] = KeyFrameScale{ scale, time };
					}

					validBoneChannelIndex++;
				}
			}

			mAnimations[animationName] = new Animation{ animationName, duration, ticksPerSecond, positionBoneChannelViews, rotationBoneChannelViews, scaleBoneChannelViews, positionKeyFrames, rotationKeyFrames, scaleKeyFrames };
		}
	}

	void Model::PrintStats()
	{
		LOG("Model %s\n", mName.data());
		LOG("  Materials\n");

		for (auto const& [name, material] : mMaterials)
		{
			LOG("    %s\n", name.data());
		}

		LOG("\n");
		LOG("  Meshes\n");

		for (auto const& [name, mesh] : mMeshes)
		{
			LOG("    %s\n", name.data());
		}

		LOG("\n");
		LOG("  Animations\n");

		for (auto const& [name, animation] : mAnimations)
		{
			LOG("    %s\n", name.data());
		}

		LOG("\n");
	}

	AsyncImageResource Model::LoadImageOfType(aiScene const* Scene, aiMaterial const* Material, aiTextureType Type)
	{
		AsyncImageResource resource = {};

		U8* raw = 0;

		U32 textureCount = Material->GetTextureCount(Type);

		if (textureCount > 0)
		{
			assert(textureCount == 1);

			for (U32 i = 0; i < textureCount; ++i)
			{
				aiString textureFile = {};

				aiReturn result = Material->GetTexture(Type, i, &textureFile);

				if (result == AI_SUCCESS)
				{
					if (textureFile.C_Str()[0] == '*')
					{
						U32 textureIndex = std::atoi(&textureFile.C_Str()[1]);

						aiTexture const* texture = Scene->mTextures[textureIndex];

						if (std::strcmp(texture->achFormatHint, "png") == 0)
						{
							if (texture->mHeight)
							{
								U8 const* data = (U8 const*)texture->pcData;
								U32 size = texture->mWidth * texture->mHeight * sizeof(aiTexel);

								raw = stbi_load_from_memory(data, (I32)size, (I32*)&resource.Width, (I32*)&resource.Height, (I32*)&resource.Channels, 0);
							}
							else
							{
								U8 const* data = (U8 const*)texture->pcData;
								U32 size = texture->mWidth * sizeof(aiTexel);

								raw = stbi_load_from_memory(data, (I32)size, (I32*)&resource.Width, (I32*)&resource.Height, (I32*)&resource.Channels, 0);
							}
						}
						else
						{
							assert(0);
						}
					}
					else
					{
						assert(0);
					}
				}
			}
		}

		if (raw)
		{
			switch (resource.Channels)
			{
				case 4:
				{
					U32 sizeRgba = resource.Width * resource.Height * 4;

					resource.Data = new U8[sizeRgba];

					std::memcpy(resource.Data, raw, sizeRgba);

					break;
				}
				case 3:
				{
					U32 sizeRgb = resource.Width * resource.Height * 3;
					U32 sizeRgba = resource.Width * resource.Height * 4;

					resource.Data = new U8[sizeRgba];
					resource.Channels = 4;

					for (U32 i = 0, j = 0; i < sizeRgb; i += 3, j += 4)
					{
						resource.Data[j + 0] = raw[i + 0];
						resource.Data[j + 1] = raw[i + 1];
						resource.Data[j + 2] = raw[i + 2];
						resource.Data[j + 3] = 0xFF;
					}

					break;
				}
				case 2:
				{
					U32 sizeRg = resource.Width * resource.Height * 2;

					resource.Data = new U8[sizeRg];

					std::memcpy(resource.Data, raw, sizeRg);

					break;
				}
				case 1:
				{
					U32 sizeR = resource.Width * resource.Height * 1;

					resource.Data = new U8[sizeR];

					std::memcpy(resource.Data, raw, sizeR);

					break;
				}
				default: assert(0); break;
			}

			stbi_image_free(raw);
		}

		return resource;
	}

	Image* Model::ExchangeResourceForImageAndReleaseData(AsyncImageResource const& Resource)
	{
		Image* image = 0;

		if (Resource.Data)
		{
			switch (Resource.Channels)
			{
				case 4: image = ImageVariance::CreateImage2DRGBA(Resource.Data, Resource.Width, Resource.Height, VK_FORMAT_R8G8B8A8_UNORM); break;
				case 2: image = ImageVariance::CreateImage2DRG(Resource.Data, Resource.Width, Resource.Height, VK_FORMAT_R8G8_UNORM); break;
				case 1: image = ImageVariance::CreateImage2DR(Resource.Data, Resource.Width, Resource.Height, VK_FORMAT_R8_UNORM); break;
				default: assert(0); break;
			}

			delete Resource.Data;
		}
		else
		{
			std::vector<U8> dummyColor = { 0, 0, 0, 255 };

			image = ImageVariance::CreateImage2DRGBA(dummyColor.data(), 1, 1);
		}

		return image;
	}
}