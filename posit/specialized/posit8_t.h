#pragma once
// posit8.h: standard 8-bit posit C implementation
//
// Copyright (C) 2017-2019 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include <math.h>  // for INFINITY

static const posit8_t posit8_sign_mask = { { 0x80 } };

// characterization tests
inline bool posit8_isnar(posit8_t p) { return (p.v == 0x80); }
inline bool posit8_iszero(posit8_t p) { return (p.v == 0x00); }
inline bool posit8_isone(posit8_t p) { return (p.v == 0x40); }      // pattern 010000...
inline bool posit8_isminusone(posit8_t p) { return (p.v == 0xC0); } // pattern 110000...
inline bool posit8_isneg(posit8_t p) { return (p.v & 0x80); }
inline bool posit8_ispos(posit8_t p) { return !(p.v & 0x80); }
inline bool posit8_ispowerof2(posit8_t p) { return !(p.v & 0x1); }

inline int posit8_sign_value(posit8_t p) { return (p.v & 0x80 ? -1 : 1); }

// decode and extraction

// decode_regime takes the raw bits of the posit, and returns the regime run-length, m, and the remaining fraction bits in remainder
inline int8_t posit8_decode_regime(const uint8_t bits, uint8_t* remaining) {
	int8_t m = 0;
	*remaining = (bits << 2) & 0xFF;
	if (bits & 0x40) {  // positive regimes
		while (*remaining >> 7) {
			++m;
			*remaining = (*remaining << 1) & 0xFF;
		}
	}
	else {              // negative regimes
		m = -1;
		while (!(*remaining >> 7)) {
			--m;
			*remaining = (*remaining << 1) & 0xFF;
		}
		*remaining &= 0x7F;
	}
	return m;
}
inline int8_t posit8_extractAddand(const uint8_t bits, int8_t m, uint8_t* remaining) {
	*remaining = (bits << 2) & 0xFF;
	if (bits & 0x40) {  // positive regimes
		while (*remaining >> 7) {
			--m;
			*remaining = (*remaining << 1) & 0xFF;
		}
	}
	else {              // negative regimes
		++m;
		while (!(*remaining >> 7)) {
			++m;
			*remaining = (*remaining << 1) & 0xFF;
		}
		*remaining &= 0x7F;
	}
	return m;
}
inline int8_t posit8_extractMultiplicand(const uint8_t bits, int8_t m, uint8_t* remaining) {
	*remaining = (bits << 2) & 0xFF;
	if (bits & 0x40) {  // positive regimes
		while (*remaining >> 7) {
			++m;
			*remaining = (*remaining << 1) & 0xFF;
		}
	}
	else {              // negative regimes
		--m;
		while (!(*remaining >> 7)) {
			--m;
			*remaining = (*remaining << 1) & 0xFF;
		}
		*remaining &= 0x7F;
	}
	return m;
}
inline int8_t posit8_extractDividand(const uint8_t bits, int8_t m, uint8_t* remaining) {
	*remaining = (bits << 2) & 0xFF;
	if (bits & 0x40) {  // positive regimes
		while (*remaining >> 7) {
			--m;
			*remaining = (*remaining << 1) & 0xFF;
		}
	}
	else {              // negative regimes
		++m;
		while (!(*remaining >> 7)) {
			++m;
			*remaining = (*remaining << 1) & 0xFF;
		}
		*remaining &= 0x7F;
	}
	return m;
}
inline uint8_t posit8_round(const int8_t m, uint16_t fraction) {
	uint8_t scale, regime, bits;
	if (m < 0) {
		scale = (-m & 0xFF);
		regime = 0x40 >> scale;
	}
	else {
		scale = m + 1;
		regime = 0x7F - (0x7F >> scale);
	}

	if (scale > 6) {
		bits = m<0 ? 0x1 : 0x7F;  // minpos and maxpos
	}
	else {
		fraction = (fraction & 0x3FFF) >> scale;
		uint8_t final_fbits = (uint8_t)(fraction >> 8);
		bool bitNPlusOne = (bool)(0x80 & fraction);
		bits = (uint8_t)regime + (uint8_t)final_fbits;
		// n+1 frac bit is 1. Need to check if another bit is 1 too if not round to even
		if (bitNPlusOne) {
			uint8_t moreBits = (0x7F & fraction) ? 0x01 : 0x00;
			bits += (bits & 0x01) | moreBits;
		}
	}
	return bits;
}
inline uint8_t posit8_adjustAndRound(const int8_t k, uint16_t fraction, bool nonZeroRemainder) {
	uint8_t scale, regime, bits;
	if (k < 0) {
		scale = (-k & 0xFF);
		regime = 0x40 >> scale;
	}
	else {
		scale = k + 1;
		regime = 0x7F - (0x7F >> scale);
	}

	if (scale > 6) {
		bits = k<0 ? 0x1 : 0x7F;  // minpos and maxpos
	}
	else {
		//remove carry and rcarry bits and shift to correct position
		fraction &= 0x7F;
		uint8_t final_fbits = (uint_fast16_t)fraction >> (scale + 1);
		bool bitNPlusOne = (bool)(0x1 & (fraction >> scale));
		bits = (uint8_t)regime + (uint8_t)final_fbits;
		if (bitNPlusOne) {
			uint8_t moreBits = (((1 << scale) - 1) & fraction) ? 0x01 : 0x00;
			if (nonZeroRemainder) moreBits = 0x01;
			// n+1 frac bit is 1. Need to check if another bit is 1 too if not round to even
			bits += (bits & 0x01) | moreBits;
		}
	}
	return bits;
}

