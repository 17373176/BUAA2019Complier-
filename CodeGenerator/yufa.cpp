
#include "SyntaxAnalysis.h"
#include <iostream>

using namespace std;

extern int tmpVarCount;

//��׼���캯��,���ó�ʼ����Ա�б�
SyntaxAnalysis::SyntaxAnalysis(Error& myError, LexicalAnalysis& myLexicalAnalysis) :myError(myError), myLexicalAnalysis(myLexicalAnalysis) {
	isMainVoid = false;
	return_integer = 0;
	return_declare_funcName = "GLOBAL";
}

//������ں���
void SyntaxAnalysis::startAnalysis() {
	bool nextSymFlag;
	nextSymFlag = myLexicalAnalysis.nextSym();//����ÿһ���ݹ��½������ӳ���ǰ����Ҫ�Ƚ���Ԥ��
	if (nextSymFlag) {        //����<����>����
		if (!ZSQX_procedure())
			return;
	}
	else {
		myError.SyntaxAnalysisError(EmptyFileError, getLineNumber());
		return;
	}
	//����ļ����Ƿ��ж��������
	if (!myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(AfterMainContentError, getLineNumber());
	}
}
//������ ::= �ۣ�����˵�����ݣۣ�����˵������[<�������岿��>]   ����������
bool SyntaxAnalysis::ZSQX_procedure() {
	bool nextSymFlag;
	//���Ƿ��г���˵��
	ZSQX_constDescription("GLOBAL");
	//�Ƿ��б���˵��
	ZSQX_varDescription(true, "GLOBAL");
	//����Ƿ��к�������
	ZSQX_functionDefinition();
	//��ʽ����void main(){...}
	//�ں������岿��һ����ʶ��void main�ٷ���
	SymbolCode symbol;
	//�ȼ���Ƿ�Ϊmain
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == MAINSY && isMainVoid)) {
		myError.SyntaxAnalysisError(NoMainFunctionError, getLineNumber());
		return false;
	}
	//��������Ķ�ȡ
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main() lack (");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LSBRACKET)) {//(
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack (");
		myLexicalAnalysis.setNextSym();
	}

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack )");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RSBRACKET)) {//)
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack )");
		myLexicalAnalysis.setNextSym();
	}

	//�������,main��������ű�
	pushItem("main", "GLOBAL", VoidType);

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack {");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LBBRACKET)) {//{
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack {");
		myLexicalAnalysis.setNextSym();
	}

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack componud statement");
		return false;
	}
	//�������
	FourYuanItem four;
	four.type = FunctionDef;
	four.funcType = VoidType;
	four.target = "main";
	globalTmpCodeArr.push_back(four);

	ZSQX_compoundStatement("main");

	if (myLexicalAnalysis.isFinish()) {//û�н�β��},�ļ���ȡ����,����
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack }");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RBBRACKET)) {//}
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "main lack }");
		myLexicalAnalysis.setNextSym();
	}
	//Ԥ��:
	myLexicalAnalysis.nextSym();
	//������������,procedureʶ��
	return true;
}

//������˵���� ::= const���������壾;{ const���������壾;}
bool SyntaxAnalysis::ZSQX_constDescription(string funcName) {
	bool nextSymFlag;
	SymbolCode symbol;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	//����const
	if (!(symbol == CONSTSY)) {
		return false;
	}

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack definition after \"const\".");
		return false;
	}
	//����<��������>
	ZSQX_constDefinition(funcName);

	if (myLexicalAnalysis.isFinish()) {//δ��������,�Ѿ������ļ���β
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon after const definition.");
		return false;
	}
	//����;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == SEMI)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon after const definition.");
		myLexicalAnalysis.setNextSym();
	}

	//����{...}
	while (true) {
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			break;
		}
		//const
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == CONSTSY)) {
			break;
		}

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack definition after \"const\".");
			break;
		}
		//<��������>
		ZSQX_constDefinition(funcName);

		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon after const definition.");
			break;
		}
		//����;
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == SEMI)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon after const definition.");
			myLexicalAnalysis.setNextSym();
		}
	}
	return true;
}

/*���������壾 ::= int����ʶ��������������{,����ʶ��������������}
		| char����ʶ���������ַ���{,����ʶ���������ַ���} */
bool SyntaxAnalysis::ZSQX_constDefinition(string funcName) {
	bool nextSymFlag;
	SymbolCode symbol;
	string id;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == INTSY) {//int
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier after int.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		//��ʶ��
		if (!(symbol == IDENTIFIER)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after int is not identifier.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}
		id = myLexicalAnalysis.getGlobalString();

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack = after identifier.");
			return false;
		}
		//=
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == ASSIGN)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after identifier is not =");
			myLexicalAnalysis.skipRead(';');
			return false;
		}

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack integer after =.");
			return false;
		}
		//����
		ZSQX_integer();
		//����ű�
		pushItem(id, funcName, return_integer);
		while (true) {
			if (myLexicalAnalysis.isFinish()) {
				break;
			}
			//,
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == COMMA)) {
				break;
			}

			//���ж���,����ȴɶ��û��
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!(nextSymFlag)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack contents after comma.");
				break;
			}
			//��ʶ��
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == IDENTIFIER)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after comma is not identifier.");
				myLexicalAnalysis.skipRead(';');
				break;
			}
			id = myLexicalAnalysis.getGlobalString();
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!(nextSymFlag)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack = after identifier.");
				break;
			}
			//=
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == ASSIGN)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after identifier is not =.");
				myLexicalAnalysis.skipRead(';');
				break;
			}

			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!(nextSymFlag)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack integer after =.");
				break;
			}
			//����
			ZSQX_integer();
			pushItem(id, funcName, return_integer);
		}

	}
	else if (symbol == CHARSY) {//char
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier after char.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		//��ʶ��
		if (!(symbol == IDENTIFIER)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after char is not identifier.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}
		id = myLexicalAnalysis.getGlobalString();

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack = after identifier.");
			return false;
		}
		//=
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == ASSIGN)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after identifier is not =.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!(nextSymFlag)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack const character after =.");
			return false;
		}
		//�ַ�
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == CHAR)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after = is not const character.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}
		pushItem(id, funcName, myLexicalAnalysis.getGlobalChar());
		while (true) {
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag)
				break;
			//,
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == COMMA)) {
				break;
			}

			//���ж���,����ȴɶ��û��
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!(nextSymFlag)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack contents after comma.");
				break;
			}
			//��ʶ��
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == IDENTIFIER)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after comma is not identifier.");
				myLexicalAnalysis.skipRead(';');
				break;
			}
			id = myLexicalAnalysis.getGlobalString();

			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!(nextSymFlag)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack = after identifier.");
				break;
			}
			//=
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == ASSIGN)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after identifier is not =.");
				myLexicalAnalysis.skipRead(';');
				break;
			}

			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!(nextSymFlag)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack const character after =.");
				break;
			}
			//�ַ�
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == CHAR)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after = is not const character.");
				myLexicalAnalysis.skipRead(';');
				break;
			}
			pushItem(id, funcName, myLexicalAnalysis.getGlobalChar());
		}
	}
	else {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "incomplete const definition.");
		myLexicalAnalysis.skipRead(';');
		return false;
	}

	return true;
}

//������˵���� ::= ���������壾;{���������壾;}
//����ķ�������---���ڸ�ȫ�ֵĺ����������ڳ�ͻ
bool SyntaxAnalysis::ZSQX_varDescription(bool isGlobal, string funcName) {
	bool nextSymFlag;
	SymbolCode symbol;
	int point = myLexicalAnalysis.getPoint();
	SymbolCode symbolCode = myLexicalAnalysis.getGlobalSymbol();
	//����Ƿ��Ƕ����ȫ�ֺ��������Ǳ�������
	if (isGlobal) {
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == CHARSY || symbol == INTSY) {
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (nextSymFlag) {
				symbol = myLexicalAnalysis.getGlobalSymbol();
				if (symbol == IDENTIFIER) {
					nextSymFlag = myLexicalAnalysis.nextSym();
					if (nextSymFlag) {
						symbol = myLexicalAnalysis.getGlobalSymbol();
						if (symbol == LSBRACKET || symbol == LBBRACKET) {//( or {
							myLexicalAnalysis.resetPoint(point);
							myLexicalAnalysis.resetSymbol(symbolCode);
							return false;
						}
					}
				}
			}
			//������Ǻ�������,�����,��������ķ���
			myLexicalAnalysis.resetPoint(point);
			myLexicalAnalysis.resetSymbol(symbolCode);
		}
		else {
			return false;
		}
	}
	//�������ȫ�ֵ�,����������
	if (!ZSQX_varDefinition(funcName))
		return false;

	if (myLexicalAnalysis.isFinish()) {
		return false;
	}
	//;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == SEMI)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon after var definition.");
		myLexicalAnalysis.setNextSym();
	}

	while (true) {
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag)
			break;
		//�ٴμ���Ƿ���ֺ����������������
		if (isGlobal) {
			point = myLexicalAnalysis.getPoint();
			symbol = symbolCode = myLexicalAnalysis.getGlobalSymbol();
			if (symbol == CHARSY || symbol == INTSY) {
				nextSymFlag = myLexicalAnalysis.nextSym();
				if (nextSymFlag) {
					symbol = myLexicalAnalysis.getGlobalSymbol();
					if (symbol == IDENTIFIER) {
						nextSymFlag = myLexicalAnalysis.nextSym();
						if (nextSymFlag) {
							symbol = myLexicalAnalysis.getGlobalSymbol();
							if (symbol == LSBRACKET || symbol == LBBRACKET) {//( or {
								myLexicalAnalysis.resetPoint(point);
								myLexicalAnalysis.resetSymbol(symbolCode);
								break;
							}
						}
					}
				}
				//������Ǻ�������,�����,��������ķ���
				myLexicalAnalysis.resetPoint(point);
				myLexicalAnalysis.resetSymbol(symbolCode);
			}
		}
		if (!ZSQX_varDefinition(funcName))
			break;
		//;
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == SEMI)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon after var definition.");
			myLexicalAnalysis.setNextSym();
		}
	}
	return true;
}

