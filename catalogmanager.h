#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "all.h"

using namespace std;
class catalogmanager
{
    public:
        catalogmanager();
        virtual ~catalogmanager();

        int dataBaseNum;
        int totalTableNum;
        string dataBaseNameNow;

        vector<string> dbV;
        vector<Table> tableV;
        vector<Attribute> attrV;

    public:
        int createDatabase(string dataBaseName);
        int dropDatabase(string dataBaseName);//TODO test again
        int useDataBase(string dataBaseName);
        int quit();
        int createTable(Table& tableName);
        int deleteTable(int tableIndex);
        int openDbFile();
        int closeDbFile();
        int getTableIndex(string tableName);//TODO used in vector
        int getTableNum(string tableNameIn);//TODO used in buffer
        int getAttrNumber(int tableIndex);
        int getAttrIndex(int tableIndex,string attrName);
        DataType getDataType(int tableIndex, int attrIndex);
        int getDataLength(int tableIndex, int attrIndex);
        int getAttrType(int tableIndex, int attrIndex);
        string getIndexName(string indexName);//TODO
        int createIndex(string indexName, int tableIndex, int attrIndex);//TODO
        int deleteIndex(string indexName);//TODO
        DataType switchIntToEnum(int enumIn);
        int switchEnumToInt(DataType DataType);
        int addRecord(int tableIndex);//TODO
        int deleteRecord(int tableIndex);//TODO
        vector<const Table*> getTableStructs(){
            vector<const Table*> v;
            for (int i = 0; i < tableV.size(); ++i)
            {
                v.push_back(&(tableV.at(i)));
            }
            return v;
        }
        Table const* getTableInformation(string tableName);//TODO
};

#endif // CATALOGMANAGER_H
