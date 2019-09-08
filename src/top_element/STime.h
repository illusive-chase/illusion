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
#include "Scalar.h"
#include "SPattern.h"

namespace fl {

	class SPeriod {
	private:
		clock_t s;
		SPeriod(clock_t seconds) :s(seconds) {}

	public:
		SPeriod(int dd, int HH, int mm = 0, int ss = 0) :s(((dd * 24 + HH) * 60 + mm) * 60 + ss) {}

		SPeriod& operator +=(const SPeriod& rhs) { return s += rhs.s, *this; }
		SPeriod& operator -=(const SPeriod& rhs) { return s -= rhs.s, *this; }
		SPeriod& operator *=(unsigned num) { return s *= num, *this; }
		SPeriod& operator /=(unsigned num) { return s /= num, *this; }
		SPeriod& operator %=(unsigned num) { return s %= num, *this; }

		SPeriod operator +(SPeriod rhs) const { return rhs += *this; }
		SPeriod operator -(SPeriod rhs) const { return rhs -= *this; }
		SPeriod operator *(unsigned num) const { return SPeriod(*this) *= num; }
		SPeriod operator /(unsigned num) const { return SPeriod(*this) /= num; }
		SPeriod operator %(unsigned num) const { return SPeriod(*this) %= num; }
		unsigned operator /(const SPeriod& rhs) const { return s / rhs.s; }
		SPeriod operator %(const SPeriod& rhs) const { return SPeriod(s % rhs.s); }

		bool operator >(const SPeriod& rhs) const { return s > rhs.s; }
		bool operator >=(const SPeriod& rhs) const { return s >= rhs.s; }
		bool operator <(const SPeriod& rhs) const { return s < rhs.s; }
		bool operator <=(const SPeriod& rhs) const { return s <= rhs.s; }
		bool operator ==(const SPeriod& rhs) const { return s == rhs.s; }
		bool operator !=(const SPeriod& rhs) const { return s != rhs.s; }

		bool positive() const { return s > 0; }
		bool negative() const { return s < 0; }
		bool zero() const { return s == 0; }

		clock_t days() const { return s / 86400; }
		clock_t hours() const { return s / 3600; }
		clock_t minutes() const { return s / 60; }
		clock_t seconds() const { return s; }
		
	};

	class STime {
	private:
		int h, m, s;
		STime& format() {
			m += s / 60;
			s %= 60;
			h += m / 60;
			m %= 60;
			h %= 24;
			return *this;
		}

	public:

		STime(unsigned HH, unsigned mm = 0, unsigned ss = 0) :h(HH), m(mm), s(ss) { format(); }

		STime(SPattern<3> & sp):h(0), m(0), s(0) { sp.write(h, m, s); }

		SPeriod operator -(const STime& rhs) const { return SPeriod(0, h - rhs.h, m - rhs.m, s - rhs.s); }
		STime& operator +=(const SPeriod& p) { return s + p.seconds(), format(); }
		STime operator +(const SPeriod& p) const { return STime(*this) += p; }
		STime& operator -=(const SPeriod& p) { return s - p.seconds(), format(); }
		STime operator -(const SPeriod& p) const { return STime(*this) -= p; }
		bool operator >(const STime& rhs) const { return (*this - rhs).positive(); }
		bool operator <(const STime& rhs) const { return (*this - rhs).negative(); }
		bool operator >=(const STime& rhs) const { return !((*this - rhs).negative()); }
		bool operator <=(const STime& rhs) const { return !((*this - rhs).positive()); }
		bool operator ==(const STime& rhs) const { return (*this - rhs).zero(); }
		bool operator !=(const STime& rhs) const { return !((*this - rhs).zero()); }

		void to_pattern(SPattern<3> & sp) const { sp.read(h, m, s); }
		
	};
}