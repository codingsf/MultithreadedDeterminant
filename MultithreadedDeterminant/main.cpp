#include <iostream>
#include <chrono>


#include "ComputeCofactorTask.h"
#include "ArgumentParser.h"
#include "Logger.h"
#include "Helpers.h"

int main(int argc, char** argv)
{
	using namespace std;

	vector<ArgumentDefinition> validArguments =
	{
		{ "n", true, "\\d+" },
		{ "i", true, "" },
		{ "o", true, "" },
		{ "t", true, "\\d+" },
		{ "q", false, "" }
	};
	ArgumentParser parser(validArguments);
	auto arguments = parser.ParseArgs(argc, argv);

	if (parser.HasError())
	{
		return -1;
	}
	if (arguments.find("n") != arguments.end() && arguments.find("i") != arguments.end())
	{
		cerr << "Arguments -i and -n cannot be used together." << endl;
		return -1;
	}
	Logger logger(arguments.find("q") != arguments.end());

	unique_ptr<double[]> matrix;
	unsigned size;
	if (arguments.find("n") != arguments.end())
	{
		size = stoi(arguments.find("n")->second);
		matrix.reset(Helpers::GenerateRandomMatrix(size));
	}
	else if (arguments.find("i") != arguments.end())
	{
		auto results = Helpers::ParseMatrixFromFile(arguments.find("i")->second);
		if (results.second == 0)
		{
			return -1;
		}
		matrix.reset(results.first);
		size = results.second;
	}
	else
	{
		cerr << "At least one of arguments n and i must be supplied" << endl;
		return -1;
	}

	unsigned threadCount = 1;
	if (arguments.find("t") != arguments.end())
	{
		threadCount = std::stoi(arguments.find("t")->second);
	}

	ThreadPool pool(threadCount);
	std::vector<mpf_class> answerStorage;
	answerStorage.reserve(size);
	for (auto i = 0u; i < size; ++i)
	{
		pool.AddTask(new ComputeCofactorTask(matrix.get(), size, 0, i, answerStorage));
	}

	auto beforeComputation = chrono::high_resolution_clock::now();

	pool.FinishWork();
	mpf_class det(0, ComputeCofactorTask::PRECISION);
	for (auto& detMinor : answerStorage)
	{
		det += detMinor;
	}

	auto timeDistance = chrono::high_resolution_clock::now() - beforeComputation;
	auto totalRunningTime = chrono::duration_cast<chrono::milliseconds>(timeDistance).count();

	Logger::Instance().Info("Final Result: ");
	Logger::Instance().ImportantInfo(Helpers::PrintMpfNumber(det));;

	Logger::Instance().ImportantInfo("Total Running Time: " + to_string(totalRunningTime));

	if (arguments.find("o") != arguments.end())
	{
		Helpers::WriteResultToFile(arguments["o"], det);
	}
}