/*���������壾 ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��)
			{,(����ʶ����|����ʶ������[�����޷�����������]�� )}*/
bool SyntaxAnalysis::ZSQX_varDefinition(string funcName) {
	bool nextSymFlag;
	SymbolCode symbol;
	ValueType valueType;
	string id;
	int length;
	//���ͱ�ʶ��
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == INTSY || symbol == CHARSY)) {
		return false;
	}

	valueType = (symbol == INTSY) ? IntType : CharType;

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier after type identifier.");
		return false;
	}
	//��ʶ��
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == IDENTIFIER)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after type identifier is not identifier.");
		myLexicalAnalysis.skipRead(';');
		return false;
	}

	id = myLexicalAnalysis.getGlobalString();

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		return true;
	}
	//ʶ���Ƿ���[
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == LMBRACKET) {
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack contents after [.");
			return false;
		}
		//�޷�������
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == INTNUM) {
			int number = myLexicalAnalysis.getGlobalNumber();
			if (number == 0) {//���벻Ϊ0
				myError.SyntaxAnalysisError(NotUnsignedIntegerError, getLineNumber());
				myLexicalAnalysis.skipRead(';');
				return false;
			}
		}
		else {
			myError.SyntaxAnalysisError(NotUnsignedIntegerError, getLineNumber());
			myLexicalAnalysis.skipRead(';');
			return false;
		}
		length = myLexicalAnalysis.getGlobalNumber();

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ] after int number.");
			return false;
		}
		//]
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == RMBRACKET)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after int number is not ].");
			myLexicalAnalysis.setNextSym();
		}
		pushItem(id, funcName, valueType, length);
		//����Ԥ��
		myLexicalAnalysis.nextSym();
	}
	else {
		pushItem(id, funcName, Variable, valueType);
	}
	//loop
	while (true) {
		//,
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == COMMA))
			break;

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier after comma");
			break;
		}
		//��ʶ��
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == IDENTIFIER)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after comma is not integer.");
			break;
		}
		id = myLexicalAnalysis.getGlobalString();

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			break;
		}
		//ʶ���Ƿ���[
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == LMBRACKET) {
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack contents after [.");
				break;
			}
			//�޷�������
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (symbol == INTNUM) {
				int number = myLexicalAnalysis.getGlobalNumber();
				if (number == 0) {//���벻Ϊ0
					myError.SyntaxAnalysisError(NotUnsignedIntegerError, getLineNumber());
					myLexicalAnalysis.skipRead(';');
					break;
				}
			}
			else {
				myError.SyntaxAnalysisError(NotUnsignedIntegerError, getLineNumber());
				myLexicalAnalysis.skipRead(';');
				break;
			}
			length = myLexicalAnalysis.getGlobalNumber();

			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ]");
				break;
			}
			//]
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == RMBRACKET)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ]");
				myLexicalAnalysis.setNextSym();
			}
			pushItem(id, funcName, valueType, length);
			//����Ԥ��
			myLexicalAnalysis.nextSym();
		}
		else {
			pushItem(id, funcName, Variable, valueType);
		}
	}

	return true;
}

//<�������岿��> ::= {���з���ֵ�������壾|���޷���ֵ�������壾}
bool SyntaxAnalysis::ZSQX_functionDefinition() {
	SymbolCode symbol;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == INTSY || symbol == CHARSY) {
		ZSQX_haveReturnValueFunctionDefinition();
	}
	else if (symbol == VOIDSY) {
		ZSQX_noReturnValueFunctionDefinition();
	}
	else {
		return false;
	}

	while (true) {
		if (myLexicalAnalysis.isFinish()) {
			break;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();

		if (symbol == INTSY || symbol == CHARSY) {
			ZSQX_haveReturnValueFunctionDefinition();
		}
		else if (symbol == VOIDSY) {
			ZSQX_noReturnValueFunctionDefinition();
		}
		else {
			break;
		}
	}
	return true;
}

/*���з���ֵ�������壾 ::= ������ͷ������(������������)�� ��{����������䣾��}��|������ͷ������{����������䣾��}��*/
bool SyntaxAnalysis::ZSQX_haveReturnValueFunctionDefinition() {
	bool nextSymFlag;
	SymbolCode symbol;
	string funcName;

	haveReturn = false;//����ÿһ�����ڷ���ֵ�ĺ���������Ϊfalse
	//����ͷ��
	ZSQX_declareHead();
	funcName = return_declare_funcName;

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ( or {.");
		return false;
	}
	//����Ƿ�Ϊ������
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == LSBRACKET) {
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack param table.");
			return false;
		}
		//������
		ZSQX_paramTable(funcName);

		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
			return false;
		}
		//)
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == RSBRACKET)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after param table need ).");
			myLexicalAnalysis.setNextSym();
		}
		//����Ԥ��
		myLexicalAnalysis.nextSym();
	}
	//{
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LBBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack {.");
		myLexicalAnalysis.setNextSym();
	}
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack compound statement.");
		return false;
	}
	//�������
	//��tmpVarCount����Ϊ0
	tmpVarCount = 0;
	ZSQX_compoundStatement(funcName);

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack }.");
		return false;
	}
	//}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RBBRACKET)) {
		myError.SyntaxAnalysisError(GeneralError, getLineNumber(), "is not } to end");
		myLexicalAnalysis.setNextSym();
	}
	//Ԥ��
	myLexicalAnalysis.nextSym();
	if (!haveReturn) {
		myError.ReturnStatementError(ExistNoneReturnError, getLineNumber(), funcName);
	}
	//�з���ֵ�ĺ�������Ҫ��
	return true;
}

//���޷���ֵ�������壾 ::= void����ʶ����(������������)����{����������䣾��}��| void����ʶ����{����������䣾��}��
bool SyntaxAnalysis::ZSQX_noReturnValueFunctionDefinition() {
	bool nextSymFlag;
	SymbolCode symbol;
	string funcName;
	//void
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == VOIDSY)) {
		return false;
	}

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack function name.");
		return false;
	}
	//��ʶ��
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == IDENTIFIER)) {
		if (symbol == MAINSY) {
			isMainVoid = true;
			return true;
		}
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack function name.");
		myLexicalAnalysis.skipRead(';');
		return false;
	}

	funcName = myLexicalAnalysis.getGlobalString();
	pushItem(funcName, "GLOBAL", VoidType);
	FourYuanItem insert;
	insert.type = TmpCodeType::FunctionDef;
	insert.funcType = VoidType;
	insert.target = funcName;
	globalTmpCodeArr.push_back(insert);

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(GeneralError, getLineNumber());
		return false;
	}
	//����Ƿ������
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == LSBRACKET) {
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after ( lack contents.");
			return false;
		}
		//������
		ZSQX_paramTable(funcName);

		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
			return false;
		}
		//)
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == RSBRACKET)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
			myLexicalAnalysis.setNextSym();
		}
		//����Ԥ��
		myLexicalAnalysis.nextSym();
	}
	//{
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LBBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack {.");
		myLexicalAnalysis.setNextSym();
	}
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack compound statement.");
		return false;
	}
	//�������
	//��tmpVarCount����Ϊ0
	tmpVarCount = 0;
	ZSQX_compoundStatement(funcName);

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack }.");
		return false;
	}
	//}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RBBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack }.");
		myLexicalAnalysis.setNextSym();
	}
	//Ԥ��
	myLexicalAnalysis.nextSym();
	FourYuanItem item;
	item.type = ReturnEmpty;
	globalTmpCodeArr.push_back(item);
	return true;
}

//�������� ::= �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}
bool SyntaxAnalysis::ZSQX_paramTable(string funcName) {
	bool nextSymFlag;
	SymbolCode symbol;
	ValueType valueType;
	string id;
	//���ͱ�ʶ�� int|char
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == INTSY || symbol == CHARSY)) {
		if (symbol == RSBRACKET)
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "param table can\'t be empty.");
		else {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "parament nedd start with int or char.");
			myLexicalAnalysis.skipRead(')');
		}
		return false;
	}

	valueType = (symbol == INTSY) ? IntType : CharType;

	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
		return false;
	}
	//��ʶ��
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == IDENTIFIER)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
		myLexicalAnalysis.skipRead(')');
		return false;
	}
	id = myLexicalAnalysis.getGlobalString();
	pushItem(id, funcName, Parament, valueType);
	FourYuanItem item;
	item.type = ParamDef;
	item.target = id;
	item.valueType = valueType;
	globalTmpCodeArr.push_back(item);

	while (true) {
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			break;
		}
		//,
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == COMMA)) {
			break;
		}

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack type identifier.");
			break;
		}
		//���ͱ�ʶ�� int|char
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == INTSY || symbol == CHARSY)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack type identifier.");
			myLexicalAnalysis.skipRead(')');
			break;
		}
		valueType = (symbol == INTSY) ? IntType : CharType;

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
			break;
		}
		//��ʶ��
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == IDENTIFIER)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
			myLexicalAnalysis.skipRead(')');
			break;
		}
		id = myLexicalAnalysis.getGlobalString();
		pushItem(id, funcName, Parament, valueType);
		FourYuanItem item;
		item.type = ParamDef;
		item.target = id;
		item.valueType = valueType;
		globalTmpCodeArr.push_back(item);
	}

	return true;
}

