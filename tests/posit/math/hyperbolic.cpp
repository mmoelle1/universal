// function_hyperbolic.cpp: test suite runner for hyperbolic functions (sinh/cosh/tanh/atanh/acosh/asinh)
//
// Copyright (C) 2017-2021 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <universal/utility/directives.hpp>

// when you define POSIT_VERBOSE_OUTPUT the code will print intermediate results for selected arithmetic operations
//#define POSIT_VERBOSE_OUTPUT
#define POSIT_TRACE_SQRT

// use default number system library configuration
#include <universal/number/posit/posit.hpp>
#include <universal/verification/posit_math_test_suite.hpp>

// generate specific test case that you can trace with the trace conditions in posit.h
// for most bugs they are traceable with _trace_conversion and _trace_add
template<size_t nbits, size_t es, typename Ty>
void GenerateTestCaseSinh(Ty a) {
	Ty ref;
	sw::universal::posit<nbits, es> pa, pref, psinh;
	pa = a;
	ref = std::sinh(a);
	pref = ref;
	psinh = sw::universal::sinh(pa);
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << a << " -> sinh(" << a << ") = " << std::setw(nbits) << ref << std::endl;
	std::cout << pa.get() << " -> sinh( " << pa << ") = " << psinh.get() << " (reference: " << pref.get() << ")   " ;
	std::cout << (pref == psinh ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::setprecision(5);
}

template<size_t nbits, size_t es, typename Ty>
void GenerateTestCaseCosh(Ty a) {
	Ty ref;
	sw::universal::posit<nbits, es> pa, pref, pcosh;
	pa = a;
	ref = std::cosh(a);
	pref = ref;
	pcosh = sw::universal::cosh(pa);
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << a << " -> cosh(" << a << ") = " << std::setw(nbits) << ref << std::endl;
	std::cout << pa.get() << " -> cosh( " << pa << ") = " << pcosh.get() << " (reference: " << pref.get() << ")   ";
	std::cout << (pref == pcosh ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::setprecision(5);
}

template<size_t nbits, size_t es, typename Ty>
void GenerateTestCaseTanh(Ty a) {
	Ty ref;
	sw::universal::posit<nbits, es> pa, pref, ptanh;
	pa = a;
	ref = std::tanh(a);
	pref = ref;
	ptanh = sw::universal::tanh(pa);
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << a << " -> tanh(" << a << ") = " << std::setw(nbits) << ref << std::endl;
	std::cout << pa.get() << " -> tanh( " << pa << ") = " << ptanh.get() << " (reference: " << pref.get() << ")   ";
	std::cout << (pref == ptanh ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::setprecision(5);
}

template<size_t nbits, size_t es, typename Ty>
void GenerateTestCaseAsinh(Ty a) {
	Ty ref;
	sw::universal::posit<nbits, es> pa, pref, pasinh;
	pa = a;
	ref = std::asinh(a);
	pref = ref;
	pasinh = sw::universal::asinh(pa);
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << a << " -> asinh(" << a << ") = " << std::setw(nbits) << ref << std::endl;
	std::cout << pa.get() << " -> asinh( " << pa << ") = " << pasinh.get() << " (reference: " << pref.get() << ")   ";
	std::cout << (pref == pasinh ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::setprecision(5);
}

template<size_t nbits, size_t es, typename Ty>
void GenerateTestCaseAcosh(Ty a) {
	Ty ref;
	sw::universal::posit<nbits, es> pa, pref, pacosh;
	pa = a;
	ref = std::acosh(a);
	pref = ref;
	pacosh = sw::universal::acosh(pa);
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << a << " -> acosh(" << a << ") = " << std::setw(nbits) << ref << std::endl;
	std::cout << pa.get() << " -> acosh( " << pa << ") = " << pacosh.get() << " (reference: " << pref.get() << ")   ";
	std::cout << (pref == pacosh ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::setprecision(5);
}

template<size_t nbits, size_t es, typename Ty>
void GenerateTestCaseAtanh(Ty a) {
	Ty ref;
	sw::universal::posit<nbits, es> pa, pref, patanh;
	pa = a;
	ref = std::atanh(a);
	pref = ref;
	patanh = sw::universal::atanh(pa);
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << a << " -> atanh(" << a << ") = " << std::setw(nbits) << ref << std::endl;
	std::cout << pa.get() << " -> atanh( " << pa << ") = " << patanh.get() << " (reference: " << pref.get() << ")   ";
	std::cout << (pref == patanh ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::setprecision(5);
}


#define MANUAL_TESTING 1
#define STRESS_TESTING 0

const double pi = 3.14159265358979323846;

int main()
try {
	using namespace std;
	using namespace sw::universal;

	//bool bReportIndividualTestCases = true;
	int nrOfFailedTestCases = 0;

	std::string tag = "Addition failed: ";

#if MANUAL_TESTING
	// generate individual testcases to hand trace/debug
	GenerateTestCaseSinh<16, 1, double>(pi / 4.0);
	GenerateTestCaseCosh<16, 1, double>(pi / 4.0);
	GenerateTestCaseTanh<16, 1, double>(pi / 4.0);
	GenerateTestCaseAsinh<16, 1, double>(pi / 2.0);
	GenerateTestCaseAcosh<16, 1, double>(pi / 2.0);
	GenerateTestCaseAtanh<16, 1, double>(pi / 4.0);

	cout << endl;

	// manual exhaustive test
	nrOfFailedTestCases += ReportTestResult(VerifySinh<2, 0>(true), "posit<2,0>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<3, 0>(true), "posit<3,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<3, 1>(true), "posit<3,1>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<4, 0>(true), "posit<4,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<4, 1>(true), "posit<4,1>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<5, 0>(true), "posit<5,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<5, 1>(true), "posit<5,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<5, 2>(true), "posit<5,2>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 0>(true), "posit<8,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifyCosh<8, 0>(true), "posit<8,0>", "cosh");
	nrOfFailedTestCases += ReportTestResult(VerifyTanh<8, 0>(true), "posit<8,0>", "tanh");
	nrOfFailedTestCases += ReportTestResult(VerifyAtanh<8, 0>(true), "posit<8,0>", "atanh");
	nrOfFailedTestCases += ReportTestResult(VerifyAcosh<8, 0>(true), "posit<8,0>", "acosh");
	nrOfFailedTestCases += ReportTestResult(VerifyAsinh<8, 0>(true), "posit<8,0>", "asinh");
#else

	cout << "Posit hyperbolic sine/cosine/tangent function validation" << endl;

	nrOfFailedTestCases += ReportTestResult(VerifySinh<2, 0>(tag, bReportIndividualTestCases), "posit<2,0>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<3, 0>(tag, bReportIndividualTestCases), "posit<3,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<3, 1>(tag, bReportIndividualTestCases), "posit<3,1>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<4, 0>(tag, bReportIndividualTestCases), "posit<4,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<4, 1>(tag, bReportIndividualTestCases), "posit<4,1>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<5, 0>(tag, bReportIndividualTestCases), "posit<5,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<5, 1>(tag, bReportIndividualTestCases), "posit<5,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<5, 2>(tag, bReportIndividualTestCases), "posit<5,2>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<6, 0>(tag, bReportIndividualTestCases), "posit<6,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<6, 1>(tag, bReportIndividualTestCases), "posit<6,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<6, 2>(tag, bReportIndividualTestCases), "posit<6,2>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<6, 3>(tag, bReportIndividualTestCases), "posit<6,3>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<7, 0>(tag, bReportIndividualTestCases), "posit<7,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<7, 1>(tag, bReportIndividualTestCases), "posit<7,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<7, 2>(tag, bReportIndividualTestCases), "posit<7,2>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<7, 3>(tag, bReportIndividualTestCases), "posit<7,3>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<7, 4>(tag, bReportIndividualTestCases), "posit<7,4>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 0>(tag, bReportIndividualTestCases), "posit<8,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 1>(tag, bReportIndividualTestCases), "posit<8,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 2>(tag, bReportIndividualTestCases), "posit<8,2>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 3>(tag, bReportIndividualTestCases), "posit<8,3>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 4>(tag, bReportIndividualTestCases), "posit<8,4>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<8, 5>(tag, bReportIndividualTestCases), "posit<8,5>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 0>(tag, bReportIndividualTestCases), "posit<9,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 1>(tag, bReportIndividualTestCases), "posit<9,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 2>(tag, bReportIndividualTestCases), "posit<9,2>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 3>(tag, bReportIndividualTestCases), "posit<9,3>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 4>(tag, bReportIndividualTestCases), "posit<9,4>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 5>(tag, bReportIndividualTestCases), "posit<9,5>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<9, 6>(tag, bReportIndividualTestCases), "posit<9,6>", "sinh");
	
	nrOfFailedTestCases += ReportTestResult(VerifySinh<10, 0>(tag, bReportIndividualTestCases), "posit<10,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<10, 1>(tag, bReportIndividualTestCases), "posit<10,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<10, 2>(tag, bReportIndividualTestCases), "posit<10,2>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<10, 7>(tag, bReportIndividualTestCases), "posit<10,7>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<12, 0>(tag, bReportIndividualTestCases), "posit<12,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<12, 1>(tag, bReportIndividualTestCases), "posit<12,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<12, 2>(tag, bReportIndividualTestCases), "posit<12,2>", "sinh");

	nrOfFailedTestCases += ReportTestResult(VerifySinh<16, 0>(tag, bReportIndividualTestCases), "posit<16,0>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<16, 1>(tag, bReportIndividualTestCases), "posit<16,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<16, 2>(tag, bReportIndividualTestCases), "posit<16,2>", "sinh");


#if STRESS_TESTING
	// nbits=64 requires long double compiler support
	// nrOfFailedTestCases += ReportTestResult(VerifyThroughRandoms<64, 2>(tag, bReportIndividualTestCases, OPCODE_SQRT, 1000), "posit<64,2>", "sinh");


	nrOfFailedTestCases += ReportTestResult(VerifySinh<10, 1>(tag, bReportIndividualTestCases), "posit<10,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<12, 1>(tag, bReportIndividualTestCases), "posit<12,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<14, 1>(tag, bReportIndividualTestCases), "posit<14,1>", "sinh");
	nrOfFailedTestCases += ReportTestResult(VerifySinh<16, 1>(tag, bReportIndividualTestCases), "posit<16,1>", "sinh");
	
#endif  // STRESS_TESTING

#endif  // MANUAL_TESTING

	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::posit_arithmetic_exception& err) {
	std::cerr << "Uncaught posit arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::quire_exception& err) {
	std::cerr << "Uncaught quire exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::universal::posit_internal_exception& err) {
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
