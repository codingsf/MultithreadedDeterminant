#include "Helpers.h"

#include <memory>
#include <fstream>
#include <iostream>
#include <random>

#include "Logger.h"

namespace Helpers
{
std::pair<double*, unsigned> ParseMatrixFromFile(const std::string& path)
{
	std::ifstream file(path, std::ifstream::in);
	if (!file)
	{
		Logger::Instance().Error("No such input file: " + path);
		return std::make_pair(nullptr, 0);
	}
	unsigned size = 0;
	file >> size;
	unsigned sizeSquared = size * size;
	std::unique_ptr<double[]> matrix(new double[sizeSquared]);
	for (unsigned i = 0; i < size; ++i)
	{
		for (unsigned j = 0; j < size; ++j)
		{
			if (file.bad())
			{
				Logger::Instance().Error("End of input file reached prematurely");
				file.close();
				return std::make_pair(nullptr, 0);
			}
			file >> matrix[i * size + j];
		}
		std::string linefeed;
		std::getline(file, linefeed);
	}

	file.close();
	return std::make_pair(matrix.release(), size);
}

void WriteResultToFile(const std::string& path, double result)
{
	std::ofstream file(path, std::ofstream::trunc);
	if (!file)
	{
		Logger::Instance().Error("Error while writing result to file: " +  path + ". Check for privileges.");
	}
	file << result << std::endl;
	file.close();
}
#ifdef USE_GMP
std::string PrintMpfNumber(const mpf_class& number)
{
	if (number == 0)
	{
		return "0";
	}
	mp_exp_t exponent = number.get_mpf_t()->_mp_exp;
	number.get_str(exponent);
	auto asText = number.get_str(exponent);
	return asText;
}

void WriteResultToFile(const std::string& path, const mpf_class& result)
{
	std::ofstream file(path, std::ofstream::trunc);
	if (!file)
	{
		Logger::Instance().Error("Error while writing result to file: " + path + ". Check for privileges.");
	}
	file << PrintMpfNumber(result) << std::endl;
	file.close();
}
#endif

double* GenerateRandomMatrix(unsigned size)
{
	auto sizeSquared = size * size;
	std::unique_ptr<double[]> matrix(new double[sizeSquared]);

	std::default_random_engine generator;
	std::uniform_real_distribution<> uniformDistribution(0.0, 1.0);
	for (unsigned i = 0; i < sizeSquared; i++)
	{
		matrix[i] = i * i * 1e-7; //uniformDistribution(generator);
	}
	return matrix.release();
}

}