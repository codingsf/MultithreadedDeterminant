#pragma once
#include <vector>
#include <string>
#include "DynamicBitset.h"
#include "ThreadPool.h"

#include <gmpxx.h>

class ComputeCofactorTask : public Task
{
public:
	ComputeCofactorTask(double* matrix, unsigned size,
		unsigned row, unsigned col, mpf_class& globalResult);

	virtual void Solve() override;
	static const unsigned PRECISION;
private:
	int ComputePermutationParity(const std::vector<int>& permutation);
	double GetElementAtMinor(unsigned i, unsigned j);


	double* m_Matrix;
	unsigned m_MatrixSize;
	unsigned m_Size;
	unsigned m_Row;
	unsigned m_Col;

	mpf_class& m_GlobalResult;

	DynamicBitset m_Visited;
};