//��������䣾 ::= �ۣ�����˵�����ݣۣ�����˵�����ݣ�����䣾��
bool SyntaxAnalysis::ZSQX_compoundStatement(string funcName) {
	ZSQX_constDescription(funcName);
	ZSQX_varDescription(false, funcName);

	while (true) {
		if (!(ZSQX_statement(funcName, false, noUseCache, 1)))//��ʼΪ1
			break;
	}

	return true;
}

//�����ʽ�� ::= �ۣ������ݣ��{���ӷ�����������}
ExpRet SyntaxAnalysis::ZSQX_expression(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	bool x_flag = false;//��Ϊ�Ƿ���ǰ׺���flag

	//���ɴ����й�
	bool isSure = false;
	int expResult = 0;
	ValueType type;
	vector<PostfixItem> tar, obj;
	ExpRet returnValue;


	//����Ƿ���[+|-]
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == ADD || symbol == SUB) {
		x_flag = true;
		PostfixItem item;
		item.type = CharType;
		item.number = (symbol == ADD) ? '+' : '-';
		item.isNotOperator = false;
		tar.push_back(item);
		//����Ԥ��
		myLexicalAnalysis.nextSym();
	}
	//<��>
	ZSQX_item(tar, funcName, isCache, cache, weight);
	//ѭ��
	while (true) {
		if (myLexicalAnalysis.isFinish()) {
			break;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == ADD || symbol == SUB)) {
			break;
		}
		PostfixItem item;
		item.type = CharType;
		item.number = (symbol == ADD) ? '+' : '-';
		item.isNotOperator = false;
		tar.push_back(item);

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack item.");
			break;
		}
		//<��>
		ZSQX_item(tar, funcName, isCache, cache, weight);
	}
	//���ʽ����
	if (tar.size() == 0) {
		returnValue.isEmpty = true;
	}
	else
		returnValue.isEmpty = false;
	turnToPostfixExp(tar, obj);
	returnValue.name = calculateExp(obj, isSure, type, expResult, getLineNumber(), isCache, cache, funcName);
	returnValue.isSurable = isSure;
	returnValue.type = type;
	if (isSure) {
		if (type == IntType)
			returnValue.number = expResult;
		else
			returnValue.character = expResult;
	}
	return returnValue;
}

//��� ::= �����ӣ�{���˷�������������ӣ�}
bool SyntaxAnalysis::ZSQX_item(vector<PostfixItem>& obj, string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	ZSQX_factor(obj, funcName, isCache, cache, weight);

	while (true) {
		if (myLexicalAnalysis.isFinish()) {
			break;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == MULT || symbol == DIV)) {
			break;
		}
		PostfixItem item;
		item.type = CharType;
		item.number = (symbol == MULT) ? '*' : '/';
		item.isNotOperator = false;
		obj.push_back(item);
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack factor.");
			break;
		}
		//<����>
		ZSQX_factor(obj, funcName, isCache, cache, weight);
	}
	return true;
}

//�����ӣ� ::= ����ʶ����[��(��<ֵ������>��)��]������ʶ������[�������ʽ����]��|��(�������ʽ����)������������|���ַ���
bool SyntaxAnalysis::ZSQX_factor(vector<PostfixItem>& obj, string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	bool preRead = true;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	PostfixItem item;
	ExpRet item2;
	FourYuanItem item3;
	string id;

	switch (symbol) {
	case IDENTIFIER:
		id = myLexicalAnalysis.getGlobalString();
		//��ӵĵط�
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			preRead = false;
			break;
		}
		//��ʶ���Ѿ�ʶ��
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == LMBRACKET) {//[
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression after [.");
				return false;
			}
			//<���ʽ>
			item2 = ZSQX_expression(funcName, isCache, cache, weight);
			string index2;
			int orderx;
			if (item2.isSurable) {
				item3.type = AssignState;
				item3.target = generateVar();
				item3.isLeftArr = false;
				item3.isTargetArr = false;
				if (item2.type == CharType) {
					orderx = idArrExpCheck(id, funcName, true, item2.character);
					char gv[10] = { '\0' };
					sprintf(gv, "%d", item2.character);
					item3.left = gv;
					item3.op = '+';
					item3.right = "0";
				}
				else {
					orderx = idArrExpCheck(id, funcName, true, item2.number);
					char gv[10] = { '\0' };
					sprintf(gv, "%d", item2.number);
					item3.left = gv;
					item3.op = '+';
					item3.right = "0";
				}
				if (isCache) {
					cache.push_back(item3);
				}
				else {
					globalTmpCodeArr.push_back(item3);
				}
				index2 = item3.target;
			}
			else {
				orderx = idArrExpCheck(id, funcName, false);
				if (item2.name.size() > 0 && item2.name.at(0) == 'T') {
					item3.type = AssignState;
					item3.target = generateVar();
					item3.isTargetArr = item3.isLeftArr = false;
					item3.left = item2.name;
					item3.op = '+';
					item3.right = "0";
					if (isCache) {
						cache.push_back(item3);
					}
					else {
						globalTmpCodeArr.push_back(item3);
					}
					index2 = item3.target;
				}
				else {
					index2 = item2.name;
				}
			}
			if (orderx >= 0) {
				item3.type = AssignState;
				item3.target = generateVar();
				item3.isLeftArr = true;
				item3.isTargetArr = false;
				char ggg[10] = { '\0' };
				sprintf(ggg, "%d", orderx);
				item3.left = "G" + string(ggg) + id;
				item3.index2 = index2;
				if (isCache) {
					cache.push_back(item3);
				}
				else {
					globalTmpCodeArr.push_back(item3);
				}

				item.type = StringType;
				SymbolTableItem t = globalSymbolTable.at(orderx);
				item.isNotCharVar = true;
				if (t.getValueType() == CharType)
					item.isNotCharVar = false;
				item.str = item3.target;
				obj.push_back(item);
			}
			else {
				item.type = StringType;
				item.str = id;
			}

			if (myLexicalAnalysis.isFinish()) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ].");
				return false;
			}
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == RMBRACKET)) {//]
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ].");
				myLexicalAnalysis.setNextSym();
			}
		}
		else if (symbol == LSBRACKET) {//(
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack param table.");
				return false;
			}
			//<ֵ������>
			vector<ValueType> retParamTable = ZSQX_valueParamTable(funcName, isCache, cache, weight);
			funcCallCheck(id, true, retParamTable);
			item3.type = FunctionCall;
			item3.target = id;
			if (isCache) {
				cache.push_back(item3);
			}
			else {
				globalTmpCodeArr.push_back(item3);
			}
			item3.type = AssignState;
			item3.target = generateVar();
			item3.isTargetArr = item3.isLeftArr = false;
			item3.left = "Ret";
			item3.op = '+';
			item3.right = "0";
			if (isCache) {
				cache.push_back(item3);
			}
			else {
				globalTmpCodeArr.push_back(item3);
			}
			//������:��麯���Ƿ����,���������ڷ��ű��order,�����䷵�����;���isChar��ֵ
			item.type = StringType;
			item.str = item3.target;
			item.isNotCharVar = true;
			obj.push_back(item);

			if (myLexicalAnalysis.isFinish()) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
				return false;
			}
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == RSBRACKET)) {//)
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
				myLexicalAnalysis.setNextSym();
			}
		}
		else {//�����Ǳ�ʶ��,����Ԥ��
			preRead = false;
			int order = idCheckInFactor(id, funcName);
			if (order >= 0) {
				//��Ҫ����Ƿ����޲εĺ���
				SymbolTableItem y = globalSymbolTable.at(order);
				if (y.getItemType() == Constant) {
					item.type = y.getValueType();
					item.number = (item.type == IntType) ? (y.getConstInt()) : (y.getConstChar());
					if (item.type == CharType)
						item.isNotOperator = true;
				}
				else if (y.getItemType() == Function) {//�޲����Ĵ�����ֵ�ĺ�������

					item3.type = FunctionCall;
					item3.target = id;
					if (isCache) {
						cache.push_back(item3);
					}
					else {
						globalTmpCodeArr.push_back(item3);
					}
					item3.type = AssignState;
					item3.target = generateVar();
					item3.isTargetArr = item3.isLeftArr = false;
					item3.left = "Ret";
					item3.op = '+';
					item3.right = "0";
					if (isCache) {
						cache.push_back(item3);
					}
					else {
						globalTmpCodeArr.push_back(item3);
					}
					item.type = StringType;
					item.str = item3.target;
					item.isNotCharVar = (y.getFuncType() == ReturnCharType) ? false : true;
				}
				else {
					item.isNotCharVar = true;
					if ((y.getItemType() == Variable || y.getItemType() == Parament) && y.getValueType() == CharType)
						item.isNotCharVar = false;
					item.type = StringType;
					char ggg[10] = { '\0' };
					sprintf(ggg, "%d", order);
					item.str = "G" + string(ggg) + id;;//G12ppp����
					addWeight(order, weight);
				}
			}
			else {
				item.type = StringType;
				item.isNotCharVar = true;
				item.str = id;
				if (isCache) {
					cache.push_back(item3);
				}
				else {
					globalTmpCodeArr.push_back(item3);
				}
			}
			obj.push_back(item);
		}
		break;
	case INTNUM:
	case ADD:
	case SUB:
		ZSQX_integer();
		item.type = IntType;
		item.number = return_integer;
		obj.push_back(item);
		preRead = false;
		break;
	case CHAR://ֱ����ȷ
		item.type = CharType;
		item.number = myLexicalAnalysis.getGlobalChar();
		item.isNotOperator = true;
		obj.push_back(item);
		break;
	case LSBRACKET:
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "only (.");
			return false;
		}
		//<���ʽ
		item2 = ZSQX_expression(funcName, isCache, cache, weight);
		if (item2.isSurable) {
			item.type = IntType;
			item.number = (item2.type == IntType) ? item2.number : item2.character;
		}
		else {
			item.type = StringType;
			if (item2.name.size() > 0 && item2.name.at(0) == 'T') {
				item3.type = AssignState;
				item3.target = generateVar();
				item3.isTargetArr = item3.isLeftArr = false;
				item3.left = item2.name;
				item3.op = '+';
				item3.right = "0";
				if (isCache) {
					cache.push_back(item3);
				}
				else {
					globalTmpCodeArr.push_back(item3);
				}
				item.str = item3.target;
			}
			else {
				item.str = item2.name;
			}
			item.isNotCharVar = (item2.type == IntType) ? true : false;
		}
		obj.push_back(item);
		//)
		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == RSBRACKET)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
			myLexicalAnalysis.setNextSym();
		}
		break;
	default:
		return false;
		break;
	}

	//Ԥ��
	if (preRead)
		myLexicalAnalysis.nextSym();
	return true;
}

