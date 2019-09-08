#pragma once
#include "Struct.h"
#include "System.h"
#include <unordered_map>


namespace fl {

	template <unsigned para_num>
	class SPattern {
	private:

		struct set_function{
			int cnt;
			wstring(&token_ref)[para_num];
			std::unordered_map<wstring, wstring>& mp_ref;
			set_function(wstring(&token_ref)[para_num],
				  std::unordered_map<wstring, wstring>& mp_ref
			):token_ref(token_ref), mp_ref(mp_ref), cnt(0) {}
			void operator ()(const wstring& s) { token_ref[cnt++] = s; mp_ref[s] = L""; }
		};

		struct io_stream {
		private:
			wstring* const str;
		public:
			io_stream():str(nullptr) {}
			io_stream(wstring& str):str(&str) {}
			io_stream& operator <<(const wstring& rhs) { return *str = rhs, *this; }
			io_stream& operator <<(int rhs) { return (wstringstream() << rhs) >> *str, * this; }
			io_stream& operator <<(scalar rhs) { return (wstringstream() << rhs) >> *str, * this; }
			io_stream& operator >>(wstring& rhs) { return rhs = *str, *this; }
			io_stream& operator >>(int& rhs) { return wstringstream(*str) >> rhs, *this; }
			io_stream& operator >>(scalar& rhs) { return wstringstream(*str) >> rhs, *this; }
			bool fail() const { return str; }
			operator bool() const { return str; }
		};

		struct write_function {
			int cnt;
			wstring(&token_ref)[para_num];
			std::unordered_map<wstring, wstring>& mp_ref;
			write_function(wstring(&token_ref)[para_num],
						  std::unordered_map<wstring, wstring>& mp_ref
			):token_ref(token_ref), mp_ref(mp_ref), cnt(0) {}
			void operator ()(wstring& s) { s = mp_ref[token_ref[cnt++]]; }
			void operator ()(int& para) { wstringstream(mp_ref[token_ref[cnt++]]) >> para; }
			void operator ()(scalar& para) { wstringstream(mp_ref[token_ref[cnt++]]) >> para; }
		};

		struct read_function{
			int cnt;
			wstring(&token_ref)[para_num];
			std::unordered_map<wstring, wstring>& mp_ref;
			read_function(wstring(&token_ref)[para_num],
						 std::unordered_map<wstring, wstring>& mp_ref
			):token_ref(token_ref), mp_ref(mp_ref), cnt(0) {}
			void operator ()(const wstring& s) { mp_ref[token_ref[cnt++]] = s; }
			void operator ()(int para) { (wstringstream() << para) >> mp_ref[token_ref[cnt++]]; }
			void operator ()(scalar para) { (wstringstream() << para) >> mp_ref[token_ref[cnt++]]; }
		};

		wstring token[para_num];
		std::unordered_map<wstring, wstring> mp;

	public:
		template<typename ...T>
		SPattern(T ...args) {
			set_function f(token, mp);
			VariableParamentsIterator<para_num, set_function, T...>::iter(f, args...);
		}

		template<typename ...T>
		void read(T ...args) {
			read_function f(token, mp);
			VariableParamentsIterator<para_num, read_function, T...>::iter(f, args...);
		}

		template<typename ...T>
		void write(T ...args) {
			write_function f(token, mp);
			VariableParamentsIterator<para_num, write_function, T...>::iter(f, args...);
		}

		wstring to_string(const wstring& p) {
			int pi = 0, pl = (int)p.length();
			int ti = 0;
			wstringstream ret;
			for (;;) {
				if (pi == pl) return wstring();
				bool match = true;
				for (int i = 0, len = (int)token[ti].length(); i < len; ++i) {
					if (pi + i == pl) return wstring();
					if (p[pi + i] != token[ti][i]) {
						match = false;
						break;
					}
				}
				if (match) {
					pi += (int)token[ti].length();
					ret << mp[token[ti++]];
					if (ti == para_num) {
						for (; pi < pl; ++pi) ret << p[pi];
						return ret.str();
					}
				} else ret << p[pi++];
			}
			return wstring();
		}
		

		bool format(const wstring& p, const wstring& s) {
			int pi = 0, si = 0;
			int pl = (int)p.length(), sl = (int)s.length();
			int ti = 0;
			for (;; ++pi, ++si) {
				if (pi == pl) return si == sl;
				else if (si == sl) return false;
				
				bool match = true;
				if (ti < para_num) {
					for (int i = 0, len = (int)token[ti].length(); i < len; ++i) {
						if (pi + i == pl) return false;
						if (p[pi + i] != token[ti][i]) {
							match = false;
							break;
						}
					}
				} else match = false;

				if (match) {
					pi += (int)token[ti].length();
					if (pi == pl) {
						mp[token[ti++]] = wstring(s.begin() + si, s.end());
						return true;
					} else {
						int sj = si;
						while (s[si] != p[pi]) {
							if (si++ == sl) return false;
						}
						mp[token[ti++]] = wstring(s.begin() + sj, s.begin() + si);
					}
				} else if (p[pi] != s[si]) return false;
			}
			return false;
		}

		io_stream operator[](const wstring& str) {
			if (mp.count(str)) return io_stream(mp[str]);
			return io_stream();
		}


	};

}