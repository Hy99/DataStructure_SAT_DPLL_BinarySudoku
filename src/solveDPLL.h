//����DPLL���������õ�����������

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "structure.h"

//�޸Ĵ������ԡ�ɾ������Ӧ�Ľṹ
void notClause(Clause* current)
{
	current->state = false;
}
void notLiteral(Literal* current)
{
	current->state = false;
}
//�޸Ĵ������ԡ���ԭ����Ӧ�Ľṹ
void yesclause(Clause* current)
{
	current->state = true;
}
void yesliteral(Literal* current)
{
	current->state = true;
}


//�ж��Ƿ�ֻ��һ������
bool clauseSingle(Clause* current)
{
	if (current->clauselen == 1 && current->state == true)
		return true;
	return false;
}

//�ҵ����־��Ӧ������
int findLit(Clause* current)
{
	int num = 0;
	Clause* tempc = current->next;
	while (tempc != NULL)
	{
		if (clauseSingle(tempc))
		{
			Literal* templ = tempc->sub->next;
			while (templ != NULL)
			{
				if (templ->state) 
				{
					num = templ->num;
					return num;
				}
				templ = templ->next;
			}
		}
		tempc = tempc->next;
	}
	return 0;
}

//�ж��Ƿ�Ϊ��
bool IsEmpty(Clause* current)
{
	Clause* tempc = current->next;
	while (tempc != NULL)
	{
		if (tempc->state) return false;
		tempc = tempc->next;
	}
	return true;
}

//�Ƿ���ڵ��־�
bool ExistOne(const Clause* current)
{
	Clause* tempc = current->next;
	while (tempc != NULL)
	{
		if (clauseSingle(tempc)) return true;
		tempc = tempc->next;
	}
	return false;
}

//���ٿռ�洢����
void CreateVspace(Clause* current)
{
	Clause* tempc = current;
	Clause* tempch = current->next;
	insertClause(tempc, 0);
	tempc = tempc->next;
	tempc->state = false;
	Literal* head = createLiteral();
	addsub(tempc, head, 0);
	tempc->next = tempch;
}

//ɾ���־����������
void deliteral(Clause* current)
{
	Literal* templ = current->next->sub;
	free(templ->next);
	templ->next = NULL;
}

void quickSort(int* array, int* arraynum, int low, int high)
{
	int i, j, key1, key;
	i = low;
	j = high;
	key1 = array[low];
	key = arraynum[low];
	if (low < high) {
		while (i < j) {
			while (i < j && arraynum[j] <= key) j--;
			if (i < j) {
				array[i] = array[j];
				arraynum[i] = arraynum[j];
			}
			while (i < j && arraynum[i] >= key) i++;
			if (i < j) {
				array[j] = array[i];
				arraynum[j] = arraynum[i];
			}
		}
		array[i] = key1;
		arraynum[i] = key;
		int standard = i;
		quickSort(array, arraynum, low, standard - 1);
		quickSort(array, arraynum, standard + 1, high);
	}
}

//ѡȡ��һ������
int chooseFirstLit(Clause* current)
{
	int num = 0;
	int i = 0;
	Clause* tempc = current->next->next;

	while (tempc != NULL)
	{
		if (tempc->state)
		{
			Literal* templ = tempc->sub->next;
			while (templ != NULL)
			{
				if (templ->state)
				{
					num = templ->num;
					break;
				}
				templ = templ->next;
			}
		}
		if (num != 0) break;
		tempc = tempc->next;
	}
	return num;
}
//ѡȡ����
int chooseMostLit(Clause* current, int* key, int* keynum, int num)
{
	int lit = 0, i = 0;
	while (keynum[i] == 0) i++;
	if (i >= num) return 0;
	lit = key[i];
	key[i] = 0;
	keynum[i] = 0;
	return lit;
}


//��������
void copyArray(int* key, int* keynum, int *ckey, int *ckeynum, int num) {
	int i = 0, j = 0;
	while (i < num) {
		ckey[i] = key[i];
		i++;
	}
	while (j < num) {
		ckeynum[j] = keynum[j];
		j++;
	}
}


//�ı����ֵķ���
void reverse(Clause* current, int chosenliteral)
{
	Clause* tempc = current->next;
	Literal* templ = tempc->sub->next;
	templ->num = -chosenliteral;
	tempc->state = true;
}

