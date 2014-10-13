#include "stdafx.h"
#include "Matrix.h"
#include <stdexcept>


//удаление матрицы из памяти
void Matrix::del(matrix m, int size)
{
	for (int i = 0; i < size; i++)
		delete[] m[i];
	delete[] m;
}

//сложение матриц
matrix Matrix::add(matrix a, matrix b, int size)
{
	matrix c = new float*[size];
	for (int i = 0; i < size; i++){
		c[i] = new float[size];
		for (int j = 0; j < size; j++){
			c[i][j] = a[i][j] + b[i][j];
		}
	}
	return c;
}

//сложение трех матриц (для винограда-штрассена)
matrix Matrix::add(matrix a, matrix b, matrix c, int size)
{
	matrix d = new float*[size];
	for (int i = 0; i < size; i++){
		d[i] = new float[size];
		for (int j = 0; j < size; j++){
			d[i][j] = a[i][j] + b[i][j] + c[i][j];
		}
	}
	return d;
}

//вычитание матриц
matrix Matrix::subtract(matrix a, matrix b, int size)
{
	matrix c = new float*[size];
	for (int i = 0; i < size; i++){
		c[i] = new float[size];
		for (int j = 0; j < size; j++){
			c[i][j] = a[i][j] - b[i][j];
		}
	}
	return c;
}

//умножение матриц (оптимизированный с тчоки зрения кеша алгоритм )
matrix Matrix::multiply(matrix a, matrix b, int size)
{
	matrix c = new float*[size];
	for (int i = 0; i < size; i++){
		c[i] = new float[size]();
		for (int j = 0; j < size; j++){
			for (int z = 0; z < size; z++)
				c[i][z] += a[i][j] * b[j][z];
		}
	}
	return c;
}

//умножение матриц (алгоритм винограда-штрассена)
matrix Matrix::multiplyLarge(matrix a, matrix b, int size)
{
	if (size <= 128)
		return multiply(a, b, size);
	int h = size / 2;
	matrix a11 = getPart(a, h, 0, 0);
	matrix a12 = getPart(a, h, 0, h);//top right corner
	matrix a21 = getPart(a, h, h, 0);//bottom left corner
	matrix a22 = getPart(a, h, h, h);//bottom right corner
	matrix b11 = getPart(b, h, 0, 0);//top left corner
	matrix b12 = getPart(b, h, 0, h);//top right corner
	matrix b21 = getPart(b, h, h, 0);//bottom left corner
	matrix b22 = getPart(b, h, h, h);//bottom right corner

	matrix s1 = add(a21, a22, h);
	matrix s2 = subtract(s1, a11, h);
	matrix s3 = subtract(a11, a21, h);
	del(a21, h);
	matrix s4 = subtract(a12, s2, h);
	matrix s5 = subtract(b12, b11, h);
	matrix s6 = subtract(b22, s5, h);
	matrix s7 = subtract(b22, b12, h);
	del(b12, h);
	matrix s8 = subtract(s6, b21, h);

	matrix p1 = multiplyLarge(s2, s6, h);
	del(s2, h);	del(s6, h);
	matrix p2 = multiplyLarge(a11, b11, h);
	del(a11, h); del(b11, h);
	matrix p3 = multiplyLarge(a12, b21, h);
	del(a12, h); del(b21, h);
	matrix p4 = multiplyLarge(s3, s7, h);
	del(s3, h); del(s7, h);
	matrix p5 = multiplyLarge(s1, s5, h);
	del(s1, h); del(s5, h);
	matrix p6 = multiplyLarge(s4, b22, h);
	del(b22, h); del(s4, h);
	matrix p7 = multiplyLarge(a22, s8, h);
	del(a22, h); del(s8, h);

	matrix t1 = add(p1, p2, h);
	del(p1, h);
	matrix t2 = add(t1, p4, h);
	del(p4, h);
	matrix c11 = add(p2, p3, h);
	del(p2, h); del(p3, h);
	matrix c12 = add(t1, p5, p6, h);
	del(p6, h); del(t1, h);
	matrix c21 = subtract(t2, p7, h);
	del(p7, h);
	matrix c22 = add(t2, p5, h);
	del(p5, h); del(t2, h);

	matrix m = new float*[size];
	for (int i = 0; i < size; i++)
		m[i] = new float[size];
	for (int i = 0; i < h; i++){
		for (int j = 0; j < h; j++){
			m[i][j] = c11[i][j];
			m[i][j + h] = c12[i][j];
			m[i + h][j] = c21[i][j];
			m[i + h][j + h] = c22[i][j];

		}
	}
	del(c11, h);
	del(c12, h);
	del(c21, h);
	del(c22, h);
	return m;
}

