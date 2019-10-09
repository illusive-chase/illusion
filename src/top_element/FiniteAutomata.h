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
#include "Regex.h"
#include <set>
#include <unordered_set>

namespace fl {


	class FA {
	public:
		using handle = unsigned;
		using Regex = Regex<char>;
		virtual ~FA() {}
		FA() {}
		virtual void print(std::ostream& os) const = 0;
		virtual unsigned size() const = 0;
		virtual handle build(const Regex& reg) = 0;
		virtual std::set<handle> parse(const char* str) const = 0;
	};

	class NFA :public FA {
	private:
		struct SubNFA { handle a, b; };
		
		struct Edge {
			handle to;
			Edge* next;
			Edge(handle to, Edge* next) :to(to), next(next) {}
			~Edge() { if (next) delete next; }
		};

		struct State {
			bool accept;
			Edge* edges[128];
			State(bool accept = false) : accept(accept), edges{} {}
		};

		std::vector<State> states;
		std::stack<handle> free;

		ILL_INLINE void link(handle a, handle b, char w) { states[a].edges[w] = new Edge(b, states[a].edges[w]); }

		handle create() {
			handle n = 0U;
			if (free.empty()) {
				n = (handle)states.size();
				states.push_back(State());
				return n;
			}
			n = free.top();
			free.pop();
			return n;
		}

		void release(handle h) {
			for (int i = 0; i < 128; ++i) if (states[h].edges[i]) delete states[h].edges[i];
			states[h] = State();
			free.push(h);
		}

		handle reduce(handle a, handle b) {
			memcpy(states[a].edges, states[b].edges, sizeof(states[b].edges));
			states[b] = State();
			free.push(b);
			return a;
		}

		mutable std::vector<bool> parse_mask;
		mutable std::queue<handle> parse_que;
		mutable unsigned parse_count;

		void add_state(handle a) const {
			++parse_count;
			parse_que.push(a);
			parse_mask[a] = true;
			for (Edge* e = states[a].edges[0]; e; e = e->next) {
				if(!parse_mask[e->to]) add_state(e->to);
			}
		}

	public:
		NFA() :parse_count(0U) { create(); }
		~NFA() { for (State& st : states) for (int i = 0; i < 128; ++i) if (st.edges[i]) delete st.edges[i]; }

		std::set<handle> parse(const char* str) const override {
			parse_count = 0U;
			parse_mask = std::vector<bool>(states.size());
			parse_que = std::queue<handle>();
			add_state(0U);
			std::set<handle> ret;
			while (*str) {
				unsigned i=0, len=parse_count;
				parse_count = 0U;
				parse_mask = std::vector<bool>(states.size());
				for (; i < len; ++i) {
					const State& st = states[parse_que.front()];
					if (st.accept) ret.insert(parse_que.front());
					parse_que.pop();
					for (Edge* e = st.edges[*str]; e; e = e->next) if (!parse_mask[e->to]) add_state(e->to);
				}
				if (parse_que.empty()) return ret;
				str++;
			}
			while (!parse_que.empty()) {
				const State& st = states[parse_que.front()];
				if (st.accept) ret.insert(parse_que.front());
				parse_que.pop();
			}
			return ret;
		}

		handle build(const Regex& reg) override {
			SubNFA* stack = new SubNFA[reg.length() << 1]();
			unsigned len = 0;
			for (Regex::Iterator it = reg.begin(); it != reg.end(); ++it) {
				if (*it >= 0) {
					handle a = create(), b = create();
					link(a, b, *it);
					stack[len++] = { a,b };
				} else {
					switch (*it)
					{
					case Regex::ReserveCharSet::AND:
					{
						SubNFA& rs = stack[--len];
						SubNFA& ls = stack[len - 1];
						handle n = create();
						link(n, rs.a, '\0'), link(n, ls.a, '\0');
						ls.a = n, n = create();
						link(rs.b, n, '\0'), link(ls.b, n, '\0');
						ls.b = n;
					}
					break;
					case Regex::ReserveCharSet::CAT:
					{
						SubNFA& rs = stack[--len];
						SubNFA& ls = stack[len - 1];
						reduce(ls.b, rs.a);
						ls.b = rs.b;
					}
					break;
					case Regex::ReserveCharSet::ANY:
					case Regex::ReserveCharSet::SOME:
					case Regex::ReserveCharSet::BIN:
					{
						SubNFA& rs = stack[len - 1];
						if(*it != Regex::ReserveCharSet::BIN) link(rs.b, rs.a, '\0');
						handle n = create();
						link(n, rs.a, '\0');
						rs.a = n;
						link(rs.b, n = create(), '\0');
						rs.b = n;
						if (*it != Regex::ReserveCharSet::SOME) link(rs.a, n, '\0');
					}
					break;
					}
				}
			}
			link(0, stack[0].a, '\0');
			handle ret = stack[0].b;
			states[ret].accept = true;
			delete[] stack;
			return ret;
		}

