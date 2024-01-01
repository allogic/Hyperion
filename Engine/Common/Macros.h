#pragma once

#include <cstdio>

#ifndef NDEBUG
	#define LOG(FMT, ...) std::printf(FMT, __VA_ARGS__);
#else
	#define LOG(FMT, ...)
#endif

#ifndef NDEBUG
	#define VK_CHECK(EXPR) { \
		VkResult result = (EXPR); \
		if (result != VK_SUCCESS) { \
			std::printf("At %s function %s failed with %d\n", __FILE__, #EXPR, result); \
		} \
	}
#else
	#define VK_CHECK(EXPR) (EXPR)
#endif