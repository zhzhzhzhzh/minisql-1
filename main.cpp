#include <iostream>
#include "stdio.h"
#include "BufferManager.h"
#include "catalogmanager.h"
#include "recordmanager.h"
//#include "IndexManager.h"
#include "Interpret.h"
#include "typedefs.h"
#include "macro.h"

using namespace std;



RecordManager Mrecord;
//IndexManager index;
catalogmanager Mcatalog;
Interpret query;
BufferManager buf;

/*API*/

void insert(Table* T, Row row){
	int tableIndex = Mcatalog.getTableIndex(T->tableName);
	Mrecord.NewQuery();

	for (int i = 1; i<T->attrNumber; i++){
cout << i << " " << T->attributes[i].attrName << endl;
		switch (T->attributes[i].dataType){
		case String:{Mrecord.AppendValue(row.columns.at(i-1).c_str()); break; }
		case Int:{Mrecord.AppendValue(atoi(row.columns.at(i-1).c_str())); break; }
		case Float:{Mrecord.AppendValue((float)atof(row.columns.at(i-1).c_str())); break; }
		default:
			break;
		}

	}
	Mrecord.InsertRecord(tableIndex);
	

}
 
void printTable(vector<vector<Record*>> r, int tableIndex, vector <Attribute>attributes)
{
	int i,j, attrIndex;
	cout << "\n\t" << endl;
	for (i=0; i < attributes.size(); i++)
		cout << attributes[i].attrName << "\t" << endl;
	
	cout << "\n\t" << endl;
	for (i = 0; i < r.size(); i++)
	{
		for (j = 0; j < attributes.size(); j++)
		{
			attrIndex = Mcatalog.getAttrIndex(tableIndex, attributes[i].attrName);
			DataType type = Mcatalog.getDataType(tableIndex, attrIndex);
			switch (type) {
			case Int:
				cout << *(int*)r[i][j]->data[attrIndex] << "\t" << endl;
				break;

			case Float:
				cout << *(float*)r[i][j]->data[attrIndex] << "\t" << endl;
				break;

			case String:
				cout << *(char*)r[i][j]->data[attrIndex] << "\t" << endl;
				break;

			default:
				break;
			}
		}
	}
}

