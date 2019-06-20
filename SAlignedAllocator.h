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

#pragma once
#include "Scalar.h"

namespace fl {
	void* AlignedAlloc(size_t size, int alignment);
	void AlignedFree(void* ptr);

	template<typename T,unsigned Alignment>
	class AlignedAllocator {

		using self_type = AlignedAllocator<T, Alignment>;

	public:

		AlignedAllocator() {}
		template<typename Other> AlignedAllocator(const AlignedAllocator<Other, Alignment>&) {}

		T* address(T& ref) const { return &ref; }
		const T* address(const T& ref) { return &ref; }
		T* allocate(int n, const T* hint = 0) {
			void(hint);
			return reinterpret_cast<T*>(AlignedAlloc(sizeof(T)*n, Alignment));
		}
		void construct(T* ptr, const T& value) { new (ptr) T(value); }
		void deallocate(T* ptr) { AlignedFree(reinterpret_cast<void*>(ptr)); }
		void destroy(T* ptr) { ptr->~T(); }

		template <typename O>
		struct rebind { using other = AlignedAllocator<O, Alignment>; };

		template <typename O>
		self_type& operator=(const AlignedAllocator<O, Alignment>&) { return *this; }

		friend bool operator==(const self_type&, const self_type&) { return true; }

	};


#ifdef ILL_SSE

#ifdef _MSC_VER
#pragma warning(disable : 4556)  // value of intrinsic immediate argument '4294967239' is out of range '0 - 255'
#endif

#define ILL_SHUFFLE(x, y, z, w) (((w) << 6 | (z) << 4 | (y) << 2 | (x)) & 0xff)
#define ill_pshufd_ps(_a, _mask) _mm_shuffle_ps((_a), (_a), (_mask))
#define ill_splat3_ps(_a, _i) ill_pshufd_ps((_a), ILL_SHUFFLE(_i, _i, _i, 3))
#define ill_splat_ps(_a, _i) ill_pshufd_ps((_a), ILL_SHUFFLE(_i, _i, _i, _i))

#define illv3AbsiMask (_mm_set_epi32(0x00000000, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF))
#define illvAbsMask (_mm_set_epi32(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF))
#define illvFFF0Mask (_mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))
#define illv3AbsfMask illCastiTo128f(illv3AbsiMask)
#define illvFFF0fMask illCastiTo128f(illvFFF0Mask)
#define illvxyzMaskf illvFFF0fMask
#define illvAbsfMask illCastiTo128f(illvAbsMask)

	const __m128 ILL_ATTRIBUTE_ALIGNED16(illvMzeroMask) = { -0.0f, -0.0f, -0.0f, -0.0f };
	const __m128 ILL_ATTRIBUTE_ALIGNED16(illv1110) = { 1.0f, 1.0f, 1.0f, 0.0f };
	const __m128 ILL_ATTRIBUTE_ALIGNED16(illvHalf) = { 0.5f, 0.5f, 0.5f, 0.5f };
	const __m128 ILL_ATTRIBUTE_ALIGNED16(illv1_5) = { 1.5f, 1.5f, 1.5f, 1.5f };

#endif



	template <typename T>
	class AlignedArray {

	private:

		AlignedAllocator<T, 16> m_allocator;

		int m_size;
		int m_capacity;
		T* m_data;
		bool m_ownsMemory;

	public:
		ILL_INLINE AlignedArray<T>& operator=(const AlignedArray<T>& other) {
			copyFromArray(other);
			return *this;
		}

	protected:
		ILL_INLINE int allocSize(int size) { return (size ? size * 2 : 1); }

		ILL_INLINE void copy(int start, int end, T* dest) const {
			for (int i = start; i < end; ++i) new (&dest[i]) T(m_data[i]);
		}

		ILL_INLINE void init() {
			m_ownsMemory = true;
			m_data = 0;
			m_size = 0;
			m_capacity = 0;
		}
		ILL_INLINE void destroy(int first, int last) {
			for (int i = first; i < last; i++) m_data[i].~T();
		}

