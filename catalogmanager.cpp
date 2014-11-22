#include "catalogmanager.h"

catalogmanager::catalogmanager()
{
    //ctordfdsjj
}

catalogmanager::~catalogmanager()
{
    //dtor
}
//flag=1,database already exist;flag=0,create a database successfully
int catalogmanager::isDBExisted(string db_name)//return 1,find
{
    openDbFile();
    int flag = 0;
    int i;
    for(i = 0; i < dataBaseNum; i++)
    {
        if(dbV[i] == db_name)
        {
            flag = 1;
            i = dataBaseNum;
        }
    }
    closeDbFile();
    return flag;
}
int catalogmanager::createDatabase(string dataBaseName)
{
    openDbFile();
    int flag = 0;
    int i;
    for(i = 0; i < dataBaseNum; i++)
    {
        if(dbV[i] == dataBaseName)
            break;
    }
    if(i != dataBaseNum)
        flag = 1;
    if(dataBaseNum == 0)
        flag = 0;
    if(flag == 0)
    {
        #if WIN
        string a = "mkdir ";
        string cmd = a  + dataBaseName;
        system(cmd.c_str());
        #endif

        #if MACOS
        string a = "mkdir ";
        string cmd = a  + dataBaseName;
        system(cmd.c_str());
        #endif

        string newFileName = ".\\" + dataBaseName + "\\" + dataBaseName + "Info" + ".txt";
        fstream newfile;
        newfile.open(newFileName.c_str(), fstream::out);
        newfile << 0;
        newfile.close();
        dataBaseNum++;
        dbV.push_back(dataBaseName);
    }
    closeDbFile();
    return flag;
}
int catalogmanager::dropDatabase(string dataBaseName)
{
    openDbFile();
    int flag = 0;
    int i;
    for(i = 0; i < dataBaseNum; i++)
    {
        //cout << dbV[i] << endl;
        if(dbV[i] == dataBaseName)
            break;
    }
    //cout << i << endl;
    if(i == dataBaseNum)
        flag = 1;//can not find the database
    if(flag == 0)
    {
        #if WIN
        quit();
        string a = "rmdir ";
        string b = "/S /Q ";
        string c = a + b + dataBaseName;
        system(c.c_str());
        #endif

        #if MACOS
        string a = "rm ";
        string b = "-rf";
        string c = a + b + dataBaseName;
        system(c.c_str());
        #endif
        /*for(int j = 0; j < totalTableNum; j++)
        {
            deleteTable(j);
            cout << "delete table " << j << endl;
        }
        quit();
        string a = "del ";
        string b = ".\\";
        string c = "\\";
        string d = "Info.txt";
        string Info = a + b + dataBaseName + c + dataBaseName + d;
        system(Info.c_str());
        string e = "rmdir ";
        string cmd = e + b + dataBaseName;
        system(cmd.c_str());*/
        dbV.erase(dbV.begin() + i);
        dataBaseNum--;
    }
    closeDbFile();
    return flag;
}
int catalogmanager::openDbFile()//flag = 0 successfully
{
    int flag = 0;
    const string fileName = "dataBaseInfo.txt";
    fstream fin(fileName.c_str(),ios::in);
    if(fin.fail()!= true)
    {
        fin >> dataBaseNum;
        string temp_dataBaseName;
        for(int i = 0; i < dataBaseNum; i++)
        {
            fin >> temp_dataBaseName;
            dbV.push_back(temp_dataBaseName);
        }
        fin.close();
    }
    else
    {
        flag = 1;
    }
    return flag;
}
int catalogmanager::closeDbFile()//flag = 0 successfully
{
    int flag = 0;
    const string fileName = "dataBaseInfo.txt";
    fstream fout(fileName.c_str(), ios::out);
    if(fout.fail()!= true)
    {
        fout << dataBaseNum << endl;
        for(int i = 0;i < dataBaseNum; i++)
        {
            fout << dbV[i] << endl;
        }
        fout.close();
    }
    else
    {
        flag = 1;
    }
    return flag;
}
int catalogmanager::useDataBase(string newDataBaseName)
{
    int flag = 0;
    if(newDataBaseName == dataBaseNameNow)
    {

    }
    else
    {
        if(dataBaseNameNow == "null")//when start the program
        {

        }
        else
        {
            //load the information of the old database
            string FileName = ".\\"  + dataBaseNameNow + "Info" + ".txt";
            fstream fout;
            fout.open(FileName.c_str(), fstream::out);
            if(fout.fail() != true)
            {
                fout << totalTableNum << endl;
                for(int i = 0; i < totalTableNum; i++)
                {
                    fout << tableV[i].tableName << endl;
                    fout << tableV[i].attrNumber << endl;
                    fout << tableV[i].recordNum << endl;
                    fout << tableV[i].tableNum << endl;
                    for(int j = 0; j < tableV[i].attrNumber; j++)
                    {
                        fout << tableV[i].attributes[j].attrName << " ";
                        fout << switchEnumToInt(tableV[i].attributes[j].dataType)<< " ";
                        fout << tableV[i].attributes[j].dataLength << " ";
                        fout << tableV[i].attributes[j].attrType << " ";
                        fout << tableV[i].attributes[j].indexName << endl;
                    }//for
                }//for
                fout.close();
            }
            else
            {
                flag = 1;
            }
            string pathOld = "cd ..";
            system(pathOld.c_str());
        }
        //read the information of the new database

        string FileName = ".\\" + newDataBaseName + "\\" + newDataBaseName + "Info" + ".txt";
        fstream fin;
        fin.open(FileName.c_str(), fstream::in);
        if(fin.fail() != true)
        {
            fin >> totalTableNum;
            for(int i = 0; i < totalTableNum; i++)
            {
                Table tmp_table;
                fin >> tmp_table.tableName;
                fin >> tmp_table.attrNumber;
                fin >> tmp_table.recordNum;
                fin >> tmp_table.tableNum;
                for(int j = 0; j < tmp_table.attrNumber; j++)
                {
                    int tmpInt;
                    Attribute tmp_attribute;
                    fin >> tmp_attribute.attrName;
                    fin >> tmpInt;
                    tmp_attribute.dataType = switchIntToEnum(tmpInt);
                    fin >> tmp_attribute.dataLength;
                    fin >> tmp_attribute.attrType;
                    fin >> tmp_attribute.indexName;
                    tmp_table.attributes.push_back(tmp_attribute);
                }//for
                tableV.push_back(tmp_table);
            }//for
            fin.close();
            dataBaseNameNow = newDataBaseName;
        }//if
        else//if(fail)
        {
            flag = 1;
        }
    }
    string  pathNew = "cd " + dataBaseNameNow;
    system(pathNew.c_str());
    Debug("use some dataBase successfully");
return flag;
}

