/*interpret.h*/
#ifndef _interpret_h
#define _interpret_h

#include "macro.h"
#include "typedefs.h"

class Index
{
public:
	string index_name;	//all the datas is store in file index_name.index
	string table_name;	//the name of the table on which the index is create
	int column;			//on which column the index is created
	int columnLength;
	int blockNum;		//number of block the datas of the index occupied in the file index_name.table
	Index() : column(0), blockNum(0){}
};

class Row
{
public:
	vector<string> columns;
};


class Condition{
public:
	Operator op;
	int columnNum;
	string value;
};

class Interpret {
public:
	int state;		                       //command����״̬
	string TableName;		               //Ҫ�����ı����
	string IndexName;		               //Ҫ������������
	string ColName;                        //�洢Ҫ����������
	string db_name;                        //Ҫ���������ݿ���
	string fileName;                       //Ҫִ�е��ļ���
	vector<Attribute> column;
	vector<Condition> condition;		   //Ҫ�Ƚϵ�where�־������
	vector<string > Lop;
	Row row;		                       //Ҫ�����ֵ����
	Table NewTableInfo;                    //����Ҫ�½��ı����Ϣ
	const Table* TableInfo;                      //��������catalogmanager���ݵı�ͷ����Ϣ
	Index getIndexInfo;

	//friend void execute();
	//���캯��
	Interpret(){
		state = UNKNOW;
		TableName = "";
		IndexName = "";
		ColName = "";
		db_name = "";
		fileName = "";
	}
	~Interpret(){}

	void Parse(char* command);
	//��ʼ������
	void initcol();
	void initcond();
	void initvalue();
	void inittable(){
		NewTableInfo.tableName = "";
		NewTableInfo.attrNumber = 0;
		NewTableInfo.attributes.clear();
	}
	void initindex(){
		getIndexInfo.blockNum = getIndexInfo.column = -1;
		getIndexInfo.index_name = "";
	}
	bool GetWord(string & src, string & des);   //����sql����е���Ч�ַ�������ؼ��֡������������������ȵ�

protected:

	void MakeInitilate(){
		state = UNKNOW;
		TableName = "";
		IndexName = "";
		initcol();
		initcond();
		initvalue();
		inittable();
		initindex();
	}

	bool GetStr(string & src, string & des);

	short int IsInt(const char *);  //�ж��ַ����ǲ���int����
	short int IsFloat(char *input); //�ж��ַ����ǲ���float����
};

#endif