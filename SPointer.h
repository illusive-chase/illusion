/// @file SPointer.h
/// @brief �������AutoPtr
/// @date 2019/3/31

#pragma once


#include "Scalar.h"


namespace fl {

	/// @brief ָ����࣬���������಻Ӧ���Է�ָ�����ʽ����
	class AutoPtr {
	public:
		virtual ~AutoPtr() = 0;
	};
}