		ILL_INLINE unsigned size() const override { return (unsigned)states.size(); }

		void print(std::ostream& os) const override {
			handle id = 0U;
			for (const State& it : states) {
				for (int c = 0; c < 128; ++c) {
					char prt = (c ? ((c == '\n' || c == '\t' || c == ' ') ? '#' : c) : '~');
					for (const Edge* e = it.edges[c]; e; e = e->next) os << id << ' ' << e->to << ' ' << prt << std::endl;
				}
				++id;
			}
		}

	};


	


	class DFA :public FA {
	private:
		struct HelperFunction {
			bool nullable;
			std::set<handle> first, last;
			HelperFunction() :nullable(false) {}
			HelperFunction(handle value) :nullable(false) { first.insert(value), last.insert(value); }
			HelperFunction(HelperFunction&& rhs) noexcept :nullable(rhs.nullable), first(std::move(rhs.first)), last(std::move(rhs.last)) {}
		};

		struct ImportantState {
			handle id;
			char c;
			std::set<handle> follow;
			ImportantState(handle id, char c) :id(id), c(c) {}
		};

		struct MapState {
			std::set<handle> set;
			handle h;
			MapState(handle h) :h(h) {}
		};

		struct State {
			unsigned accept;
			handle to[128];
			State(unsigned accept = 0U) :accept(accept), to() {}
		};

		std::vector<State> states;
		std::stack<handle> free;
		unsigned accept_constant;

		struct compare {
			bool operator ()(const MapState& msa, const MapState& msb) const {
				const std::set<handle>& a = msa.set, &b = msb.set;
				if (a.size() != b.size()) return a.size() < b.size();
				for (auto ita = a.begin(), itb = b.begin(); ita != a.end(); ++ita, ++itb) {
					if (*ita != *itb) return *ita < *itb;
				}
				return false;
			}
		};

		void MakeUnion(MapState& a, std::set<handle>& b, handle accept) {
			if (b.count(accept)) states[a.h].accept = accept_constant;
			a.set.insert(b.begin(), b.end());
		}

		handle create() {
			handle n = 0U;
			if (free.empty()) {
				n = (handle)states.size();
				states.push_back(State());
				return n;
			}
			n = free.top();
			free.pop();
			return n;
		}

		handle to_create() const {
			if (free.empty()) return (handle)states.size();
			return free.top();
		}

		void release(handle h) {
			states[h] = State();
			free.push(h);
		}

		void link(handle a, handle b, char w) { states[a].to[w] = b; }

		unsigned find(std::vector<int>& uss, int x){
			int p = uss[x];
			while (uss[p] != p && ~uss[p]) p = uss[p];
			for (int temp = uss[x]; temp != p; temp = uss[x]) {
				uss[x] = p;
				x = temp;
			}
			return p;
		}

		void merge(std::vector<int>& uss, int i, int j) {
			uss[find(uss, i)] = find(uss, j);
		}

	public:
		DFA() :accept_constant(0U) {}

		ILL_INLINE unsigned size() const override { return (unsigned)states.size(); }

		void print(std::ostream& os = std::cout) const override {
			handle id = 0U;
			for (const State& it : states) {
				for (int c = 0; c < 128; ++c) {
					if (it.to[c]) {
						char prt = (c ? ((c == '\n' || c == '\t' || c == ' ') ? '#' : c) : '~');
						os << id << ' ' << it.to[c] << ' ' << prt << std::endl;
					}
				}
				++id;
			}
			os << std::endl;
			id = 0U;
			for (const State& it : states) {
				if (it.accept) os << id << " ";
				id++;
			}
			os << std::endl;
		}

