#pragma once
// areal.hpp: definition of an arbitrary configuration linear floating-point representation
//
// Copyright (C) 2017-2021 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <cassert>
#include <limits>

#include <universal/native/ieee-754.hpp>
#include <universal/blockbin/blockbinary.hpp>
#include <universal/areal/exceptions.hpp>

// compiler specific operators
#if defined(__clang__)
/* Clang/LLVM. ---------------------------------------------- */


#elif defined(__ICC) || defined(__INTEL_COMPILER)
/* Intel ICC/ICPC. ------------------------------------------ */


#elif defined(__GNUC__) || defined(__GNUG__)
/* GNU GCC/G++. --------------------------------------------- */


#elif defined(__HP_cc) || defined(__HP_aCC)
/* Hewlett-Packard C/aC++. ---------------------------------- */

#elif defined(__IBMC__) || defined(__IBMCPP__)
/* IBM XL C/C++. -------------------------------------------- */

#elif defined(_MSC_VER)
/* Microsoft Visual Studio. --------------------------------- */


#elif defined(__PGI)
/* Portland Group PGCC/PGCPP. ------------------------------- */

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
/* Oracle Solaris Studio. ----------------------------------- */

#endif

#define THROW_ARITHMETIC_EXCEPTION 0

namespace sw::universal {
		
// Forward definitions
template<size_t nbits, size_t es, typename bt> class areal;
template<size_t nbits, size_t es, typename bt> areal<nbits,es,bt> abs(const areal<nbits,es,bt>& v);
constexpr bool AREAL_NIBBLE_MARKER = true;

template<size_t nbits, size_t es, typename bt>
void extract_fields(const blockbinary<nbits, bt>& raw_bits, bool& _sign, blockbinary<es, bt>& _exponent, blockbinary<nbits - es - 1, bt>& _fraction) {

}

// fill an areal object with mininum positive value
template<size_t nbits, size_t es, typename bt>
areal<nbits, es, bt>& minpos(areal<nbits, es, bt>& aminpos) {

	return aminpos;
}
// fill an areal object with maximum positive value
template<size_t nbits, size_t es, typename bt>
areal<nbits, es, bt>& maxpos(areal<nbits, es, bt>& amaxpos) {

	return amaxpos;
}
// fill an areal object with mininum negative value
template<size_t nbits, size_t es, typename bt>
areal<nbits, es, bt>& minneg(areal<nbits, es, bt>& aminneg) {

	return aminneg;
}
// fill an areal object with maximum negative value
template<size_t nbits, size_t es, typename bt>
areal<nbits, es, bt>& maxneg(areal<nbits, es, bt>& amaxneg) {

	return amaxneg;
}

// template class representing a value in scientific notation, using a template size for the number of fraction bits
template<size_t NBITS, size_t EBITS, typename bt = uint8_t>
class areal {
public:
	static constexpr size_t bitsInByte = 8;
	static constexpr size_t bitsInBlock = sizeof(bt) * bitsInByte;
	static_assert(bitsInBlock <= 32, "storage unit for block arithmetic needs to be <= uint32_t");

	static constexpr size_t nbits = NBITS;
	static constexpr size_t es = EBITS;

	static constexpr size_t nrBlocks = 1 + ((nbits - 1) / bitsInBlock);
	static constexpr uint64_t storageMask = (0xFFFFFFFFFFFFFFFFul >> (64 - bitsInBlock));
	static constexpr bt maxBlockValue = (uint64_t(1) << bitsInBlock) - 1;

	static constexpr size_t MSU = nrBlocks - 1; // MSU == Most Significant Unit, as MSB is already taken
		// warning C4310 : cast truncates constant value
	static constexpr bt MSU_MASK = (bt(0xFFFFFFFFFFFFFFFFul) >> (nrBlocks * bitsInBlock - nbits));
	static constexpr bt SIGN_BIT_MASK = bt(bt(1) << ((nbits - 1) % bitsInBlock));

	static constexpr size_t fbits  = nbits - 1 - es;    // number of fraction bits excluding the hidden bit
	static constexpr size_t fhbits = fbits + 1;         // number of fraction bits including the hidden bit
	static constexpr size_t abits = fhbits + 3;         // size of the addend
	static constexpr size_t mbits = 2 * fhbits;         // size of the multiplier output
	static constexpr size_t divbits = 3 * fhbits + 4;   // size of the divider output

	// constructors
	areal() noexcept : _block{ 0 } {};

	// decorated/converting constructors

	/// <summary>
	/// construct an areal from another, block type bt must be the same
	/// </summary>
	/// <param name="rhs"></param>
	template<size_t nnbits, size_t ees>
	areal(const areal<nnbits, ees, bt>& rhs) {
		// this->assign(rhs);
	}