DataType catalogmanager::switchIntToEnum(int enumIn)
{
    if(enumIn == 0)
        return Int;
    else if(enumIn == 1)
        return Float;
    else //if(enumIn == 2)
        return String;
}

int catalogmanager::switchEnumToInt(DataType dataTypeIn)
{
    if(dataTypeIn == Int)
        return 0;
    else if(dataTypeIn == Float)
        return 1;
    else //if(dataTypeIn == String)
        return 2;
}

int catalogmanager::quit()
{
    int flag = 0;
	Debug("database now " << dataBaseNameNow);
    string FileName = ".\\" + dataBaseNameNow + "Info.txt";
    fstream fout;
    fout.open(FileName.c_str(), fstream::out);
    if(fout.fail() != true)
    {
        fout << totalTableNum << endl;
        for(int i = 0; i < totalTableNum; i++)
        {
            fout << tableV[i].tableName << endl;
            fout << tableV[i].attrNumber << endl;
            fout << tableV[i].recordNum << endl;
            fout << tableV[i].tableNum << endl;
            for(int j = 0; j < tableV[i].attrNumber; j++)
            {
                fout << tableV[i].attributes[j].attrName << " ";
                fout << switchEnumToInt(tableV[i].attributes[j].dataType)<< " ";
                fout << tableV[i].attributes[j].dataLength << " ";
                fout << tableV[i].attributes[j].attrType << " ";
                fout << tableV[i].attributes[j].indexName << endl;
            }
        }
        fout.close();
    }
    else
    {
        flag = 1;
    }
    string path = "cd ..";
    system(path.c_str());
    Debug("quit the database successfully!!");
    return flag;
}
int catalogmanager::createTable(Table& tableNameIn)//TODO datalength increment
{
    /*int flag = 0;
    string newfileName = ".\\" + dataBaseNameNow + "\\" + tableName.tableName + "Info" + ".txt";
    fstream newFile;
    newFile.open(newfileName.c_str(),fstream::out);
    if(newFile.fail() != true)
    {
        newFile.close();
        tableNum++;
        tableV.push_back(tableName);
    }
    else
    {
        flag = 1;
    }
    return flag;*/
    Table T_temp;
    T_temp.tableName = tableNameIn.tableName;
    T_temp.attrNumber = tableNameIn.attrNumber;
    T_temp.attributes = tableNameIn.attributes;
    T_temp.recordNum = tableNameIn.recordNum;
    int i,j;
    int tableNumArray[32];
    //int flag = 0;
    for(i = 0; i < 32; i++)
    {
        tableNumArray[i] = i;
    }
    for(i = 0; i < totalTableNum; i++)
    {
        j = tableV[i].tableNum;
        tableNumArray[j] = -1;
    }
    for(i = 0; i < 32; i++)
    {
        if(tableNumArray[i] != -1)
        {
            T_temp.tableNum = i;
            i = 32;
        }

    }
    totalTableNum++;
    tableV.push_back(T_temp);
    Debug("create a table successfully!!");
    return 0;


}
int catalogmanager::deleteTable(int tableIndex)
{
    //cout << tableIndex << endl;

    /*cout << "totalTableNum is " << totalTableNum << endl;
    string a = "del ";
    string b = ".\\";
    string c = "\\";
    string d = "Info.txt";
    cout << "everthing is OK" << endl;*/
    //string e = tableV[tableIndex].tableName;
    //cout << e << endl;
    //string cmd = a + b + dataBaseNameNow + c + e + d;
    //system(cmd.c_str());
    tableV.erase(tableV.begin() + tableIndex);
    totalTableNum--;
    Debug("delete finished");
    return 0;
}