// assignment operators for native types
posit8_t posit8_assign_int8(const signed char rhs) {
	// special case for speed as this is a common initialization
	posit8_t p = { { 0x00} };
	if (rhs == 0) {
		return p;
	}
	if (rhs == -128) {
		// 0x80 is special in int8 arithmetic as it is its own negation= 
		p.v = 0x80;// NaR
		return p; 
	}
	bool sign = (bool)(rhs & posit8_sign_mask.v);
	int8_t v = sign ? -rhs : rhs; // project to positive side of the projective reals
	uint8_t raw;
	if (v > 48) { // +-maxpos
		raw = 0x7F;
	}
	else {
		uint8_t mask = 0x40;
		int8_t k = 6;
		uint8_t fraction_bits = v;
		while (!(fraction_bits & mask)) {
			k--;
			fraction_bits <<= 1;
		}
		fraction_bits = (fraction_bits ^ mask);
		raw = (0x7F ^ (0x3F >> k)) | (fraction_bits >> (k + 1));

		mask = 0x1 << k; //bitNPlusOne
		if (mask & fraction_bits) {
			if (((mask - 1) & fraction_bits) | ((mask << 1) & fraction_bits)) raw++;
		}
	}

	p.v = (sign ? -raw : raw);
	return p;
}
posit8_t posit8_assign_float32(const float rhs) {
	posit8_t p = { { 0x00} };
	return p;
}

posit8_t posit8_set_raw_bits(uint64_t value) {
	posit8_t p = { { (uint8_t)(value & 0xff) } };
	return p;
}

