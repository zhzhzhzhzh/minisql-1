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
        string a = "mkdir ";
        string b = ".\\";
        string cmd = a + b + dataBaseName;
        system(cmd.c_str());
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
        cout << dbV[i] << endl;
        if(dbV[i] == dataBaseName)
            break;
    }
    cout << i << endl;
    if(i == dataBaseNum)
        flag = 1;//can not find the database
    if(flag == 0)
    {
        for(int j = 0; j < tableNum; j++)
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
        string f = "-r ";
        string cmd = e + f + b + dataBaseName;
        system(cmd.c_str());
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
        if(dataBaseNameNow == "null")
        {

        }
        else
        {
            //load the information of the old database
            string FileName = ".\\" + dataBaseNameNow + "\\" + dataBaseNameNow + "Info" + ".txt";
            fstream fout;
            fout.open(FileName.c_str(), fstream::out);
            if(fout.fail() != true)
            {
                fout << tableNum << endl;
                for(int i = 0; i < tableNum; i++)
                {
                    fout << tableV[i].tableName << endl;
                    fout << tableV[i].attrNumber << endl;
                    fout << tableV[i].recordNum << endl;
                    fout << tableV[i].tableNum << endl;
                    for(int j = 0; j < tableV[i].attrNumber; j++)
                    {
                        fout << tableV[i].attributes[j].attrName << " ";
                        fout << tableV[i].attributes[j].dataType << " ";
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
        }
        //read the information of the new database

        string FileName = ".\\" + newDataBaseName + "\\" + newDataBaseName + "Info" + ".txt";
        fstream fin;
        fin.open(FileName.c_str(), fstream::in);
        if(fin.fail() != true)
        {
            fin >> tableNum;
            for(int i = 0; i < tableNum; i++)
            {
                Table tmp_table;
                fin >> tmp_table.tableName;
                fin >> tmp_table.attrNumber;
                fin >> tmp_table.recordNum;
                fin >> tmp_table.tableNum;
                for(int j = 0; j < tmp_table.attrNumber; j++)
                {
                    Attribute tmp_attribute;
                    fin >> tmp_attribute.attrName;
                    fin >> tmp_attribute.dataType;
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
return flag;
}
int catalogmanager::quit()
{
    int flag = 0;
    string FileName = ".\\" + dataBaseNameNow + "\\" + dataBaseNameNow + "Info.txt";
    fstream fout;
    fout.open(FileName.c_str(), fstream::out);
    if(fout.fail() != true)
    {
        fout << tableNum << endl;
        for(int i = 0; i < tableNum; i++)
        {
            fout << tableV[i].tableName << endl;
            fout << tableV[i].attrNumber << endl;
            fout << tableV[i].recordNum << endl;
            fout << tableV[i].tableNum << endl;
            for(int j = 0; j < tableV[i].attrNumber; j++)
            {
                fout << tableV[i].attributes[j].attrName << " ";
                fout << tableV[i].attributes[j].dataType << " ";
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
    return flag;
}
int catalogmanager::createTable(Table& tableName)
{
    int flag = 0;
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
    return flag;
}
int catalogmanager::deleteTable(int tableIndex)
{
    cout << tableIndex << endl;

    cout << "tableNume is " << tableNum << endl;
    string a = "del ";
    string b = ".\\";
    string c = "\\";
    string d = "Info.txt";
    cout << "everthing is OK" << endl;
    string e = tableV[tableIndex].tableName;
    cout << e << endl;
    string cmd = a + b + dataBaseNameNow + c + e + d;
    system(cmd.c_str());
    tableV.erase(tableV.begin() + tableIndex);
    tableNum--;
    return 0;
    cout << "delete finished" << endl;
}

int catalogmanager::getTableIndex(string tableName)
{
    int flag = -1;
    int i;
    for(i = 0; i < tableNum; i++)
    {
        if(tableV[i].tableName == tableName)
            break;
    }
    if(i < tableNum)
        flag = tableV[i].tableNum;
    return flag;
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

int catalogmanager::getDataType(int tableIndex, int attrIndex)
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
string catalogmanager::getIndexName(int tableIndex, int attrIndex)
{
    return tableV[tableIndex].attributes[attrIndex].indexName;
}
