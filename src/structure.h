//包含数据结构的定义，每种数据类型的创建，插入和销毁，以及文件的处理

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//文字
typedef struct literal
{
	int location;//位置
	int num;//值
	bool state;//存在性
	struct literal* next;//后继
}Literal;

//字句
typedef struct clause
{
	int location;//位置
	int clauselen;//长度
	bool state;//存在性
	struct literal* sub;//包含的文字链表
	struct clause* next;//后继
}Clause;

//文字的符号
typedef struct sign
{
	int sgn;//符号
	int num;//值
	struct sign* next;//后继
}Sign;

//记录被删除的文字或字句
typedef struct memory
{
	int deLitralLocation;//文字的位置
	int deClauseLocation;//字句的位置
	struct memory* next;//后继
}Memory;

//创建字句
Clause* createClause()
{
	Clause* cla = (Clause*)malloc(sizeof(Clause));
	cla->clauselen = 0;
	cla->location = 0;
	cla->state = true;
	cla->sub = NULL;
	cla->next = NULL;
	return cla;
}

//创建文字
Literal* createLiteral()
{
	Literal* lit = (Literal*)malloc(sizeof(Literal));
	lit->location = 0;
	lit->num = 0;
	lit->state = true;
	lit->next = NULL;
	return lit;
}

//创建Sign
Sign* createSign()
{
	Sign* sig = (Sign*)malloc(sizeof(Sign));
	sig->num = 0;
	sig->sgn = -1;
	sig->next = NULL;
	return sig;
}

//创建Memory
Memory* createMemory()
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));
	mem->deClauseLocation = 0;
	mem->deLitralLocation = 0;
	mem->next = NULL;
	return mem;
}

//在链表尾插入字句
void insertClause(Clause* Current, int location)
{
	Clause* cla = (Clause*)malloc(sizeof(Clause));
	cla->location = location;
	cla->clauselen = 0;
	cla->state = true;
	cla->next = NULL;
	cla->sub = NULL;
	Current->next = cla;
}

//在链表尾插入文字
void insertLiteral(Literal* Current, int num, int location)
{
	Literal* lit = (Literal*)malloc(sizeof(Literal));
	lit->num = num;
	lit->location = location;
	lit->state = true;
	lit->next = NULL;
	Current->next = lit;
}

//在链表尾插入符号
void insertSign(Sign* Current, int num)
{
	Sign* sig = (Sign*)malloc(sizeof(Sign));
	Sign* temps = Current->next;
	Sign* temph = Current;
	sig->num = abs(num);
	if (num > 0) sig->sgn = 1;
	else sig->sgn = -1;
	sig->next = NULL;
	while (temps != NULL)
	{
		if (temps->num == abs(num))//优先修改已有的项，如果没有一致的再改为在末尾加入新项
		{
			temps->sgn = sig->sgn;
			free(sig);
			break;
		}
		temph = temph->next;
		temps = temps->next;
	}
	if (temps == NULL) temph->next = sig;
}

//在链表尾插入Mem
void insertMemory(Memory* Current, int deClauseLocation, int deLitralLocation)
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));
	mem->deClauseLocation = deClauseLocation;
	mem->deLitralLocation = deLitralLocation;
	mem->next = NULL;
	Current->next = mem;
}

//删除文字
void destroyLiteral(Literal* Current)
{
	Literal* templ = Current, * templn;
	while (templ != NULL)
	{
		templn = templ->next;
		free(templ);
		templ = templn;
	}
}

//删除字句
void destroyClause(Clause* Current)
{
	Clause* tempc = Current;
	Clause* tempcn;
	while (tempc != NULL)
	{
		destroyLiteral(tempc->sub);
		tempcn = tempc->next;
		free(tempc);
		tempc = tempcn;
	}
}

//删除符号
void destroySign(Sign* Current)
{
	Sign* temps = Current, * tempn;
	while (temps != NULL)
	{
		tempn = temps->next;
		free(temps);
		temps = tempn;
	}
}

//删除Mem
void destroyMemory(Memory* Current)
{
	Memory* tempm = Current, * tempn;
	while (tempm != NULL)
	{
		tempn = tempm->next;
		free(tempm);
		tempm = tempn;
	}
}