posit8_t posit8_negate(posit8_t p) {
	p.v = -p.v; // 0 and NaR are invariant under uint8 arithmetic
	return p;
}
posit8_t posit8_addp8(posit8_t lhs, posit8_t rhs) {
//	printf("lhs = 0x%x  rhs = 0x%x\n", uint32_t(lhs.v), uint32_t(rhs.v));
	posit8_t p = { { 0x80 } };
	// process special cases
	if (posit8_isnar(lhs) || posit8_isnar(rhs)) {   // NaR
		return p;
	}
	if (posit8_iszero(lhs) || posit8_iszero(rhs)) { // zero
		p.v = (uint8_t)(lhs.v | rhs.v);
		return p;
	}
	bool sign = (bool)(lhs.v & posit8_sign_mask.v);
	if (sign) {
		lhs.v = -lhs.v & 0xFF;
		rhs.v = -rhs.v & 0xFF;
	}
	if (lhs.v < rhs.v) {
		uint8_t tmp = lhs.v;
		lhs.v = rhs.v;
		rhs.v = tmp;
	}
					
	// decode the regime of lhs
	uint8_t remaining = 0;
	int8_t m = posit8_decode_regime(lhs.v, &remaining);
	uint16_t frac16A = (0x80 | remaining) << 7;
	// adjust shift and extract fraction bits of rhs
	 int8_t shiftRight = posit8_extractAddand(rhs.v, m, &remaining);
	uint16_t frac16B = (0x80 | remaining) << 7;

	// Work-around CLANG (LLVM) compiler when shifting right more than number of bits
	(shiftRight>7) ? (frac16B = 0) : (frac16B >>= shiftRight); 

	frac16A += frac16B;

	bool rcarry = (bool)(0x8000 & frac16A); // is MSB set
	if (rcarry) {
		m++;
		frac16A >>= 1;
	}

	uint8_t raw = posit8_round(m, frac16A);
	p.v = (sign ? -raw : raw);
	return p;
}
posit8_t posit8_subp8(posit8_t lhs, posit8_t rhs) {
	// process special cases
	posit8_t p = { { 0x80} };
	if (posit8_isnar(lhs) || posit8_isnar(rhs)) {
		return p;
	}
	if (posit8_iszero(lhs) || posit8_iszero(rhs)) {
		p.v = (uint8_t)(lhs.v | rhs.v);
		return p;
	}
	// Both operands are actually the same sign if rhs inherits sign of sub: Make both positive
	bool sign = (bool)(lhs.v & posit8_sign_mask.v);
	(sign) ? (lhs.v = (-lhs.v & 0xFF)) : (rhs.v = (-rhs.v & 0xFF));

	if (lhs.v == rhs.v) {
		p.v = 0;
		return p;
	}
	if (lhs.v < rhs.v) {
		uint8_t tmp = lhs.v;
		lhs.v = rhs.v;
		rhs.v = tmp;
		sign = !sign;
	}

	// decode the regime of lhs
	uint8_t remaining = 0;
	int8_t m = posit8_decode_regime(lhs.v, &remaining);
	uint16_t frac16A = (0x80 | remaining) << 7;
	// adjust shift and extract fraction bits of rhs
	int8_t shiftRight = posit8_extractAddand(rhs.v, m, &remaining);
	uint16_t frac16B = (0x80 | remaining) << 7;

	// do the subtraction of the fractions
	if (shiftRight >= 14) {
		p.v = (sign ? -lhs.v : lhs.v);
		return p;
	}
	else {
		frac16B >>= shiftRight;
	}
	frac16A -= frac16B;

	while ((frac16A >> 14) == 0) {
		m--;
		frac16A <<= 1;
	}
	bool ecarry = (bool)(0x4000 & frac16A);
	if (!ecarry) {
		m--;
		frac16A <<= 1;
	}

	uint8_t raw = posit8_round(m, frac16A);
	p.v = (sign ? -raw : raw);
	return p;
}
posit8_t posit8_mulp8(posit8_t lhs, posit8_t rhs) {
	// process special cases
	posit8_t p = { { 0x80} };
	if (posit8_isnar(lhs) || posit8_isnar(rhs)) {
		return p;
	}
	if (posit8_iszero(lhs) || posit8_iszero(rhs)) {
		p.v = 0;
		return p;
	}

	// calculate the sign of the result
	bool sign = (bool)(lhs.v & 0x80) ^ (bool)(rhs.v & 0x80);
	lhs.v = lhs.v & 0x80 ? -lhs.v : lhs.v;
	rhs.v = rhs.v & 0x80 ? -rhs.v : rhs.v;

	// decode the regime of lhs
	uint8_t remaining = 0;
	int8_t m = posit8_decode_regime(lhs.v, &remaining);
	uint8_t lhs_fraction = (0x80 | remaining);
	// adjust shift and extract fraction bits of rhs
	m = posit8_extractMultiplicand(rhs.v, m, &remaining);
	uint8_t rhs_fraction = (0x80 | remaining);
	uint16_t result_fraction = (uint16_t)(lhs_fraction) * (uint16_t)(rhs_fraction);

	bool rcarry = (bool)(result_fraction & 0x8000);
	if (rcarry) {
		m++;
		result_fraction >>= 1;
	}

	// round
	uint8_t raw = posit8_round(m, result_fraction);
	p.v = (sign ? -raw : raw);
	return p;
}
posit8_t posit8_divp8(posit8_t lhs, posit8_t rhs) {
	// process special cases
	posit8_t p = { { 0x80 } };
	if (posit8_isnar(lhs) || posit8_isnar(rhs) || posit8_iszero(rhs)) {
		return p;
	}
	if (posit8_iszero(lhs)) {
		p.v = 0;
		return p;
	}

	// calculate the sign of the result
	bool sign = (bool)(lhs.v & 0x80) ^ (bool)(rhs.v & 0x80);
	lhs.v = lhs.v & 0x80 ? -lhs.v : lhs.v;
	rhs.v = rhs.v & 0x80 ? -rhs.v : rhs.v;

	// decode the regime of lhs
	uint8_t remaining = 0;
	int8_t m = posit8_decode_regime(lhs.v, &remaining);
	uint16_t lhs_fraction = (0x80 | remaining) << 7;
	// adjust shift and extract fraction bits of rhs
	m = posit8_extractDividand(rhs.v, m, &remaining);
	uint8_t rhs_fraction = (0x80 | remaining);
	div_t result = div(lhs_fraction, (uint16_t)(rhs_fraction));
	uint16_t result_fraction = result.quot;
	uint16_t remainder = result.rem;

	if (result_fraction != 0) {
		bool rcarry = result_fraction >> 7; // this is the hidden bit (7th bit) , extreme right bit is bit 0
		if (!rcarry) {
			--m;
			result_fraction <<= 1;
		}
	}

	// round
	uint8_t raw = posit8_adjustAndRound(m, result_fraction, remainder != 0);
	p.v = (sign ? -raw : raw);
	return p;
}

