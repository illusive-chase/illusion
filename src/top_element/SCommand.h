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
#include "FiniteAutomata.h"
#include "Struct.h"
namespace fl {
	namespace ui {

		template<unsigned MAX_OP_SIZE = 128U>
		class SCommand {
		private:

			class compare_function {
			public:
				const wchar_t (&buffer)[MAX_OP_SIZE + 1];
				const unsigned& len;
				bool& ret;
				unsigned p;
				compare_function(const wchar_t(&buffer)[MAX_OP_SIZE + 1], 
								 const unsigned& len, bool& ret)
					:buffer(buffer), len(len), ret(ret), p(0) {}

				void operator()(const wchar_t* cmd) {
					if (!ret) return;
					for (; p <= len; ++p, ++cmd) {
						if (!buffer[p] || !*cmd) {
							if (buffer[p] || *cmd) return ret = false, void();
							return ++p, void();
						}
						if (buffer[p] != *cmd) return ret = false, void();
					}
					ret = false;
				}

				template<typename T>
				void operator()(T& target) {
					if (ret) {
						unsigned bg = p;
						while (buffer[p++]);
						ret = bool(wstringstream(wstring(buffer + bg)) >> target);
					}
				}

			};

		
			wchar_t buffer[MAX_OP_SIZE + 1];
			unsigned len;

		public:

			bool read() {
				std::wcout << L">> ";
				len = 0;
				for (;;) {
					wchar_t tmp = std::wcin.get();
					if (tmp == L' ' || tmp == L'\r') {
						buffer[len++] = 0;
						if (len == MAX_OP_SIZE) return buffer[len] = 0, false;
						do {
							tmp = std::wcin.get();
							if (std::wcin.fail()) break;
						} while (tmp == L' ' || tmp == L'\r');
					}
					if (std::wcin.fail() || tmp == L'\n') {
						buffer[len] = 0;
						break;
					}
					buffer[len++] = tmp;
					if (len == MAX_OP_SIZE) return buffer[len] = 0, false;
				}
				return !std::wcin.fail();
			}

			template<typename ...T>
			bool parse(T&... args) {
				bool ret = true;
				compare_function f(this->buffer, this->len, ret);
				VariableParamentsIterator
					<sizeof...(T), compare_function, T& ...>::iter(f, args...);
				return ret;
			}

		};

	}
}