		ILL_INLINE void* allocate(int size) {
			if (size) return m_allocator.allocate(size);
			return 0;
		}

		ILL_INLINE void deallocate() {
			if (m_data) {
				if (m_ownsMemory) m_allocator.deallocate(m_data);
				m_data = 0;
			}
		}

	public:
		AlignedArray() { init(); }

		~AlignedArray() { clear(); }

		///Generally it is best to avoid using the copy constructor of an AlignedArray, and use a (const) reference to the array instead.
		AlignedArray(const AlignedArray& otherArray) {
			init();
			int otherSize = otherArray.size();
			resize(otherSize);
			otherArray.copy(0, otherSize, m_data);
		}

		/// return the number of elements in the array
		ILL_INLINE int size() const { return m_size; }

		ILL_INLINE const T& at(int n) const {
			ILLAssert(n >= 0);
			ILLAssert(n < size());
			return m_data[n];
		}

		ILL_INLINE T& at(int n) {
			ILLAssert(n >= 0);
			ILLAssert(n < size());
			return m_data[n];
		}

		ILL_INLINE const T& operator[](int n) const {
			ILLAssert(n >= 0);
			ILLAssert(n < size());
			return m_data[n];
		}

		ILL_INLINE T& operator[](int n) {
			ILLAssert(n >= 0);
			ILLAssert(n < size());
			return m_data[n];
		}

		///clear the array, deallocated memory. Generally it is better to use array.resize(0), to reduce performance overhead of run-time memory (de)allocations.
		ILL_INLINE void clear() {
			destroy(0, size());
			deallocate();
			init();
		}

		ILL_INLINE void pop_back() {
			ILLAssert(m_size > 0);
			m_size--;
			m_data[m_size].~T();
		}

		///resize changes the number of elements in the array. If the new size is larger, the new elements will be constructed using the optional second argument.
		///when the new number of elements is smaller, the destructor will be called, but memory will not be freed, to reduce performance overhead of run-time memory (de)allocations.
		ILL_INLINE void resizeNoInitialize(int newsize) {
			int curSize = size();
			if (newsize < curSize);
			else {
				if (newsize > size()) reserve(newsize);
				//leave this uninitialized
			}
			m_size = newsize;
		}

		ILL_INLINE void resize(int newsize, const T& fillData = T()) {
			int curSize = size();
			if (newsize < curSize) {
				for (int i = newsize; i < curSize; i++) m_data[i].~T();
			} else {
				if (newsize > size()) reserve(newsize);
				for (int i = curSize; i < newsize; i++) new (&m_data[i]) T(fillData);
			}
			m_size = newsize;
		}

		ILL_INLINE T& expandNonInitializing() {
			int sz = size();
			if (sz == capacity()) {
				reserve(allocSize(size()));
			}
			m_size++;
			return m_data[sz];
		}

		ILL_INLINE T& expand(const T& fillValue = T()) {
			int sz = size();
			if (sz == capacity()) {
				reserve(allocSize(size()));
			}
			m_size++;
			new (&m_data[sz]) T(fillValue);  //use the in-place new (not really allocating heap memory)
			return m_data[sz];
		}

		ILL_INLINE void push_back(const T& _Val) {
			int sz = size();
			if (sz == capacity()) reserve(allocSize(size()));
			new (&m_data[m_size]) T(_Val);
			m_size++;
		}

		/// return the pre-allocated (reserved) elements, this is at least as large as the total number of elements,see size() and reserve()
		ILL_INLINE int capacity() const { return m_capacity; }

		ILL_INLINE void reserve(int _Count) {  // determine new minimum length of allocated storage
			if (capacity() < _Count) {  // not enough room, reallocate
				T* s = (T*)allocate(_Count);
				ILLAssert(s);
				copy(0, size(), s);
				destroy(0, size());
				deallocate();
				m_ownsMemory = true;
				m_data = s;
				m_capacity = _Count;
			}
		}

		class less { public: bool operator()(const T& a, const T& b) { return (a < b); } };

