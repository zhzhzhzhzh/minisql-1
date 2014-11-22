//
//  typedefs.h
//  record-index-manager
//
//  Created by ying on 11/16/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#ifndef record_index_manager_typedefs_h
#define record_index_manager_typedefs_h

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "environment.h"


#define MAX_CONCURRENT_TABLE 1
#define MAX_TABLE_NUMBER 32


#define DEBUGG 1
#define Debug(MESSAGE) {if(DEBUGG){cout<<MESSAGE<<endl;}}
#define DebugS(MESSAGE) {if(DEBUGG){cout<<MESSAGE;}}


#define TEST 1


#if MACOS
#define SETPYTHONPATH "import sys\nsys.path.append('/Users/ying/Documents/DEV/record-index-manager/record-index-manager')"
#endif
#if WIN
#define SETPYTHONPATH "import sys\nsys.path.append('D:\\db3.0\\minisql-1')"
#endif

#if WIN
typedef unsigned int uint;
#endif

using namespace std;

enum Operator{
    Equal = 0,
    Less = 1,
    Greater = 2,
    LessEqual = 3,
    GreaterEqual = 4,
    NotEqual = 5
};




/*
 uuid is used to uniquely identify every record in the table.
 This is useful when it involves the index.
 Because we cannot store pointer value to every record,
 we can store the uuid of every record.
 
 When the records are written to the file, the uuids are saved.
 When the records are read from the file, the uuid are read,
 and thus pointers can point to the correct records.
 */
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
    // TODO change to enum type
    DataType dataType;//dataType = 0 nchar;dataType = 1,int; dataType = 2,float;
    int dataLength;
    // TODO use enum type
    int attrType;//attrType = 0 primary;attrType = 1 unique; attrType = 2,null
    string indexName; // if not exists, indexName = "null"
};

struct Table
{
    string dbName;
    string tableName;
    int attrNumber;
    int recordNum;
    int tableNum;
    vector<Attribute> attributes;
};

struct FileInf;

#endif /* ALL_H_INCLUDED */
