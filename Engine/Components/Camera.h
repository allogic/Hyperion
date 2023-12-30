#pragma once

#include <Engine/Component.h>

namespace engine
{
	namespace CameraProjectionMode
	{
		enum Type
		{
			Perspective,
			Orthographic,
		};
	}

	class Camera : public Component
	{
	public:

		inline auto GetCameraMode() const { return mCameraMode; }
		inline auto GetFov() const { return mFov; }
		inline auto GetNear() const { return mNear; }
		inline auto GetFar() const { return mFar; }
		inline auto GetLeft() const { return mLeft; }
		inline auto GetRight() const { return mRight; }
		inline auto GetTop() const { return mTop; }
		inline auto GetBottom() const { return mBottom; }

	public:

		inline void SetCameraMode(CameraProjectionMode::Type Value) { mCameraMode = Value; }
		inline void SetFov(R32 Value) { mFov = Value; }
		inline void SetNear(R32 Value) { mNear = Value; }
		inline void SetFar(R32 Value) { mFar = Value; }
		inline void SetLeft(R32 Value) { mLeft = Value; }
		inline void SetRight(R32 Value) { mRight = Value; }
		inline void SetTop(R32 Value) { mTop = Value; }
		inline void SetBottom(R32 Value) { mBottom = Value; }

	public:

		Camera();
		virtual ~Camera();

	private:

		CameraProjectionMode::Type mCameraMode = CameraProjectionMode::Perspective;

	private:

		R32 mFov = glm::radians(45.0F);
		R32 mNear = 0.001F;
		R32 mFar = 1000.0F;

	private:

		R32 mLeft = -1.0F;
		R32 mRight = 1.0F;
		R32 mTop = 1.0F;
		R32 mBottom = -1.0F;
	};
}