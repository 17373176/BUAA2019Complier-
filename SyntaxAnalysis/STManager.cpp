/* STManager.cpp�ļ�����STManager��ĺ�������
 *
 */

#include "STManager.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

STManager::STManager() { // ��׼���캯�� 
	reDeclareFuncName = "GLOBAL"; // ȫ����
}

// ���ű��������������
 // int����
bool STManager::pushItem(string id, string functionName, int num) {
	if (!isInsert(id, functionName)) // �ظ����岻�ɲ���
		return false;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemKind(Con_Kind);
	newItem.setValueType(Int_Type);
	newItem.setConInt(num);
	globalSymbolTable.push_back(newItem); // ��ӵ����ű�����β��
	return true;
}
// char����
bool STManager::pushItem(string id, string functionName, char character) { 
	if (!isInsert(id, functionName))
		return false;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemKind(Con_Kind);
	newItem.setValueType(Char_Type);
	newItem.setConChar(character);
	globalSymbolTable.push_back(newItem);
	return true;
}
// ����
bool STManager::pushItem(string id, string functionName, VALUE_TYPE valueType, int len) {
	if (!isInsert(id, functionName))
		return false;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemKind(Var_Kind);
	newItem.setValueType(valueType);
	newItem.setArrLen(len);
	globalSymbolTable.push_back(newItem);
	return true;
}
// ����+����
bool STManager::pushItem(string id, string functionName, ITEM_KIND itemKind, VALUE_TYPE valueType) {
	if (!isInsert(id, functionName))
		return false;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemKind(itemKind);
	newItem.setValueType(valueType);
	globalSymbolTable.push_back(newItem);
	return true;
}
// ����
bool STManager::pushItem(string id, string functionName, FUNC_TYPE funcType) {
	if (!isInsert(id, functionName))
		return false;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemKind(Func_Kind);
	newItem.setFuncType(funcType);
	globalSymbolTable.push_back(newItem);
	return true;
}
// ����Ƿ�����
bool STManager::isInsert(string id, string functionName) {
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (functionName == item.getFuncName() && id == item.getIdName()) // ��������ͬ,����Ҳ��ͬ
			return false;
	}
	return true;
}

// �����飼��ʶ������[�������ʽ����]��������-1δ���壬-2�±�Խ�磬-3�±겻ȷ�������ͣ�����ֵΪ���ű����
int STManager::idArrExpCheck(string identifier, string funcName, bool surable, int index) {
	bool globalIndexOut = false; // ��ȫ�����鵫Խ��
	bool notglobalArr = false; // ����ȫ������
	bool isDefined = false; // δ�����ʶ��
	int order = -1;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL" && item.getIdName() == identifier) { // ȫ��,��ʶ������ͬ
			isDefined = true; // ������
			if (item.getArrLen() > 0) { // ������
				if (surable) { // �����±�ֵ��ȷ����
					if (index >= item.getArrLen() || index < 0) { // Խ��
						globalIndexOut = true;
					}
					else order = item.getOrder();
				}
				else order = item.getOrder();
			}
			else notglobalArr = true; // ��������
		}
		else if (item.getFuncName() == funcName && item.getIdName() == identifier) { // ���ں�����������
			isDefined = true;
			if (item.getArrLen() > 0) {
				if (surable) {
					if (index >= item.getArrLen() || index < 0) {//Խ��
						//myError.SemanticAnalysisError(ArrIndexOutOfRangeError, getLineNumber(), identifier);
						return -1;
					}
					else return item.getOrder();
				}
				else return item.getOrder();
			}
			else {//��������
				if (globalIndexOut) {
					break;
				}
				else {
					//myError.SemanticAnalysisError(TypeNotMatchError, getLineNumber(), identifier);
					return -1;
				}
			}
		}
	}

	if (globalIndexOut) {
		//myError.SemanticAnalysisError(ArrIndexOutOfRangeError, getLineNumber(), identifier);
		return -1;
	}
	if (notglobalArr) {
		//myError.SemanticAnalysisError(TypeNotMatchError, getLineNumber(), identifier);
		return -1;
	}
	//��ʶ��δ����
	if (!isDefined) {
		//myError.SemanticAnalysisError(NotDefinitionError, getLineNumber(), identifier);
		return -1;
	}
	return order;
}

// ��ʶ�����������(������Ϊvoid����),���ʽ�У���ֵ����ұ�,����-1Ϊδ�����������ʶ��������ֵ��Ϊ���ڷ��ű�����,ͨ����ſ��Բ��Ҹ���
int STManager::idCheckInFactor(string identifier, string funcName) {
	bool foundInGlobal = false; // ��ʾ��global�з��ִ˽ṹ��������
	bool isDefined = false;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {//ȫ��������
			if (item.getIdName() == identifier) {//��ʶ��������ͬ
				isDefined = true;
				if (item.getArrLen() > 0 || (item.getItemKind() == Func_Kind && item.getFuncType() == Void_Type)) 
					foundInGlobal = true;
				else return item.getOrder();
			}
		}
		else if (item.getFuncName() == funcName) { // ��������ͬ(�ֲ�������)
			if (item.getIdName() == identifier) {
				isDefined = true;
				if (item.getArrLen() > 0) //Ϊ����,����
					return -1;
				else return item.getOrder();
			}
		}
	}
	if (foundInGlobal)
		return -1;
	if (!isDefined) 
		return -1;
	return -1;
}

