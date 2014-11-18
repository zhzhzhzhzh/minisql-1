#ifndef ALL_H_INCLUDED
#define ALL_H_INCLUDED
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

typedef long unsigned UUID;

enum DataType{
    // supported types
    Int = 0,
    Float = 1,
    String =2,
    
    // uuid type
    Uuid = 3
};

struct Record{
    vector<void *> data;
    struct Record * next;
};

struct Attribute
{
    string attrName;
    int dataType;//dataType = 0 nchar;dataType = 1,int; dataType = 2,float;
    int dataLength;
    int attrType;//attrType = 0 primary;attrType = 1 unique; attrType = 2,null
    string index_name;
};

 struct Table
{
    string tableName;
    int attrNumber;
    int recordNum;
    int tableNum;			
    vector<Attribute> attributes;
};

struct FileInf;

#endif /* ALL_H_INCLUDED */

