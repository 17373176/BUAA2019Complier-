/* STManager.h : ��ͷ�ļ����ڷ��ű������ӷ��ű����Լ������ű�
 * �����������ű�,����ű����������صĲ�������
 */

#pragma once

#include <string>
#include <vector>
#include "SymbolTableItem.h"
#include "CodeGenerator.h"

using namespace std;

extern vector<SymbolTableItem> globalSymbolTable; // ���ű�

class STManager
{
private:
	string reDeclareFuncName; // �����������ĺ�������
	SYM relation; // ��ϵ�����

public:
	STManager();

	vector<SymbolTableItem> getTable(); // ��÷��ű�

	// ����ű���,���ú�������
	bool pushItem(string id, string functionName, int num); // ����int
	bool pushItem(string id, string functionName, char ch); // ����char
	bool pushItem(string id, string functionName, VALUE_TYPE valueType, int size); // ��������
	bool pushItem(string id, string functionName, FUNC_TYPE funcType); // ����
	bool pushItem(string id, string functionName, ITEM_KIND itemKind, VALUE_TYPE valueType); // ���������
	
	// �����ű�
	bool isInsert(string id, string functionName); // ����Ƿ�������
	
	int idCheckInFactor(string identifier, string funcName); // �������б�ʶ�����
	
	FUNC_TYPE idCheckInState(string identifier); // ����б�ʶ�����
	//����ʶ������[�������ʽ����]���������븳ֵ����м������
	int idArrExpCheck(string identifier, string funcName, bool surable, int index = 0);
	//����ʶ������(��<ֵ������>��)���������ü�飬�����������е�(���ʽ�е�,��Ҫ�ж��Ƿ����з���ֵ)
	int funcCallCheck(string identifier, bool isInExp, vector<VALUE_TYPE> paramType);
	//���ͼ��
	/*bool checkTypeMatch(VALUE_TYPE s_type, VALUE_TYPE e_type) {
		if (s_type == Char_Type && e_type == Int_Type) {
			//myError.SemanticAnalysisError(AssignIntToCharError, getLineNumber(), "");
		}
	}*/
	//�Ը�ֵ��䵥���ı�ʶ���ļ��
	int checkAssignId(string identifier, string funcName);

	bool checkReturn(string funcName); // ���������,�޷���ֵ��return
	bool checkReturn(string funcName, VALUE_TYPE retType); // �з���ֵ��

	// ��÷��ű���˽�б���
	VALUE_TYPE getItemValueType(int order); // �õ�order��Ӧ�ķ��ű���ֵ����

	//void addWeight(int order, int weight); // �Ż����ֵĺ���, ��Ҫ����Ƿ����Ҫ��
};

