/* SymbolTableItem.h : ���ļ�����SymbolTableItem�ࡣ
 * SymbolTableItem���ű��ʶ��Ŀ�꣺C����
 */

#pragma once // ������ֻ֤�ᱻ����һ��

#include <string>
#include <vector>
#include "ConstEnumDefine.h"

using namespace std;

class SymbolTableItem
{
private:
	static int numberCount;    // ���ű������������̬���������ڼ���

	string idName;             // ��ʶ��������
	int order;				   // ���ű����ڵ����,��0��ʼ������numberCount
	string functionName;	   // ���ű������ĺ���, ȫ�ַ��ű���������Ϊ"GLOBAL"
	ITEM_KIND itemKind;		   // ���ű�������
	VALUE_TYPE valueType;	   // ֵ����
	FUNC_TYPE functionType;	   // ��������

	int conIntValue;		   // ��������
	char conCharValue;		   // �����ַ�
	int length;				   // ����ĳ���, ����Ǳ�����ӦΪ0
	int weight;				   // ֻ��Ժ����ڲ��ļ򵥱����Լ�������Ч

public:
	SymbolTableItem(string id, string funcName); // ��ʼ�����캯��

	// ��÷��ű�������
	int getOrder() { return order; }
	string getIdName() { return idName; }
	string getFuncName() { return functionName; }
	ITEM_KIND getItemKind() { return itemKind; }
	VALUE_TYPE getValueType() { return valueType; }
	FUNC_TYPE getFuncType() { return functionType; }
	int getArrLen() { return length; }
	int getConInt() { return conIntValue; }
	char getConChar() { return conCharValue; }
	//int getWeight() { return weight; }

	// set���÷��ű���
	void setItemKind(ITEM_KIND kind) { itemKind = kind; }
	void setValueType(VALUE_TYPE type) { valueType = type; }
	void setArrLen(int len) { length = len; }
	void setConInt(int value) { conIntValue = value; }
	void setConChar(char value) { conCharValue = value; }
	void setFuncType(FUNC_TYPE type) { functionType = type; }
	//void addWeight(int num) { weight += num; }
};