/*����䣾 ::= ��������䣾����ѭ����䣾| ��{��������䣾����}��������ʶ����[��(��<ֵ������>��)��];
		������ֵ��䣾;��������䣾;����д��䣾;��;|�������䣾����������䣾;*/
bool SyntaxAnalysis::ZSQX_statement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	bool preRead = true;
	string id;
	FourYuanItem four;
	four.type = FunctionCall;

	switch (symbol) {
	case IFSY:
		ZSQX_conditionStatement(funcName, isCache, cache, weight);
		preRead = false;
		break;
	case WHILESY:
		ZSQX_loopStatement(funcName, isCache, cache, 10 * weight);
		preRead = false;
		break;
	case LBBRACKET:
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			break;
		}
		while (true) {
			//<���>
			if (!ZSQX_statement(funcName, isCache, cache, weight))
				break;
			if (myLexicalAnalysis.isFinish()) {
				break;
			}
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == RBBRACKET)) { //}
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack }.");
			myLexicalAnalysis.setNextSym();
		}
		break;
	case IDENTIFIER://��ֵ�����������
		id = myLexicalAnalysis.getGlobalString();
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {//�����ı�ʶ��
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "not an illegal statement.");
			myLexicalAnalysis.skipRead('\n');
			break;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == SEMI) {//�޲ε���
			idCheckInState(id);//����Ƿ��Ǻ���
			four.target = id;
			if (isCache) {
				cache.push_back(four);
			}
			else {
				globalTmpCodeArr.push_back(four);
			}
			break;
		}
		else if (symbol == LSBRACKET) {//�вε���
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack param table.");
				return false;
			}
			//<ֵ������>
			vector<ValueType> retParamTable = ZSQX_valueParamTable(funcName, isCache, cache, weight);
			funcCallCheck(id, false, retParamTable);
			four.target = id;
			if (isCache) {
				cache.push_back(four);
			}
			else {
				globalTmpCodeArr.push_back(four);
			}

			if (myLexicalAnalysis.isFinish()) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
				return false;
			}
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == RSBRACKET)) {//)
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
				myLexicalAnalysis.setNextSym();
			}

			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
				return false;
			}
			//;
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == SEMI)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
				myLexicalAnalysis.setNextSym();
			}
		}
		else if (symbol == ASSIGN || symbol == LMBRACKET) {//��ֵ���ʶ��ǰ׺(ȥ����ʶ��)
			ZSQX_assignStatement(funcName, id, isCache, cache, weight);

			if (myLexicalAnalysis.isFinish()) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
				return false;
			}
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == SEMI)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
				myLexicalAnalysis.setNextSym();
			}
		}
		else {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "illegal content after statement identifier.");
			myLexicalAnalysis.skipRead('\n');
			return false;
		}
		break;
	case SCANFSY:
		ZSQX_readStatement(funcName, isCache, cache, weight);

		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == SEMI)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
			myLexicalAnalysis.setNextSym();
		}
		break;
	case PRINTFSY:
		ZSQX_writeStatement(funcName, isCache, cache, weight);

		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == SEMI)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
			myLexicalAnalysis.setNextSym();
		}
		break;
	case SEMI://�����
		break;
	case SWITCHSY:
		ZSQX_situationStatement(funcName, isCache, cache, weight);

		preRead = false;
		break;
	case RETURNSY:
		ZSQX_returnStatement(funcName, isCache, cache, weight);

		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == SEMI)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack semicolon.");
			myLexicalAnalysis.skipRead(';');
			myLexicalAnalysis.setNextSym();
		}
		break;
	default:
		return false;
		break;
	}
	//Ԥ��
	if (preRead)
		myLexicalAnalysis.nextSym();
	return true;
}

//����ֵ��䣾 ::= ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
//ʵ�ʷ�������  �������ʽ��|��[�������ʽ����]��=�����ʽ��
bool SyntaxAnalysis::ZSQX_assignStatement(string funcName, string id, bool isCache, vector<FourYuanItem>& cache, int weight) {
	//���������ʶ��,�Ѿ�������з����ó�
	bool nextSymFlag;
	SymbolCode symbol;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	FourYuanItem fourItem;
	fourItem.type = AssignState;
	fourItem.isTargetArr = fourItem.isLeftArr = false;
	fourItem.op = '+';
	fourItem.right = "0";

	if (symbol == ASSIGN) {//=
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
			return false;
		}
		int order = checkAssignId(id, funcName);
		addWeight(order, weight);
		//<���ʽ>
		ExpRet ret = ZSQX_expression(funcName, isCache, cache, weight);
		if (order >= 0)
			checkTypeMatch(globalSymbolTable.at(order).getValueType(), ret.type);
		char ggg[10] = { '\0' };
		sprintf(ggg, "%d", order);
		fourItem.target = (order >= 0) ? ("G" + string(ggg) + id) : id;
		if (ret.isSurable) {
			char x[15] = { '\0' };
			sprintf(x, "%d", (ret.type == IntType) ? ret.number : ret.character);
			fourItem.left = x;
		}
		else {
			if (ret.name.size() > 0 && ret.name.at(0) == 'T') {
				FourYuanItem fourItem1;
				fourItem1.type = AssignState;
				fourItem1.target = generateVar();
				fourItem1.isTargetArr = fourItem1.isLeftArr = false;
				fourItem1.left = ret.name;
				fourItem1.op = '+';
				fourItem1.right = "0";
				if (isCache) {
					cache.push_back(fourItem1);
				}
				else {
					globalTmpCodeArr.push_back(fourItem1);
				}
				fourItem.left = fourItem1.target;
			}
			else {
				fourItem.left = ret.name;
			}
		}
		if (isCache) {
			cache.push_back(fourItem);
		}
		else {
			globalTmpCodeArr.push_back(fourItem);
		}

	}
	else if (symbol == LMBRACKET) {//[
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
			return false;
		}
		//<���ʽ>
		fourItem.isTargetArr = true;
		ExpRet ret = ZSQX_expression(funcName, isCache, cache, weight);
		int orderx;
		if (ret.isSurable) {
			if (ret.type == CharType) {
				orderx = idArrExpCheck(id, funcName, true, ret.character);
				char x[15] = { '\0' };
				sprintf(x, "%d", ret.character);
				fourItem.index1 = x;
			}
			else {
				orderx = idArrExpCheck(id, funcName, true, ret.number);
				char x[15] = { '\0' };
				sprintf(x, "%d", ret.number);
				fourItem.index1 = x;
			}
		}
		else {
			orderx = idArrExpCheck(id, funcName, false);
			if (ret.name.size() > 0 && ret.name.at(0) == 'T') {
				FourYuanItem fourItem1;
				fourItem1.type = AssignState;
				fourItem1.target = generateVar();
				fourItem1.isTargetArr = fourItem1.isLeftArr = false;
				fourItem1.left = ret.name;
				fourItem1.op = '+';
				fourItem1.right = "0";
				if (isCache) {
					cache.push_back(fourItem1);
				}
				else {
					globalTmpCodeArr.push_back(fourItem1);
				}
				fourItem.index1 = fourItem1.target;
			}
			else {
				fourItem.index1 = ret.name;
			}
		}
		if (orderx >= 0) {
			char ggg[10] = { '\0' };
			sprintf(ggg, "%d", orderx);
			fourItem.target = "G" + string(ggg) + id;
		}

		//]
		if (myLexicalAnalysis.isFinish()) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ].");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == RMBRACKET)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ].");
			myLexicalAnalysis.setNextSym();
		}
		//assign =
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack =.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == ASSIGN)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack =.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}
		//������� = <���ʽ>
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
			return false;
		}
		//<���ʽ>
		ret = ZSQX_expression(funcName, isCache, cache, weight);
		if (ret.isSurable) {
			char x[15] = { '\0' };
			sprintf(x, "%d", ret.type == IntType ? ret.number : ret.character);
			fourItem.left = x;
		}
		else {
			if (ret.name.size() > 0 && ret.name.at(0) == 'T') {
				FourYuanItem fourItem1;
				fourItem1.type = AssignState;
				fourItem1.target = generateVar();
				fourItem1.isTargetArr = fourItem1.isLeftArr = false;
				fourItem1.left = ret.name;
				fourItem1.op = '+';
				fourItem1.right = "0";
				if (isCache) {
					cache.push_back(fourItem1);
				}
				else {
					globalTmpCodeArr.push_back(fourItem1);
				}
				fourItem.left = fourItem1.target;
			}
			else {
				fourItem.left = ret.name;
			}
		}
		checkTypeMatch(globalSymbolTable.at(orderx).getValueType(), ret.type);
		if (isCache) {
			cache.push_back(fourItem);
		}
		else {
			globalTmpCodeArr.push_back(fourItem);
		}
	}
	else {
		return false;
	}
	return true;
}