int catalogmanager::getTableIndex(string tableName)
{
    int flag = -1;
    int i;
    for(i = 0; i < totalTableNum; i++)
    {
        if(tableV[i].tableName == tableName)
            break;
    }
    if(i < totalTableNum)
        flag = i;
    return flag;
}
int catalogmanager::getTableNum(string tableName)
{
    int i = getTableIndex(tableName);
    return tableV[i].tableNum;
}

int catalogmanager::getAttrIndex(int tableIndex, string attrName)
{
    int flag = -1;
    int i;
    for(i = 0; i < tableV[tableIndex].attrNumber; i++)
    {
        if(tableV[tableIndex].attributes[i].attrName == attrName)
            break;
    }
    if(i < tableV[tableIndex].attrNumber)
    {
        flag = i;
    }
    return flag;
}

DataType catalogmanager::getDataType(int tableIndex, int attrIndex)
{
    return tableV[tableIndex].attributes[attrIndex].dataType;
}
int catalogmanager::getDataLength(int tableIndex, int attrIndex)
{
    return tableV[tableIndex].attributes[attrIndex].dataLength;
}
int catalogmanager::getAttrType(int tableIndex, int attrIndex)
{
    return tableV[tableIndex].attributes[attrIndex].attrType;
}
string catalogmanager::getIndexName(string indexName)
{
    int i=0 ,j=0;
    int flag = 0;
    for(i = 0; i < totalTableNum; i++)
    {
        for(j = 0; j < tableV[i].attrNumber; j++)
        {
            if(indexName == tableV[i].attributes[j].indexName)
            {
                flag = 1;
            }
        }
    }
    if(flag == 1)
        return indexName;
    else
        return "null";
}
int catalogmanager::createIndex(string indexName, int tableIndex, int attrIndex)//return 1 for success
{
    string indexNameTemp = getIndexName(indexName);
    if(indexNameTemp == "null")
    {
        tableV[tableIndex].attributes[attrIndex].indexName = indexName;
        return 0;
    }
    else
        return 1;
}
int catalogmanager::deleteIndex(string indexName)
{
    string indexNameTemp = getIndexName(indexName);
    if(indexNameTemp == "null")
        return 1;
    else
    {
        int i=0 ,j=0;
        int flag = 0;
        for(i = 0; i < totalTableNum; i++)
        {
            for(j = 0; j < tableV[i].attrNumber; j++)
            {
                if(indexName == tableV[i].attributes[j].indexName)
                {
                    flag =1;
                    break;
                }
            }
            if(flag == 1)
                break;
        }
        tableV[i].attributes[j].indexName = "null";
        return 0;
    }
}
int catalogmanager::addRecord(int tableIndex)
{
    tableV[tableIndex].recordNum++;
    return 0;
}
int catalogmanager::deleteRecord(int tableIndex)
{
    tableV[tableIndex].recordNum--;
    return 0;
}
int catalogmanager::getAttrNumber(int tableIndex)
{
    return tableV[tableIndex].attrNumber;
}
const Table* catalogmanager::getTableInformation(string tableName)
{
    int i = 0;
    for(i = 0; i < totalTableNum; i++)
    {
        if(tableV[i].tableName == tableName)
            break;
    }
    //Table temp_table;
    //temp_table.tableName = tableV[i].tableName;
    //temp_table.attrNumber = tableV[i].attrNumber;
    //temp_table = tableV[i];
    return &tableV[i];
}