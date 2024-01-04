#define TINYGLTF_IMPLEMENTATION

#include <Engine/Scene.h>

#include <Engine/Assimp/Importer.hpp>
#include <Engine/Assimp/Exporter.hpp>
#include <Engine/Assimp/scene.h>
#include <Engine/Assimp/postprocess.h>

#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Actors/Player.h>

#include <Engine/Ecs/Components/Camera.h>
#include <Engine/Ecs/Components/Renderable.h>

#include <Engine/Renderer/Material.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Renderer/Vertex.h>

#include <Engine/Stbimage/stb_image.h>

#include <Engine/Utility/ImageUtility.h>

#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/BufferVariance.h>
#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/ImageVariance.h>

namespace hyperion
{
	Scene::Scene()
	{
		mRoot = CreateEntity<Entity>("Root");
		mPlayer = (Entity*)CreateEntity<Player>("Player");
	}

	Scene::~Scene()
	{
		delete mRoot;

		for (auto const& [entity, sharedVertexBuffer] : mSharedVertexBuffers)
		{
			delete sharedVertexBuffer;
		}

		for (auto const& [entity, sharedIndexBuffer] : mSharedIndexBuffers)
		{
			delete sharedIndexBuffer;
		}

		for (auto const& material : mSharedMaterials)
		{
			delete material;
		}
	}

	bool Scene::Load(std::filesystem::path const& File)
	{
		Assimp::Importer importer = {};

		U32 importFlags = aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

		aiScene const* scene = importer.ReadFile(File.string(), importFlags);

		if (scene && (scene->mFlags & ~(AI_SCENE_FLAGS_INCOMPLETE)) && scene->mRootNode)
		{
			LoadMaterials(scene);
			LoadNodesRecursive(scene, scene->mRootNode);

			return true;
		}

		return false;
	}

	bool Scene::Safe(std::filesystem::path const& File)
	{
		return false;
	}

