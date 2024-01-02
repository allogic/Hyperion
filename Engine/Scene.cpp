#define TINYGLTF_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <Engine/Scene.h>

#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Actors/Player.h>

#include <Engine/Ecs/Components/Camera.h>
#include <Engine/Ecs/Components/Renderable.h>

#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/BufferVariance.h>
#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/ImageVariance.h>

#define CONVERT_VERTEX_BUFFER(TYPE, BUFFER, ACCESSOR, VERTICES, MEMBER) \
{ \
	VERTICES.resize(glm::max(VERTICES.size(), ACCESSOR.count)); \
	TYPE const* data = (TYPE const*)(BUFFER.data.data() + ACCESSOR.byteOffset); \
	for (U64 i = 0; i < ACCESSOR.count; ++i) \
	{ \
		switch (ACCESSOR.type) \
		{ \
			case TINYGLTF_TYPE_VEC2: VERTICES[i].MEMBER = R32V4{ (R32)(*(data + i + 0)), (R32)(*(data + i + 1)),                   0.0F,                   0.0F }; break; \
			case TINYGLTF_TYPE_VEC3: VERTICES[i].MEMBER = R32V4{ (R32)(*(data + i + 0)), (R32)(*(data + i + 1)), (R32)(*(data + i + 2)),                   0.0F }; break; \
			case TINYGLTF_TYPE_VEC4: VERTICES[i].MEMBER = R32V4{ (R32)(*(data + i + 0)), (R32)(*(data + i + 1)), (R32)(*(data + i + 2)), (R32)(*(data + i + 3)) }; break; \
			default: assert(0); break; \
		} \
	} \
} \

#define CONVERT_INDEX_BUFFER(TYPE, BUFFER, ACCESSOR, INDICES) \
{ \
	INDICES.resize(glm::max(INDICES.size(), ACCESSOR.count)); \
	TYPE const* data = (TYPE const*)(BUFFER.data.data() + ACCESSOR.byteOffset); \
	for (U64 i = 0; i < ACCESSOR.count; ++i) \
	{ \
		switch (ACCESSOR.type) \
		{ \
			case TINYGLTF_TYPE_SCALAR: INDICES[i] = (U32)(*(data + i)); break; \
			default: assert(0); break; \
		} \
	} \
} \

