#include "stdafx.h"
#include "FMatrix.h"


#include <random>

FMatrix::FMatrix()
	:m_Data(nullptr)
{

}

FMatrix::FMatrix(int rows, int columns) :
	m_Rows(rows),
	m_Columns(columns),
	m_Data(new float[rows*columns]),
	m_Size(rows*columns)
{

}

FMatrix::~FMatrix() {
	
	SAFE_DELETE(m_Data);
}

void FMatrix::Resize(int nrOfRows, int nrOfColumns)
{
	m_Rows = nrOfRows;
	m_Columns = nrOfColumns;
	m_Size = m_Rows * m_Columns;
	m_Data = new float[m_Size];
}

void FMatrix::Set(int row, int column, float value)
{
	int index = rcToIndex(row, column);
	if (index > -1 && index < m_Size) {
		m_Data[index] = value;
	}
	else {
		printf("Wrong index! [%d, %d]\n", row, column);
	}
}

void FMatrix::Add(int row, int column, float toAdd)
{
	int index = rcToIndex(row, column);
	if (index > -1 && index < m_Size) {
		m_Data[index] += toAdd;
	}
	else {
		printf("Wrong index! [%d, %d]\n", row, column);
	}
}

void FMatrix::SetAll(float value) {
	for (int i = 0; i < m_Size; ++i)
	{
		m_Data[i] = value;
	}
}

void FMatrix::SetRowAll(int r, float value) {
	for (int c = 0; c < m_Columns; ++c) {
		Set(r, c, value);
	}
}

void FMatrix::Copy(FMatrix& other) {
	int maxRows = min(GetNrOfRows(), other.GetNrOfRows());
	int maxColumns = min(GetNrOfColumns(), other.GetNrOfColumns());

	for (int c_row = 0; c_row < maxRows; ++c_row) {
		for (int c_column = 0; c_column < maxColumns; ++c_column) {
			float oVal = other.Get(c_row, c_column);
			Set(c_row, c_column, oVal);
		}
	}
}

void FMatrix::Add(FMatrix& other)
{
	int maxRows = min(GetNrOfRows(), other.GetNrOfRows());
	int maxColumns = min(GetNrOfColumns(), other.GetNrOfColumns());

	for (int c_row = 0; c_row < maxRows; ++c_row) {
		for (int c_column = 0; c_column < maxColumns; ++c_column) {
			float oVal = other.Get(c_row, c_column);
			float thisVal = Get(c_row, c_column);
			Set(c_row, c_column, thisVal + oVal);
		}
	}
}

void FMatrix::Subtract(FMatrix& other)
{
	int maxRows = min(GetNrOfRows(), other.GetNrOfRows());
	int maxColumns = min(GetNrOfColumns(), other.GetNrOfColumns());

	for (int c_row = 0; c_row < maxRows; ++c_row) {
		for (int c_column = 0; c_column < maxColumns; ++c_column) {
			float oVal = other.Get(c_row, c_column);
			float thisVal = Get(c_row, c_column);
			Set(c_row, c_column, thisVal -oVal);
		}
	}
}

float FMatrix::Get(int row, int column) {
	int index = rcToIndex(row, column);
	if (index > -1 && index < m_Size) {
		return m_Data[index];
	}
	else {
		return -1;
	}
}

void FMatrix::Randomize(float min, float max)
{
	for (int i = 0; i < m_Size; ++i)
	{
		float r = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		m_Data[i] = r;
	}
}

int FMatrix::rcToIndex(int r, int c)
{
	return c * m_Rows + r;
}

void FMatrix::MatrixMultiply(FMatrix& op2, FMatrix& result) {
	int maxRows = min(GetNrOfRows(), result.GetNrOfRows());
	int maxColumns = min(op2.GetNrOfColumns(), result.GetNrOfColumns());

	for (int c_row = 0; c_row < maxRows; ++c_row) {
		for (int c_column = 0; c_column < maxColumns; ++c_column) {
			float sum = 0;
			for (int index = 0; index < GetNrOfColumns(); ++index) {
				sum += Get(c_row, index) * op2.Get(index, c_column);
			}
			result.Set(c_row, c_column, sum);
		}
	}
}

void FMatrix::ScalarMultiply(float scalar)
{
	for (int i = 0; i < m_Size; ++i) {
		m_Data[i] *= scalar;
	}
}

void FMatrix::Sigmoid() {
	for (int i = 0; i < m_Size; ++i)
	{
		float val = m_Data[i];
		m_Data[i] = 1 / (1 + exp(-val));
	}
}

float FMatrix::Sum() {
	float sum = 0;
	for (int i = 0; i < m_Size; ++i)
	{
		sum += m_Data[i];
	}
	return sum;
}

float FMatrix::Dot(FMatrix& op2) {
	int mR = min(GetNrOfRows(), op2.GetNrOfRows());
	int mC = min(GetNrOfColumns(), op2.GetNrOfColumns());

	float dot = 0;
	for (int c_row = 0; c_row < mR; ++c_row) {
		for (int c_column = 0; c_column < mC; ++c_column) {
			float v1 = Get(c_row, c_column);
			float v2 = Get(c_row, c_column);
			dot += v1 * v2;
		}
	}
	return dot;
}

float FMatrix::Max()
{
	float max = -FLT_MAX;
	for (int c_row = 0; c_row < m_Rows; ++c_row) {
		for (int c_column = 0; c_column < m_Columns; ++c_column) {
			float value = Get(c_row, c_column);
			if (value > max) {
				max = value;
			}

		}
	}
	return max;
}

float FMatrix::Max(int& r, int& c)
{
	float max = -FLT_MAX;
	for (int c_row = 0; c_row < m_Rows; ++c_row) {
		for (int c_column = 0; c_column < m_Columns; ++c_column) {
			float value = Get(c_row, c_column);
			if (value > max) {
				max = value;
				r = c_row;
				c = c_column;
			}
			
		}
	}
	return max;
}

float FMatrix::MaxOfRow(int r) {
	float max = -FLT_MAX;
	for (int c_column = 0; c_column < m_Columns; ++c_column) {
		float value = Get(r, c_column);
		if (value > max) {
			max = value;
			
		}
	}
	return max;
}

void FMatrix::Print() {
	for (int c_row = 0; c_row < m_Rows; ++c_row) {
		for (int c_column = 0; c_column < m_Columns; ++c_column) {
			float value = Get(c_row, c_column);
			printf("%.3f\t", value);
		}
		printf("\n");
	}
}