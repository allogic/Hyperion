#define TINYGLTF_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <Engine/Scene.h>

#include <Engine/Assimp/Importer.hpp>
#include <Engine/Assimp/Exporter.hpp>
#include <Engine/Assimp/scene.h>
#include <Engine/Assimp/postprocess.h>

#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Actors/Player.h>

#include <Engine/Ecs/Components/Camera.h>
#include <Engine/Ecs/Components/Renderable.h>

#include <Engine/Renderer/Renderer.h>
#include <Engine/Renderer/Vertex.h>

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
	}

	bool Scene::Load(std::filesystem::path const& File)
	{
		Assimp::Importer importer = {};

		U32 importFlags = aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

		aiScene const* scene = importer.ReadFile(File.string(), importFlags);

		if (scene && (scene->mFlags & ~(AI_SCENE_FLAGS_INCOMPLETE)) && scene->mRootNode)
		{
			LoadNodesRecursive(scene, scene->mRootNode);

			return true;
		}

		return false;
	}

	bool Scene::Safe(std::filesystem::path const& File)
	{
		return false;
	}

	void Scene::LoadNodesRecursive(aiScene const* Scene, aiNode const* Node, Entity* Parent)
	{
		Entity* entity = CreateEntity<Entity>(Node->mName.C_Str(), Parent);

		LOG("Node:%s\n", Node->mName.C_Str());

		Transform* transform = GetTransform(entity);

		aiVector3D position = {};
		aiQuaternion rotation = {};
		aiVector3D scale = {};

		Node->mTransformation.Decompose(scale, rotation, position);

		transform->LocalPosition = R32V3{ position.x, position.y, position.z };
		transform->LocalEulerAngles = glm::degrees(glm::eulerAngles(R32Q{ rotation.w, rotation.x, rotation.y, rotation.z }));
		transform->LocalScale = R32V3{ scale.x, scale.y, scale.z };

		std::vector<PhysicallyBasedVertex> vertices = {};
		std::vector<U32> indices = {};

		if (Node->mNumMeshes > 0)
		{
			assert(Node->mNumMeshes == 1);

			for (U32 i = 0; i < Node->mNumMeshes; ++i)
			{
				aiMesh const* mesh = Scene->mMeshes[Node->mMeshes[i]];

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

				Buffer* vertexBuffer = BufferVariance::CreateVertex(vertices.data(), sizeof(PhysicallyBasedVertex) * vertices.size());
				Buffer* indexBuffer = BufferVariance::CreateIndex(indices.data(), sizeof(U32) * indices.size());

				mSharedVertexBuffers[entity] = vertexBuffer;
				mSharedIndexBuffers[entity] = indexBuffer;

				Renderable* renderable = entity->AttachComponent<Renderable>();

				renderable->SetSharedVertexBuffer(vertexBuffer);
				renderable->SetSharedIndexBuffer(indexBuffer);
			}
		}

		for (U32 i = 0; i < Node->mNumChildren; ++i)
		{
			LoadNodesRecursive(Scene, Node->mChildren[i], entity);
		}
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
			gRenderer->UpdatePhysicallyBasedDescriptorSets(i);
		}
	}

	void Scene::Update()
	{
		mRoot->Update();
	}
}