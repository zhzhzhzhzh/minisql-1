#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "typedefs.h"

using namespace std;
class catalogmanager
{
    public:
        catalogmanager();
        virtual ~catalogmanager();
        //define the structure of a table
        int dataBaseNum;
        int tableNum;
        string dataBaseNameNow;

        vector<string> dbV;
        vector<Table> tableV;
        vector<Attribute> attrV;

    public:
        int createDatabase(string dataBaseName);//get
        int dropDatabase(string dataBaseName);
        int useDataBase(string dataBaseName);//get
        int quit();//get
        int createTable(Table& tableName);//get
        int deleteTable(int tableIndex);//get
        int openDbFile();//get
        int closeDbFile();//get
        int getTableIndex(string tableName);//get
        int getAttrIndex(int tableIndex,string attrName);//get
        int getDataType(int tableIndex, int attrIndex);//get
        int getDataLength(int tableIndex, int attrIndex);//get
        int getAttrType(int tableIndex, int attrIndex);//get
        string getIndexName(int tableIndex, int attrIndex);
};

#endif // CATALOGMANAGER_H