//нахождение обратной  матрицы
matrix Matrix::inversion(matrix m, int size)
{
	matrix a = new float*[size];
	for (int i = 0; i < size; i++)
	{
		a[i] = new float[size];
		for (int j = 0; j < size; j++)
		{
			a[i][j] = m[i][j];
		}
	}
	//создаем единичную матрицу
	float** e = new float*[size];
	for (int i = 0; i < size; i++)
	{
		e[i] = new float[size]();
		e[i][i] = 1.0f;
	}
	for (int k = 0; k < size; k++)
	{
		if (a[k][k] == 0)
		{
			bool changed = false;
			for (int i = k + 1; i < size; ++i)
			{
				if (a[i][k] == 0)
				{
					std::swap(a[k], a[i]);
					std::swap(e[k], e[i]);
					changed = true;
					break;
				}
			}

			if (!changed)
			{
				printf("Bad\n");
				del(e, size);
				return nullptr;
			}
		}

		float temp = (int)a[k][k];
		for (int j = 0; j < size; j++)
		{
			a[k][j] /= temp;
			e[k][j] /= temp;
		}
		//Генерим нолики в левом нижнем углу
		for (int i = k + 1; i < size; i++)
		{
			float mul = a[i][k];

			for (int j = 0; j < size; j++)
			{
				a[i][j] -= mul* a[k][j];
				e[i][j] -= mul* e[k][j];
			}
		}
	}
	for (int k = size - 1; k > 0; k--)
	{
		for (int i = k - 1; i + 1 > 0; i--)
		{
			float mul = a[i][k];

			for (int j = 0; j < size; j++)
			{
				a[i][j] -= mul* a[k][j];
				e[i][j] -= mul* e[k][j];
			}
		}
	}


	del(a, size);
	return e;
}

//деление матриц
matrix Matrix::divide(matrix a, matrix b, int size)
{
	matrix r = inversion(b, size);
	matrix res = multiplyLarge(a, r, size);
	del(r, size);
	return res;
}

//нахождение 1/4 матрицы
matrix Matrix::getPart(matrix values, int size, int startRow, int startColumn)
{
	matrix c = new float *[size];
	for (int i = 0, i2 = startRow; i < size; i++, i2++) {
		c[i] = new float[size];
		for (int j = 0, j2 = startColumn; j < size; j++, j2++) {
			c[i][j] = values[i2][j2];
		}
	}
	return c;
}

Matrix::Matrix()
{
}

//заполняет матрицу радномными числами
Matrix::Matrix(int size, int maxRandom)
{
	this->size = size;
	values = new float*[size];
	for (int i = 0; i < size; i++){
		values[i] = new float[size];
		for (int j = 0; j < size; j++){
			values[i][j] = (float)(rand() % maxRandom) + 1;
		}
	}
}


Matrix::~Matrix()
{
	del(values, size);
}

//вывод матрицы в консоль
void Matrix::Print()
{
	if (values == nullptr)
		return;
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			float f = values[i][j];
			printf("%f ", f);
		}
		printf("\n");
	}
	printf("***********\n");
}

Matrix* Matrix::Add(Matrix* a, Matrix* b)
{
	if (a->size != b->size)
	{
		printf("Sizes do not match");
		return nullptr;
	}
	Matrix* m = new Matrix();
	m->values = add(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

Matrix* Matrix::Subtract(Matrix* a, Matrix* b)
{
	if (a->size != b->size)
	{
		printf("Sizes do not match");
		return nullptr;
	}
	Matrix* m = new Matrix();
	m->values = subtract(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

Matrix* Matrix::Multiply(Matrix* a, Matrix* b)
{
	if (a->size != b->size)
	{
		printf("Sizes do not match");
		return nullptr;
	}
	Matrix* m = new Matrix();
	m->values = multiply(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

Matrix* Matrix::MultiplyLarge(Matrix* a, Matrix* b)
{
	if (a->size != b->size)
	{
		printf("Sizes do not match");
		return nullptr;
	}
	Matrix* m = new Matrix();
	m->values = multiplyLarge(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

Matrix* Matrix::Divide(Matrix* a, Matrix* b)
{
	if (a->size != b->size)
	{
		printf("Sizes do not match");
		return nullptr;
	}
	Matrix* m = new Matrix();
	m->values = divide(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

//сравние матриц
bool Matrix::Equals(Matrix* a, Matrix* b){
	if (a->size != b->size)
		return false;
	for (int i = 0; i < a->size; i++){
		for (int j = 0; j < a->size; j++){
			if (fabs(a->values[i][j] - b->values[i][j]) > 0.001)
			{
				printf("matrix 1: %f; matrix 2: %f [%d,%d]\n", a->values[i][j], b->values[i][j], i, j);
				return false;
			}
		}
	}
	return true;
}

