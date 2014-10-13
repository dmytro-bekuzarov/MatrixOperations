#include "stdafx.h"
#include "Matrix.h"
#include "StopWatch.h"

int _tmain(int argc, _TCHAR* argv[])
{
	StopWatch timer;
	for (int size = 32; size <= 2048; size *= 2){
		printf("\n==========\nSize: %d\n",size);
		Matrix m1 (size, 10);
		Matrix m2(size, 10);

		timer.Start();
		Matrix* m3 = Matrix::Add(&m1, &m2);
		timer.Stop();
		printf("Addition done. Time: %f milliseconds\n", timer.GetDuration());

		timer.Start();
		Matrix* m4 = Matrix::Subtract(m3, &m2);
		timer.Stop();
		printf("Subtraction done. Time: %f milliseconds\n", timer.GetDuration());

		printf("Matrix 4 is equal to matrix 1: %s\n", Matrix::Equals(m4, &m1) ? "true" : "false");

		timer.Start();
		Matrix* m5 = Matrix::Multiply(&m1, &m2);
		timer.Stop();
		printf("Multiplication done (method 1). Time: %f milliseconds\n", timer.GetDuration());

		timer.Start();
		Matrix* m6 = Matrix::MultiplyLarge(&m1, &m2);
		timer.Stop();
		printf("Multiplication done (method 2). Time: %f milliseconds\n", timer.GetDuration());

		printf("Matrix 3 is equal to matrix 4: %s\n", Matrix::Equals(m5, m6) ? "true" : "false");


		timer.Start();
		Matrix* m7 = Matrix::Divide(m5, &m1);
		timer.Stop();
		printf("Division done. Time: %f milliseconds\n", timer.GetDuration());

		printf("Matrix 2 is equal to matrix 5: %s\n", Matrix::Equals(&m2, m7) ? "true" : "false");
	}
	printf("\nAll done. Press any key.\n");
	getchar();
	return 0;
}

