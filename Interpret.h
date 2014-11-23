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
	int state;		                       //command解析状态
	string TableName;		               //要操作的表格名
	string IndexName;		               //要操作的索引名
	string ColName;                        //存储要搜索的列名
	string db_name;                        //要操作的数据库名
	string fileName;                       //要执行的文件名
	vector<Attribute> column;
	vector<Condition> condition;		   //要比较的where字句的链表
	vector<string > Lop;
	Row row;		                       //要插入的值链表
	Table NewTableInfo;                    //存入要新建的表的信息
	const Table* TableInfo;                      //接收来自catalogmanager传递的表头的信息
	Index getIndexInfo;

	//friend void execute();
	//构造函数
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
	//初始化函数
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
	bool GetWord(string & src, string & des);   //读入sql语句中的有效字符，例如关键字、‘（’、‘、‘’等等

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

	short int IsInt(const char *);  //判断字符串是不是int类型
	short int IsFloat(char *input); //判断字符串是不是float类型
};

#endif