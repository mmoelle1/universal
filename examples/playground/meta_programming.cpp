// meta_programming.cpp: playground to experiment with meta programming techniques to generalize functions and algorithms
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include "common.hpp"
#include <complex>

// when you define POSIT_VERBOSE_OUTPUT executing an ADD the code will print intermediate results
//#define POSIT_VERBOSE_OUTPUT
#define POSIT_TRACE_CONVERSION
// enable posit arithmetic exceptions
#define POSIT_THROW_ARITHMETIC_EXCEPTION 1
#include <universal/posit/posit>

template<typename T>
struct hasSerialize {
	typedef char yes[1];
	typedef char no[2];

	// helper to determine if serialize is a function
	template<typename T, T function>
	struct reallyHas;

	template<typename T> static yes& test(reallyHas<std::string(T::*)(), &T::serialize>* /*unused*/) {}
	template<typename T> static yes& test(reallyHas<std::string(T::*)() const, &T::serialize>* /*unused*/) {}

	template<typename T> static no& test(...) {}

	// constant used as return value for the test
	static const bool value = sizeof(test<T>(0)) == sizeof(yes);
};

struct A {};

std::string to_string(const A& a) {
	return "I am an A";
}

struct B {
	std::string serialize() const {
		return "I am a B";
	}
};

struct C {
	std::string serialize;
};

std::string to_string(const C& c) {
	return "I am a C";
}

namespace sw {
	// typeless struct, will always fail substitution
	template<bool b, typename T = void>
	struct enable_if {};

	// specialization for type T
	template<typename T>
	struct enable_if<true, T> {
		using type = T;
	};

	template<typename T>
	typename enable_if<hasSerialize<T>::value, std::string>::type serialize(const T& obj) {
		return obj.serialize();
	}
	template<typename T>
	typename enable_if<!hasSerialize<T>::value, std::string>::type serialize(const T& obj) {
		return to_string(obj);
	}
}

int main(int argc, char** argv)
try {
	using namespace std;
	bool bSuccess = true;

	A a;
	B b;
	C c;

	/* goal
	cout << serialize(a) << endl;
	cout << serialize(b) << endl;
	cout << serialize(c) << endl;
	*/

	cout << hasSerialize<A>::value << endl;
	cout << hasSerialize<B>::value << endl;
	cout << hasSerialize<C>::value << endl;

	// pedantic
	sw::enable_if<true, int>::type t1; // type t1 is an int
	sw::enable_if<hasSerialize<B>::value, int>::type t2; // t2 is an int
	// enable_if<false, int>::type t3;  doesn't compile as enable_if<false, ...> doesn't have a type type
	// enable_if<hasSerialize<A>::value, int>::type t4; doesn't compile as enable_if<false, ...> doesn't have a type type

	// with enable_if we have the indirection to dispatch the right function: serialize or to_string
	cout << sw::serialize(a) << endl;
	cout << sw::serialize(b) << endl;
	cout << sw::serialize(c) << endl;

	return (bSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const posit_arithmetic_exception& err) {
	std::cerr << "Uncaught posit arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const quire_exception& err) {
	std::cerr << "Uncaught quire exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const posit_internal_exception& err) {
	std::cerr << "Uncaught posit internal exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::runtime_error& err) {
	std::cerr << "Uncaught runtime exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}
