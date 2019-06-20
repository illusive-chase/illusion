/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2013 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#include "SAlignedAllocator.h"

void * fl::AlignedAlloc(size_t size, int alignment) {
#if defined(ILL_HAS_ALIGNED_ALLOCATOR)
	return _aligned_malloc(size, (size_t)alignment);
#else
	void *ret;
	char *real;
	real = (char *)AlignedAlloc(size + sizeof(void *) + (alignment - 1));
	if (real) {
		ret = AlignPointer(real + sizeof(void *), alignment);
		*((void **)(ret)-1) = (void *)(real);
	} else {
		ret = (void *)(real);
	}
	return (ret);
#endif
}

void fl::AlignedFree(void * ptr) {
#if defined(B3_HAS_ALIGNED_ALLOCATOR)
	if (ptr) _aligned_free(ptr);
#else
	void *real;
	if (ptr) {
		real = *((void **)(ptr)-1);
		AlignedFree(real);
	}
#endif
}