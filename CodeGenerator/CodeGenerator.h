/* CodeGenerator.h : ��ͷ�ļ��Ǵ���������صĶ���
 * �����м�������ɣ�MIPS��������
 */

#pragma once

#include <iostream>
#include <map>
#include <vector>
#include "SymbolTableItem.h"

 /*-------�궨��------*/
#define TEMP_REGISTER 6 // ��ʱ�����Ĵ���$t4~$t9

using namespace std;

/*------��������------*/
const int tempStackMax = 100; // ������ʱ����ջ���Ϊ100�ֽ�(�����25������)
const unsigned int dataBaseAddr = 0x10010000; // data����ʼ��ַ

//extern vector<SymbolTableItem> globalSymbolTable; // �����ⲿ�ķ��ű�

/*------�������ɺ�������------*/
string generateLabel();			// ���ɱ�ǩ������
string generateVar();				// ������ʱ����������
string generateStrLabel();         // �����ַ�����ǩ
bool isStringDigit(string);	    // �ж��ַ����Ƿ������ִ�
int strToInt(string);				// �ַ���תΪ����

// ���ʽ����ؼ��㴦��
void turnToPostfixExp(vector<PostfixItem>, vector<PostfixItem>&); // ת��Ϊ��׺���ʽ
string calculateExp(vector<PostfixItem>&, bool&, VALUE_TYPE&, int&, bool, vector<FourYuanItem>&, string); // �����׺���ʽֵ
void objExpLeftR(vector<PostfixItem>&, bool&, string&, int&); // �����׺���ʽ��������

void writeItmCodeToFile(string, vector<FourYuanItem>, map<string, unsigned>&);			// ���м����д�뵽�ļ���
void generateMipsCode(string, bool);			// ���м���뷭���MIPS������Դ���

void generateData(ofstream&);		// ����data��
void generateText(ofstream&, vector<FourYuanItem>);		// ����text��

void getAddrOfGlobal(string, string, ofstream&);
void getAddrOfLocal(string, string, string, ofstream&);
void getAddrOfTemp(int, string, ofstream&);
void helpAssignStatement(FourYuanItem, ofstream&);
int getGlobalVarSumSpace();
void initializeStack(string, ofstream&);
void helpFunctionDef(string, ofstream&);
void helpBJump(FourYuanItem, ofstream&);
void helpReturn(ofstream&);
void cancelEscapeChar(string&);    // ȡ���ַ����е�ת���ַ�

void generateOpItmArr(vector<FourYuanItem>&);
/*
void op_initializeStack(string, ofstream&);
void op_helpAssignStatement(FourYuanItem, ofstream&);
void op_helpFunctionDef(string, ofstream&);
void op_helpBJump(FourYuanItem, ofstream&);
void op_helpReturn(ofstream&, string);
void op_generateText(ofstream&);
*/