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
//#define ILL_NOISY

#include "Scalar.h"
#ifdef ILL_NOISY
#include <type_traits>
#endif


namespace fl {

	template<typename T>
	class sptr {
	protected:
		unsigned *cnt;
		T* ptr;
	public:

		class wptr {
		private:
			const T* ptr;
			const unsigned* cnt;
		public:
			template<typename U>
			wptr(const sptr<U>& uptr) :ptr(static_cast<T*>(uptr.ptr)), cnt(uptr.cnt) {}
			wptr() :ptr(nullptr), cnt(nullptr) {}
			wptr(const sptr<T>& ptr) :ptr(ptr.ptr), cnt(ptr.cnt) {}

			ILL_INLINE const T& operator*() const { return *ptr; }
			ILL_INLINE unsigned count() const { return *cnt; }
			ILL_INLINE const T* operator->() const { return ptr; }
			ILL_INLINE operator bool() const { return ptr; }

		};


		template<typename U>
		friend class sptr;

		friend class wptr;

		template<typename U>
		sptr(const sptr<U>& uptr) :ptr(static_cast<T*>(uptr.ptr)), cnt(uptr.cnt) { 
			if (ptr) {
#ifdef ILL_NOISY
				printf("constructor: %s\n", typeid(T).name());
#endif
				(*cnt)++;
			}
		}

		sptr(const sptr<T>& rhs) : ptr(rhs.ptr), cnt(rhs.cnt) {
			if (ptr) {
#ifdef ILL_NOISY
				printf("constructor: %s\n", typeid(T).name());
#endif
				(*cnt)++;
			}
		}

		template<typename U>
		sptr(sptr<U>&& uptr) noexcept :ptr(static_cast<T*>(uptr.ptr)), cnt(uptr.cnt) {
			uptr.ptr = nullptr, uptr.cnt = nullptr;
		}

		sptr(sptr<T>&& rhs) noexcept :ptr(rhs.ptr), cnt(rhs.cnt) { 
			rhs.ptr = nullptr, rhs.cnt = nullptr; 
		}

		sptr() :ptr(nullptr), cnt(nullptr) {}
		explicit sptr(T* tptr) :ptr(tptr), cnt(tptr ? (new unsigned(1)) : nullptr) {
#ifdef ILL_NOISY
			if (ptr) printf("constructor: %s\n", typeid(T).name());
#endif
		}
		sptr(nullptr_t p) :ptr(nullptr), cnt(nullptr) {}
		~sptr() { 
#ifdef ILL_NOISY
			if (ptr) printf("destructor: %s\n", typeid(T).name());
#endif
			if (ptr && (--(*cnt) == 0)) { delete cnt; delete ptr; }
		}
		
		ILL_INLINE sptr<T>& operator=(const sptr<T>& rhs) { 
			if (ptr && (--(*cnt) == 0)) {
				delete cnt;
				delete ptr;
#ifdef ILL_NOISY
				printf("destructor: %s\n", typeid(T).name());
#endif
			}
			ptr = rhs.ptr;
			cnt = rhs.cnt;
			if (ptr) {
				(*cnt)++;
#ifdef ILL_NOISY
				printf("constructor: %s\n", typeid(T).name());
#endif
			}
			return *this;
		}
		ILL_INLINE sptr<T>& operator=(nullptr_t) {
			if (ptr && (--(*cnt) == 0)) { 
				delete cnt; 
				delete ptr; 
#ifdef ILL_NOISY
				printf("destructor: %s\n", typeid(T).name());
#endif
			}
			ptr = nullptr;
			cnt = nullptr;
			return *this;
		}
		ILL_INLINE sptr<T>& operator=(sptr<T>&& rhs) noexcept { 
			ptr = rhs.ptr, cnt = rhs.cnt, rhs.ptr = nullptr, rhs.cnt = nullptr; 
			return *this;
		}
		ILL_INLINE bool operator==(const sptr<T>& rhs) const { return ptr == rhs.ptr; }
		ILL_INLINE bool operator!=(const sptr<T>& rhs) const { return ptr != rhs.ptr; }
		ILL_INLINE T& operator*() { return *ptr; }
		ILL_INLINE unsigned count() const { return *cnt; }
		ILL_INLINE T* operator->() { return ptr; }
		ILL_INLINE operator bool() const { return ptr; }
		ILL_INLINE T* raw() { return ptr; }
		void operator delete(void* p) = delete;
		void operator delete[](void* p) = delete;
	};
}