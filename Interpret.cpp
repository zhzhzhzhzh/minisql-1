#include<iostream>
#include "macro.h"
#include"typedefs.h"
//#include "RecordManager.h"
#include "catalogmanager.h"
#include "Interpret.h"


using namespace std;


extern catalogmanager Mcatalog;

//This function is used to initiate the 'column' structure
void Interpret::initcol()
{
	if (column.size()>0){
		column.clear();
	}
}

//This function is used to initiate the 'condition' structure
void Interpret::initcond()
{
	if (condition.size()>0){
		condition.clear();
	}
}

//This function is used to initiate the 'insertvalue' structure
void Interpret::initvalue()
{
	if (row.columns.size()>0){
		row.columns.clear();
	}
}

//This function is used to get a word ( maybe a 'token' is more appropriate )
//from the 'src' string and copy it into the 'des' string
bool Interpret::GetWord(string & src, string & des)
{
	des.clear();
	unsigned int srcpos = 0, despos = 0;
	char temp = ' ';

	for (; srcpos<src.length(); srcpos++) {
		if (temp == ' ' || temp == '\t' || temp == 10 || temp == 13)
			temp = src[srcpos];
		else break;
	}
	if (srcpos == src.length() && (temp == ' ' || temp == '\t' || temp == 10 || temp == 13))
		return false;

	switch (temp)
	{
	case ',':
	case '(':
	case ')':
	case '*':
	case '=':
	case '\'':
		des += temp;
		src.erase(0, srcpos);
		break;
	case '<':
		des += temp;
		temp = src[srcpos++];
		if (temp == '=' || temp == '>')
		{
			des += temp;
			src.erase(0, srcpos);
		}
		else
		{
			src.erase(0, srcpos - 1);
		}
		break;
	case '>':
		des += temp;
		temp = src[srcpos++];
		if (temp == '=')
		{
			des += temp;
			src.erase(0, srcpos);
		}
		else
		{
			src.erase(0, srcpos - 1);
		}
		break;
	default:
		do{
			des += temp;
			if (srcpos < src.length())
				temp = src[srcpos++];
			else  {
				src.erase(0, srcpos);
				des[despos++] = '\0';
				return true;
			}
		} while (temp != '*' && temp != ',' && temp != '(' && temp != ')'
			&& temp != ' ' && temp != '\t' && temp != '=' && temp != '>'
			&& temp != '<' && temp != '\'' && temp != 10 && temp != 13);
		src.erase(0, srcpos - 1);
	}
	return true;
}

bool Interpret::GetStr(string& src, string& des)
{
	unsigned int pos = 0;
	des.clear();
	char temp;
	if (src[0] == '\'') {
		des = "";
		return true;
	}
	else for (pos = 0; pos<src.length(); pos++) {
		if (src[pos] != '\''){
			temp = src[pos];
			des += temp;
		}
		else {
			src.erase(0, pos);
			return true;
		}
	}
	return false;
}

