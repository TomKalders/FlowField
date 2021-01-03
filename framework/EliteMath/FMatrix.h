#ifndef FMATRIX_H
#define FMATRIX_H

class FMatrix
{
private:
	float* m_Data;
	int m_Rows, m_Columns;
	int m_Size;
public:
	FMatrix();
	FMatrix(int rows, int columns);
	virtual ~FMatrix();

	void Resize(int rows, int columns);

	float Get(int r, int c);
	void Set(int r, int c, float val);
	void SetAll(float value);
	void SetRowAll(int row,float value);

	void Add(int r, int c, float val);

	void Randomize(float min, float max);

	int GetNrOfRows() {
		return m_Rows;
	}
	int GetNrOfColumns() {
		return m_Columns;
	}
	void MatrixMultiply(FMatrix& op2, FMatrix& result);
	void ScalarMultiply(float scalar);
	void Copy(FMatrix& op);
	void Add(FMatrix& op);
	void Subtract(FMatrix& op);
	void Sigmoid();
	float Sum();
	float Dot(FMatrix& op2);
	float Max();
	float Max(int& r, int& c);
	float MaxOfRow(int r);
	
	void Print();
private:
	int rcToIndex(int r, int c);

	
};
#endif

