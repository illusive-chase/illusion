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
#include <memory>
#include <type_traits>

#define PRIVATE_ARGS_GLUE(x, y) x y

#define PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1,_2,_3,_4,_5,_6,_7,_8,_9, _10, N, ...) N
#define PRIVATE_MACRO_VAR_ARGS_IMPL(args) PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args
#define COUNT_MACRO_VAR_ARGS(...) PRIVATE_MACRO_VAR_ARGS_IMPL((__VA_ARGS__,10, 9,8,7,6,5,4,3,2,1,0))

#define PRIVATE_MACRO_CHOOSE_HELPER2(M,count)  M##count
#define PRIVATE_MACRO_CHOOSE_HELPER1(M,count) PRIVATE_MACRO_CHOOSE_HELPER2(M,count)
#define PRIVATE_MACRO_CHOOSE_HELPER(M,count)   PRIVATE_MACRO_CHOOSE_HELPER1(M,count)

#define INVOKE_VAR_MACRO(M,...) PRIVATE_ARGS_GLUE(PRIVATE_MACRO_CHOOSE_HELPER(M,COUNT_MACRO_VAR_ARGS(__VA_ARGS__)), (__VA_ARGS__))

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
	public:
		template<typename A, typename B>
		struct TypeEqual { static constexpr bool value = false; };

		template<typename B>
		struct TypeEqual<B, B> { static constexpr bool value = true; };

	private:

		template<unsigned Index, typename ...Args>
		struct TypeArrayValue;

		template<unsigned Index, typename First, typename ...Rest>
		struct TypeArrayValue<Index, First, Rest...> { using value = typename TypeArrayValue<Index - 1, Rest...>::value; };

		template<typename First, typename ...Rest>
		struct TypeArrayValue<0, First, Rest...> { using value = First; };

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

		template<typename Iterator, typename ...Args>
		struct TypeArrayAssign;

		template<typename Iterator, typename First, typename ...Rest>
		struct TypeArrayAssign<Iterator, First, Rest...> {
			static void assign(const First& para, const Rest&... paras, Iterator& it) {
				*(it++) = para;
				TypeArrayAssign<Iterator, Rest...>::assign(paras..., it);
			}
		};

		template<typename Iterator, typename Last>
		struct TypeArrayAssign<Iterator, Last> {
			static void assign(const Last& para, Iterator& it) { *it = para; }
		};

	public:

		template<typename ...Element>
		struct TypeArray {
			static constexpr int length = sizeof... (Element);
			template<typename Find> static constexpr unsigned getIndex() {
				return TypeArrayBase<Find, Element...>::getIndex();
			}
			template<unsigned Index> using type = typename TypeArrayValue<Index, Element...>::value;

			template<typename Iterator>
			static void assign(const Element&... paras, Iterator it) {
				TypeArrayAssign<Iterator, Element...>::assign(paras..., it);
			}
		};

	};

	template<int N, typename F, typename ...T>
	struct VariableParamentsIterator {};

	template<int N, typename F, typename First, typename ...T>
	struct VariableParamentsIterator<N, F, First, T...> {
		static void iter(F& func, First arg, T ...args) {
			func(arg);
			VariableParamentsIterator<N - 1, F, T...>::iter(func, args...);
		}
	};

	template<int N, typename F, typename Last>
	struct VariableParamentsIterator<N, F, Last> {
		static void iter(F& func, Last arg) {
			func(arg);
			static_assert(N == 1, "Incorrect Length.");
		}
	};

	class ToolBase {
	public:
		virtual ~ToolBase() {};
		virtual bool activate() = 0;
		virtual bool deactivate() = 0;
	};


	template<typename ...T>
	class ToolBox {
	public:
		using Array = TypeTrait::TypeArray<T...>;
		static constexpr unsigned size = Array::length;

	private:
		ToolBase* tools[size];
		unsigned active;
		template<unsigned N> struct initializer {
			using S = Array::type<N - 1>;
			static void init(ToolBase** tb) { 
				if (--tb)* tb = new S;
				initializer<N - 1>::init(tb);
			}
		};
		template<> struct initializer<1U> {
			using S = Array::type<0U>;
			static void init(ToolBase** tb) { if (--tb)* tb = new S; }
		};

	public:
		ToolBox() :active(0), tools() {}
		~ToolBox() { for (int i = 0; i < size; ++i) if (tools[i]) delete tools[i]; }
		void init() { initializer<size>::init(tools + size); tools[0]->activate(); }
		void init(T* ... args) { TypeTrait::TypeArray<T*...>::assign(args..., tools); initializer<size>::init(tools + size); tools[0]->activate(); }

		template<typename S, typename R = typename std::enable_if<bool(Array::getIndex<S>() < size), bool> ::type>
		bool switch_to() {
			if (active != Array::getIndex<S>() && tools[active]->deactivate()) {
				if (tools[Array::getIndex<S>()]->activate()) active = Array::getIndex<S>();
				else tools[active]->activate();
				return true;
			}
			return false;
		}

		template<typename S>
		bool is_active() {
			return active == Array::getIndex<S>();
		}

	};

	template<typename T, unsigned N>
	class Symbol {};

#define ILL_DECLARE_SYMBOL_IMPL_2(related_class, symbol) \
void symbol(Symbol<related_class, 0U>) {} \
using Symbol##symbol = void(*)(Symbol<related_class, 0U>);

#define ILL_DECLARE_SYMBOL_IMPL_3(related_class, symbol1, symbol2) \
void symbol1(Symbol<related_class, 0U>) {} \
using Symbol##symbol1 = void(*)(Symbol<related_class, 0U>); \
void symbol2(Symbol<related_class, 1U>) {} \
using Symbol##symbol2 = void(*)(Symbol<related_class, 1U>);

#define ILL_DECLARE_SYMBOL_IMPL_4(related_class, symbol1, symbol2, symbol3) \
void symbol1(Symbol<related_class, 0U>) {} \
using Symbol##symbol1 = void(*)(Symbol<related_class, 0U>); \
void symbol2(Symbol<related_class, 1U>) {} \
using Symbol##symbol2 = void(*)(Symbol<related_class, 1U>); \
void symbol3(Symbol<related_class, 1U>) {} \
using Symbol##symbol3 = void(*)(Symbol<related_class, 1U>);


#define ILL_DECLARE_SYMBOL(...) INVOKE_VAR_MACRO(ILL_DECLARE_SYMBOL_IMPL_, __VA_ARGS__)

}