/* The API Function */
void execute(void)
{
	int i;
	int attrIndex = -1;
	int TableIndex = -1;
	vector<vector<Record*>> selrecord;
	vector <const Table* > tables;
	tables = Mcatalog.getTableStructs();
	for (i = 0; i < tables.size(); i++)
		Mrecord.LoadTable(tables[i]);
	
	switch (query.state)
	{
	    case CRETAB:
	    {
				   if (!Mcatalog.createTable(query.NewTableInfo)){
					   Mrecord.CreateTable(&query.NewTableInfo);
					   cout << "Table " << query.NewTableInfo.tableName << " has been created successfully" << endl;
				   }
				   else
					   cout << "something wrong happened in the database" << endl;
				   break;
	    }
	    case CREIND:
	    {
				   TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
				   int attrIndex = query.getIndexInfo.column;
				   if (query.TableInfo->attributes[query.getIndexInfo.column].attrType != 0){
					   cout << "Column " << query.TableInfo->attributes[query.getIndexInfo.column].attrName << "  is not primary." << endl;
					   break;
				   }
				   else if (!Mcatalog.createIndex(query.IndexName, TableIndex, attrIndex)){
					   //BuildIndex(uint table, uint attribute);
					   cout << "Index " << query.IndexName << " has been created successfully" << endl;
					   //调用Mrecordmanage的函数
				   }
				   else
					   cout << "something wrong happened in the database" << endl;
				   break;
	    }
	    case CREDB:
	    {
				  if (!Mcatalog.createDatabase(query.db_name))
					   cout << "create database successfully" << endl;
				  else
				       cout << "something wrong happened in the database" << endl;
				  break;
	    }
	    case DRPTAB:
		{
				  TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
				  Mrecord.DropTable(query.TableInfo);
			      if (!Mcatalog.deleteTable(TableIndex)){
				       cout << "Table " << query.TableInfo->tableName << " has been dropped successfully" << endl;
		          }
		          else
			           cout << "something wrong happened in the database" << endl;
		          break;
	    }
        case DRPDB:
	    {
				  if (!Mcatalog.dropDatabase(query.db_name)){
					   cout << "Database " << query.db_name << " has been dropped successfully" << endl;
				  }
				  else
					   cout << "something wrong happened in the database" << endl;
				  break;
	    }
	    case DRPIND:
	    {
				  if (!Mcatalog.deleteIndex(query.IndexName)){
						   //DropIndex(uint table, uint attribute);
					   cout << "Index " << query.IndexName << " has been dropped successfully" << endl;
				  }
				  else
					   cout << "something wrong happened in the database" << endl;
				  break;
	    }
	    case QUIT:
	    {
				  Mrecord.OnQuit();
				  Mcatalog.quit();
				  cout << "Have a good day! Press any key to close this window." << endl;
				  getchar();
				  exit(0);
				  break;
	    }
	    case INSERT:
	    {
				  insert(query.TableInfo, query.row);
				  TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
				  Mcatalog.addRecord(TableIndex);
				  cout << "One record has been inserted successfully" << endl;
				  break;
	    }
		case USEDB:
		{
				  if (Mcatalog.useDataBase(query.db_name))
					   cout << "something wrong happened in the database" << endl;
				  break;
		}
		case SELECT_NOWHERE_CAULSE:
		{
				  Mrecord.NewQuery();
				  TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
				  Mrecord.ChooseTable(TableIndex);
				  selrecord = Mrecord.SelectRecord(); 
				  //根据列及返回的record打印表
				  printTable(selrecord, TableIndex, query.column);
				  break;
		}
		case SELECT_WHERE_CAULSE:
		{
				  Mrecord.NewQuery();
				  TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
			      Mrecord.ChooseTable(TableIndex);
				  for (i = 0; i<query.condition.size(); i++){
					  switch (Mcatalog.getDataType(TableIndex, query.condition[i].columnNum))
					  {
					      case String:{
									  Mrecord.PushCondition(TableIndex, query.condition[i].columnNum, query.condition[i].op, query.condition[i].value); 
									  break;
					      }
					      case Int:{
								   Mrecord.PushCondition(TableIndex, query.condition[i].columnNum, query.condition[i].op, atoi(query.condition[i].value.c_str()));  
								   break;
					      }
					      case Float:{
									 Mrecord.PushCondition(TableIndex, query.condition[i].columnNum, query.condition[i].op, (float)atof(query.condition[i].value.c_str()));  
									 break;
					      }
					  }
				  }
										
				  Mrecord.PushLogicOp(query.condition[i].Lop);
				  selrecord = Mrecord.SelectRecord();
				  printTable(selrecord, TableIndex, query.column);
				  break;
		}
		case DELETE_WHERE_CAULSE:
		{
									Mrecord.NewQuery();
									TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
									Mrecord.ChooseTable(TableIndex);
									for (i = 0; i < query.condition.size(); i++){
										switch (Mcatalog.getDataType(TableIndex, query.condition[i].columnNum))
										{
										    case String:{
														Mrecord.PushCondition(TableIndex, query.condition[i].columnNum, query.condition[i].op, query.condition[i].value);  //TODO  overload
														break;
										    }
										    case Int:{
													 Mrecord.PushCondition(TableIndex, query.condition[i].columnNum, query.condition[i].op, atoi(query.condition[i].value.c_str()));  //TODO  overload
													 break;
										    }
										    case Float:{
													   Mrecord.PushCondition(TableIndex, query.condition[i].columnNum, query.condition[i].op, (float)atof(query.condition[i].value.c_str()));  //TODO  overload
													   break;
										    }
										}
										Mrecord.DeleteRecord(TableIndex);
										break;
									}
		}
		case DELETE_NOWHERE_CAULSE:
		{
									  Mrecord.NewQuery();
									  attrIndex;
									  TableIndex = Mcatalog.getTableIndex(query.TableInfo->tableName);
									  Mrecord.ChooseTable(TableIndex);
									  Mrecord.DeleteRecord(TableIndex);
									  break;
		}
			/*-----below is some error information------*/
		case TABLEEXISTED:{
							  cout << "The table has been created,please check the database" << endl;
							  break;
		}
		case INSERTERR:{
						   cout << "Incorrect usage of \"insert\" query! Please check your input!" << endl;
						   break;
		}
		case INDEXERROR:{
							cout << "The index on primary key of table has been existed" << endl;
							break;
		}
		case CREINDERR:{
						   cout << "Incorrect usage of \"create index\" query! Please check your input!" << endl;
						   break;
		}
		case EMP:{
					 cout << "EMP query! Please enter your command!" << endl;
					 break;
		}
		case UNKNOW:{
						cout << "UNKNOW query! Please check your input!" << endl;
						break;
		}
		case SELERR:{
						cout << "Incorrect usage of \"select\" query! Please check your input!" << endl;
						break;
		}
		case CRETABERR:{
						   cout << "Incorrect usage of \"create table\" query! Please check your input!" << endl;
						   break;
		}
		case DELETEERR:{
						   cout << "Incorrect usage of \"delete from\" query! Please check your input!" << endl;
						   break;
		}
		case DRPTABERR:{
						   cout << "Incorrect usage of \"drop table\" query! Please check your input!" << endl;
						   break;
		}
		case DRPINDERR:{
						   cout << "Incorrect usage of \"drop index\" query! Please check your input!" << endl;
						   break;
		}
		case VOIDPRI:{
						 cout << "Error: invalid primary key! Please check your input!" << endl;
						 break;
		}
		case VOIDUNI:{
						 cout << "Error: invalid unique key! Please check your input!" << endl;
						 break;
		}
		case CHARBOUD:{
						  cout << "Error: only 1~255 charactors is allowed! Please check your input!" << endl;
						  break;
		}
		case NOPRIKEY:{
						  cout << "No primary key is defined! Please check your input!" << endl;
						  break;
		}
		case TABLEERROR:{
							cout << "Table is not existed,please check the database" << endl;
							break;
		}
		case INDEXEROR:{
						   cout << "Index is not existed,please check the database" << endl;
						   break;
		}
		case COLUMNERROR:{
							 cout << "One column is not existed" << endl;
							 break;
		}
		case INSERTNUMBERERROR:{
								   cout << "The column number is not according to the columns in our database" << endl;
								   break;
		}
		case DBERR:{
					   cout << "The database is not existed" << endl;
					   break;
		}
		case CREDBERR:{
						  cout << "The database is already existed" << endl;
						  break;
		}
		case INSERT_VALUES_TYPERR:{
									  cout << "Some of the values are not correct" << endl;
									  break;
		}

		}
}

