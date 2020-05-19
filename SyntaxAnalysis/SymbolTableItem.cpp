/* SymbolTableItem.cpp�ļ�����SymbolTableItem��ĺ�������
 */

#include "SymbolTableItem.h"
#include <vector>

using namespace std;

int SymbolTableItem::numberCount = 0;

//ȫ�ַ��ű�
vector<SymbolTableItem> globalSymbolTable;

SymbolTableItem::SymbolTableItem(string id, string funcName) { // ���캯���Ķ���
	order = numberCount++;
	idName = id;
	functionName = funcName;
	length = 0; // Ĭ��Ϊ0,Ĭ���Ǳ�����������
	weight = 0;
}