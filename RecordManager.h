//
//  RecordManager.h
//  record-index-manager
//
//  Created by ying on 10/27/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#ifndef __record_index_manager__RecordManager__
#define __record_index_manager__RecordManager__

#include <iostream>
#include <string>
#include <vector>

#define DEBUG 1
#define Debug(MESSAGE) {if(DEBUG){cout<<MESSAGE<<endl;}}


#define MAX_CONCURRENT_TABLE 4
#define MAX_TABLE_NUMBER 32

using std::string;
using namespace std;

enum Conditions{
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

struct Value{
    uint table;
    uint attribute;
};

union ConditionOp2{
    int valueInt;
    float valueFloat;
    string valueString;
    Value value_2;
};

struct ConditionExpr{
    Value value_1;
    Conditions condition;
    ConditionOp2 op2;
};



class RecordManager{
public:
    RecordManager() {};
    ~RecordManager() {};
    
    void Initialize();
    
    void NewQuery(void);
    
    void SetTableAttributeDataType(uint table, vector<DataType> dataType);
    void ChooseTable(uint table);   // reentrant
    
    void PushCondition(uint table, uint attribute, Conditions condition, int value);
    void PushCondition(uint table, uint attribute, Conditions condition, float value);
    void PushCondition(uint table, uint attribute, Conditions condition, string value);
    void PushCondition(uint table_1, uint attribute_1, Conditions condition, uint table_2, uint attribute_2);
    
    void PushLogicOp(LogicOp op);

    bool AppendValue(int recordData);
    bool AppendValue(float recordData);
    bool AppendValue(string recordData);
    
    
    void InsertRecord(uint table);
    void DeleteRecord(uint table);
    void SelectRecord(uint table, uint *attributes = NULL);
    
    void Commit(void);
    
    
    
    
    // test=======================
    Record* root;
    Record* lastRecord;

    
    void PrintRecord(uint table)
    {
        Record * r = root->next;
        
        while (r != NULL) {
            Debug("uuid "<<*(UUID*)r->data[0]);
            
            for(int i=1;i<r->data.size();i++){
                DataType type = tableRecordDataTypes[table]->at(i-1);
                switch (type) {
                    case Int:
                        cout<<*(int*)r->data[i]<<endl;
                        break;
                        
                    case Float:
                        cout<<*(float*)r->data[i]<<endl;
                        break;
                        
                    case String:
                        cout<<*(string*)r->data[i]<<endl;
                        break;
                        
                    default:
                        break;
                }
            }
            r = r->next;
        }

    }
    
    Record* GetRecord(uint table, UUID uuid)
    {
        ;
    }
    
    // test=======================

    
    
private:
    uint currentTables[MAX_CONCURRENT_TABLE];
    //int tableCount;
    
    // logic control
    bool isTableAttributeSet[MAX_TABLE_NUMBER];
    bool isTableChosen;

    // table description
    vector<DataType>* tableRecordDataTypes[MAX_TABLE_NUMBER];
    UUID currentLastUUID = 0;
    //uint currentTables[MAX_CONCURRENT_TABLE];
    //int currentTableCount;
    
    // data
    vector<void*> newRecord;
    

    vector<void*> conditions;
    
    
    // function
    UUID NextUUID(void);

    bool Evaluate();
    
    

    
};

#endif /* defined(__record_index_manager__RecordManager__) */