//��������䣾::= if ��(������������)������䣾else����䣾
bool SyntaxAnalysis::ZSQX_conditionStatement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	FourYuanItem four;
	symbol = myLexicalAnalysis.getGlobalSymbol();

	string label1 = generateLabel();
	string label2 = generateLabel();

	//int tmpVarCountCopy = tmpVarCount;

	//if
	if (!(symbol == IFSY)) {
		return false;
	}
	//ʶ��(
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		myLexicalAnalysis.setNextSym();
	}
	//ʶ��<����>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack condition.");
		return false;
	}

	string judge = ZSQX_condition(funcName, isCache, cache, weight);
	//������ת���
	switch (relation) {
	case LESS:
		four.type = BGEZ;
		four.target = label1;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case LESSEQ:
		four.type = BGZ;
		four.target = label1;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case EQUAL:
		four.type = BNZ;
		four.target = label1;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case MORE:
		four.type = BLEZ;
		four.target = label1;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case MOREEQ:
		four.type = BLZ;
		four.target = label1;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case NOTEQ:
		four.type = BEZ;
		four.target = label1;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	}

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		return false;
	}
	//)
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		myLexicalAnalysis.setNextSym();
	}

	//����<���>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack statement.");
		return false;
	}
	//
	tmpVarCount = 0;
	ZSQX_statement(funcName, isCache, cache, weight);
	//�������������ת
	four.type = Jump;
	four.target = label2;
	if (isCache) {
		cache.push_back(four);
	}
	else {
		globalTmpCodeArr.push_back(four);
	}

	four.type = Label;
	four.target = label1;
	if (isCache) {
		cache.push_back(four);
	}
	else {
		globalTmpCodeArr.push_back(four);
	}

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack else.");
		return false;
	}
	//else
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == ELSESY)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack else.");
		myLexicalAnalysis.skipRead('\n');
		return false;
	}

	//����<���>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack statement.");
		return false;
	}
	ZSQX_statement(funcName, isCache, cache, weight);
	tmpVarCount = 0;
	four.type = Label;
	four.target = label2;
	if (isCache) {
		cache.push_back(four);
	}
	else {
		globalTmpCodeArr.push_back(four);
	}
	//tmpVarCount = tmpVarCountCopy;
	return true;
}

//�������� ::= �����ʽ������ϵ������������ʽ���������ʽ�� 
//����ϵ������� ::= <��<=��>��>=��!=��==
string SyntaxAnalysis::ZSQX_condition(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	FourYuanItem fourItem;
	string  left, right;

	//int copy = tmpVarCount;

	//<���ʽ>
	ExpRet ret1 = ZSQX_expression(funcName, isCache, cache, weight);
	if (ret1.isSurable) {
		fourItem.type = AssignState;
		fourItem.isLeftArr = fourItem.isTargetArr = false;
		char x[15] = { '\0' };
		sprintf(x, "%d", ret1.type == IntType ? ret1.number : ret1.character);
		fourItem.target = generateVar();
		fourItem.left = x;
		fourItem.op = '+';
		fourItem.right = "0";
		if (isCache) {
			cache.push_back(fourItem);
		}
		else {
			globalTmpCodeArr.push_back(fourItem);
		}
		left = fourItem.target;
	}
	else {
		if (ret1.name.size() > 0 && ret1.name.at(0) == 'T') {
			FourYuanItem fourItem1;
			fourItem1.type = AssignState;
			fourItem1.target = generateVar();
			fourItem1.isTargetArr = fourItem1.isLeftArr = false;
			fourItem1.left = ret1.name;
			fourItem1.op = '+';
			fourItem1.right = "0";
			if (isCache) {
				cache.push_back(fourItem1);
			}
			else {
				globalTmpCodeArr.push_back(fourItem1);
			}
			left = fourItem1.target;
			fourItem.target = left;
		}
		else {
			left = ret1.name;
			fourItem.target = ret1.name;
		}
	}

	if (!myLexicalAnalysis.isFinish()) {
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol >= LESS && symbol <= MORE) {
			//��ʶ���ϵ�����
			relation = symbol;
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack statement.");
				return "";
			}
			//<���ʽ>
			ExpRet ret2 = ZSQX_expression(funcName, isCache, cache, weight);
			if (ret2.isSurable) {
				fourItem.type = AssignState;
				fourItem.isLeftArr = fourItem.isTargetArr = false;
				char x[15] = { '\0' };
				sprintf(x, "%d", ret2.type == IntType ? ret2.number : ret2.character);
				fourItem.target = generateVar();
				fourItem.left = x;
				fourItem.op = '+';
				fourItem.right = "0";
				if (isCache) {
					cache.push_back(fourItem);
				}
				else {
					globalTmpCodeArr.push_back(fourItem);
				}
				right = fourItem.target;
			}
			else {
				if (ret2.name.size() > 0 && ret2.name.at(0) == 'T') {
					FourYuanItem fourItem1;
					fourItem1.type = AssignState;
					fourItem1.target = generateVar();
					fourItem1.isTargetArr = fourItem1.isLeftArr = false;
					fourItem1.left = ret2.name;
					fourItem1.op = '+';
					fourItem1.right = "0";
					if (isCache) {
						cache.push_back(fourItem1);
					}
					else {
						globalTmpCodeArr.push_back(fourItem1);
					}
					right = fourItem1.target;
				}
				else {
					right = ret2.name;
				}
			}
			fourItem.target = generateVar();
			fourItem.type = AssignState;
			fourItem.isLeftArr = fourItem.isTargetArr = false;
			fourItem.left = left;
			fourItem.right = right;
			fourItem.op = '-';
			if (isCache) {
				cache.push_back(fourItem);
			}
			else {
				globalTmpCodeArr.push_back(fourItem);
			}
			//tmpVarCount = copy;
			tmpVarCount = 0;
			return fourItem.target;
		}

	}
	relation = NOTEQ;
	//tmpVarCount = copy;
	tmpVarCount = 0;
	return fourItem.target;
}

//��ѭ����䣾 ::= while ��(������������)������䣾
bool SyntaxAnalysis::ZSQX_loopStatement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;

	FourYuanItem four;

	string label1 = generateLabel();
	string label2 = generateLabel();

	//int tmpVarCountCopy = tmpVarCount;

	//while
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == WHILESY)) {
		return false;
	}

	four.type = Label;
	four.target = label1;
	if (isCache) {
		cache.push_back(four);
	}
	else {
		globalTmpCodeArr.push_back(four);
	}


	//����'('
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		myLexicalAnalysis.setNextSym();
	}
	//ʶ��<����>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack condition.");
		return false;
	}
	string judge = ZSQX_condition(funcName, isCache, cache, weight);
	//����������ת
	switch (relation) {
	case LESS:
		four.type = BGEZ;
		four.target = label2;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case LESSEQ:
		four.type = BGZ;
		four.target = label2;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case EQUAL:
		four.type = BNZ;
		four.target = label2;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case MORE:
		four.type = BLEZ;
		four.target = label2;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case MOREEQ:
		four.type = BLZ;
		four.target = label2;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	case NOTEQ:
		four.type = BEZ;
		four.target = label2;
		four.left = judge;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		break;
	}

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		return false;
	}
	//)
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		myLexicalAnalysis.setNextSym();
	}

	//<���>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack statement.");
		return false;
	}

	ZSQX_statement(funcName, isCache, cache, weight);
	four.type = Jump;//��������ת��ѭ��
	four.target = label1;
	if (isCache) {
		cache.push_back(four);
	}
	else {
		globalTmpCodeArr.push_back(four);
	}
	//ѭ��������
	four.type = Label;
	four.target = label2;
	if (isCache) {
		cache.push_back(four);
	}
	else {
		globalTmpCodeArr.push_back(four);
	}
	//tmpVarCount = tmpVarCountCopy;
	return true;
}

