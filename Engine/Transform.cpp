#include <Engine/Transform.h>
#include <Engine/Entity.h>
#include <Engine/Scene.h>

namespace engine
{
	void TransformUtil::EvaluateRecursive(Scene* Scene, Entity* Self)
	{
		if (Entity* parent = Self->GetParent())
		{
			EvaluateRecursive(Scene, parent);

			Transform* parentTransform = Scene->GetTransform(parent);
			Transform* selfTransform = Scene->GetTransform(Self);

			R32V3 localRotationRad = glm::radians(selfTransform->LocalEulerAngles);

			R32Q rotationX = glm::angleAxis(localRotationRad.x, parentTransform->LocalRight);
			R32Q rotationY = glm::angleAxis(localRotationRad.y, parentTransform->LocalUp);
			R32Q rotationZ = glm::angleAxis(localRotationRad.z, parentTransform->LocalFront);

			selfTransform->LocalRotation = glm::normalize(rotationY * rotationX * rotationZ);

			selfTransform->LocalRight = selfTransform->LocalRotation * parentTransform->LocalRight;
			selfTransform->LocalUp = selfTransform->LocalRotation * parentTransform->LocalUp;
			selfTransform->LocalFront = selfTransform->LocalRotation * parentTransform->LocalFront;

			selfTransform->WorldRotation = parentTransform->WorldRotation * glm::inverse(selfTransform->LocalRotation);
			selfTransform->WorldPosition = parentTransform->WorldPosition + glm::inverse(selfTransform->WorldRotation) * selfTransform->LocalPosition;
		}
		else
		{
			Transform* selfTransform = Scene->GetTransform(Self);

			R32V3 localRotationRad = glm::radians(selfTransform->LocalEulerAngles);

			R32Q rotationX = glm::angleAxis(localRotationRad.x, WorldRight);
			R32Q rotationY = glm::angleAxis(localRotationRad.y, WorldUp);
			R32Q rotationZ = glm::angleAxis(localRotationRad.z, WorldFront);

			selfTransform->LocalRotation = glm::normalize(rotationY * rotationX * rotationZ);

			selfTransform->LocalRight = selfTransform->LocalRotation * WorldRight;
			selfTransform->LocalUp = selfTransform->LocalRotation * WorldUp;
			selfTransform->LocalFront = selfTransform->LocalRotation * WorldFront;

			selfTransform->WorldRotation = glm::inverse(selfTransform->LocalRotation);
			selfTransform->WorldPosition = selfTransform->LocalRotation * selfTransform->LocalPosition;
		}
	}
}