#pragma once
#include <stdexcept>
#define matrix float**

class SMatrix
{
	int size = 0;
	static float* init(int size);
	static float** init2(int size);
	static void del(matrix m, int size);
	static matrix add(matrix a, matrix b, int size);
	static matrix subtract(matrix a, matrix b, int size);
	static matrix multiply(matrix a, matrix b, int size);
	static matrix multiplyLarge(matrix a, matrix b, int size);
	static matrix inversion(matrix m, int size);
	static matrix divide(matrix a, matrix b, int size);
	static matrix getPart(matrix values, int size, int startRow = 0, int startColumn = 0);
public:
	matrix values = nullptr;
	static SMatrix* Create();
	static SMatrix* Create(int size, int maxRandom);
	~SMatrix();
	void Print();
	static SMatrix* Add(SMatrix* a, SMatrix* b);
	static SMatrix* Subtract(SMatrix* a, SMatrix* b);
	static SMatrix* Multiply(SMatrix* a, SMatrix* b);
	static SMatrix* MultiplyLarge(SMatrix* a, SMatrix* b);
	static SMatrix* Divide(SMatrix* a, SMatrix* b);
	static bool Equals(SMatrix* a, SMatrix* b);
};