//�������䣾 ::= switch ��(�������ʽ����)�� ��{���������[��ȱʡ��] ��}��
bool SyntaxAnalysis::ZSQX_situationStatement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	string endLabel = generateLabel();//switch������

	FourYuanItem fourItem;
	string left;
	vector<FourYuanItem> myCache;

	int tmpVarCountCopy = tmpVarCount;

	//switch
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == SWITCHSY)) {
		return false;
	}

	//����'('
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		myLexicalAnalysis.setNextSym();
	}
	//����<���ʽ>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
		return false;
	}
	ExpRet ret = ZSQX_expression(funcName, isCache, cache, weight);
	if (ret.isSurable) {
		left = generateVar();
		fourItem.type = AssignState;
		fourItem.isTargetArr = fourItem.isLeftArr = false;
		fourItem.target = left;
		char x[15] = { '\0' };
		sprintf(x, "%d", ret.type == IntType ? ret.number : ret.character);
		fourItem.left = x;
		fourItem.op = '+';
		fourItem.right = "0";
		if (isCache) {
			cache.push_back(fourItem);
		}
		else {
			globalTmpCodeArr.push_back(fourItem);
		}
	}
	else {
		if (ret.name.size() > 0 && ret.name.at(0) == 'T') {
			FourYuanItem fourItem1;
			fourItem1.type = AssignState;
			fourItem1.target = generateVar();
			fourItem1.isTargetArr = fourItem1.isLeftArr = false;
			fourItem1.left = ret.name;
			fourItem1.op = '+';
			fourItem1.right = "0";
			if (isCache) {
				cache.push_back(fourItem1);
			}
			else {
				globalTmpCodeArr.push_back(fourItem1);
			}
			left = fourItem1.target;
		}
		else {
			left = ret.name;
		}
	}
	//leftΪ�Ƚ�case�Ĺؼ�����

	//����')'
	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		myLexicalAnalysis.setNextSym();
	}
	//����'{'
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack {.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LBBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack {.");
		myLexicalAnalysis.setNextSym();
	}
	//�����������[��ȱʡ��]
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack situation table.");
		return false;
	}
	//��תlabel�ĸ����м�ָ��,ͬʱȡ��case�е�����ֵȥ�鿴�Ƿ����ֵͬ���
	vector<CaseRet> caseTable = ZSQX_situationTable(funcName, endLabel, ret.type, myCache, weight);
	vector<int> cases;
	for (unsigned int i = 0; i < caseTable.size(); i++) {
		CaseRet t = caseTable.at(i);
		cases.push_back(t.constValue);
		fourItem.type = AssignState;
		fourItem.isLeftArr = fourItem.isTargetArr = false;
		fourItem.target = generateVar();
		fourItem.op = '-';
		fourItem.left = left;
		char x[15] = { '\0' };
		sprintf(x, "%d", t.constValue);
		fourItem.right = x;
		if (isCache) {
			cache.push_back(fourItem);
		}
		else {
			globalTmpCodeArr.push_back(fourItem);
		}// judge - const
		fourItem.type = BEZ;
		fourItem.left = fourItem.target;
		fourItem.target = t.label;
		if (isCache) {
			cache.push_back(fourItem);
		}
		else {
			globalTmpCodeArr.push_back(fourItem);
		}
	}
	checkCase(cases);
	//�����Ƿ���ȱʡ
	if (!myLexicalAnalysis.isFinish()) {
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == DEFAULTSY) {
			ZSQX_default(funcName, isCache, cache, weight);
		}
	}
	//�����ת��endLabel��ָ��
	fourItem.type = Jump;
	fourItem.target = endLabel;
	if (isCache) {
		cache.push_back(fourItem);
	}
	else {
		globalTmpCodeArr.push_back(fourItem);
	}
	//case���ֻ����м����д��
	for (unsigned int i = 0; i < myCache.size(); i++) {
		if (isCache) {
			cache.push_back(myCache.at(i));
		}
		else {
			globalTmpCodeArr.push_back(myCache.at(i));
		}
	}
	//endLabel����
	fourItem.type = Label;
	fourItem.target = endLabel;
	if (isCache) {
		cache.push_back(fourItem);
	}
	else {
		globalTmpCodeArr.push_back(fourItem);
	}
	//����'}'
	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack }.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RBBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack }.");
		myLexicalAnalysis.setNextSym();
	}
	//Ԥ��
	myLexicalAnalysis.nextSym();
	//tmpVarCount = tmpVarCountCopy;
	tmpVarCount = 0;
	return true;
}

//������� ::= ���������䣾{���������䣾}
vector<CaseRet> SyntaxAnalysis::ZSQX_situationTable(string funcName, string endLabel, ValueType type, vector<FourYuanItem>& cache, int weight) {
	//������������
	CaseRet ret = ZSQX_situationSonStatement(funcName, endLabel, type, cache, weight);
	vector<CaseRet> caseTable;
	caseTable.push_back(ret);

	int copy = tmpVarCount;

	while (true) {
		if (myLexicalAnalysis.isFinish())
			break;
		CaseRet ret = ZSQX_situationSonStatement(funcName, endLabel, type, cache, weight);
		if (!ret.recognize) {
			break;
		}
		caseTable.push_back(ret);
	}
	tmpVarCount = copy;
	return caseTable;
}

//���������䣾 ::= case��������������䣾
CaseRet SyntaxAnalysis::ZSQX_situationSonStatement(string funcName, string endLabel, ValueType type, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	CaseRet ret;
	FourYuanItem fourItem;

	ret.constValue = 0;
	ret.label = generateLabel();
	ret.recognize = false;
	//����case
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == CASESY)) {
		return ret;
	}

	//����<����>--> <����>|<�ַ�>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack const.");
		return ret;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	switch (symbol) {
	case INTNUM:
	case ADD:
	case SUB:
		ZSQX_integer();
		ret.constValue = return_integer;
		checkSwitchType(type, IntType);
		break;
	case CHAR:
		//����
		ret.constValue = myLexicalAnalysis.getGlobalChar();
		checkSwitchType(type, CharType);
		myLexicalAnalysis.nextSym();
		break;
	default:
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack const.");
		myLexicalAnalysis.skipRead('\n');
		return ret;
		break;
	}

	//����:
	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack colon.");
		return ret;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == COLON)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack colon.");
		myLexicalAnalysis.setNextSym();
	}
	//����<���>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack statement.");
		return ret;
	}
	//�����ǰ����label
	fourItem.type = Label;
	fourItem.target = ret.label;
	cache.push_back(fourItem);

	ZSQX_statement(funcName, true, cache, weight);
	//����jump��endLabel�Ĵ���
	fourItem.type = Jump;
	fourItem.target = endLabel;
	cache.push_back(fourItem);
	ret.recognize = true;
	return ret;
}

//��ȱʡ�� ::= default : ����䣾
bool SyntaxAnalysis::ZSQX_default(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;

	//����default
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == DEFAULTSY)) {
		return false;
	}

	//����:
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack :.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == COLON)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack :.");
		myLexicalAnalysis.setNextSym();
	}
	//����<���>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack statement.");
		return false;
	}

	ZSQX_statement(funcName, isCache, cache, weight);
	return true;

}

//��ֵ������ ::= �����ʽ��{,�����ʽ��}
vector<ValueType> SyntaxAnalysis::ZSQX_valueParamTable(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	ExpRet ret;
	FourYuanItem four;

	vector<string> paramTable;
	vector<ValueType> retParamTable;
	//����<���ʽ>
	ret = ZSQX_expression(funcName, isCache, cache, weight);
	if (!ret.isEmpty)
		retParamTable.push_back(ret.type);
	else {
		return retParamTable;
	}
	if (ret.isSurable) {
		char x[15] = { '\0' };
		sprintf(x, "%d", ret.type == IntType ? ret.number : ret.character);
		four.type = AssignState;
		four.isLeftArr = four.isTargetArr = false;
		four.target = generateVar();
		four.op = '+';
		four.right = "0";
		four.left = x;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		paramTable.push_back(four.target);
	}
	else {
		if (ret.name.size() > 0 && ret.name.at(0) == 'T') {
			FourYuanItem fourItem1;
			fourItem1.type = AssignState;
			fourItem1.target = generateVar();
			fourItem1.isTargetArr = fourItem1.isLeftArr = false;
			fourItem1.left = ret.name;
			fourItem1.op = '+';
			fourItem1.right = "0";
			if (isCache) {
				cache.push_back(fourItem1);
			}
			else {
				globalTmpCodeArr.push_back(fourItem1);
			}
			paramTable.push_back(fourItem1.target);
		}
		else {
			paramTable.push_back(ret.name);
		}
	}

	while (true) {
		if (myLexicalAnalysis.isFinish()) {
			break;
		}
		//����,
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == COMMA)) {
			break;
		}
		//����<���ʽ>
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
			break;
		}

		ret = ZSQX_expression(funcName, isCache, cache, weight);
		retParamTable.push_back(ret.type);
		if (ret.isSurable) {
			char x[15] = { '\0' };
			sprintf(x, "%d", ret.type == IntType ? ret.number : ret.character);
			four.type = AssignState;
			four.isLeftArr = four.isTargetArr = false;
			four.target = generateVar();
			four.op = '+';
			four.right = "0";
			four.left = x;
			if (isCache) {
				cache.push_back(four);
			}
			else {
				globalTmpCodeArr.push_back(four);
			}
			paramTable.push_back(four.target);
		}
		else {
			if (ret.name.size() > 0 && ret.name.at(0) == 'T') {
				FourYuanItem fourItem1;
				fourItem1.type = AssignState;
				fourItem1.target = generateVar();
				fourItem1.isTargetArr = fourItem1.isLeftArr = false;
				fourItem1.left = ret.name;
				fourItem1.op = '+';
				fourItem1.right = "0";
				if (isCache) {
					cache.push_back(fourItem1);
				}
				else {
					globalTmpCodeArr.push_back(fourItem1);
				}
				paramTable.push_back(fourItem1.target);
			}
			else {
				paramTable.push_back(ret.name);
			}
		}
	}
	for (unsigned int i = 0; i < paramTable.size(); i++) {
		four.type = ValueParamDeliver;
		four.target = paramTable.at(i);
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
	}

	return retParamTable;
}


//������䣾 ::= scanf ��(������ʶ����{,����ʶ����}��)��
bool SyntaxAnalysis::ZSQX_readStatement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	FourYuanItem four;
	int order;
	string id;
	//����scanf
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == SCANFSY)) {
		return false;
	}
	//����'('
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		myLexicalAnalysis.setNextSym();
	}
	//����<��ʶ��>
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == IDENTIFIER)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
		myLexicalAnalysis.skipRead(';');
		return false;
	}

	id = myLexicalAnalysis.getGlobalString();
	order = checkAssignId(id, funcName);
	addWeight(order, weight);
	if (order >= 0) {
		four.type = (globalSymbolTable.at(order).getValueType() == IntType) ? ReadInt : ReadChar;
		char ggg[10] = { '\0' };
		sprintf(ggg, "%d", order);
		four.target = "G" + string(ggg) + id;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
	}//û�ҵ�,������

	while (true) {
		//����,
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			break;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == COMMA)) {
			break;
		}
		//����<��ʶ��>
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
			break;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == IDENTIFIER)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
			myLexicalAnalysis.skipRead(';');
			break;
		}

		id = myLexicalAnalysis.getGlobalString();
		order = checkAssignId(id, funcName);
		addWeight(order, weight);
		if (order >= 0) {
			four.type = (globalSymbolTable.at(order).getValueType() == IntType) ? ReadInt : ReadChar;
			char ggg[10] = { '\0' };
			sprintf(ggg, "%d", order);
			four.target = "G" + string(ggg) + id;
			if (isCache) {
				cache.push_back(four);
			}
			else {
				globalTmpCodeArr.push_back(four);
			}
		}//û�ҵ�,������
	}

	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		myLexicalAnalysis.setNextSym();
	}
	//Ԥ��
	myLexicalAnalysis.nextSym();
	return true;
}