void AddSeperator(char *command)
{
	unsigned len = strlen(command);
	command[len] = ' ';
	command[len + 1] = '\0';
}

short int IsComEnd(char *input)
{
	unsigned int next = strlen(input) - 1;
	char prev = ' ';
	while (next >= 0 && (prev == '\t' || prev == ' '))
	{
		prev = input[next];
		next--;
	}
	if (prev == ';')
	{
		input[next + 1] = '\0';
		return 1;
	}
	return 0;
}



int main()
{
	cout << "Welcome !" << endl;

	vector<Condition> conditions;
	Table tableinfor;
	Index indexinfor;
	Row insertValue;
	//Data datas;
	char command[COMLEN] = "";
	char input[INPUTLEN] = "";
	char word[WORDLEN] = "";
	short int ComEnd = 0;

	while (1)
	{
		strcpy_s(command, "");//command清零
		ComEnd = 0;
		cout << "miniSQL>>";
		while (!ComEnd)
		{

			gets_s(input);
			if (IsComEnd(input))
				ComEnd = 1;
			strcat(command, input);//保存分号结束之前的命令，不含分号
			AddSeperator(command);//在command命令的字符串加一个空格在字符串结尾，并在空格后补上'\0'
		}
		query.Parse(command);
		execute();
	}
	getchar();
	return 0;
}