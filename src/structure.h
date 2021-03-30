//�������ݽṹ�Ķ��壬ÿ���������͵Ĵ�������������٣��Լ��ļ��Ĵ���

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//����
typedef struct literal
{
	int location;//λ��
	int num;//ֵ
	bool state;//������
	struct literal* next;//���
}Literal;

//�־�
typedef struct clause
{
	int location;//λ��
	int clauselen;//����
	bool state;//������
	struct literal* sub;//��������������
	struct clause* next;//���
}Clause;

//���ֵķ���
typedef struct sign
{
	int sgn;//����
	int num;//ֵ
	struct sign* next;//���
}Sign;

//��¼��ɾ�������ֻ��־�
typedef struct memory
{
	int deLitralLocation;//���ֵ�λ��
	int deClauseLocation;//�־��λ��
	struct memory* next;//���
}Memory;

//�����־�
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

//��������
Literal* createLiteral()
{
	Literal* lit = (Literal*)malloc(sizeof(Literal));
	lit->location = 0;
	lit->num = 0;
	lit->state = true;
	lit->next = NULL;
	return lit;
}

//����Sign
Sign* createSign()
{
	Sign* sig = (Sign*)malloc(sizeof(Sign));
	sig->num = 0;
	sig->sgn = -1;
	sig->next = NULL;
	return sig;
}

//����Memory
Memory* createMemory()
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));
	mem->deClauseLocation = 0;
	mem->deLitralLocation = 0;
	mem->next = NULL;
	return mem;
}

//������β�����־�
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

//������β��������
void insertLiteral(Literal* Current, int num, int location)
{
	Literal* lit = (Literal*)malloc(sizeof(Literal));
	lit->num = num;
	lit->location = location;
	lit->state = true;
	lit->next = NULL;
	Current->next = lit;
}

//������β�������
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
		if (temps->num == abs(num))//�����޸����е�����û��һ�µ��ٸ�Ϊ��ĩβ��������
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

//������β����Mem
void insertMemory(Memory* Current, int deClauseLocation, int deLitralLocation)
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));
	mem->deClauseLocation = deClauseLocation;
	mem->deLitralLocation = deLitralLocation;
	mem->next = NULL;
	Current->next = mem;
}

//ɾ������
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

//ɾ���־�
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

//ɾ������
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

//ɾ��Mem
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

//�������������ĳ���־�
void addsub(Clause* clauseCurrent, Literal* litralCurrent, int clauselen)
{
	clauseCurrent->sub = litralCurrent;
	clauseCurrent->clauselen = clauselen;
}

//��Sign���������Ա�������ļ��Ĺ淶��
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

//���ļ�����in����out���޸ĺ�׺
void fileHandle(char* in, char* out)
{
	char res[10] = ".res";//��׺��
	int len = strlen(in);
	in[len - 1] = '\0';
	for (int i = 0; i < len; i++)
		out[i] = in[i];
	for (int i = len - 5; i < len - 1; i++)
		out[i] = res[i - len + 5];
}

//��.res�����������
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

//���ļ�����ȡ��������
void fileExtract(FILE* in, int* clauseNum, int* literalNum)
{
	char cnfFile[200];
	do{
		fgets(cnfFile, 50, in);
	} while (cnfFile[0] != 'p');//�ҵ�p��ͷ����һ��
	int count = 6;
	for (; cnfFile[count] != ' '; count++)
		(*literalNum) = (cnfFile[count] - '0') + (*literalNum) * 10;//����������
	count++;
	for (; cnfFile[count] != '\n'; count++)
		(*clauseNum) = (cnfFile[count] - '0') + (*clauseNum) * 10;//�����Ӿ���
}

//���ļ�����ȡ��Ϣ����CNF����
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

//չʾ�־�
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

//չʾ����
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