	void Scene::LoadMaterials(aiScene const* Scene)
	{
		mSharedMaterials.resize(Scene->mNumMaterials);
		mAsyncMaterialImageResources.resize(Scene->mNumMaterials * 17);

		for (U32 i = 0, j = 0; i < Scene->mNumMaterials; ++i, j += 17)
		{
			mAsyncMaterialImageResources[j + 0] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_DIFFUSE); });
			mAsyncMaterialImageResources[j + 1] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_SPECULAR); });
			mAsyncMaterialImageResources[j + 2] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_AMBIENT); });
			mAsyncMaterialImageResources[j + 3] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_EMISSIVE); });
			mAsyncMaterialImageResources[j + 4] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_HEIGHT); });
			mAsyncMaterialImageResources[j + 5] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_NORMALS); });
			mAsyncMaterialImageResources[j + 6] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_SHININESS); });
			mAsyncMaterialImageResources[j + 7] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_OPACITY); });
			mAsyncMaterialImageResources[j + 8] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_DISPLACEMENT); });
			mAsyncMaterialImageResources[j + 9] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_LIGHTMAP); });
			mAsyncMaterialImageResources[j + 10] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_REFLECTION); });
			mAsyncMaterialImageResources[j + 11] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_BASE_COLOR); });
			mAsyncMaterialImageResources[j + 12] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_NORMAL_CAMERA); });
			mAsyncMaterialImageResources[j + 13] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_EMISSIVE); }); // TODO
			mAsyncMaterialImageResources[j + 14] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_METALNESS); });
			mAsyncMaterialImageResources[j + 15] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_DIFFUSE_ROUGHNESS); });
			mAsyncMaterialImageResources[j + 16] = std::async(std::launch::async, [=]() { return LoadImageOfType(Scene, Scene->mMaterials[i], aiTextureType_AMBIENT_OCCLUSION); });
		}

		for (auto const& asyncMaterialImageResource : mAsyncMaterialImageResources)
		{
			asyncMaterialImageResource.wait();
		}

		for (U32 i = 0, j = 0; i < Scene->mNumMaterials; ++i, j += 17)
		{
			Image* diffuseImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 0].get());
			Image* specularImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 1].get());
			Image* ambientImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 2].get());
			Image* emissiveImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 3].get());
			Image* heightImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 4].get());
			Image* normalImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 5].get());
			Image* shininessImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 6].get());
			Image* opacityImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 7].get());
			Image* displacementImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 8].get());
			Image* lightMapImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 9].get());
			Image* reflectionImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 10].get());
			Image* baseColorImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 11].get());
			Image* normalCameraImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 12].get());
			Image* emissionColorImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 13].get());
			Image* metalnessImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 14].get());
			Image* diffuseRoughnessImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 15].get());
			Image* ambientOcclusionImage = ExchangeResourceForImageAndRelease(mAsyncMaterialImageResources[j + 16].get());

			mSharedMaterials[i] = new Material;

			mSharedMaterials[i]->SetName(Scene->mMaterials[i]->GetName().C_Str());

			mSharedMaterials[i]->SetDiffuseImage(diffuseImage);
			mSharedMaterials[i]->SetSpecularImage(specularImage);
			mSharedMaterials[i]->SetAmbientImage(ambientImage);
			mSharedMaterials[i]->SetEmissiveImage(emissiveImage);
			mSharedMaterials[i]->SetHeightImage(heightImage);
			mSharedMaterials[i]->SetNormalImage(normalImage);
			mSharedMaterials[i]->SetShininessImage(shininessImage);
			mSharedMaterials[i]->SetOpacityImage(opacityImage);
			mSharedMaterials[i]->SetDisplacementImage(displacementImage);
			mSharedMaterials[i]->SetLightMapImage(lightMapImage);
			mSharedMaterials[i]->SetReflectionImage(reflectionImage);
			mSharedMaterials[i]->SetBaseColorImage(baseColorImage);
			mSharedMaterials[i]->SetNormalCameraImage(normalCameraImage);
			mSharedMaterials[i]->SetEmissionColorImage(emissionColorImage);
			mSharedMaterials[i]->SetMetalnessImage(metalnessImage);
			mSharedMaterials[i]->SetDiffuseRoughnessImage(diffuseRoughnessImage);
			mSharedMaterials[i]->SetAmbientOcclusionImage(ambientOcclusionImage);
		}
	}

	void Scene::LoadNodesRecursive(aiScene const* Scene, aiNode const* Node, Entity* Parent)
	{
		Entity* entity = CreateEntity<Entity>(Node->mName.C_Str(), Parent);

		Transform* transform = GetTransform(entity);

		aiVector3D position = {};
		aiQuaternion rotation = {};
		aiVector3D scale = {};

		Node->mTransformation.Decompose(scale, rotation, position);

		transform->LocalPosition = R32V3{ position.x, position.y, position.z };
		transform->LocalEulerAngles = glm::degrees(glm::eulerAngles(R32Q{ rotation.w, rotation.x, rotation.y, rotation.z }));
		transform->LocalScale = R32V3{ scale.x, scale.y, scale.z };
		transform->WorldSpace = 0;

		std::vector<PhysicallyBasedVertex> vertices = {};
		std::vector<U32> indices = {};

		if (Node->mNumMeshes > 0)
		{
			assert(Node->mNumMeshes == 1);

			for (U32 i = 0; i < Node->mNumMeshes; ++i)
			{
				aiMesh const* mesh = Scene->mMeshes[Node->mMeshes[i]];
				aiMaterial const* material = Scene->mMaterials[mesh->mMaterialIndex];

				assert(mesh->mNumVertices > 0);

				vertices.resize(mesh->mNumVertices);

				for (U32 j = 0; j < mesh->mNumVertices; ++j)
				{
					if (mesh->HasPositions())
					{
						vertices[j].Position = R32V3{ mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
					}

					if (mesh->HasNormals())
					{
						vertices[j].Normal = R32V3{ mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
					}

					if (mesh->HasTangentsAndBitangents())
					{
						vertices[j].Tangent = R32V3{ mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
						vertices[j].BiTangent = R32V3{ mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
					}

#pragma warning (push)
#pragma warning (disable : 26819)
					switch (mesh->GetNumColorChannels())
					{
						case 8: vertices[j].ColorChannel7 = R32V4{ mesh->mColors[7][j].r, mesh->mColors[7][j].g, mesh->mColors[7][j].b, mesh->mColors[7][j].a };
						case 7: vertices[j].ColorChannel6 = R32V4{ mesh->mColors[6][j].r, mesh->mColors[6][j].g, mesh->mColors[6][j].b, mesh->mColors[6][j].a };
						case 6: vertices[j].ColorChannel5 = R32V4{ mesh->mColors[5][j].r, mesh->mColors[5][j].g, mesh->mColors[5][j].b, mesh->mColors[5][j].a };
						case 5: vertices[j].ColorChannel4 = R32V4{ mesh->mColors[4][j].r, mesh->mColors[4][j].g, mesh->mColors[4][j].b, mesh->mColors[4][j].a };
						case 4: vertices[j].ColorChannel3 = R32V4{ mesh->mColors[3][j].r, mesh->mColors[3][j].g, mesh->mColors[3][j].b, mesh->mColors[3][j].a };
						case 3: vertices[j].ColorChannel2 = R32V4{ mesh->mColors[2][j].r, mesh->mColors[2][j].g, mesh->mColors[2][j].b, mesh->mColors[2][j].a };
						case 2: vertices[j].ColorChannel1 = R32V4{ mesh->mColors[1][j].r, mesh->mColors[1][j].g, mesh->mColors[1][j].b, mesh->mColors[1][j].a };
						case 1: vertices[j].ColorChannel0 = R32V4{ mesh->mColors[0][j].r, mesh->mColors[0][j].g, mesh->mColors[0][j].b, mesh->mColors[0][j].a };
						case 0: break;
						default: assert(0); break;
					}

					switch (mesh->GetNumUVChannels())
					{
						case 8: vertices[j].TexCoordChannel7 = R32V3{ mesh->mTextureCoords[7][j].x, mesh->mTextureCoords[7][j].y, mesh->mTextureCoords[7][j].z };
						case 7: vertices[j].TexCoordChannel6 = R32V3{ mesh->mTextureCoords[6][j].x, mesh->mTextureCoords[6][j].y, mesh->mTextureCoords[6][j].z };
						case 6: vertices[j].TexCoordChannel5 = R32V3{ mesh->mTextureCoords[5][j].x, mesh->mTextureCoords[5][j].y, mesh->mTextureCoords[5][j].z };
						case 5: vertices[j].TexCoordChannel4 = R32V3{ mesh->mTextureCoords[4][j].x, mesh->mTextureCoords[4][j].y, mesh->mTextureCoords[4][j].z };
						case 4: vertices[j].TexCoordChannel3 = R32V3{ mesh->mTextureCoords[3][j].x, mesh->mTextureCoords[3][j].y, mesh->mTextureCoords[3][j].z };
						case 3: vertices[j].TexCoordChannel2 = R32V3{ mesh->mTextureCoords[2][j].x, mesh->mTextureCoords[2][j].y, mesh->mTextureCoords[2][j].z };
						case 2: vertices[j].TexCoordChannel1 = R32V3{ mesh->mTextureCoords[1][j].x, mesh->mTextureCoords[1][j].y, mesh->mTextureCoords[1][j].z };
						case 1: vertices[j].TexCoordChannel0 = R32V3{ mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y, mesh->mTextureCoords[0][j].z };
						case 0: break;
						default: assert(0); break;
					}
#pragma warning (pop)
				}

				indices.resize(mesh->mNumFaces * 3);

				for (U32 j = 0, k = 0; j < mesh->mNumFaces; ++j, k += 3)
				{
					aiFace const face = mesh->mFaces[j];

					assert(face.mNumIndices == 3);

					indices[k + 0] = face.mIndices[0];
					indices[k + 1] = face.mIndices[1];
					indices[k + 2] = face.mIndices[2];
				}

				Material* sharedMaterial = mSharedMaterials[mesh->mMaterialIndex];

				Buffer* vertexBuffer = BufferVariance::CreateVertex(vertices.data(), sizeof(PhysicallyBasedVertex) * vertices.size());
				Buffer* indexBuffer = BufferVariance::CreateIndex(indices.data(), sizeof(U32) * indices.size());

				mSharedVertexBuffers[entity] = vertexBuffer;
				mSharedIndexBuffers[entity] = indexBuffer;

				Renderable* renderable = entity->AttachComponent<Renderable>();

				renderable->SetSharedMaterial(sharedMaterial);
				renderable->SetSharedVertexBuffer(vertexBuffer);
				renderable->SetSharedIndexBuffer(indexBuffer);
			}
		}

		for (U32 i = 0; i < Node->mNumChildren; ++i)
		{
			LoadNodesRecursive(Scene, Node->mChildren[i], entity);
		}
	}

	AsyncMaterialImageResource Scene::LoadImageOfType(aiScene const* Scene, aiMaterial const* Material, aiTextureType Type)
	{
		AsyncMaterialImageResource resource = {};

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

	Image* Scene::ExchangeResourceForImageAndRelease(AsyncMaterialImageResource const& Resource)
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

	void Scene::DestroyEntity(Entity* Entity)
	{
		// TODO
	}

	Transform* Scene::GetTransform(Entity* Entity)
	{
		return mTransformHierarchy.GetTransform(Entity->GetLevelIndex(), Entity->GetTransformIndex());
	}

	Camera* Scene::GetCamera(Entity* Entity)
	{
		if (!mCameras[Entity])
		{
			mCameras[Entity] = new Camera;
		}

		return mCameras[Entity];
	}

	void Scene::DispatchTransformHierarchy()
	{
		mTransformHierarchy.Dispatch();
	}

	void Scene::PrintHierarchy()
	{
		mTransformHierarchy.Print();
	}

	void Scene::Commit()
	{
		mEntitiesToBeRendered.clear();
		mEntitiesToBeComputed.clear();

		for (auto const& entity : mEntities)
		{
			Renderable* renderable = entity->GetComponent<Renderable>();

			if (renderable)
			{
				mEntitiesToBeRendered.emplace_back(entity);
			}
		}

		U32 entitiesToBeRenderedCount = (U32)mEntitiesToBeRendered.size();

		gRenderer->BuildPhysicallBasedDescriptorSets(entitiesToBeRenderedCount);

		for (U32 i = 0; i < entitiesToBeRenderedCount; ++i)
		{
			Entity* entity = mEntitiesToBeRendered[i];

			Renderable* renderable = entity->GetComponent<Renderable>();

			Material* sharedMaterial = renderable->GetSharedMaterial();

			if (sharedMaterial)
			{
				gRenderer->UpdatePhysicallyBasedDescriptorSets(i, sharedMaterial);
			}
		}
	}

	void Scene::Update()
	{
		mRoot->Update();
	}
}