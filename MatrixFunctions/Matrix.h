#pragma once
#define matrix float**

class Matrix
{
	int size = 0;
	matrix values = nullptr;
	static void del(matrix m, int size);
	static matrix add(matrix a, matrix b, int size);
	static matrix add(matrix a, matrix b, matrix c, int size);
	static matrix subtract(matrix a, matrix b, int size);
	static matrix multiply(matrix a, matrix b, int size);
	static matrix multiplyLarge(matrix a, matrix b, int size);
	static matrix inversion(matrix a, int size);
	static matrix divide(matrix a, matrix b, int size);
	static matrix getPart(matrix values, int size, int startRow = 0, int startColumn = 0);
public:
	Matrix();
	Matrix(int size, int maxRandom);
	~Matrix();
	void Print();
	static Matrix* Add(Matrix* a, Matrix* b);
	static Matrix* Subtract(Matrix* a, Matrix* b);
	static Matrix* Multiply(Matrix* a, Matrix* b);
	static Matrix* MultiplyLarge(Matrix* a, Matrix* b);
	static Matrix* Divide(Matrix* a, Matrix* b);
	static bool Equals(Matrix* a, Matrix* b);
};