//��Ŀ��������Ϊ���־����CNF
void litToClause(Clause* current, int chosenliteral)
{
	Clause* tempc = current->next;
	Literal* templ = tempc->sub;
	insertLiteral(templ, chosenliteral, 1);
	tempc->clauselen = 1;
	tempc->state = true;
}

//��ԭ
void regain(Clause* clauseCurrent, Memory* memoryCurrent)
{
	Memory* tempm = memoryCurrent->next;
	while (tempm != NULL)
	{
		Clause* tempc = clauseCurrent->next;
		while (tempc->location != tempm->deClauseLocation)
			tempc = tempc->next;
		yesclause(tempc);
		Literal* templ = tempc->sub->next;
		while (templ->location != tempm->deLitralLocation)
			templ = templ->next;
		yesliteral(templ);
		if (tempc == clauseCurrent->next) tempc->clauselen = 1;
		else tempc->clauselen = tempc->clauselen + 1;
		tempm = tempm->next;
	}
}


//DPLL
bool DPLLsolving(Clause* clauseCurrent, Sign* signCurrent, int *key, int *keynum, int num)
{
	Memory* tempm = createMemory();//���ڵݹ�ļ�������
	Memory* tempmh = tempm;//�����ڱ��ε��־�ѭ���еļ�������

	while (ExistOne(clauseCurrent))//���ڵ��־�
	{
		int lit = findLit(clauseCurrent);//�ҵ����־������
		insertSign(signCurrent, lit);//ȷ����Ӧ���־����ֵ
		Clause* tempc = clauseCurrent->next;
		while (tempc != NULL)
		{
			if (tempc->state)
			{
				Literal* templ = tempc->sub->next;
				while (templ != NULL)
				{
					if (templ->state)
					{
						if (templ->num == lit)//����Ƕ�Ӧ����������ֵ��ͬ
						{
							notClause(tempc);//ɾ���־�
							templ = tempc->sub->next;
							while (templ != NULL)
							{
								if (templ->state)
								{
									notLiteral(templ);//ɾ���������ж�Ӧ��ֵ�ĸ�����
									insertMemory(tempmh, tempc->location, templ->location);//��¼ÿһ��ɾ��
									tempmh = tempmh->next;
									tempc->clauselen = tempc->clauselen - 1;
								}
								templ = templ->next;
							}//endwhile
						}//endif
						else if (templ->num == -lit)//����Ƕ�Ӧ�����ֵ���ֵ��ͬ
						{
							notLiteral(templ);//��ɾ������
							insertMemory(tempmh, tempc->location, templ->location);
							tempmh = tempmh->next;
							tempc->clauselen = tempc->clauselen - 1;
							templ = templ->next;
						}//endif
						else templ = templ->next;
					}//endif
					else templ = templ->next;
				}//endwhile
			}//endif
			tempc = tempc->next;
		}//endwhile
		if (IsEmpty(clauseCurrent))//Ϊ��֤�������㣬��ԭCNF��ɾ��memory
		{
			regain(clauseCurrent, tempm);
			destroyMemory(tempm);
			return true;
		}
		tempc = clauseCurrent->next;
		while (tempc != NULL)
		{
			if (tempc->clauselen == 0 && tempc->state == true)//���ڿ��־䣬֤����������
			{
				regain(clauseCurrent, tempm);
				destroyMemory(tempm);
				return false;
			}
			tempc = tempc->next;
		}
	}//endwhile
	int chosenv;
	//chosenv = chooseFirstLit(clauseCurrent);//����ĳ�ַ���ѡ��һ���־�
	chosenv = chooseMostLit(clauseCurrent, key, keynum, num);
	if (chosenv == 0) 
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return true;
	}
	if (clauseCurrent->next->sub->next != NULL) deliteral(clauseCurrent);
	litToClause(clauseCurrent, chosenv);
	bool pass;
	pass = DPLLsolving(clauseCurrent, signCurrent, key, keynum, num);
	if (pass)//�ݹ�ͨ��
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return true;
	}
	reverse(clauseCurrent, chosenv);//��ͨ����ת������
	pass = DPLLsolving(clauseCurrent, signCurrent, key, keynum, num);
	if (pass)//�ݹ�ͨ��
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return true;
	}
	else//ȷʵ��ͨ��
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return false;
	}
}