		handle build(const Regex& reg) override {
			++accept_constant;
			std::stack<HelperFunction> hf;
			std::vector<ImportantState> istate;
			for (Regex::Iterator it = reg.begin(); it != reg.end(); ++it) {
				if (*it >= 0) {
					hf.push(HelperFunction(handle(istate.size())));
					istate.push_back(ImportantState((handle)istate.size(), *it));
				} else {
					switch (*it)
					{
					case Regex::ReserveCharSet::AND:
					{
						HelperFunction rs(std::move(hf.top()));
						hf.pop();
						HelperFunction& ls = hf.top();
						ls.nullable = ls.nullable || rs.nullable;
						ls.first.insert(rs.first.begin(), rs.first.end());
						ls.last.insert(rs.last.begin(), rs.last.end());
					}
					break;
					case Regex::ReserveCharSet::CAT:
					{
						HelperFunction rs(std::move(hf.top()));
						hf.pop();
						HelperFunction& ls = hf.top();
						for (handle h : ls.last) istate[h].follow.insert(rs.first.begin(), rs.first.end());
						if (ls.nullable) ls.first.insert(rs.first.begin(), rs.first.end());
						if (rs.nullable) ls.last.insert(rs.last.begin(), rs.last.end());
						else ls.last = std::move(rs.last);
						ls.nullable = ls.nullable && rs.nullable;
					}
					break;
					case Regex::ReserveCharSet::ANY:
					case Regex::ReserveCharSet::SOME:
					case Regex::ReserveCharSet::BIN:
					{
						HelperFunction& ls = hf.top();
						if(*it != Regex::ReserveCharSet::SOME) ls.nullable = true;
						if(*it != Regex::ReserveCharSet::BIN)
							for (handle h : ls.last) istate[h].follow.insert(ls.first.begin(), ls.first.end());
					}
					break;
					}
				}
			}
			
			for (handle h : hf.top().last) istate[h].follow.insert((handle)istate.size());

			std::set<MapState, compare> vis_mstates;
			std::queue<MapState> mstates;
			std::unordered_set<char> char_map;
			std::vector<char> char_set;
			mstates.push(MapState(create()));
			vis_mstates.insert(mstates.front());
			
			MakeUnion(mstates.front(), hf.top().first, (handle)istate.size());
			for (ImportantState& it : istate) {
				if (char_map.insert(it.c).second) char_set.push_back(it.c);
			}
			
			while (!mstates.empty()) {
				MapState it = std::move(mstates.front());
				mstates.pop();
				for (char c : char_set) {
					handle last = create();
					MapState nmstate(last);
					for (handle h : it.set) {
						if (h == (handle)istate.size()) continue;
						ImportantState& rstate = istate[h];
						if (rstate.c == c) MakeUnion(nmstate, rstate.follow, (handle)istate.size());
					}
					auto pair = vis_mstates.insert(std::move(nmstate));
					if (pair.second) mstates.push(*pair.first);
					else release(last);
					link(it.h, pair.first->h, c);
				}
			}
			return accept_constant;
		}

		std::set<handle> parse(const char* str) const override {
			std::set<handle> ret;
			for (handle p = 0U; *str; ++str) {
				if (states[p].accept) ret.insert(states[p].accept);
				p = states[p].to[*str];
				if (!p) return ret;
			}
			return ret;
		}

		void optimize() {
			std::vector<unsigned> partition(states.size());
			std::vector<std::vector<handle>> groups(2);
			for (unsigned i = 0; i < states.size(); ++i) {
				if (states[i].accept) partition[i]++, groups[1].push_back(i);
				else groups[0].push_back(i);
			}

			for (bool newFlag = true; newFlag;) {
				newFlag = false;
				for (unsigned ii = 0; ii < groups.size(); ++ii) {
					std::vector<handle>& group = groups[ii];
					if (group.size() == 1) continue;
					std::vector<std::vector<handle>> newGroups;
					std::vector<int> uss(group.size());
					for (unsigned i = 0; i < uss.size(); ++i) uss[i] = i;
					for (unsigned i = 0; i < uss.size(); ++i) {
						if (uss[i] != i) continue;
						for (unsigned j = i + 1; j < uss.size(); ++j) {
							bool same = true;
							handle* ito = states[group[i]].to;
							handle* jto = states[group[j]].to;
							for (int k = 0; k < 128; ++k) {
								if (partition[ito[k]] != partition[jto[k]]) {
									same = false;
									break;
								}
							}
							if (same) merge(uss, (int)i, (int)j);
						}
					}

					for (unsigned i = 0, group_id = find(uss, (int)i), 
						 part_id = (unsigned)(groups.size() + newGroups.size());
						 i < uss.size(); ++i) 
					{
						if (!~uss[i]) continue;
						uss[i] = -1;
						std::vector<handle> newGroup(1, group[i]);
						if (i) partition[group[i]] = part_id;
						for (unsigned j = i + 1; j < uss.size(); ++j) {
							if (!~uss[j]) continue;
							if (find(uss, (int)j) == group_id) {
								newGroup.push_back(group[j]), uss[j] = -1;
								if (i) partition[group[j]] = part_id;
							}
						}
						newGroups.push_back(std::move(newGroup));
					}

					group = std::move(newGroups[0]);
					if (newGroups.size() > 1) newFlag = true;
					for (unsigned j = 1; j < newGroups.size(); ++j) groups.push_back(std::move(newGroups[j]));
				}
			}

			for (auto& it : groups) {
				for (auto& every : it) std::cout << every << ' ';
				std::cout << std::endl;
			}
		}

	};


}