//��д��䣾 ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
bool SyntaxAnalysis::ZSQX_writeStatement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	FourYuanItem four;

	int tmpVarCountCopy = tmpVarCount;

	//����printf
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == PRINTFSY)) {
		return false;
	}
	//����'('
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == LSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack (.");
		myLexicalAnalysis.setNextSym();
	}
	//���Ӷ��Ĳ���
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (!nextSymFlag) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "after ( lack contents.");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == STRING) {

		string str = myLexicalAnalysis.getGlobalString();//��Ҫԭ����ӡ���ַ���
		four.type = PrintStr;
		four.target = str;
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}

		//�����Ƿ�Ϊ,
		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack comma.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		//ֻ��Ϊ,�ż�������,��������
		if (symbol == COMMA) {
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
				return false;
			}
			ExpRet ret = ZSQX_expression(funcName, isCache, cache, weight);
			if (ret.isSurable) {
				four.type = (ret.type == IntType) ? PrintInt : PrintChar;
				char x[15] = { '\0' };
				if (ret.type == IntType)
					sprintf(x, "%d", ret.number);
				else
					x[0] = ret.character;
				four.target = x;
			}
			else {
				four.isNotPrintCharId = (ret.type == CharType) ? false : true;
				four.type = PrintId;
				four.target = ret.name;
			}
			if (isCache) {
				cache.push_back(four);
			}
			else {
				globalTmpCodeArr.push_back(four);
			}
		}
		four.type = PrintChar;
		four.target = "\n";
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
	}
	else {
		//�������ʽ?
		ExpRet ret = ZSQX_expression(funcName, isCache, cache, weight);
		if (ret.isSurable) {
			four.type = (ret.type == IntType) ? PrintInt : PrintChar;
			char x[15] = { '\0' };
			if (ret.type == IntType)
				sprintf(x, "%d", ret.number);
			else
				x[0] = ret.character;
			four.target = x;
		}
		else {
			four.isNotPrintCharId = (ret.type == CharType) ? false : true;
			four.type = PrintId;
			four.target = ret.name;
		}
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
		four.type = PrintChar;
		four.target = "\n";
		if (isCache) {
			cache.push_back(four);
		}
		else {
			globalTmpCodeArr.push_back(four);
		}
	}

	//����')'
	if (myLexicalAnalysis.isFinish()) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		return false;
	}
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RSBRACKET)) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
		myLexicalAnalysis.setNextSym();
	}

	//Ԥ��
	myLexicalAnalysis.nextSym();
	tmpVarCount = tmpVarCountCopy;
	return true;
}

//��������䣾 ::= return[��(�������ʽ����)��]
bool SyntaxAnalysis::ZSQX_returnStatement(string funcName, bool isCache, vector<FourYuanItem>& cache, int weight) {
	bool nextSymFlag;
	SymbolCode symbol;
	bool preRead = true;

	FourYuanItem item;

	int tmpVarCountCopy = tmpVarCount;

	//����return
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (!(symbol == RETURNSY)) {
		return false;
	}
	//����Ƿ���[...]����
	nextSymFlag = myLexicalAnalysis.nextSym();
	if (nextSymFlag) {
		symbol = myLexicalAnalysis.getGlobalSymbol();
		//�����'('
		if (symbol == LSBRACKET) {
			nextSymFlag = myLexicalAnalysis.nextSym();
			if (!nextSymFlag) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack expression.");
				return false;
			}
			//����<���ʽ>
			ExpRet ret = ZSQX_expression(funcName, isCache, cache, weight);
			if (ret.isSurable) {
				if (ret.type == IntType) {
					item.type = ReturnInt;
					char x[15] = { '\0' };
					sprintf(x, "%d", ret.number);
					item.target = x;
					if (isCache) {
						cache.push_back(item);
					}
					else {
						globalTmpCodeArr.push_back(item);
					}
				}
				else {
					item.type = ReturnChar;
					char x[15] = { '\0' };
					sprintf(x, "%d", ret.character);
					item.target = x;
					if (isCache) {
						cache.push_back(item);
					}
					else {
						globalTmpCodeArr.push_back(item);
					}
				}
			}
			else {
				item.type = ReturnId;
				item.target = ret.name;
				if (isCache) {
					cache.push_back(item);
				}
				else {
					globalTmpCodeArr.push_back(item);
				}
			}
			checkReturn(funcName, ret.type);
			//����')'
			if (myLexicalAnalysis.isFinish()) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
				return false;
			}
			symbol = myLexicalAnalysis.getGlobalSymbol();
			if (!(symbol == RSBRACKET)) {
				myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack ).");
				myLexicalAnalysis.setNextSym();
			}
			haveReturn = true;
		}
		else {
			if (funcName == "main")
				item.type = OverProcedure;
			else
				item.type = ReturnEmpty;
			checkReturn(funcName);
			if (isCache) {
				cache.push_back(item);
			}
			else {
				globalTmpCodeArr.push_back(item);
			}
			preRead = false;
		}
	}
	else {
		preRead = false;
	}

	if (preRead)
		myLexicalAnalysis.nextSym();
	tmpVarCount = tmpVarCountCopy;
	return true;
}

//�������� ::= �ۣ������ݣ��޷�����������0
//ע��,0ǰ�治�������κε�������
bool SyntaxAnalysis::ZSQX_integer() {
	bool nextSymFlag;
	SymbolCode symbol;
	int value = 0;
	bool isMinus = false;//�Ƿ���ȡ��
	//�����Ƿ���+��-
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == ADD || symbol == SUB) {

		isMinus = (symbol == ADD) ? false : true;

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack int num after +|-.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (symbol == INTNUM) {
			value = myLexicalAnalysis.getGlobalNumber();
			if (value == 0) { // ������Ϊ0
				myError.SyntaxAnalysisError(ZeroPrefixSignError, getLineNumber());
				return false;
			}
		}
		else {//�����޷�������
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack unsigned integer.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}
	}
	else if (symbol != INTNUM) {
		myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "vital part is not integer.");
		myLexicalAnalysis.skipRead(';');
		return false;
	}
	else {
		value = myLexicalAnalysis.getGlobalNumber();
	}
	if (isMinus)
		value = -value;
	return_integer = value;//���÷���ֵ
	myLexicalAnalysis.nextSym();
	return true;
}

//������ͷ���� ::= int ����ʶ���� |char ����ʶ����
bool SyntaxAnalysis::ZSQX_declareHead() {
	bool nextSymFlag;
	SymbolCode symbol;
	FunctionType type;
	symbol = myLexicalAnalysis.getGlobalSymbol();
	if (symbol == INTSY || symbol == CHARSY) {

		type = (symbol == INTSY) ? ReturnIntType : ReturnCharType;

		nextSymFlag = myLexicalAnalysis.nextSym();
		if (!nextSymFlag) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
			return false;
		}
		symbol = myLexicalAnalysis.getGlobalSymbol();
		if (!(symbol == IDENTIFIER)) {
			myError.SyntaxAnalysisError(LackComposedPartError, getLineNumber(), "lack identifier.");
			myLexicalAnalysis.skipRead(';');
			return false;
		}
		return_declare_funcName = myLexicalAnalysis.getGlobalString();
		pushItem(return_declare_funcName, "GLOBAL", type);
		FourYuanItem insert;
		insert.type = TmpCodeType::FunctionDef;
		insert.target = return_declare_funcName;
		insert.funcType = type;
		globalTmpCodeArr.push_back(insert);
	}
	else {
		return false;
	}
	//Ԥ��
	myLexicalAnalysis.nextSym();
	return true;
}















//��������
void SyntaxAnalysis::pushItem(string id, string functionName, int num) {
	if (!isAbleInsert(id, functionName))
		return;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemType(Constant);
	newItem.setValueType(IntType);
	newItem.setConstInt(num);
	globalSymbolTable.push_back(newItem);
}
//�ַ�����
void SyntaxAnalysis::pushItem(string id, string functionName, char character) {
	if (!isAbleInsert(id, functionName))
		return;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemType(Constant);
	newItem.setValueType(CharType);
	newItem.setConstChar(character);
	globalSymbolTable.push_back(newItem);
}
//�������
void SyntaxAnalysis::pushItem(string id, string functionName, ValueType valueType, int size) {
	if (!isAbleInsert(id, functionName))
		return;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemType(Variable);
	newItem.setValueType(valueType);
	newItem.setArrSize(size);
	globalSymbolTable.push_back(newItem);
}
//����+����
void SyntaxAnalysis::pushItem(string id, string functionName, ItemType itemType, ValueType valueType) {
	if (!isAbleInsert(id, functionName))
		return;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemType(itemType);
	newItem.setValueType(valueType);
	globalSymbolTable.push_back(newItem);
}
//����
void SyntaxAnalysis::pushItem(string id, string functionName, FunctionType funcType) {
	if (!isAbleInsert(id, functionName))
		return;
	SymbolTableItem newItem(id, functionName);
	newItem.setItemType(Function);
	newItem.setFuncType(funcType);
	globalSymbolTable.push_back(newItem);
}

//����Ƿ�����
bool SyntaxAnalysis::isAbleInsert(string id, string functionName) {
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		//��������ͬ,����Ҳ��ͬ
		if (functionName == item.getFuncName() && id == item.getId()) {
			myError.SemanticAnalysisError(DeclareConflictError, getLineNumber(), id);
			return false;
		}
	}
	return true;
}

