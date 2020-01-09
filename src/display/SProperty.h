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
#include "../top_element/Scalar.h"

namespace fl {
	namespace display {

		

		class SProperty {
		private:

			class SPropertyBase {
			public:
				virtual void* get() = 0;
				virtual const void* get_const() const = 0;
				virtual ~SPropertyBase() {}
			};

			template<typename T>
			class SPropertyImpl : public SPropertyBase {
				T value;
			public:
				SPropertyImpl(const T& value) : value(value) {}
				SPropertyImpl(T&& value) : value(value) {}
				void* get() override { return &value; }
				const void* get_const() const override { return &value; }
			};

			SPropertyBase* address;

		public:
			SProperty() :address(nullptr) {}
			SProperty(SProperty&& rhs) noexcept :address(rhs.address) { rhs.address = nullptr; }
			~SProperty() { if (address) delete address; }
			ILL_UNCOPYABLE(SProperty);

			template<typename T>
			ILL_INLINE T& as() {
				return *reinterpret_cast<T*>(dynamic_cast<SPropertyImpl<T>*>(address)->get());
			}
			template<typename T>
			ILL_INLINE const T& as() const {
				return *reinterpret_cast<const T*>(dynamic_cast<SPropertyImpl<T>*>(address)->get_const());
			}
			template<typename T>
			ILL_INLINE void operator =(const T& rhs) { address = new SPropertyImpl<T>(rhs); }

			template<typename T>
			SProperty& operator =(SProperty&& rhs) noexcept { address = rhs.address; rhs.address = nullptr; return *this; }
		};

	}
}