		template <typename L>
		void quickSortInternal(const L& CompareFunc, int lo, int hi) {
			//  lo is the lower index, hi is the upper index
			//  of the region of array a that is to be sorted
			int i = lo, j = hi;
			T x = m_data[(lo + hi) / 2];

			//  partition
			do {
				while (CompareFunc(m_data[i], x)) i++;
				while (CompareFunc(x, m_data[j])) j--;
				if (i <= j) {
					swap(i, j);
					i++;
					j--;
				}
			} while (i <= j);

			//  recursion
			if (lo < j) quickSortInternal(CompareFunc, lo, j);
			if (i < hi) quickSortInternal(CompareFunc, i, hi);
		}

		template <typename L>
		void quickSort(const L& CompareFunc) {
			//don't sort 0 or 1 elements
			if (size() > 1) quickSortInternal(CompareFunc, 0, size() - 1);
		}

		template <typename L>
		void downHeap(T* pArr, int k, int n, const L& CompareFunc) {
			/*  PRE: a[k+1..N] is a heap */
			/* POST:  a[k..N]  is a heap */

			T temp = pArr[k - 1];
			/* k has child(s) */
			while (k <= n / 2) {
				int child = 2 * k;

				if ((child < n) && CompareFunc(pArr[child - 1], pArr[child])) child++;

				/* pick larger child */
				if (CompareFunc(temp, pArr[child - 1])) {
					/* move child up */
					pArr[k - 1] = pArr[child - 1];
					k = child;
				} else break;
			}
			pArr[k - 1] = temp;
		} /*downHeap*/

		void swap(int index0, int index1) {
			char temp[sizeof(T)];
			memcpy(temp, &m_data[index0], sizeof(T));
			memcpy(&m_data[index0], &m_data[index1], sizeof(T));
			memcpy(&m_data[index1], temp, sizeof(T));
		}

		template <typename L>
		void heapSort(const L& CompareFunc) {
			/* sort a[0..N-1],  N.B. 0 to N-1 */
			int n = m_size;
			for (int k = n / 2; k > 0; k--) downHeap(m_data, k, n, CompareFunc);

			/* a[1..N] is now a heap */
			while (n >= 1) {
				swap(0, n - 1); /* largest of a[0..n-1] */
				n = n - 1;
				/* restore a[1..i-1] heap */
				downHeap(m_data, 1, n, CompareFunc);
			}
		}

		///non-recursive binary search, assumes sorted array
		int findBinarySearch(const T& key) const {
			int first = 0;
			int last = size() - 1;

			//assume sorted array
			while (first <= last) {
				int mid = (first + last) / 2;  // compute mid point.
				if (key > m_data[mid])
					first = mid + 1;  // repeat search in top half.
				else if (key < m_data[mid])
					last = mid - 1;  // repeat search in bottom half.
				else
					return mid;  // found it. return position /////
			}
			return size();  // failed to find key
		}

		int findLinearSearch(const T& key) const {
			int index = size();
			int i;

			for (i = 0; i < size(); i++) {
				if (m_data[i] == key) {
					index = i;
					break;
				}
			}
			return index;
		}

		int findLinearSearch2(const T& key) const {
			int index = -1;
			int i;

			for (i = 0; i < size(); i++) {
				if (m_data[i] == key) {
					index = i;
					break;
				}
			}
			return index;
		}

		void remove(const T& key) {
			int findIndex = findLinearSearch(key);
			if (findIndex < size()) {
				swap(findIndex, size() - 1);
				pop_back();
			}
		}

		//PCK: whole function
		void initializeFromBuffer(void* buffer, int size, int capacity) {
			clear();
			m_ownsMemory = false;
			m_data = (T*)buffer;
			m_size = size;
			m_capacity = capacity;
		}

		void copyFromArray(const AlignedArray& otherArray) {
			int otherSize = otherArray.size();
			resize(otherSize);
			otherArray.copy(0, otherSize, m_data);
		}

		void removeAtIndex(int index) {
			if (index < size()) {
				swap(index, size() - 1);
				pop_back();
			}
		}
	};
}