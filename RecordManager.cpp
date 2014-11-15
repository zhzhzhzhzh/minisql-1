//
//  RecordManager.cpp
//  record-index-manager
//
//  Created by ying on 10/27/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#include "RecordManager.h"

void RecordManager::Initialize()
{
    for(int i=0; i<MAX_TABLE_NUMBER; i++){
        // is attribute set
        isTableAttributeSet[i] = false;
        // is record datatype set
        if(tableRecordDataTypes[i]!=NULL){
            delete tableRecordDataTypes[i];
            tableRecordDataTypes[i] = NULL;
        }
    }
    // is table chosen
    //isTableChosen = false;
    // clear chosen table
    //currentTableCount = 0;
    
    // clear UUID count
    currentLastUUID = 0;
    
    // clear new record
    newRecord.clear();
    
    return;
}

void RecordManager::NewQuery(void)
{
    //isTableChosen = false;
    //currentTableCount = 0;

    conditions.clear();
    
    
    newRecord.clear();
    
    return;
}


void RecordManager::SetTableAttributeDataType(uint table, vector<DataType> dataType)
{
    if(tableRecordDataTypes[table] != NULL){
        delete tableRecordDataTypes[table];
        Debug("delete former record datatype of table "<<table);
    }
    tableRecordDataTypes[table] = new vector<DataType>(dataType);
    isTableAttributeSet[table] = true;
    

    Debug("table "<<table<<" attributes set to: ");
    for(int i=0;i<tableRecordDataTypes[table]->size();i++){
        Debug(tableRecordDataTypes[table]->at(i));
    }
    
    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Conditions condition, int value)
{
    /*
    ConditionExpr* expr = new ConditionExpr;
    expr.table_1 = table;
    expr.table_2 = attribute;
    expr.condition=condition;
    expr.op2.valueInt = value;
    
    conditions.push_back(expr);
     */
    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Conditions condition, float value)
{
    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Conditions condition, string value)
{
    return;
}

void RecordManager::PushCondition(uint table_1, uint attribute_1, Conditions condition, uint table_2, uint attribute_2)
{
    return;
}


UUID RecordManager::NextUUID(void)
{
    return currentLastUUID+=1;
}

bool RecordManager::AppendValue(int recordData)
{
    int *value = new int(recordData);
    newRecord.push_back(value);
    
    return true;
}

bool RecordManager::AppendValue(float recordData)
{
    float *value = new float(recordData);
    newRecord.push_back(value);
    
    return true;
}

bool RecordManager::AppendValue(string recordData)
{
    string *value = new string(recordData);
    newRecord.push_back(value);
    
    return true;
}

void RecordManager::InsertRecord(uint table)
{
    if(isTableAttributeSet[table] == false){
        Debug("table attribute not set");
        return;
    }
    
    Record* record = new Record;

    UUID *uuid = new UUID(NextUUID());
    record->data.push_back(uuid);
    
    for (int i=0; i<newRecord.size(); i++) {
        record->data.push_back(newRecord[i]);
    }
    record->next = NULL;
    

    lastRecord->next = record;
    lastRecord = record;
    
    
    newRecord.clear(); 
    
    return;
}
