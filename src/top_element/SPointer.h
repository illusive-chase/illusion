/*
MIT License

Copyright (c) 2019 illusive-chase

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/
#pragma once


#include "Scalar.h"


namespace fl {

	// Class AutoPtr is a base class.
	// All derived classes must be allocated on the heap. (I tend to omit 'the instance of')
	// They should be deallocated either in destructor of container(this means you don't need to release them manually) or by you.
	// Actually, I really know that I should use smart pointer instead.
	// I plan to refactor it in the near future.
	class AutoPtr {
	public:
		virtual ~AutoPtr() = 0;
	};
}