#define CONVERT_VERTICES(BUFFER, ACCESSOR, VERTICES, MEMBER) \
switch (ACCESSOR.componentType) \
{ \
	case TINYGLTF_COMPONENT_TYPE_BYTE:           CONVERT_VERTEX_BUFFER( I8, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  CONVERT_VERTEX_BUFFER( U8, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_SHORT:          CONVERT_VERTEX_BUFFER(I16, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: CONVERT_VERTEX_BUFFER(U16, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_INT:            CONVERT_VERTEX_BUFFER(I32, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   CONVERT_VERTEX_BUFFER(U32, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_FLOAT:          CONVERT_VERTEX_BUFFER(R32, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	case TINYGLTF_COMPONENT_TYPE_DOUBLE:         CONVERT_VERTEX_BUFFER(R64, BUFFER, ACCESSOR, VERTICES, MEMBER); break; \
	default: assert(0); break; \
} \

#define CONVERT_INDICES(BUFFER, ACCESSOR, INDICES) \
switch (ACCESSOR.componentType) \
{ \
	case TINYGLTF_COMPONENT_TYPE_BYTE:           CONVERT_INDEX_BUFFER( I8, BUFFER, ACCESSOR, INDICES); break; \
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  CONVERT_INDEX_BUFFER( U8, BUFFER, ACCESSOR, INDICES); break; \
	case TINYGLTF_COMPONENT_TYPE_SHORT:          CONVERT_INDEX_BUFFER(I16, BUFFER, ACCESSOR, INDICES); break; \
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: CONVERT_INDEX_BUFFER(U16, BUFFER, ACCESSOR, INDICES); break; \
	case TINYGLTF_COMPONENT_TYPE_INT:            CONVERT_INDEX_BUFFER(I32, BUFFER, ACCESSOR, INDICES); break; \
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   CONVERT_INDEX_BUFFER(U32, BUFFER, ACCESSOR, INDICES); break; \
	default: assert(0); break; \
} \

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
		bool result = false;

		tinygltf::TinyGLTF loader = {};

		std::string errorMsg = "";
		std::string warningMsg = "";

		result = loader.LoadASCIIFromFile(&mModel, &errorMsg, &warningMsg, File.string());

		if (!errorMsg.empty())
		{
			LOG(errorMsg.data());
		}

		if (!warningMsg.empty())
		{
			LOG(warningMsg.data());
		}

		if (result)
		{
			tinygltf::Scene const& scene = mModel.scenes[mModel.defaultScene];

			for (auto const& node : scene.nodes)
			{
				LoadNodesRecursive(mModel.nodes[node], 0);
			}
		}

		return result;
	}

	bool Scene::Safe(std::filesystem::path const& File)
	{
		return false;
	}

	/*
	void Scene::LoadBuffer()
	{
		U32 bufferCount = (U32)mModel.bufferViews.size();

		mBuffers.resize(bufferCount);

		for (U32 i = 0; i < bufferCount; ++i)
		{
			tinygltf::BufferView const& bufferView = mModel.bufferViews[i];

			if (bufferView.buffer >= 0)
			{
				tinygltf::Buffer const& buffer = mModel.buffers[bufferView.buffer];

				void* data = (void*)(buffer.data.data() + bufferView.byteOffset);
				U64 size = bufferView.byteLength;

				if (bufferView.target > 0)
				{
					switch (bufferView.target)
					{
						case TINYGLTF_TARGET_ARRAY_BUFFER: mBuffers[i] = BufferVariance::CreateVertex(data, size); break;
						case TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER: mBuffers[i] = BufferVariance::CreateIndex(data, size); break;
						default: assert(0); break;
					}
				}
			}
		}
	}

	void Scene::LoadImages()
	{
		U32 imageCount = (U32)mModel.images.size();

		mImages.resize(imageCount);

		for (U32 i = 0; i < imageCount; ++i)
		{
			tinygltf::Texture const& texture = mModel.textures[i];

			if (texture.source >= 0)
			{
				tinygltf::Image const& image = mModel.images[texture.source];

				void* data = (void*)image.image.data();
				U32 width = image.width;
				U32 height = image.height;

				assert(image.bits == 8); // TODO

				switch (image.component)
				{
					case 1: mImages[i] = ImageVariance::CreateRImage2D(data, width, height); break;
					case 2: mImages[i] = ImageVariance::CreateRgImage2D(data, width, height); break;
					case 3: mImages[i] = ImageVariance::CreateRgbImage2D(data, width, height); break;
					case 4: mImages[i] = ImageVariance::CreateRgbaImage2D(data, width, height); break;
					default: assert(0); break;
				}
			}
		}
	}
	*/

	void Scene::LoadNodesRecursive(tinygltf::Node const& Node, Entity* Parent)
	{
		Entity* entity = CreateEntity<Entity>(Node.name, Parent);

		LOG("Node:%s\n", Node.name.data());

		Transform* transform = GetTransform(entity);

		if (Node.translation.size() >= 3)
		{
			transform->LocalPosition = R32V3{ Node.translation[0], Node.translation[1], Node.translation[2] };
		}

		if (Node.rotation.size() >= 3)
		{
			transform->LocalEulerAngles = R32V3{ Node.rotation[0], Node.rotation[1], Node.rotation[2] };
		}

		if (Node.scale.size() >= 3)
		{
			transform->LocalScale = R32V3{ Node.scale[0], Node.scale[1], Node.scale[2] };
		}

		if (Node.mesh >= 0)
		{
			tinygltf::Mesh const& mesh = mModel.meshes[Node.mesh];

			LOG("Mesh:%s\n", mesh.name.data());

			std::vector<PbrVertex> vertices = {};
			std::vector<U32> indices = {};

			assert(mesh.primitives.size() == 1);

			for (auto const& primitive : mesh.primitives)
			{
				// primitive.mode // TODO
				// primitive.material // TODO

				for (auto const& [attributeName, attribIndex] : primitive.attributes)
				{
					tinygltf::Accessor const& attributeAccessor = mModel.accessors[attribIndex];
					tinygltf::BufferView const& bufferView = mModel.bufferViews[attributeAccessor.bufferView];
					tinygltf::Buffer const& buffer = mModel.buffers[bufferView.buffer];

					assert(bufferView.target == TINYGLTF_TARGET_ARRAY_BUFFER);

					LOG("  Attribute:%s BufferView:%d Count:%llu ByteOffset:%llu ComponentType:%d Type:%d\n", attributeName.data(), attributeAccessor.bufferView, attributeAccessor.count, attributeAccessor.byteOffset, attributeAccessor.componentType, attributeAccessor.type);

					if (attributeName == "POSITION")
					{
						CONVERT_VERTICES(buffer, attributeAccessor, vertices, Position);
					}
					else if (attributeName == "NORMAL")
					{
						CONVERT_VERTICES(buffer, attributeAccessor, vertices, Normal);
					}
					else if (attributeName == "TEXCOORD_0")
					{
						CONVERT_VERTICES(buffer, attributeAccessor, vertices, TexCoord0);
					}
					else if (attributeName == "WEIGHTS_0")
					{
						CONVERT_VERTICES(buffer, attributeAccessor, vertices, Weight0);
					}
					else if (attributeName == "JOINTS_0")
					{
						CONVERT_VERTICES(buffer, attributeAccessor, vertices, Joint0);
					}
					else
					{
						LOG("Missing attribute %s\n", attributeName.data());

						assert(0);
					}
				}

				tinygltf::Accessor const& indexAccessor = mModel.accessors[primitive.indices];
				tinygltf::BufferView const& bufferView = mModel.bufferViews[indexAccessor.bufferView];
				tinygltf::Buffer const& buffer = mModel.buffers[bufferView.buffer];

				assert(bufferView.target == TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);

				LOG("  Indices BufferView:%d Count:%llu ByteOffset:%llu ComponentType:%d Type:%d\n", indexAccessor.bufferView, indexAccessor.count, indexAccessor.byteOffset, indexAccessor.componentType, indexAccessor.type);

				CONVERT_INDICES(buffer, indexAccessor, indices);
			}

			LOG("\n");

			Buffer* vertexBuffer = BufferVariance::CreateVertex(vertices.data(), vertices.size());
			Buffer* indexBuffer = BufferVariance::CreateIndex(indices.data(), indices.size());

			mSharedVertexBuffers[entity] = vertexBuffer;
			mSharedIndexBuffers[entity] = indexBuffer;

			Renderable* renderable = entity->AttachComponent<Renderable>();

			renderable->SetSharedVertexBuffer(vertexBuffer);
			renderable->SetSharedIndexBuffer(indexBuffer);
		}

		for (auto const& childNode : Node.children)
		{
			LoadNodesRecursive(mModel.nodes[childNode], entity);
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

	void Scene::Update()
	{
		mRoot->Update();
	}
}