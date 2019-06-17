/// @file SPointer.h
/// @brief 定义基类AutoPtr
/// @date 2019/3/31

#pragma once


#include "SConfig.h"


namespace fl {

	/// @brief 指针基类，所有派生类不应该以非指针的形式存在
	class AutoPtr {
	public:
		virtual ~AutoPtr() = 0;
	};
}