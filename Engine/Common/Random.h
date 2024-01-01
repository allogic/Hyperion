#pragma once

#include <random>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class Random
	{
	public:

		template<typename T>
		static T NextIntBetween(T Min, T Max);

		template<typename T>
		static T NextRealBetween(T Min, T Max);
	};
}

namespace hyperion
{
	template<typename T>
	T Random::NextIntBetween(T Min, T Max)
	{
		static auto randomDevice = std::random_device{};
		static auto generator = std::mt19937{ randomDevice() };
		static auto distribution = std::uniform_int_distribution<T>{ Min, Max };

		return distribution(generator);
	}

	template<typename T>
	T Random::NextRealBetween(T Min, T Max)
	{
		static auto randomDevice = std::random_device{};
		static auto generator = std::mt19937{ randomDevice() };
		static auto distribution = std::uniform_real_distribution<T>{ Min, Max };

		return distribution(generator);
	}
}