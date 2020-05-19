#pragma once

#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "STManager.h"

using namespace std;

struct Token_Sym { // ��������Ӧ����
	SYM symbol; // �������
	string word; // ����
	int lines; // ������������
	Token_Sym() { this->symbol = INEXIST; this->lines = 0; }
};

struct Exp_ret { // ���ʽ����
	string name; // ���ʽ���صı�ʶ����--->ͳһ�涨,���ʽ����Ҫ��һ�����ر�����(��ʱ����)
	bool surable; // �Ƿ�ȷ��ֵ
	VALUE_TYPE type; // ֵ����
	int num;
	char ch;
	bool isEmpty; // �Ƿ��ǿյ�
	Exp_ret() { this->surable = false;  this->type = Empty_Type; this->isEmpty = true; this->num = -1; this->ch = '\0'; }
};

/*------��������------*/
char getNextCh();                       // ��ȡ��һ���ַ�
void backIndex();                       // �˻ص���һ���ַ�
bool isAlnum(char ch);                  // �ж��Ƿ�����ķ������ĸ���֣������»���
bool isChar(char ch);                   // �ж��Ƿ�����ķ�����ַ��������»���
bool isNum(char ch);					 // �ж��Ƿ�Ϊ����
bool isString(char ch);                 // �ж��Ƿ�����ķ����ַ����е��ַ�
int isReserve();                        // �ж�token�е��ַ����Ǳ����ֻ��Ǳ�ʶ��
int getNum(string);						 // �ַ���ת����
void initial();                         // ���ַ����鵥�ʷ������
SYM getSymbol();                        // ��ȡ��Ӧ������
SYM getPreSymbol();                     // ��ȡԤ������
string getTokenSymWord();               // ��õ�ǰ��ӡ����
string getWord();						 // ��õ�ǰ����
string getWord(int);					 // ���index�±괦�ĵ���
int getNowSymLines();                   // ��õ�ǰ������������
void backPreSymbol();                   // ����Ԥ������
void printSymbol(SYM);                  // ��ӡʶ�𵥴ʵ������
void printSyntax(int);                  // ��ӡʶ�𵥴ʵ��﷨�ɷ�
void analysisError(int, int);           // �������������ʾ�����������<����������ʻ��﷨���������������>
void errorHandling(int, ERRORSYM);      // ���������<���������У��������>

/*------�﷨��������------*/
void syntaxProcedure();                 // ������
void syntaxFunc();                      // <����>
void syntaxcConstDeclare(string);      // ������˵����
void syntaxConstDefine(string);        // ���������壾
void constIntDefine(string);           // ��չ����-INT��������
void constCharDefine(string);          // ��չ����-CHAR��������
void syntaxVarDeclare(string);         // ������˵����
bool syntaxVarDefine(SYM, string);     // ���������壾
void syntaxReFuncDefine(string);       // ���з���ֵ�������壾
void syntaxNoReFuncDefine();           // ���޷���ֵ�������壾
bool syntaxDeclareHead();              // ������ͷ����
void syntaxParamTable(string);         // ��������
void syntaxCompStatement(string);      // ��������䣾
void syntaxStatementLine(string, bool, vector<FourYuanItem>&);      // ������У�
void syntaxStatement(string, bool, vector<FourYuanItem>&);          // <���>
void syntaxConditionState(string, bool, vector<FourYuanItem>&);     // ��������䣾
string syntaxCondition(string, bool, vector<FourYuanItem>&);          // <����>
void syntaxLoopState(string, bool, vector<FourYuanItem>&);          // <ѭ�����>
string syntaxStep();                     // ��������
bool syntaxCallReFunc(string, bool, vector<FourYuanItem>&);         // <�з���ֵ�����������>
bool syntaxCallNoFunc(string, bool, vector<FourYuanItem>&);         // ���޷���ֵ����������䣾
vector<VALUE_TYPE> syntaxValueParam(string, bool, vector<FourYuanItem>&);   // ��ֵ������
bool syntaxAssignState(string, bool, vector<FourYuanItem>&);        // <��ֵ���>
bool syntaxReadState(string, bool, vector<FourYuanItem>&);          // ������䣾
bool syntaxWriteState(string, bool, vector<FourYuanItem>&);         // ��д��䣾
bool syntaxString();                   // <�ַ���>
bool syntaxReturnState(string, bool, vector<FourYuanItem>&);        // ��������䣾
Exp_ret syntaxExpre(string, bool, vector<FourYuanItem>&);           // <���ʽ>
int syntaxTerm(string, vector<PostfixItem>&, bool, vector<FourYuanItem>&);                // <��>
int syntaxFactor(string, vector<PostfixItem>&, bool, vector<FourYuanItem>&);              // <����>
void syntaxMain();                     // <������>
