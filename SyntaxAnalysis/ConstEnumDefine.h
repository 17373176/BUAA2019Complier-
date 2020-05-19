/* ConstEnumDefine.h : ��ͷ�ļ�����ȫ�ֳ�����ö�����͡��������
 *
 * SYM����롢ITEM_KIND���ű������͡�VALUE_TYPEֵ���͡�FUNC_TYPE�����������͡�ERRORSYM��������롢
 * SYM_to_str[SYMBOL_NUM]�����ӳ���ַ������顢wordNameArr[KEY_WORD_NUM]�ؼ��ֱ����ֵ������顢
 * syntaxWord[SYNTAX_ELE_NUM]�﷨�ɷ����顢syntaxError[SYNTAX_ERROR_NUM]�﷨�����ʶ�롢
 * errorSym_to_o[SYNTAX_ERROR_NUM] ����������ϸ��Ϣ�����errorSym_to_str[SYNTAX_ERROR_NUM]����������
 *
 */

#pragma once

#include <string>

#define TEXT_LEN 512 * 512 * 4   // �����ı����볤��
#define SYNTAX_WORD_NUM 1024 * 4 // ʶ�𵥴ʸ���
#define KEY_WORD_NUM 40          // �ؼ��ֱ����ָ���
#define WORD_LEN 100             // ���ʳ��� 
#define SYMBOL_NUM 40            // ������������
#define SYNTAX_ELE_NUM 40        // �﷨�ɷָ���
#define SYNTAX_ERROR_NUM 40      // ����������

#define GLOBAL_FUNCNAME "GLOBAL" // ȫ���򣬺������ɺ���������

using namespace std;

/*------��������------*/
enum SYM { // �����
	IDENFR, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK,
	IFTK, ELSETK, DOTK, WHILETK, FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS,
	MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA,
	LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE, INEXIST, JUMP // ��������Ҫ������
};

enum ITEM_KIND { // ���ű������ͣ�����(������length����)������������������������
	Con_Kind, Var_Kind, Para_Kind, Func_Kind
};

enum VALUE_TYPE { // ֵ����
	Int_Type, Char_Type, String_Type, Empty_Type
};

enum FUNC_TYPE { // ������������
	Void_Type, ReturnInt_Type, ReturnChar_Type, NotDefine_Func
};

enum ERRORSYM { // ���������
	a_ErrorS, b_ErrorS, c_ErrorS, d_ErrorS, e_ErrorS, f_ErrorS, g_ErrorS, h_ErrorS,
	i_ErrorS, j_ErrorS, k_ErrorS, l_ErrorS, m_ErrorS, n_ErrorS, o_ErrorS, my_ErrorS
};

const string SYM_to_str[SYMBOL_NUM] = { // �����ӳ���ַ�������
	"IDENFR", "INTCON", "CHARCON", "STRCON", "CONSTTK", "INTTK", "CHARTK", "VOIDTK", "MAINTK",
	"IFTK", "ELSETK", "DOTK", "WHILETK", "FORTK", "SCANFTK", "PRINTFTK", "RETURNTK", "PLUS",
	"MINU", "MULT", "DIV", "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN",
	"COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "INEXIST"
};

const string wordNameArr[KEY_WORD_NUM] = { // �ؼ��ֱ����ֵ�������
	"const", "int", "char", "void", "main", "if", "else",
	"do", "while", "for", "scanf", "printf", "return"
};

const string errorSym_to_str[SYNTAX_ERROR_NUM] = { // ����������
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "my"
};
