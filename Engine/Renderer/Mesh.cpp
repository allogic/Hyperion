#include <Engine/Renderer/Mesh.h>

namespace hyperion
{
	Mesh::Mesh()
	{

	}

	Mesh::~Mesh()
	{
		delete mVertexBuffer;
		delete mIndexBuffer;
	}
}