// ����к�����ʶ����飬�����ж��Ƿ��иú��������غ�������
FUNC_TYPE STManager::idCheckInState(string identifier) {
	// ����ֻ���Ǻ���,����ֻ��Ҫ����ȫ�ֵļ���
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") 
			if (item.getIdName() == identifier) 
				if (item.getItemKind() == Func_Kind)
					return item.getFuncType();
	}
	return NotDefine_Func; // û�ж���
}

// ����ʶ������(��<ֵ������>��)����������û�в����ĺ������ü��,�����������е�(���ʽ�е�,��Ҫ�ж��Ƿ����з���ֵ)
// -1δ���壬����1���������ƥ�䣬2�������Ͳ�ƥ�䣬3�������з���ֵ
int STManager::funcCallCheck(string identifier, bool isInExp, vector<VALUE_TYPE> paramType) {
	bool isDefined = false; // �Ƿ���Ҫ���в������(����δ��������Ҫ����)
	vector<VALUE_TYPE> actualParam;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") 
			if (item.getIdName() == identifier)
				if (item.getItemKind() == Func_Kind) {//�Ǻ���
					isDefined = true;
					if (isInExp && item.getFuncType() == Void_Type) // �����з���ֵ
						return 3;
					for (unsigned int j = i + 1; j < globalSymbolTable.size(); j++) {
						item = globalSymbolTable.at(j);
						if (item.getFuncName() == identifier && item.getItemKind() == Para_Kind)
							actualParam.push_back(item.getValueType());
						else break;
					}
					break;
				}
	}
	if (!isDefined)
		return -1;
	// ��������
	if (paramType.size() == 0 && actualParam.size() == 0) // ������������0��
		return 0;
	if (paramType.size() != actualParam.size()) // ����������ƥ��
		return 1;
	for (unsigned int i = 0; i < paramType.size(); i++) {
		VALUE_TYPE first = actualParam.at(i);
		VALUE_TYPE second = paramType.at(i);
		if (first != second) //�������Ͳ�ƥ��
			return 2;
	}
	return 0; // ����
}

// �Ը�ֵ�������ߵı�ʶ��,�Լ�scanf�е����ı�ʶ���ļ��(scanfʵ�ʾ��ǶԱ����ĸ�ֵ����)
// ����-1Ϊδ����,-2Ϊ����������ı�ʶ��,-3Ϊ�������ܱ���ֵ, ����ֵΪ���ϣ���Ϊ���ڷ��ű�����,ͨ����ſ��Բ��Ҹ���
int STManager::checkAssignId(string identifier, string funcName) {
	bool isDefined = false; // �Ƿ���
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {
			if (item.getIdName() == identifier) {
				isDefined = true; // ��ȫ�ֶ���
				if (item.getItemKind() == Var_Kind && item.getArrLen() == 0) // ��ȫ�ֱ���,�൱���Ѷ���
					return item.getOrder();
				else if (item.getItemKind() == Con_Kind) return -3; // ȫ�ֳ���
			}
		}
		else if (item.getFuncName() == funcName) { // �Ǻ����ڲ���
			if (item.getIdName() == identifier) {
				isDefined = true;
				if ((item.getItemKind() == Var_Kind && item.getArrLen() == 0) 
					|| (item.getItemKind() == Para_Kind)) { // �Ǹú����ڵı������߲���
					return item.getOrder();
				}
				else if (item.getItemKind() == Con_Kind) return -3; // �ú����ڵĳ���
			}
		}
	}
	if (!isDefined) 
		return -1;
	return -2;
}

// �޷���ֵ����return����飬trueΪ�޷���ֵ����
bool STManager::checkReturn(string funcName) {
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getItemKind() == Func_Kind && item.getIdName() == funcName) {
			if (item.getFuncType() != Void_Type) // �����޷���ֵ����
				return false;
			return true;
		}
	}
	return false;
}
// �з���ֵ����return�����,��ƥ�䷵��false��ƥ�䷵��true
bool STManager::checkReturn(string funcName, VALUE_TYPE retType) {
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getItemKind() == Func_Kind && item.getIdName() == funcName) {
			if ((item.getFuncType() == ReturnChar_Type && retType == Int_Type) || 
				(item.getFuncType() == ReturnInt_Type && retType  == Char_Type)) { // ��������������return���Ͳ�ͬ
				return false;
			}
			return true;
		}
	}
	return false;
}

VALUE_TYPE STManager::getItemValueType(int order) { // �õ�order��Ӧ�ķ��ű���ֵ����
	return globalSymbolTable.at(order).getValueType();
}

/*
//��Ȩ----���ü���
void STManager::addWeight(int order, int weight) {
	SymbolTableItem item = globalSymbolTable.at(order);
	if ((item.getItemKind() == Var_Kind || item.getItemKind() == Para_Kind) && item.getFuncName() != "GLOBAL"
		&& item.getArrLen() == 0) {
		globalSymbolTable.at(order).addWeight(weight);
	}
}
*/