//This function is used to interpret the 'command' string which comes from user's 
//input, and return a corresponding parse parsetree
void Interpret::Parse(char* command)
{
	string word = "";
	string temp = command;
	short int FindPriKey = 0, FindUnique = 0;
	Attribute tempcol;
	Condition tempcon;
	string   temprow;
	bool flag;
	int i = 0;

	MakeInitilate();//将解释树中的变量全部初始化为空值或NULL
	flag = GetWord(temp, word);//把temp字符串中第一个空格前的单词去掉，并将该单词赋予WORD作为字符串,flag返回1
	//若是空查询
	if (!flag)
	{
		state = EMP;
		return;
	}

	//若是退出命令
	else if (strcmp(word.c_str(), "quit") == 0)
	{
		flag = GetWord(temp, word);
		if (!flag)
			state = QUIT;
		return;
	}
	else if (strcmp(word.c_str(), "create") == 0)
	{
		flag = GetWord(temp, word);
		if (!flag)
			return;
		//解析create table命令
		if (strcmp(word.c_str(), "table") == 0)
		{
			state = CRETABERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			TableName = word;
			if (Mcatalog.getTableIndex(TableName) != -1){
				state = TABLEEXISTED;
				return;
			}
			NewTableInfo.tableName = word;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			tempcol.attrName = "uuidName";
			tempcol.attrType = 2;
			tempcol.dataLength = INTLEN;
			tempcol.dataType = Uuid;
			tempcol.indexName = "null";
			NewTableInfo.attributes.push_back(tempcol);
			if (strcmp(word.c_str(), "(") != 0)
				return;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "unique") == 0 || strcmp(word.c_str(), "primary") == 0)
				return;
			tempcol.attrName = word;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "int") == 0)
			{
				tempcol.dataType = Int;
				tempcol.dataLength = INTLEN;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "unique") == 0)
				{
					tempcol.attrType = 1;
					flag = GetWord(temp, word);
					if (!flag)
						return;
				}
			}
			else if (strcmp(word.c_str(), "float") == 0)
			{
				tempcol.dataType = Float;
				tempcol.dataLength = FLOATLEN;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "unique") == 0)
				{
					tempcol.attrType = 1;
					flag = GetWord(temp, word);
					if (!flag)
						return;
				}
			}
			else if (strcmp(word.c_str(), "char") == 0)
			{
				tempcol.dataType = String;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "(") != 0)
					return;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (!IsInt(word.c_str()))
					return;
				tempcol.dataLength = atoi(word.c_str()) + 1;
				if (tempcol.dataLength > 256 || tempcol.dataLength < 2)
				{
					state = CHARBOUD;
					return;
				}
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), ")") != 0)
					return;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "unique") == 0)
				{
					tempcol.attrType = 1;
					flag = GetWord(temp, word);
					if (!flag)
						return;
				}
			}
			else return;
			NewTableInfo.attributes.push_back(tempcol);//开始加入信息表中
			tempcol.attrType = 2;
			while (strcmp(word.c_str(), ",") == 0)
			{
				flag = GetWord(temp, word);
				if (!flag)
					return;
				//如果是primary key命令
				if (strcmp(word.c_str(), "primary") == 0)
				{
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), "key") != 0)
						return;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), "(") != 0)
						return;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					for (int i = 0; i<NewTableInfo.attributes.size(); i++){
						if (strcmp(NewTableInfo.attributes[i].attrName.c_str(), word.c_str()) == 0){
							FindPriKey = 1;
							NewTableInfo.attributes[i].attrType = 0;
							//NewTableInfo.attributes[i].isUnique=1;
						}
					}
					if (!FindPriKey)
					{
						state = VOIDPRI;
						return;
					}
					FindPriKey = 0;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), ")") != 0)
						return;
					flag = GetWord(temp, word);
					if (!flag)
						return;
				}
				//如果是unique命令
				else if (strcmp(word.c_str(), "unique") == 0)
				{
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), "(") != 0)
						return;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					for (int i = 0; i<NewTableInfo.attributes.size(); i++){
						if (strcmp(NewTableInfo.attributes[i].attrName.c_str(), word.c_str()) == 0){
							FindUnique = 1;
							NewTableInfo.attributes[i].attrType = 1;
						}
					}
					if (!FindUnique)
					{
						state = VOIDUNI;
						return;
					}
					FindUnique = 0;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), ")") != 0)
						return;
					flag = GetWord(temp, word);
					if (!flag)
						return;
				}
				//如果不是unique与primary key命令
				else
				{
					tempcol.attrName = word;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), "int") == 0)
					{
						tempcol.dataType = Int;
						tempcol.dataLength = sizeof(int);
						flag = GetWord(temp, word);
						if (!flag)
							return;
						if (strcmp(word.c_str(), "unique") == 0)
						{
							tempcol.attrType = 1;
							flag = GetWord(temp, word);
							if (!flag)
								return;
						}
					}
					else if (strcmp(word.c_str(), "float") == 0)
					{
						tempcol.dataType = Float;
						tempcol.dataLength = sizeof(float);
						flag = GetWord(temp, word);
						if (!flag)
							return;
						if (strcmp(word.c_str(), "unique") == 0)
						{
							tempcol.attrType = 1;
							flag = GetWord(temp, word);
							if (!flag)
								return;
						}
					}
					else if (strcmp(word.c_str(), "char") == 0)
					{
						tempcol.dataType = String;
						flag = GetWord(temp, word);
						if (!flag)
							return;
						if (strcmp(word.c_str(), "(") != 0)
							return;
						flag = GetWord(temp, word);
						if (!flag)
							return;
						if (!IsInt(word.c_str()))
							return;
						tempcol.dataLength = sizeof(char)*(atoi(word.c_str()) + 1);
						if (tempcol.dataLength > 255 || tempcol.dataLength < 1)
						{
							state = CHARBOUD;
							return;
						}
						flag = GetWord(temp, word);
						if (!flag)
							return;
						if (strcmp(word.c_str(), ")") != 0)
							return;
						flag = GetWord(temp, word);
						if (!flag)
							return;
						if (strcmp(word.c_str(), "unique") == 0)
						{
							tempcol.attrType = 1;
							flag = GetWord(temp, word);
							if (!flag)
								return;
						}
					}
					else return;
					NewTableInfo.attributes.push_back(tempcol);//至此为止，已获得table中的name、attributes容器
					//还需要在api中计算出attriNum和totalLength
					tempcol.attrType = 2;
				}
			}
			if (strcmp(word.c_str(), ")") != 0)
				return;
			NewTableInfo.attrNumber = NewTableInfo.attributes.size();
			flag = GetWord(temp, word);
			if (!flag)
				state = CRETAB;
		}
		else if (strcmp(word.c_str(), "index") == 0)
		{
			state = CREINDERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			IndexName = word;
			getIndexInfo.index_name = word;
			if (Mcatalog.getIndexName(IndexName) != "null"){
				state = INDEXERROR;
				return;
			}
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "on") != 0)
				return;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			TableName = word;
			if (Mcatalog.getTableIndex(word) == -1){
				state = TABLEERROR;
				return;
			}
			TableInfo = Mcatalog.getTableInformation(TableName);
			/*for(int i=0;i<NewTableInfo.attributes.size();i++){
			if(NewTableInfo.attributes[i].isPrimeryKey==true){
			getIndexInfo.columnLength=NewTableInfo.attributes[i].length;
			}
			}*/
			//getIndexInfo.index_name = word;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "(") != 0)
				return;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			int tempint;
			int tabeleIndex = Mcatalog.getTableIndex(TableName);
			getIndexInfo.index_name = word;
