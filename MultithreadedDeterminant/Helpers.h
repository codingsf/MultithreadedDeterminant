#pragma once
#include <string>
#ifdef USE_GMP
#include <gmpxx.h>
#endif

namespace Helpers
{

	std::pair<double*, unsigned> ParseMatrixFromFile(const std::string& path);
	void WriteResultToFile(const std::string& path, double result);
#ifdef USE_GMP
	std::string PrintMpfNumber(const mpf_class& number);
	void WriteResultToFile(const std::string& path, const mpf_class& result);
#endif
	double* GenerateRandomMatrix(unsigned size);

}