//
//  typedefs.h
//  record-index-manager
//
//  Created by ying on 11/16/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#ifndef record_index_manager_typedefs_h
#define record_index_manager_typedefs_h

#include <vector>

#define MAX_CONCURRENT_TABLE 1
#define MAX_TABLE_NUMBER 32


#define DEBUG 1
#define Debug(MESSAGE) {if(DEBUG){cout<<MESSAGE<<endl;}}

#define TEST 1


using namespace std;

enum Operator{
    Equal = 0,
    Less = 1,
    Greater = 2,
    LessEqual = 3,
    GreaterEqual = 4,
    NotEqual = 5
};

enum LogicOp{
    And = 0,
    Or = 1
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


#endif