/*����Ƿ���(�����Ƿ�ƥ��)
bool SyntaxAnalysis::isDefined(string id, string functionName) {
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {
			if (id == item.getId())
				return true;
		}
		else if (item.getFuncName() == functionName) {
			if (id == item.getId()) {
				return true;
			}
		}
	}
	myError.SemanticAnalysisError(NotDefinitionError,getLineNumber(),id);
	return false;
}*/
/*����ʶ������[�������ʽ����]����Ҫ���:��ʶ���治���ڣ���ʶ����Ӧ���ǲ������飬���������,���ʽ��Ӧ���±�ֵ�Ƿ�Խ��*/
int SyntaxAnalysis::idArrExpCheck(string identifier, string funcName, bool expSurable, int index) {
	bool globalIndexOut = false;//��ȫ���з��ִ�Ϊ�����ҵ�Խ��
	bool globalNotArray = false;//ȫ���з��ֲ�������
	bool isDefined = false;
	int order = -1;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {//������ȫ��
			if (item.getId() == identifier) {//��ʶ������ͬ
				isDefined = true;
				if (item.getArrSize() > 0) {//������
					if (expSurable) {//�����±�ֵȷ��
						if (index >= item.getArrSize() || index < 0) {//Խ��
							globalIndexOut = true;
						}
						else {
							order = item.getOrder();
						}
					}
					else {
						order = item.getOrder();
					}
				}
				else {//��������
					globalNotArray = true;
				}
			}
		}
		else if (item.getFuncName() == funcName) {//�ں����������ڷ���
			if (item.getId() == identifier) {
				isDefined = true;
				if (item.getArrSize() > 0) {
					if (expSurable) {
						if (index >= item.getArrSize() || index < 0) {//Խ��
							myError.SemanticAnalysisError(ArrIndexOutOfRangeError, getLineNumber(), identifier);
							return -1;
						}
						else {
							return item.getOrder();
						}
					}
					else {
						return item.getOrder();
					}
				}
				else {//��������
					if (globalIndexOut) {
						break;
					}
					else {
						myError.SemanticAnalysisError(TypeNotMatchError, getLineNumber(), identifier);
						return -1;
					}
				}
			}
		}
	}

	if (globalIndexOut) {
		myError.SemanticAnalysisError(ArrIndexOutOfRangeError, getLineNumber(), identifier);
		return -1;
	}
	if (globalNotArray) {
		myError.SemanticAnalysisError(TypeNotMatchError, getLineNumber(), identifier);
		return -1;
	}
	//��ʶ��δ����
	if (!isDefined) {
		myError.SemanticAnalysisError(NotDefinitionError, getLineNumber(), identifier);
		return -1;
	}
	return order;
}

//��ʶ�����-->������(������Ϊ�����void����)
int SyntaxAnalysis::idCheckInFactor(string identifier, string funcName) {
	bool foundInGlobal = false; // ��ʾ��global�з��ִ˽ṹ��������
	bool isDefined = false;
	int orderInGlobal = -1;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {//ȫ��������
			if (item.getId() == identifier) {//��ʶ��������ͬ
				isDefined = true;
				if (item.getArrSize() > 0) {//Ϊ����,���
					foundInGlobal = true;
				}
				else if (item.getItemType() == Function && item.getFuncType() == VOIDSY) {
					foundInGlobal = true;
				}
				else {
					orderInGlobal = item.getOrder();
				}
			}
		}
		else if (item.getFuncName() == funcName) {//��������ͬ(�ֲ�������)
			if (item.getId() == identifier) {
				isDefined = true;
				if (item.getArrSize() > 0) {//Ϊ����,����
					myError.SemanticAnalysisError(TypeNotMatchError, getLineNumber(), identifier);
					return -1;
				}
				else {//��ȷ��
					return item.getOrder();
				}
			}
		}
	}
	if (foundInGlobal) {
		myError.SemanticAnalysisError(TypeNotMatchError, getLineNumber(), identifier);
		return -1;
	}
	//δ�����ʶ��
	if (!isDefined) {
		myError.SemanticAnalysisError(NotDefinitionError, getLineNumber(), identifier);
		return -1;
	}
	return orderInGlobal;
}

//��ʶ�����--->�����(ֻ���Ǻ���)
void SyntaxAnalysis::idCheckInState(string identifier) {
	//����ֻ���Ǻ���,����ֻ��Ҫ����ȫ�ֵļ���
	bool isDefined = false;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {
			if (item.getId() == identifier) {
				isDefined = true;
				if (item.getItemType() == Function)
					return;
			}
		}
	}
	if (!isDefined) {
		myError.SemanticAnalysisError(FuncNotDefineError, getLineNumber(), identifier);
		return;
	}
	myError.SemanticAnalysisError(StateIdNotMatchError, getLineNumber(), identifier);
}

//����ʶ������(��<ֵ������>��)��--->�����������е�(���ʽ�е�,��Ҫ�ж��Ƿ����з���ֵ)
void SyntaxAnalysis::funcCallCheck(string identifier, bool isInExp, vector<ValueType> paramType) {
	bool isDefined = false;
	bool flag = false;//�Ƿ���Ҫ���в������(����Ҫ˵�������������������)
	vector<ValueType> actualParam;

	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {
			if (item.getId() == identifier) {
				if (item.getItemType() == Function) {//�Ǻ���
					isDefined = true;
					flag = true;
					if (isInExp && item.getFuncType() == VoidType) {
						myError.SemanticAnalysisError(NeedValueButVoidFuncError, getLineNumber(), identifier);
						return;
					}
					for (unsigned int j = i + 1; j < globalSymbolTable.size(); j++) {
						item = globalSymbolTable.at(j);
						if (item.getFuncName() == identifier && item.getItemType() == Parament) {
							actualParam.push_back(item.getValueType());
						}
						else {
							break;
						}
					}
					break;
				}
			}
		}
	}
	//���в�������
	if (flag) {
		if (paramType.size() == 0) {
			myError.SemanticAnalysisError(NoneValueParamError, getLineNumber(), identifier);
			return;
		}
		if (actualParam.size() == 0) {//�޲κ���ȴ����
			myError.SemanticAnalysisError(NoneParamButDeliverError, getLineNumber(), identifier);
			return;
		}
		if (paramType.size() != actualParam.size()) {//����������ƥ��
			myError.SemanticAnalysisError(ParamNumNotMatchError, getLineNumber(), identifier);
			return;
		}
		for (unsigned int i = 0; i < paramType.size(); i++) {
			ValueType first = actualParam.at(i);
			ValueType second = paramType.at(i);
			if (first != second) {//�������Ͳ�ƥ��
				myError.SemanticAnalysisError(ParamTypeNotMatchError, getLineNumber(), identifier);
				return;
			}
		}
	}

	if (!isDefined) {
		myError.SemanticAnalysisError(FuncNotDefineError, getLineNumber(), identifier);
		return;
	}
}

//�Ը�ֵ����Լ�scanf�е����ı�ʶ���ļ��(scanfʵ�ʾ��ǶԱ����ĸ�ֵ����)
int SyntaxAnalysis::checkAssignId(string identifier, string funcName) {
	bool isDefined = false;
	int order = -1;
	bool global = false;
	for (unsigned int i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getFuncName() == "GLOBAL") {
			if (item.getId() == identifier) {
				isDefined = true;
				if (item.getItemType() == Variable && item.getArrSize() == 0) {//��ȫ�ֱ���
					order = item.getOrder();
				}
				else {
					global = true;
				}
			}
		}
		else if (item.getFuncName() == funcName) {
			if (item.getId() == identifier) {
				isDefined = true;
				if ((item.getItemType() == Variable && item.getArrSize() == 0)
					|| (item.getItemType() == Parament)) {//�Ǳ������߲���
					return item.getOrder();
				}
				else {
					myError.SemanticAnalysisError(AssignObjectNotVar, getLineNumber(), identifier);
					return -1;
				}
			}
		}
	}
	if (!isDefined) {
		myError.SemanticAnalysisError(NotDefinitionError, getLineNumber(), identifier);
		return -1;
	}
	else if (global) {
		myError.SemanticAnalysisError(AssignObjectNotVar, getLineNumber(), identifier);
		return -1;
	}
	return order;
}

//���switch��case����Ƿ������ͬ��ֵ
void SyntaxAnalysis::checkCase(vector<int> cases) {
	if (cases.size() == 0)
		return;
	sort(cases.begin(), cases.end());
	int prev = cases.at(0);
	for (unsigned int i = 1; i < cases.size(); i++) {
		int x = cases.at(i);
		if (x == prev) {
			myError.SemanticAnalysisError(CaseSameValueError, getLineNumber(), "");
			return;
		}
		prev = x;
	}
}

//��鷵����������������Ƿ�ƥ��
void SyntaxAnalysis::checkReturn(string funcName) {
	for (unsigned i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getItemType() == Function && item.getId() == funcName) {
			if (item.getFuncType() != VoidType) {
				myError.ReturnStatementError(ExistReturnEmptyError, getLineNumber(), funcName);
			}
			return;
		}
	}
}
void SyntaxAnalysis::checkReturn(string funcName, ValueType retType) {
	for (unsigned i = 0; i < globalSymbolTable.size(); i++) {
		SymbolTableItem item = globalSymbolTable.at(i);
		if (item.getItemType() == Function && item.getId() == funcName) {
			if (item.getFuncType() == VoidType) {
				myError.ReturnStatementError(VoidButReturnValueError, getLineNumber(), funcName);
			}
			else if (item.getFuncType() == ReturnCharType && retType == IntType) {
				myError.ReturnStatementError(CharButReturnIntError, getLineNumber(), funcName);
			}
			return;
		}
	}
}

//��Ȩ----���ü���
void SyntaxAnalysis::addWeight(int order, int weight) {
	SymbolTableItem item = globalSymbolTable.at(order);
	if ((item.getItemType() == Variable || item.getItemType() == Parament) && item.getFuncName() != "GLOBAL"
		&& item.getArrSize() == 0) {
		globalSymbolTable.at(order).addWeight(weight);
	}
}