posit8_t posit8_reciprocate(posit8_t rhs) {
	posit8_t one = { { 0x40 } };
	return posit8_div(one, rhs);
}

float posit8_fraction_value(uint8_t fraction) {
	float v = 0.0f;
	float scale = 1.0f;
	uint8_t mask = 0x80;
	for (int i = 5; i >= 0; i--) {
		if (fraction & mask) v += scale;
		scale *= 0.5f;
		mask >>= 1;
		if (scale == 0.0) break;
	}
	return v;
}

// conversion functions
posit8_t posit8_fromf(float f) {
	posit8_t p = { { 0x00 } };
	return p;
}
posit8_t posit8_fromd(double d) {
	posit8_t p = { { 0x00 } };
	return p;
}

float posit8_tof(posit8_t p) {
	if (p.v == 0) return 0.0f;
	if (p.v == 0x80) return INFINITY;

	uint8_t bits = (p.v & 0x80 ? -p.v : p.v);  // use 2's complement when negative	
	uint8_t fraction = 0;
	int8_t m = posit8_decode_regime(bits, &fraction);

	float s = (float)(posit8_sign_value(p));
	float r = (m > 0 ? (float)((uint32_t)(1) << m) : (1.0f / (float)((uint32_t)(1) << -m)));
	float e = 1.0;
	float f = (1.0 + posit8_fraction_value(fraction));

//	printf("sign = %f : m = %d : regime = %f : fraction = 0x%x : fraction_value %f\n", s, m, r, fraction, f);
	return s * r * e * f;
}

double posit8_tod(posit8_t p) {
	return (double)posit8_tof(p);
}

int posit8_to_int(posit8_t p) {
	if (posit8_isnar(p))  return INFINITY;
	return (int)(posit8_tof(p));
}

posit8_t posit8_float_assign(float rhs) {
	posit8_t p = { { 0x00} };
	return p;
}

// posit - posit binary logic functions
bool posit8_equal(posit8_t lhs, posit8_t rhs)          { return lhs.v == rhs.v;  }
bool posit8_notEqual(posit8_t lhs, posit8_t rhs)       { return lhs.v != rhs.v;  }
bool posit8_lessThan(posit8_t lhs, posit8_t rhs)       { return lhs.v < rhs.v; }
bool posit8_greaterThan(posit8_t lhs, posit8_t rhs)    { return lhs.v > rhs.v;  }
bool posit8_lessOrEqual(posit8_t lhs, posit8_t rhs)    { return lhs.v <= rhs.v; }
bool posit8_greaterOrEqual(posit8_t lhs, posit8_t rhs) { return lhs.v >= rhs.v; }

	// preprocessin for add and sub
	/*
	inline posit<NBITS_IS_8, ES_IS_0> operator+(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
		posit<NBITS_IS_8, ES_IS_0> result = lhs;
		if (lhs.isneg() == rhs.isneg()) {  // are the posits the same sign?
			result += rhs;
		} 
		else {
			result -= rhs;
		}
		return result;
	}
	inline posit<NBITS_IS_8, ES_IS_0> operator-(const posit<NBITS_IS_8, ES_IS_0>& lhs, const posit<NBITS_IS_8, ES_IS_0>& rhs) {
		posit<NBITS_IS_8, ES_IS_0> result = lhs;
		if (lhs.isneg() == rhs.isneg()) {  // are the posits the same sign?
			result -= rhs.twosComplement();
		}
		else {
			result += rhs.twosComplement();
		}
		return result;

	}
	// binary operator*() is provided by generic class
	*/

