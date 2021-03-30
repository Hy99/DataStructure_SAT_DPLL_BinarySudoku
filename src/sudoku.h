//包含数独的函数

#include <stdlib.h>
#include <stdio.h> 
#include "solveDPLL.h"

//根据数独棋盘初始状态
void insertSudoku(FILE* in, FILE* out)
{
	int num[36];
	int i = 0;
	do
	{
		fscanf(in, "%d", num + i);
		i++;
	} while (num[i - 1] != 0);
	i = 0;
	while (num[i] != 0) fprintf(out, "%d 0\n", num[i]);
}

//约束一：连续三个数不能相同
void limit1(FILE* out)
{
	int num;
	for (int i = 1; i <= 6; i++) {
		for (int j = 1; j <= 4; j++) {
			num = 10 * i + j;//行
			fprintf(out, "%d %d %d 0\n", num, num + 1, num + 2);
			fprintf(out, "%d %d %d 0\n", -num, -num - 1, -num - 2);
			num = 10 * j + i;//列
			fprintf(out, "%d %d %d 0\n", num, num + 10, num + 20);
			fprintf(out, "%d %d %d 0\n", -num, -num - 10, -num - 20);
		}
	}
}

//约束二：同行列01个数相等
void limit2(FILE* out)
{
	int num[4];
	for (int n = 1; n <= 6; n++)
	{
		for (int i = 1; i <= 3; i++)
		{
			for (int j = i + 1; j <= 4; j++)
			{
				for (int k = j + 1; k <= 5; k++)
				{
					for (int l = k + 1; l <= 6; l++)
					{
						num[0] = n * 10 + i;//行
						num[1] = n * 10 + j;
						num[2] = n * 10 + k;
						num[3] = n * 10 + l;
						fprintf(out, "%d %d %d %d 0\n", num[0], num[1], num[2], num[3]);
						fprintf(out, "%d %d %d %d 0\n", -num[0], -num[1], -num[2], -num[3]);
						num[0] = i * 10 + n;//列
						num[1] = j * 10 + n;
						num[2] = k * 10 + n;
						num[3] = l * 10 + n;
						fprintf(out, "%d %d %d %d 0\n", num[0], num[1], num[2], num[3]);
						fprintf(out, "%d %d %d %d 0\n", -num[0], -num[1], -num[2], -num[3]);
					}
				}
			}
		}
	}
}

//约束三：不能有相同的行和列
void limit3(FILE* out)
{
	int num0[2];
	int num1[2];
	int num2[4];
	for (int i = 1; i <= 6; i++)//行或列
	{
		for (int j = i + 1; j <= 6 ; j++)//不同的另一行或列
		{
			num0[0] = 100 + 10 * i + j;
			num0[1] = 200 + 10 * i + j;
			for (int k = 1; k <= 6; k++)//变元当前列或行
			{
				num1[0] = num0[0] * 10 + k;
				num1[1] = num0[1] * 10 + k;
				num2[0] = num1[1] * 10;
				num2[1] = num1[1] * 10 + 1;
				num2[2] = num1[0] * 10;
				num2[3] = num1[0] * 10 + 1;

				fprintf(out, "%d %d 0\n", i * 10 + k, -num2[1]);
				fprintf(out, "%d %d 0\n", j * 10 + k, -num2[1]);
				fprintf(out, "%d %d %d 0\n", -i * 10 - k, -j * 10 - k, num2[1]);
				fprintf(out, "%d %d 0\n", -i * 10 - k, -num2[0]);
				fprintf(out, "%d %d 0\n", -j * 10 - k, -num2[0]);
				fprintf(out, "%d %d %d 0\n", i * 10 + k, j * 10 + k, num2[0]);
				fprintf(out, "%d %d 0\n", -num2[0], num1[1]);
				fprintf(out, "%d %d 0\n", -num2[1], num1[1]);
				fprintf(out, "%d %d %d 0\n", num2[0], num2[1], -num1[1]);

				fprintf(out, "%d %d 0\n", k * 10 + i, -num2[3]);
				fprintf(out, "%d %d 0\n", k * 10 + j, -num2[3]);
				fprintf(out, "%d %d %d 0\n", -k * 10 - i, -k * 10 - j, num2[3]);
				fprintf(out, "%d %d 0\n", -k * 10 - i, -num2[2]);
				fprintf(out, "%d %d 0\n", -k * 10 - j, -num2[2]);
				fprintf(out, "%d %d %d 0\n", k * 10 + i, k * 10 + j, num2[2]);
				fprintf(out, "%d %d 0\n", -num2[2], num1[0]);
				fprintf(out, "%d %d 0\n", -num2[3], num1[0]);
				fprintf(out, "%d %d %d 0\n", num2[2], num2[3], -num1[0]);

				fprintf(out, "%d %d 0\n", num0[0], num1[0]);
				fprintf(out, "%d %d 0\n", num0[1], num1[1]);
			}
			fprintf(out, "%d %d %d %d %d %d %d 0\n", -num0[0], -num0[0] * 10 - 1, -num0[0] * 10 - 2, -num0[0] * 10 - 3, -num0[0] * 10 - 4, -num0[0] * 10 - 5, -num0[0] * 10 - 6);
			fprintf(out, "%d %d %d %d %d %d %d 0\n", -num0[1], -num0[1] * 10 - 1, -num0[1] * 10 - 2, -num0[1] * 10 - 3, -num0[1] * 10 - 4, -num0[1] * 10 - 5, -num0[1] * 10 - 6);
		}
	}
}

//展示棋盘
void showSudoku(Sign* Current, bool pass)
{
	if (pass) {
		Sign* temps = Current->next;
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				if (temps->sgn == 1)
					printf("1 ");
				else if (temps->sgn == -1)
					printf("0 ");
				temps = temps->next;
			}
			printf("\n");
		}
	}
}