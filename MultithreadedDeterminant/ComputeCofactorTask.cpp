#include "ComputeCofactorTask.h"

#include "Helpers.h"
#include "Logger.h"

#include <mutex>

const unsigned ComputeCofactorTask::PRECISION = 128;
static std::mutex g_Mutex;

ComputeCofactorTask::ComputeCofactorTask(double* matrix, unsigned size,
	unsigned row, unsigned col, std::vector<mpf_class>& answerStorage)
	: m_Matrix(matrix)
	, m_MatrixSize(size)
	, m_Size(size - 1)
	, m_Row(row)
	, m_Col(col)
	, m_AnswerStorage(answerStorage)
	, m_Visited(m_Size)
{
}

int ComputeCofactorTask::ComputePermutationParity(const std::vector<int>& permutation)
{
	m_Visited.SetAll(false);

	int sign = 1;
	for (auto i = 0u; i < m_Size; ++i)
	{
		if (!m_Visited[i])
		{
			int next = i;
			int cycleLength = 0;
			while (!m_Visited[next])
			{
				++cycleLength;
				m_Visited[next] = true;
				next = permutation[next];
			}
			if (cycleLength % 2 == 0)
			{
				sign = -sign;
			}
		}
	}
	return sign;
}

double ComputeCofactorTask::GetElementAtMinor(unsigned i, unsigned j)
{
	if (i >= m_Row) ++i;
	if (j >= m_Col) ++j;
	return m_Matrix[i * m_MatrixSize + j];
}

void ComputeCofactorTask::Solve()
{
	using namespace std;
	std::vector<int> permutation;
	permutation.reserve(m_Size);
	for (size_t i = 0; i < m_Size; i++)
	{
		permutation.push_back(i);
	}
	mpf_class result(0, PRECISION);
	mpf_class permutationProduct(1, PRECISION);
	do
	{
		int permutationParity = ComputePermutationParity(permutation);
		permutationProduct = 1;
		for (auto i = 0u; i < m_Size; ++i)
		{
			permutationProduct *= GetElementAtMinor(i, permutation[i]);
		}
		result += permutationParity * permutationProduct;
	} while (std::next_permutation(permutation.begin(), permutation.end()));
	// m_Row & m_Col are unsigned so we need a cast to get the actual sign
	double cofactorSign = static_cast<int>(((m_Row + m_Col) % 2) * (-2) + 1);

	result = cofactorSign * m_Matrix[m_Row * m_MatrixSize + m_Col] * result;
	{
		std::lock_guard<std::mutex> lock(g_Mutex);
		m_AnswerStorage.push_back(result);
	}
}