cout << tabeleIndex << endl;
cout << word << endl;

			tempint = Mcatalog.getAttrIndex(tabeleIndex, word);
cout << tempint << endl;
			if (tempint == -1){
				state = COLUMNERROR;
				return;
			}

			getIndexInfo.column = tempint;
			getIndexInfo.blockNum = 0;
			getIndexInfo.columnLength = NewTableInfo.attributes[tempint].dataLength;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), ")") != 0)
				return;
			flag = GetWord(temp, word);
			if (!flag)
				state = CREIND;
		}
		else if (strcmp(word.c_str(), "database") == 0)
		{
			state = CREDBERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (Mcatalog.isDBExisted(word) == -1){
				state = DBERR;
				return;
			}
			else{
				db_name = word;
				state = CREDB;
			}

		}
	}
	else if (strcmp(word.c_str(), "insert") == 0)
	{
		flag = GetWord(temp, word);
		if (!flag)
			return;
		if (strcmp(word.c_str(), "into") == 0)
		{
			state = INSERTERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			TableName = word;
			if (Mcatalog.getTableIndex(word) == -1){
				state = TABLEERROR;
				return;
			}
			TableInfo = Mcatalog.getTableInformation(TableName);
			int attribute_number = TableInfo->attrNumber;

			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "values") != 0)
				return;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (word != "(")
				return;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			i = 1;
			if (strcmp(word.c_str(), "\'") == 0)
			{
				
				if (TableInfo->attributes[i].dataType != String){
					state = INSERT_VALUES_TYPERR;
					return;
				}
				flag = GetStr(temp, word);
				if (!flag)
					return;
				temprow = word;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "\'") != 0)
					return;
			}
			else
			{
				if (TableInfo->attributes[i].dataType == String){
					state = INSERT_VALUES_TYPERR;
					return;
				}
				temprow = word;
			}
			flag = GetWord(temp, word);
			if (!flag)
				return;
			row.columns.push_back(temprow);
			while (strcmp(word.c_str(), ",") == 0)
			{
				i++;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "\'") == 0)
				{
					if (TableInfo->attributes[i].dataType != String){
						state = INSERT_VALUES_TYPERR;
						return;
					}
					flag = GetStr(temp, word);
					if (!flag)
						return;
					temprow = word;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), "\'") != 0)
						return;
				}
				else
				{
					if (TableInfo->attributes[i].dataType == String){
						state = INSERT_VALUES_TYPERR;
						return;
					}
					temprow = word;
				}
				row.columns.push_back(temprow);
				flag = GetWord(temp, word);
				if (!flag)
					return;
			}
			if (word != ")")
				return;
			
			if (row.columns.size() != attribute_number - 1){
				state = INSERTNUMBERERROR;
				return;
			}

			flag = GetWord(temp, word);
			if (!flag){
				state = INSERT;
				return;
			}
		}
	}
	else if (strcmp(word.c_str(), "drop") == 0)
	{
		flag = GetWord(temp, word);
		if (!flag)
			return;
		if (strcmp(word.c_str(), "table") == 0)
		{
			state = DRPTABERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			TableName = word;
			if (Mcatalog.getTableIndex(TableName) == -1){
				state = TABLEERROR;
				return;
			}
			TableInfo = Mcatalog.getTableInformation(TableName);
			flag = GetWord(temp, word);
			if (!flag)
				state = DRPTAB;
		}
		else if (strcmp(word.c_str(), "index") == 0)
		{
			state = DRPINDERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			IndexName = word;
			if (Mcatalog.getIndexName(IndexName) != "null"){
				state = INDEXEROR;
				return;
			}
			flag = GetWord(temp, word);
			if (!flag)
				state = DRPIND;
			//getIndexInfo=Mcatalog.getIndexInformation(
		}
		else if (strcmp(word.c_str(), "database") == 0){
			state = DROPDBERR;
			flag = GetWord(temp, word);
			if (Mcatalog.isDBExisted(word) == -1){
				state = DBERR;
				return;
			}
			else{
				db_name = word;
				state = DRPDB;
			}
				
		}

	}
	else if (strcmp(word.c_str(), "use") == 0){
		flag = GetWord(temp, word);
		if (!flag)
			return;
		flag = GetWord(temp, word);
		if (Mcatalog.isDBExisted(word) == -1){
			state = DBERR;
			return;
		}
		else{
			db_name = word;
			state = USEDB;
		}
	}
	else if (strcmp(word.c_str(), "select") == 0)
	{
		state = SELERR;

		//½âÎöselect×Ó¾ä
		flag = GetWord(temp, word);
		if (!flag)
			return;
		//initcol(column);

			tempcol.attrName = word;
			column.push_back(tempcol);
			flag = GetWord(temp, word);
			if (!flag)
				return;

			while (strcmp(word.c_str(), ",") == 0)
			{
				flag = GetWord(temp, word);
				if (!flag)
					return;
				tempcol.attrName = word;
				column.push_back(tempcol);
				flag = GetWord(temp, word);
				if (!flag)
					return;
			}

		if (strcmp(word.c_str(), "from") != 0)
			return;
		//½âÎöfrom×Ó¾ä
		flag = GetWord(temp, word);
		if (!flag)
			return;
		TableName = word;
		TableInfo = Mcatalog.getTableInformation(TableName);
		int TableIndex = Mcatalog.getTableIndex(word);
		if (TableIndex == -1){
			state = TABLEERROR;
			return;
		}

		if (column[0].attrName == "*")
		{
			column.clear();
			for (i = 1; i < TableInfo->attributes.size(); i++)
			{
				tempcol.attrName = TableInfo->attributes[i].attrName;
				column.push_back(tempcol);
			}
		}
		flag = GetWord(temp, word);
		if (!flag)
		{
			state = SELECT_NOWHERE_CAULSE;
			return;
		}

		if (strcmp(word.c_str(), "where") != 0)
			return;
		//½âÎöwhere×Ó¾ä
		flag = GetWord(temp, word);
		if (!flag)
		{
			state = SELERR;
		}
		tempcon.columnNum = Mcatalog.getAttrIndex(TableIndex, word);
		ColName = word;
		if (tempcon.columnNum == -1){
			state = COLUMNERROR;
			return;
		}
		flag = GetWord(temp, word);
		if (!flag)
			return;
		if (strcmp(word.c_str(), "<") == 0)
			tempcon.op = Less;
		else if (strcmp(word.c_str(), "<=") == 0)
			tempcon.op = LessEqual;
		else if (strcmp(word.c_str(), ">") == 0)
			tempcon.op = Greater;
		else if (strcmp(word.c_str(), ">=") == 0)
			tempcon.op = GreaterEqual;
		else if (strcmp(word.c_str(), "=") == 0)
			tempcon.op = Equal;
		else if (strcmp(word.c_str(), "<>") == 0)
			tempcon.op = NotEqual;
		else return;

		flag = GetWord(temp, word);
		if (!flag)
			return;
		if (strcmp(word.c_str(), "\'") == 0)
		{
			flag = GetStr(temp, word);
			if (!flag)
				return;
			tempcon.value = word;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "\'") != 0)
				return;
		}
		else
		{
			tempcon.value = word;
		}
		condition.push_back(tempcon);//¿ªÊ¼´òÈëÌõ¼þÈÝÆ÷
		flag = GetWord(temp, word);
		if (!flag)
		{
			state = SELECT_WHERE_CAULSE;
			return;
		}
		while ((strcmp(word.c_str(), "and") == 0) || (strcmp(word.c_str(), "or") == 0))
		{
			Lop.push_back(word);
			flag = GetWord(temp, word);
			if (!flag)
				return;
			tempcon.columnNum = Mcatalog.getAttrIndex(TableIndex, word);
			if (tempcon.columnNum == -1){
				state = COLUMNERROR;
				return;
			}
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "<") == 0)
				tempcon.op = Less;
			else if (strcmp(word.c_str(), "<=") == 0)
				tempcon.op = LessEqual;
			else if (strcmp(word.c_str(), ">") == 0)
				tempcon.op = Greater;
			else if (strcmp(word.c_str(), ">=") == 0)
				tempcon.op = GreaterEqual;
			else if (strcmp(word.c_str(), "=") == 0)
				tempcon.op = Equal;
			else if (strcmp(word.c_str(), "<>") == 0)
				tempcon.op = NotEqual;
			else return;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "\'") == 0)
			{
				flag = GetStr(temp, word);
				if (!flag)
					return;
				tempcon.value = word;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "\'") != 0)
					return;
			}
			else
			{
				tempcon.value = word;
			}
			condition.push_back(tempcon);//Ñ­»·´òÈëÌõ¼þ
			flag = GetWord(temp, word);
			if (!flag)
			{
				state = SELECT_WHERE_CAULSE;
				return;
			}
		}
	}
	else if (strcmp(word.c_str(), "delete") == 0)
	{
		flag = GetWord(temp, word);
		if (!flag)
			return;
		if (strcmp(word.c_str(), "from") == 0)
		{
			state = DELETEERR;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			TableName = word;
			int TableIndex = Mcatalog.getTableIndex(word);
			if (TableIndex == -1){
				state = TABLEERROR;
				return;
			}
			flag = GetWord(temp, word);
			//ÈôÃ»ÓÐwhere×Ó¾ä£¬ÕýÈ··µ»Ø
			TableInfo = Mcatalog.getTableInformation(TableName);
			if (!flag)
			{
				state = DELETE_NOWHERE_CAULSE;
				return;
			}
			if (strcmp(word.c_str(), "where") != 0)
				return;
			//¿ªÊ¼½âÎöwhere×Ó¾ä
			flag = GetWord(temp, word);
			if (!flag)
				return;
			int tempint;
			tempint = Mcatalog.getAttrIndex(TableIndex, word);
			if (tempint == -1){
				state = COLUMNERROR;
				return;
			}
			tempcon.columnNum = tempint;
			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "<") == 0)
				tempcon.op = Less;
			else if (strcmp(word.c_str(), "<=") == 0)
				tempcon.op = LessEqual;
			else if (strcmp(word.c_str(), ">") == 0)
				tempcon.op = Greater;
			else if (strcmp(word.c_str(), ">=") == 0)
				tempcon.op = GreaterEqual;
			else if (strcmp(word.c_str(), "=") == 0)
				tempcon.op = Equal;
			else if (strcmp(word.c_str(), "<>") == 0)
				tempcon.op = NotEqual;
			else return;

			flag = GetWord(temp, word);
			if (!flag)
				return;
			if (strcmp(word.c_str(), "\'") == 0)
			{
				flag = GetStr(temp, word);
				if (!flag)
					return;
				tempcon.value = word;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "\'") != 0)
					return;
			}
			else
			{
				tempcon.value = word;
			}
			condition.push_back(tempcon);
			flag = GetWord(temp, word);
			if (!flag)
			{
				state = DELETE_WHERE_CAULSE;
				return;
			}
			while ((strcmp(word.c_str(), "and") == 0) || (strcmp(word.c_str(), "or") == 0))
			{
				Lop.push_back(word);
				flag = GetWord(temp, word);
				if (!flag)
					return;

				if (Mcatalog.getAttrIndex(TableIndex, word) == -1){
					state = COLUMNERROR;
					return;
				}
				tempcon.columnNum = Mcatalog.getAttrIndex(TableIndex, word);
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "<") == 0)
					tempcon.op = Less;
				else if (strcmp(word.c_str(), "<=") == 0)
					tempcon.op = LessEqual;
				else if (strcmp(word.c_str(), ">") == 0)
					tempcon.op = Greater;
				else if (strcmp(word.c_str(), ">=") == 0)
					tempcon.op = GreaterEqual;
				else if (strcmp(word.c_str(), "=") == 0)
					tempcon.op = Equal;
				else if (strcmp(word.c_str(), "<>") == 0)
					tempcon.op = NotEqual;
				else return;
				flag = GetWord(temp, word);
				if (!flag)
					return;
				if (strcmp(word.c_str(), "\'") == 0)
				{
					flag = GetStr(temp, word);
					if (!flag)
						return;
					tempcon.value = word;
					flag = GetWord(temp, word);
					if (!flag)
						return;
					if (strcmp(word.c_str(), "\'") != 0)
						return;
				}
				else
				{
					tempcon.value = word;
				}
				condition.push_back(tempcon);
				flag = GetWord(temp, word);
				if (!flag)
				{
					state = DELETE_NOWHERE_CAULSE;
					return;
				}
			}
		}
	}
	else if (strcmp(word.c_str(), "execfile") == 0)
	{

		flag = GetWord(temp, word);
		if (!flag)
			return;
		fileName = word;
		state = EXECFILE;
	}
}


short int Interpret::IsInt(const char *input)
{
	int i;
	int length = strlen(input);
	if (!isdigit(input[0]) && !(input[0] == '-'))
		return 0;
	for (i = 1; i < length; i++)
	{
		if (!isdigit(input[i]))
			return 0;
	}
	return 1;
}

//This function is used to check is the string 'input' represented an float variable
//return 1 if is, 0 otherwise
short int Interpret::IsFloat(char *input)
{
	int dot = 0;
	int i;
	int length = strlen(input);
	if (!isdigit(input[0]) && !(input[0] == '-'))
		return 0;
	for (i = 1; i < length; i++)
	{
		if (!isdigit(input[i]) && input[i] != '.')
			return 0;
		else if (input[i] == '.')
			switch (dot)
		{
			case 0:
				dot++;
				break;
			default:
				return 0;
		}
	}
	return 1;
}