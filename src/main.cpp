#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "sudoku.h"


int main()
{
	bool pass;//可满足性
	char order;
	int num = 0;
	printf("-------------------------------------\n");
	printf("0 exit\t1 DPLL\t2 sudoku\n");
	printf("-------------------------------------\n");
	printf("please input order:");
	order = getchar();
	getchar();
	while (order != '0') 
	{
		int key[20000] = { 0 };
		for (int i = 0; i < 20000; i++) key[i] = i + 1;
		int keynum[20000] = { 0 };
		if (order == '1')
		{
			char* in = (char*)malloc(300 * sizeof(char));
			char* out = (char*)malloc(300 * sizeof(char));
			printf("what file do you what to deal with?\n");
			fgets(in, 200, stdin);
			fileHandle(in, out);
			FILE* PutInFile = fopen(in, "r");
			if (PutInFile == NULL)
				printf("This file does not exsist！\n");
			else
			{
				int clauseNum = 0, literalNum = 0;
				fileExtract(PutInFile, &clauseNum, &literalNum);

				Clause* clauseList = createClause();
				createStructure(PutInFile, clauseNum, clauseList, keynum);//创建cnf结构
				fclose(PutInFile);
				free(in);

				while (keynum[num] != 0) num++;//计算文字数
				for (int i = num; i < 20000; i++) {
					key[i] = 0;//多余的置零
				}
				quickSort(key, keynum, 0, num - 1);

				showSturcture(clauseList);//展示文件内容

				Sign* signList = createSign();
				Sign* sortedSign;
				CreateVspace(clauseList);
				clock_t t1 = clock();//计时开始
				pass = DPLLsolving(clauseList, signList, key, keynum, num);
				clock_t t2 = clock();//计时结束
				if (pass)
					printf("this is a satisfiable example!\n");
				else
					printf("this is an unsatisfiable example!\n");
				sortedSign = sortSign(signList);//记录结果
				destroyClause(clauseList);
				destroySign(signList);

				showSign(sortedSign, pass, t2 - t1);//屏幕输出

				FILE* PutOutFile = fopen(out, "w");
				fileOut(PutOutFile, sortedSign, pass, t2 - t1);//文件输出
				fclose(PutOutFile);
				free(out);
				destroySign(sortedSign);
			}
		}
		else if (order == '2') 
		{
			char* in = (char*)malloc(300 * sizeof(char));
			char* out = (char*)malloc(300 * sizeof(char));
			printf("what sudoku do you what to deal with?\n");
			fgets(in, 200, stdin);
			int len = strlen(in);
			in[len - 1] = '\0';
			FILE* PutInFile = fopen(in, "r");
			if (PutInFile == NULL)
				printf("This file does not exsist！\n");
			else
			{
				FILE* CNFfile;
				char sudokufile[50] = "E:\\\\SAT\\sudoku.cnf";
				sudokufile[18] = '\0';
				if ((CNFfile = fopen(sudokufile, "w")) == NULL) 
					printf("OPEN ERROR\n");
				else 
				{
					int x = 0;
					int num[36] = { 0 };
					do {
						fscanf(PutInFile, "%d", num + x);
						x++;
					} while (num[x - 1] != 0);
					fprintf(CNFfile, "p cnf 606 %d\n", 2286 + x - 1);
					x = 0;
					while (num[x] != 0) {
						fprintf(CNFfile, "%d 0\n", num[x]);
						x++;
					}
					limit1(CNFfile);
					limit2(CNFfile);
					limit3(CNFfile);
					
					fclose(PutInFile);
					fclose(CNFfile);
				}

				fileHandle(sudokufile, out);
				FILE* PutInFile = fopen("E:\\\\SAT\\sudoku.cnf", "r");
				if (PutInFile == NULL) printf("ERROR");
				else {
					int clauseNum = 0, literalNum = 0;
					fileExtract(PutInFile, &clauseNum, &literalNum);

					Clause* clauseList = createClause();
					createStructure(PutInFile, clauseNum, clauseList, keynum);//创建cnf结构
					fclose(PutInFile);
					while (keynum[num] != 0) num++;//计算文字数
					for (int i = num; i < 20000; i++) key[i] = 0;//多余的置零
					quickSort(key, keynum, 0, num - 1);//排序，找众数

					Sign* signList = createSign();
					Sign* sortedSign;
					CreateVspace(clauseList);
					clock_t t1 = clock();//计时开始
					pass = DPLLsolving(clauseList, signList, key, keynum, num);
					clock_t t2 = clock();//计时结束
					if (pass)
						printf("this is a satisfiable example!\n");
					else
						printf("this is an unsatisfiable example!\n");
					sortedSign = sortSign(signList);//记录结果
					destroyClause(clauseList);
					destroySign(signList);

					showSudoku(sortedSign, pass);

					FILE* PutOutFile = fopen(out, "w");
					fileOut(PutOutFile, sortedSign, pass, t2 - t1);//文件输出
					fclose(PutOutFile);
					free(out);
					destroySign(sortedSign);
				}
			}
		}
		printf("-------------------------------------\n");
		printf("0 exit\t1 DPLL\t2 sudoku\n");
		printf("-------------------------------------\n");
		printf("please input order:");
		order = getchar();
		getchar();
	}
	return 0;
}