	/// <summary>
	/// construct an areal from a native type, specialized for size
	/// </summary>
	/// <param name="initial_value"></param>
	areal(signed char initial_value)        { *this = initial_value; }
	areal(short initial_value)              { *this = initial_value; }
	areal(int initial_value)                { *this = initial_value; }
	areal(long long initial_value)          { *this = initial_value; }
	areal(unsigned long long initial_value) { *this = initial_value; }
	areal(float initial_value)              { *this = initial_value; }
	areal(double initial_value)             { *this = initial_value; }
	areal(long double initial_value)        { *this = initial_value; }
	areal(const areal& rhs)                 { *this = rhs; }

	// assignment operators
	areal& operator=(signed char rhs) {
		return *this = (long long)(rhs);
	}
	areal& operator=(short rhs) {
		return *this = (long long)(rhs);
	}
	areal& operator=(int rhs) {
		return *this = (long long)(rhs);
	}
	areal& operator=(long long rhs) {
		return *this;
	}
	areal& operator=(unsigned long long rhs) {
		return *this;
	}
	areal& operator=(float rhs) {

		return *this;
	}
	areal& operator=(double rhs) {

		return *this;
	}
	areal& operator=(long double rhs) {

		return *this;
	}

	// arithmetic operators
	// prefix operator
	areal operator-() const {				
		return *this;
	}

	areal& operator+=(const areal& rhs) {
		return *this;
	}
	areal& operator+=(double rhs) {
		return *this += areal(rhs);
	}
	areal& operator-=(const areal& rhs) {

		return *this;
	}
	areal& operator-=(double rhs) {
		return *this -= areal<nbits, es>(rhs);
	}
	areal& operator*=(const areal& rhs) {

		return *this;
	}
	areal& operator*=(double rhs) {
		return *this *= areal<nbits, es>(rhs);
	}
	areal& operator/=(const areal& rhs) {

		return *this;
	}
	areal& operator/=(double rhs) {
		return *this /= areal<nbits, es>(rhs);
	}
	areal& operator++() {
		return *this;
	}
	areal operator++(int) {
		areal tmp(*this);
		operator++();
		return tmp;
	}
	areal& operator--() {
		return *this;
	}
	areal operator--(int) {
		areal tmp(*this);
		operator--();
		return tmp;
	}

	// modifiers
	
	/// <summary>
	/// clear the content of this areal to zero
	/// </summary>
	/// <returns>void</returns>
	inline void clear() noexcept {
		for (size_t i = 0; i < nrBlocks; ++i) {
			_block[i] = bt(0);
		}
	}
	inline constexpr void setzero() noexcept { clear(); }
	/// <summary>
	/// set the raw bits of the areal. This is a required function in the Universal number systems
	/// that enables verification test suites to inject specific bit patterns using a common interface.
	/// </summary>
	/// <param name="raw_bits">unsigned long long carrying bits that will be written verbatim to the areal</param>
	/// <returns>reference to the areal</returns>
	inline areal& set_raw_bits(uint64_t raw_bits) noexcept {
		for (size_t i = 0; i < nrBlocks; ++i) {
			_block[i] = raw_bits & storageMask;
			raw_bits >>= bitsInBlock;
		}
		_block[MSU] &= MSU_MASK; // enforce precondition for fast comparison by properly nulling bits that are outside of nbits
		return *this;
	}

	// selectors
	inline bool sign() const { return test(nbits - 1); }
	inline bool isneg() const { return sign(); }
	inline bool ispos() const { return !sign(); }
	inline bool iszero() const { // TODO: need to deal with -0 as well
		for (size_t i = 0; i < nrBlocks; ++i) if (_block[i] != 0) return false;
		return true;
	}
	inline bool isinf() const { return false; }
	inline bool isnan() const { return false; }

	inline constexpr bool test(size_t bitIndex) const {
		return at(bitIndex);
	}
	inline constexpr bool at(size_t bitIndex) const {
		if (bitIndex < nbits) {
			bt word = _block[bitIndex / bitsInBlock];
			bt mask = bt(1 << (bitIndex % bitsInBlock));
			return (word & mask);
		}
		throw "bit index out of bounds";
	}
	inline constexpr uint8_t nibble(size_t n) const {
		if (n < (1 + ((nbits - 1) >> 2))) {
			bt word = _block[(n * 4) / bitsInBlock];
			int nibbleIndexInWord = n % (bitsInBlock >> 2);
			bt mask = 0xF << (nibbleIndexInWord * 4);
			bt nibblebits = mask & word;
			return (nibblebits >> (nibbleIndexInWord * 4));
		}
		throw "nibble index out of bounds";
	}
	inline constexpr bt block(size_t b) const {
		if (b < nrBlocks) {
			return _block[b];
		}
		throw "block index out of bounds";
	}

	inline int scale() const { return false; }
	inline std::string get() const { return std::string("tbd"); }

	long long to_long_long() const {
		return 0ll;
	}
	long double to_long_double() const {
		return 0.0l;
	}
	double to_double() const {
		return 0.0;
	}
	float to_float() const {
		return 0.0f;
	}

