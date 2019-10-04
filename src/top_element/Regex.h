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
#include "System.h"
#include <stack>
#include <queue>

namespace fl {

	template<typename> class Regex;

	template<typename C> Regex<C> operator+(Regex<C>&& a, Regex<C>&& b);
	template<typename C> Regex<C> operator|(Regex<C>&& a, Regex<C>&& b);
	template<typename C> Regex<C> operator*(Regex<C>&& a);
	template<typename C> Regex<C> operator+(Regex<C>&& a);
	template<typename C> Regex<C> operator!(Regex<C>&& a);


	template<typename CharType = char>
	class Regex {
		using ct = CharType;
		ct* stack;
		unsigned cap;
		unsigned len;

	public:
		friend Regex<ct> operator+<>(Regex<ct>&& a, Regex<ct>&& b);
		friend Regex<ct> operator|<>(Regex<ct>&& a, Regex<ct>&& b);
		friend Regex<ct> operator*<>(Regex<ct>&& a);
		friend Regex<ct> operator+<>(Regex<ct>&& a);
		friend Regex<ct> operator!<>(Regex<ct>&& a);

		class ReserveCharSet {
		public:
			enum value {
				ANY = -2,
				SOME = -3,
				BIN = -4,
				NONE = -5,
				HYPEN = -6,
				BINARY_BEGIN = -7,
				CAT = -8,
				AND = -9,
				LB = -10,
				BINARY_END = -11,
				RB = -12
			};
		};

		using Iterator = const ct*;
		Iterator begin() const { return stack; }
		Iterator end() const { return stack + len; }


		Regex(ct c) :stack(new ct[1]), cap(1U), len(1U) { *stack = c; }
		Regex(ct from, ct to) :stack(new ct[int(ILL_ASSERT(to - from + 1 > 0)), (to - from) * 2 + 1]), cap((to - from) * 2 + 1), len(to - from + 1) {
			for (unsigned i = 0; i < len; ++i) stack[i] = from + i;
			for (unsigned i = len; i < cap; ++i) stack[i] = ct(ReserveCharSet::AND);
			len = cap;
		}
		Regex(const ct*) :stack(nullptr),cap(strlen(ct)),len(cap){
			stack = new ct[cap];
			for (unsigned i = 0; i < len; ++i) stack[i] = ct[i];
		}
		Regex() :stack(nullptr), cap(0U), len(0U) {}
		Regex(const Regex<ct>& reg) :cap(reg.cap), len(reg.len) {
			if (cap) stack = new ct[cap], memcpy(stack, reg.stack, sizeof(ct) * len);
			else stack = nullptr;
		}

		Regex(Regex<ct>&& reg) noexcept :stack(reg.stack), cap(reg.cap), len(reg.len) {
			reg.stack = nullptr;
			reg.len = reg.cap = 0U;
		}

		Regex& operator=(const Regex<ct>& reg) {
			if (stack) delete[] stack;
			cap = reg.cap;
			len = reg.len;
			if (cap) stack = new ct[cap], memcpy(stack, reg.stack, sizeof(ct) * len);
			else stack = nullptr;
			return *this;
		}

		Regex& operator=(Regex<ct>&& reg) noexcept {
			if (stack) delete[] stack;
			cap = reg.cap;
			len = reg.len;
			stack = reg.stack;
			reg.stack = nullptr;
			reg.len = reg.cap = 0U;
			return *this;
		}

		~Regex() { if (stack) delete[] stack; }

