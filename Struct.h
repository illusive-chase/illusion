#pragma once
#include <memory>

namespace fl {

	template<typename T, typename Compare = std::less<T>>
	class heap {
	public:
		heap(unsigned size = 0);
		~heap();
		void push(const T& val);
		void pop();
		const T& top() const;
		unsigned size() const;
		bool empty() const;
	private:
		T* arr;
		unsigned sz;
		unsigned capacity;
		void pushup(unsigned p);
		void pushdown(unsigned p);
		void resize(unsigned new_capacity);
	};

	template<typename T, typename Compare>
	heap<T, Compare>::heap(unsigned sz) :sz(0), capacity(0) {
		arr = (T*)malloc((sz + 1) * sizeof(T));
	}

	template<typename T, typename Compare>
	heap<T, Compare>::~heap() {
		free(arr);
	}

	template<typename T, typename Compare>
	inline void heap<T, Compare>::pushup(unsigned p) {
		for (; p >> 1; p >>= 1) {
			if (Compare()(arr[p], arr[p >> 1])) swap(arr[p >> 1], arr[p]);
			else break;
		}
	}

	template<typename T, typename Compare>
	inline void heap<T, Compare>::push(const T& val) {
		if (sz == capacity) resize(capacity << 1);
		arr[++sz] = val;
		pushup(sz);
	}

	template<typename T, typename Compare>
	inline void heap<T, Compare>::pop() {
		if (!sz) return;
		swap(arr[1], arr[sz--]);
		pushdown(1);
	}

	template<typename T, typename Compare>
	inline const T& heap<T, Compare>::top() const {
		return arr[1];
	}

	template<typename T, typename Compare>
	inline unsigned int heap<T, Compare>::size() const {
		return sz;
	}

	template<typename T, typename Compare>
	inline bool heap<T, Compare>::empty() const {
		return !sz;
	}

	template<typename T, typename Compare>
	inline void heap<T, Compare>::pushdown(unsigned p) {
		for (; p << 1 <= sz;) {
			int cp = (p << 1) + ((p << 1 < sz && Compare()(arr[(p << 1) + 1], arr[p << 1])) ? 1 : 0);
			if (Compare(arr[cp], arr[p])) swap(arr[cp], arr[p]), p = cp;
			else break;
		}
	}

	template<typename T, typename Compare>
	inline void heap<T, Compare>::resize(unsigned new_capacity) {
		capacity = new_capacity;
		T* new_arr = (T*)malloc(sizeof(T) * (new_capacity + 1));
		memcpy(new_arr, arr, (sz + 1) * sizeof(T));
		free(arr);
		arr = new_arr;
	}



	class TypeTrait {
	private:
		template<typename A, typename B>
		struct TypeEqual { static constexpr bool value = false; };

		template<typename B>
		struct TypeEqual<B, B> { static constexpr bool value = true; };

		template<unsigned Index, typename ...Args>
		struct TypeArrayValue;

		template<unsigned Index, typename First, typename ...Rest>
		struct TypeArrayValue<Index, First, Rest...> { using value = typename TypeArrayValue<Index - 1, Rest...>::value; };

		template<typename First, typename ...Rest>
		struct TypeArrayValue<0, First, Rest...> { using value = First; };

		template<typename ...Args>
		struct TypeArrayLength;

		template<typename First, typename ...Rest>
		struct TypeArrayLength<First, Rest...> { static constexpr int value = TypeArrayLength<Rest...>::value + 1; };

		template<typename Last>
		struct TypeArrayLength<Last> { static constexpr int value = 1; };

		template<typename Find, typename ...Args>
		struct TypeArrayBase;

		template<typename Find, typename First, typename ...Rest>
		struct TypeArrayBase<Find, First, Rest...> {
			static constexpr unsigned getIndex() {
				return TypeEqual<First, Find>::value ? 0 : (TypeArrayBase<Find, Rest...>::getIndex() + 1);
			}
		};

		template<typename Find, typename Last>
		struct TypeArrayBase<Find, Last> {
			static constexpr unsigned getIndex() { return 1 - TypeEqual<Last, Find>::value; }
		};

	public:

		template<typename ...Element>
		struct TypeArray {
			static constexpr int length = TypeArrayLength<Element...>::value;
			template<typename Find> static constexpr unsigned getIndex() {
				return TypeArrayBase<Find, Element...>::getIndex();
			}
			template<unsigned Index>
			using type = typename TypeArrayValue<Index, Element...>::value;
		};

	};


}