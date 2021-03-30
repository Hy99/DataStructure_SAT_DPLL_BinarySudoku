//包含DPLL函数和其用到的其他函数

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "structure.h"

//修改存在性以“删除”对应的结构
void notClause(Clause* current)
{
	current->state = false;
}
void notLiteral(Literal* current)
{
	current->state = false;
}
//修改存在性以“复原”对应的结构
void yesclause(Clause* current)
{
	current->state = true;
}
void yesliteral(Literal* current)
{
	current->state = true;
}


//判断是否只有一个文字
bool clauseSingle(Clause* current)
{
	if (current->clauselen == 1 && current->state == true)
		return true;
	return false;
}

//找到单字句对应的文字
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

//判断是否为空
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

//是否存在单字句
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

//开辟空间存储文字
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

//删除字句的所有文字
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

//选取第一个文字
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
//选取众数
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


//复制数组
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


//改变文字的符号
void reverse(Clause* current, int chosenliteral)
{
	Clause* tempc = current->next;
	Literal* templ = tempc->sub->next;
	templ->num = -chosenliteral;
	tempc->state = true;
}

//将目标文字作为单字句插入CNF
void litToClause(Clause* current, int chosenliteral)
{
	Clause* tempc = current->next;
	Literal* templ = tempc->sub;
	insertLiteral(templ, chosenliteral, 1);
	tempc->clauselen = 1;
	tempc->state = true;
}

//复原
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
	Memory* tempm = createMemory();//用于递归的记忆链表
	Memory* tempmh = tempm;//仅用于本次单字句循环中的记忆链表

	while (ExistOne(clauseCurrent))//存在单字句
	{
		int lit = findLit(clauseCurrent);//找到单字句的文字
		insertSign(signCurrent, lit);//确定对应单字句的真值
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
						if (templ->num == lit)//如果是对应的文字且真值相同
						{
							notClause(tempc);//删除字句
							templ = tempc->sub->next;
							while (templ != NULL)
							{
								if (templ->state)
								{
									notLiteral(templ);//删除其中所有对应真值的该文字
									insertMemory(tempmh, tempc->location, templ->location);//记录每一个删除
									tempmh = tempmh->next;
									tempc->clauselen = tempc->clauselen - 1;
								}
								templ = templ->next;
							}//endwhile
						}//endif
						else if (templ->num == -lit)//如果是对应的文字但真值不同
						{
							notLiteral(templ);//仅删除文字
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
		if (IsEmpty(clauseCurrent))//为空证明可满足，复原CNF，删除memory
		{
			regain(clauseCurrent, tempm);
			destroyMemory(tempm);
			return true;
		}
		tempc = clauseCurrent->next;
		while (tempc != NULL)
		{
			if (tempc->clauselen == 0 && tempc->state == true)//存在空字句，证明不可满足
			{
				regain(clauseCurrent, tempm);
				destroyMemory(tempm);
				return false;
			}
			tempc = tempc->next;
		}
	}//endwhile
	int chosenv;
	//chosenv = chooseFirstLit(clauseCurrent);//根据某种方案选择一个字句
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
	if (pass)//递归通过
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return true;
	}
	reverse(clauseCurrent, chosenv);//不通过则反转该文字
	pass = DPLLsolving(clauseCurrent, signCurrent, key, keynum, num);
	if (pass)//递归通过
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return true;
	}
	else//确实不通过
	{
		regain(clauseCurrent, tempm);
		destroyMemory(tempm);
		return false;
	}
}