//把文字链表插入某个字句
void addsub(Clause* clauseCurrent, Literal* litralCurrent, int clauselen)
{
	clauseCurrent->sub = litralCurrent;
	clauseCurrent->clauselen = clauselen;
}

//将Sign链表排序，以便于输出文件的规范性
Sign* sortSign(Sign* Current)
{
	int len = 0, k = 0, w = 0;
	Sign* Temps = Current->next;
	while (Temps != NULL)
	{
		len++;
		Temps = Temps->next;
	}
	int num[20000], sgn[20000];
	Temps = Current->next;
	for (int t = 0; Temps != NULL; Temps = Temps->next, t++)
	{
		num[t] = Temps->num;
		sgn[t] = Temps->sgn;
	}
	for (int i = 0; i < len - 1; i++)
	{
		for (int j = i + 1; j < len; j++)
		{
			if (num[i] > num[j])
			{
				k = num[i];
				num[i] = num[j];
				num[j] = k;
				w = sgn[i];
				sgn[i] = sgn[j];
				sgn[j] = w;

			}
		}
	}
	Sign* Sorted = createSign();
	for (int i = 0; i < len; i++)
		insertSign(Sorted, num[i] * sgn[i]);
	return Sorted;
}

//将文件名从in存入out并修改后缀
void fileHandle(char* in, char* out)
{
	char res[10] = ".res";//后缀名
	int len = strlen(in);
	in[len - 1] = '\0';
	for (int i = 0; i < len; i++)
		out[i] = in[i];
	for (int i = len - 5; i < len - 1; i++)
		out[i] = res[i - len + 5];
}

//在.res中输出运算结果
void fileOut(FILE* out, Sign* Current, bool pass, clock_t time)
{
	if (pass == true)
	{
		fprintf(out, "s %d\nv ", pass);
		for (Sign* temps = Current->next; temps != NULL; temps = temps->next)
			fprintf(out, "%5d", temps->num * temps->sgn);
		fprintf(out, "\nt %5ld ms\n", time);
	}
	else
		fprintf(out, "s %d\nt %5ld ms\n", pass, time);
}

//从文件中提取基本数据
void fileExtract(FILE* in, int* clauseNum, int* literalNum)
{
	char cnfFile[200];
	do{
		fgets(cnfFile, 50, in);
	} while (cnfFile[0] != 'p');//找到p开头的那一行
	int count = 6;
	for (; cnfFile[count] != ' '; count++)
		(*literalNum) = (cnfFile[count] - '0') + (*literalNum) * 10;//读入文字数
	count++;
	for (; cnfFile[count] != '\n'; count++)
		(*clauseNum) = (cnfFile[count] - '0') + (*clauseNum) * 10;//读入子句数
}

//从文件中提取信息创建CNF链表
void createStructure(FILE* in, int clauseNum, Clause* Current, int *keynum)
{
	int i = 0, count = 0;
	int deClauseLocation = 0, deLitralLocation = 0;
	for (int j = 0; j < clauseNum; j++)
	{
		insertClause(Current, deClauseLocation + 1);
		Current = Current->next;
		Literal* head = createLiteral(), * templ = head;
		fscanf(in, "%d", &i);
		while (i != 0)
		{
			insertLiteral(templ, i, deLitralLocation + 1);
			templ = templ->next;
			deLitralLocation++;
			i = i > 0 ? i: -i;
			keynum[i - 1]++;
			count++;
			fscanf(in, "%d", &i);
		}
		deClauseLocation++;
		deLitralLocation = 0;
		addsub(Current, head, count);
		count = 0;
	}
}

//展示字句
void showSturcture(const Clause* head)
{
	Clause* tempc = head->next;
	while (tempc != NULL)
	{
		Literal* templ = tempc->sub->next;
		while (templ != NULL)
		{
			printf("%5d", templ->num);
			templ = templ->next;
		}
		tempc = tempc->next;
		printf("\n");
	}
}

//展示符号
void showSign(const Sign* Current, bool pass, clock_t time)
{
	if (pass == true)
	{
		for (Sign* temps = Current->next; temps != NULL; temps = temps->next)
			printf("%5d", temps->num * temps->sgn);
		printf("\nt %5ld ms\n", time);
	}
	else
		printf("t %5ld ms\n", time);
}