		static Regex<ct> MakeRegex(const ct* str) {
			Regex<ct> ret;
			std::queue<ct> que;
			std::stack<typename ReserveCharSet::value> stk;
			typename ReserveCharSet::value op = ReserveCharSet::BINARY_BEGIN;
			for (; *str;) {
				typename ReserveCharSet::value pre_op = op;
				switch (*str) 
				{
				case '+': op = ReserveCharSet::SOME; break;
				case '*': op = ReserveCharSet::ANY; break;
				case '?': op = ReserveCharSet::BIN; break;
				case '|': op = ReserveCharSet::AND; break;
				case '-': op = ReserveCharSet::HYPEN; break;
				case '[': op = ReserveCharSet::LB; break;
				case ']': op = ReserveCharSet::RB; break;
				default: op = ReserveCharSet::NONE; break;
				}

				if ((op == ReserveCharSet::NONE || op == ReserveCharSet::LB) 
					&& (pre_op > ReserveCharSet::BINARY_BEGIN || pre_op < ReserveCharSet::BINARY_END))
					op = ReserveCharSet::CAT, --str;
				switch (op)
				{
				case ReserveCharSet::HYPEN:
				{
					int len = *(++str) - que.back();
					for (int i = 0; i < len; ++i) que.push(que.back() + 1);
					for (int i = 0; i < len; ++i) que.push(ct(ReserveCharSet::AND));
				}
				break;
				case ReserveCharSet::NONE: que.push(*str); break;
				case ReserveCharSet::LB:
					stk.push(op);
					break;
				case ReserveCharSet::RB:
				{
					for (typename ReserveCharSet::value top = stk.top(); top != ReserveCharSet::LB;) {
						que.push(ct(top));
						stk.pop();
						top = stk.top();
					}
					stk.pop();
				}
				break;
				case ReserveCharSet::AND:
				case ReserveCharSet::CAT:
				{
					for (typename ReserveCharSet::value top; !stk.empty() && (top = stk.top()) >= op;) {
						que.push(ct(top));
						stk.pop();
					}
					stk.push(op);
				}
				break;
				case ReserveCharSet::ANY:
				case ReserveCharSet::SOME:
				case ReserveCharSet::BIN:
					que.push(op);
					break;
				}
				++str;
			}
			ret.cap = unsigned(que.size() + stk.size());
			ret.stack = new ct[ret.cap];
			while (!que.empty()) ret.stack[ret.len++] = que.front(), que.pop();
			while (!stk.empty()) ret.stack[ret.len++] = ct(stk.top()), stk.pop();
			return ret;
		}

		unsigned length() const { return len; }

		void trace(std::ostream& os) const {
			for (unsigned i = 0; i < len; ++i) {
				if (stack[i] < 0) os << (int)stack[i];
				else os << stack[i];
			}
			os << std::endl;
		}

	private:
		static Regex<ct> generate(Regex<ct>&& a, Regex<ct>&& b, typename ReserveCharSet::value r) {
			Regex<ct> ret(std::move(a));
			ret.len += b.len;
			if (ret.cap <= ret.len) {
				for (ret.cap = max(1U, ret.cap << 1); ret.cap <= ret.len; ret.cap <<= 1);
				ct* p = new ct[ret.cap];
				if (ret.stack) {
					memcpy(p, ret.stack, sizeof(ct) * (ret.len - b.len));
					delete[] ret.stack;
				}
				ret.stack = p;
			}
			memcpy(ret.stack + (ret.len - b.len), b.stack, sizeof(ct) * b.len);
			ret.stack[ret.len++] = ct(r);
			return ret;
		}

		static Regex<ct> generate(Regex<ct>&& a, typename ReserveCharSet::value r) {
			Regex<ct> ret(std::move(a));
			if (ret.cap == ret.len) {
				ct* p = new ct[ret.cap = max(1U, ret.cap << 1)];
				if (ret.stack) {
					memcpy(p, ret.stack, sizeof(ct) * ret.len);
					delete[] ret.stack;
				}
				ret.stack = p;
			}
			ret.stack[ret.len++] = ct(r);
			return ret;
		}
	};

	template<typename C> ILL_INLINE Regex<C> operator+(Regex<C>&& a, Regex<C>&& b) { return Regex<C>::generate(std::move(a), std::move(b), Regex<C>::ReserveCharSet::CAT); }
	template<typename C> ILL_INLINE Regex<C> operator|(Regex<C>&& a, Regex<C>&& b) { return Regex<C>::generate(std::move(a), std::move(b), Regex<C>::ReserveCharSet::AND); }
	template<typename C> ILL_INLINE Regex<C> operator*(Regex<C>&& a) { return Regex<C>::generate(std::move(a), Regex<C>::ReserveCharSet::ANY); }
	template<typename C> ILL_INLINE Regex<C> operator+(Regex<C>&& a) { return Regex<C>::generate(std::move(a), Regex<C>::ReserveCharSet::SOME); }
	template<typename C> ILL_INLINE Regex<C> operator!(Regex<C>&& a) { return Regex<C>::generate(std::move(a), Regex<C>::ReserveCharSet::BIN); }

}