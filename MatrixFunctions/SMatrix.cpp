#include "stdafx.h"
#include "SMatrix.h"


float* SMatrix::init(int size)
{
	return (float*)_aligned_malloc(size * sizeof(float), 16);
}

matrix SMatrix::init2(int size)
{
	return (matrix)_aligned_malloc(size * sizeof(float*), 16);
}

void SMatrix::del(matrix m, int size)
{
	_aligned_free(m);
}

matrix SMatrix::add(matrix a, matrix b, int size)
{
	matrix c = init2(size);
	for (int i = 0; i < size; i++){
		c[i] = init(size);
		__m128* pa = (__m128*)a[i];
		__m128* pb = (__m128*)b[i];
		__m128* pc = (__m128*)c[i];
		for (int j = 0; j < size >> 2; ++j){
			pc[j] = _mm_add_ps(pa[j], pb[j]);
		}
	}
	return c;
}

matrix SMatrix::subtract(matrix a, matrix b, int size)
{
	matrix c = init2(size);
	for (int i = 0; i < size; i++){
		c[i] = init(size);
		__m128* pa = (__m128*)a[i];
		__m128* pb = (__m128*)b[i];
		__m128* pc = (__m128*)c[i];
		for (int j = 0; j < size >> 2; ++j){
			pc[j] = _mm_sub_ps(pa[j], pb[j]);
		}
	}
	return c;
}

matrix SMatrix::multiply(matrix a, matrix b, int size)
{
	matrix c = init2(size);
	for (int i = 0; i < size; ++i){
		c[i] = init(size);
		for (int j = 0; j < size; ++j){
			c[i][j] = 0;
		}
	}
	for (int i = 0; i < size; ++i){
		__m128* pc = (__m128*)c[i];
		for (int j = 0; j < size; ++j){
			__m128* pb = (__m128*)b[j];
			__m128 pa = _mm_set1_ps(a[i][j]);
			for (int z = 0; z < size >> 2; ++z){
				pc[z] = _mm_add_ps(pc[z], _mm_mul_ps(pa, pb[z]));
			}
		}
	}
	return c;
}

matrix SMatrix::multiplyLarge(matrix a, matrix b, int size)
{
	if (size <= 512)
		return multiply(a, b, size);
	int h = size >> 1;
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
	matrix c12temp = add(t1, p5, h);
	matrix c12 = add(c12temp, p6, h);
	del(p6, h); del(t1, h); del(c12temp, h);
	matrix c21 = subtract(t2, p7, h);
	del(p7, h);
	matrix c22 = add(t2, p5, h);
	del(p5, h); del(t2, h);

	matrix m = init2(size);
	for (int i = 0; i < size; i++)
		m[i] = init(size);
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

matrix SMatrix::inversion(matrix m, int size)
{
	matrix a = init2(size);
	for (int i = 0; i < size; ++i)
	{
		a[i] = init(size);
		for (int j = 0; j < size; ++j)
		{
			a[i][j] = m[i][j];
		}
	}
	//создаем единичную матрицу
	matrix e = init2(size);
	for (int i = 0; i < size; i++)
	{
		e[i] = init(size);
		for (int j = 0; j < size; ++j)
		{
			e[i][j] = 0;
		}
		e[i][i] = 1.0f;
	}
	for (int k = 0; k < size >> 2; ++k)
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
		__m128 temp128 = _mm_set1_ps(temp);
		for (int j = 0; j < size; ++j)
		{
			__m128* pa = (__m128*)a[k];
			__m128* pe = (__m128*)e[k];
			pa[k] = _mm_div_ps(*pa, temp128);
			pe[k] = _mm_div_ps(*pe, temp128);
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

matrix SMatrix::divide(matrix a, matrix b, int size)
{
	matrix r = inversion(b, size);
	matrix res = multiplyLarge(a, r, size);
	del(r, size);
	return res;
}

matrix SMatrix::getPart(matrix values, int size, int startRow, int startColumn)
{
	matrix c = init2(size);
	for (int i = 0, i2 = startRow; i < size; i++, i2++) {
		c[i] = init(size);
		for (int j = 0, j2 = startColumn; j < size; j++, j2++) {
			c[i][j] = values[i2][j2];
		}
	}
	return c;
}

SMatrix* SMatrix::Create()
{
	return (SMatrix*)_aligned_malloc(sizeof(SMatrix*), 16);
}

SMatrix* SMatrix::Create(int size, int maxRandom)
{
	SMatrix* m = Create();
	m->size = size;
	m->values = init2(size);
	for (int i = 0; i < size; i++){
		m->values[i] = init(size);
		for (int j = 0; j < size; j++){
			m->values[i][j] = (float)(rand() % maxRandom) + 1;
		}
	}
	return m;
}


SMatrix::~SMatrix()
{
	del(values, size);
}

void SMatrix::Print()
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

SMatrix* SMatrix::Add(SMatrix* a, SMatrix* b)
{
	if (a->size != b->size)
	{
		printf("Sizes do not match");
		return nullptr;
	}
	SMatrix* m = Create();
	m->values = add(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

SMatrix* SMatrix::Subtract(SMatrix* a, SMatrix* b)
{
	if (a->size != b->size)
	{
		_tprintf(_T("Sizes do not match"));
		return nullptr;
	}
	SMatrix* m = Create();
	m->values = subtract(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

SMatrix* SMatrix::Multiply(SMatrix* a, SMatrix* b)
{
	if (a->size != b->size)
	{
		_tprintf(_T("Sizes do not match"));
		return nullptr;
	}
	SMatrix* m = Create();
	m->values = multiply(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

SMatrix* SMatrix::MultiplyLarge(SMatrix* a, SMatrix* b)
{
	if (a->size != b->size)
	{
		_tprintf(_T("Sizes do not match"));
		return nullptr;
	}
	SMatrix* m = Create();
	m->values = multiplyLarge(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

SMatrix* SMatrix::Divide(SMatrix* a, SMatrix* b)
{
	if (a->size != b->size)
	{
		_tprintf(_T("Sizes do not match"));
		return nullptr;
	}
	SMatrix* m = Create();
	m->values = divide(a->values, b->values, a->size);
	m->size = a->size;
	return m;
}

bool SMatrix::Equals(SMatrix* a, SMatrix* b)
{
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