	// make conversions to native types explicit
	explicit operator int() const { return to_long_long(); }
	explicit operator long double() const { return to_long_double(); }
	explicit operator double() const { return to_double(); }
	explicit operator float() const { return to_float(); }

protected:
	// HELPER methods
	// none

private:
	bt _block[nrBlocks];

	//////////////////////////////////////////////////////////////////////////////
	// friend functions

	// template parameters need names different from class template parameters (for gcc and clang)
	template<size_t nnbits, size_t nes, typename nbt>
	friend std::ostream& operator<< (std::ostream& ostr, const areal<nnbits,nes,nbt>& r);
	template<size_t nnbits, size_t nes, typename nbt>
	friend std::istream& operator>> (std::istream& istr, areal<nnbits,nes,nbt>& r);

	template<size_t nnbits, size_t nes, typename nbt>
	friend bool operator==(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs);
	template<size_t nnbits, size_t nes, typename nbt>
	friend bool operator!=(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs);
	template<size_t nnbits, size_t nes, typename nbt>
	friend bool operator< (const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs);
	template<size_t nnbits, size_t nes, typename nbt>
	friend bool operator> (const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs);
	template<size_t nnbits, size_t nes, typename nbt>
	friend bool operator<=(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs);
	template<size_t nnbits, size_t nes, typename nbt>
	friend bool operator>=(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs);
};

////////////////////// operators
template<size_t nnbits, size_t nes, typename nbt>
inline std::ostream& operator<<(std::ostream& ostr, const areal<nnbits,nes,nbt>& v) {

	return ostr;
}

template<size_t nnbits, size_t nes, typename nbt>
inline std::istream& operator>>(std::istream& istr, const areal<nnbits,nes,nbt>& v) {
	istr >> v._fraction;
	return istr;
}

template<size_t nnbits, size_t nes, typename nbt>
inline bool operator==(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs) { return false; }
template<size_t nnbits, size_t nes, typename nbt>
inline bool operator!=(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs) { return !operator==(lhs, rhs); }
template<size_t nnbits, size_t nes, typename nbt>
inline bool operator< (const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs) { return false; }
template<size_t nnbits, size_t nes, typename nbt>
inline bool operator> (const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs) { return  operator< (rhs, lhs); }
template<size_t nnbits, size_t nes, typename nbt>
inline bool operator<=(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs) { return !operator> (lhs, rhs); }
template<size_t nnbits, size_t nes, typename nbt>
inline bool operator>=(const areal<nnbits,nes,nbt>& lhs, const areal<nnbits,nes,nbt>& rhs) { return !operator< (lhs, rhs); }

// posit - posit binary arithmetic operators
// BINARY ADDITION
template<size_t nbits, size_t es, typename bt>
inline areal<nbits, es, bt> operator+(const areal<nbits, es, bt>& lhs, const areal<nbits, es, bt>& rhs) {
	areal<nbits, es> sum(lhs);
	sum += rhs;
	return sum;
}
// BINARY SUBTRACTION
template<size_t nbits, size_t es, typename bt>
inline areal<nbits, es, bt> operator-(const areal<nbits, es, bt>& lhs, const areal<nbits, es, bt>& rhs) {
	areal<nbits, es> diff(lhs);
	diff -= rhs;
	return diff;
}
// BINARY MULTIPLICATION
template<size_t nbits, size_t es, typename bt>
inline areal<nbits, es, bt> operator*(const areal<nbits, es, bt>& lhs, const areal<nbits, es, bt>& rhs) {
	areal<nbits, es> mul(lhs);
	mul *= rhs;
	return mul;
}
// BINARY DIVISION
template<size_t nbits, size_t es, typename bt>
inline areal<nbits, es, bt> operator/(const areal<nbits, es, bt>& lhs, const areal<nbits, es, bt>& rhs) {
	areal<nbits, es> ratio(lhs);
	ratio /= rhs;
	return ratio;
}

// convert to std::string
template<size_t nbits, size_t es, typename bt>
inline std::string to_string(const areal<nbits,es,bt>& v) {
	std::stringstream s;
	if (v.iszero()) {
		s << " zero b";
		return s.str();
	}
	else if (v.isinf()) {
		s << " infinite b";
		return s.str();
	}
//	s << "(" << (v.sign() ? "-" : "+") << "," << v.scale() << "," << v.fraction() << ")";
	return s.str();
}

template<size_t nbits, size_t es, typename bt>
inline std::string to_binary(const areal<nbits, es, bt>& number, bool nibbleMarker = false) {
	std::stringstream ss;
	ss << 'b';
	for (int i = int(nbits - 1); i >= 0; --i) {
		ss << (number.at(unsigned(i)) ? '1' : '0');
		if (i > 0 && (i % 4) == 0 && nibbleMarker) ss << '\'';
	}
	return ss.str();
}

/// Magnitude of a scientific notation value (equivalent to turning the sign bit off).
template<size_t nbits, size_t es, typename bt>
areal<nbits,es> abs(const areal<nbits,es,bt>& v) {
	return areal<nbits,es>(false, v.scale(), v.fraction(), v.isZero());
}


}  // namespace sw::universal
