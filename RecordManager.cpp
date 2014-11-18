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
    currentTablesCount = 0;
    
    // clear UUID count
    currentLastUUID = 0;
    
    // clear new record
    newRecord.clear();
    
    return;
}

void RecordManager::NewQuery(void)
{
    //isTableChosen = false;
    currentTablesCount = 0;

    conditions.clear();
    
    
    newRecord.clear();
    
    pyEvaluator.ClearConditions();
    
    return;
}


void RecordManager::SetTableAttributeDataType(uint table, vector<DataType> dataType, vector<bool> isIndexBuilt)
{
    if(tableRecordDataTypes[table] != NULL){
        delete tableRecordDataTypes[table];
        Debug("delete former record datatype of table "<<table);
    }
    tableRecordDataTypes[table] = new vector<DataType>(dataType);
    
    
    if(isTableAttributeIndexBuilt[table] != NULL){
        delete isTableAttributeIndexBuilt[table];
        Debug("delete former index description of table "<<table);
    }
    isTableAttributeIndexBuilt[table] = new vector<bool>(isIndexBuilt);
    
    isTableAttributeSet[table] = true;
    

    Debug("table "<<table<<" attributes set to: ");
    for(int i=0;i<tableRecordDataTypes[table]->size();i++){
        Debug("datatype:"<<tableRecordDataTypes[table]->at(i));
        Debug("index set:"<<isTableAttributeIndexBuilt[table]->at(i));
    }
    
    return;
}


void RecordManager::ChooseTable(uint table)
{
    Debug("this function is deprecated");
    AddCurrentTable(table);
}

void RecordManager::AddCurrentTable(uint table)
{
    if (currentTablesCount>=MAX_CONCURRENT_TABLE){
        Debug("too many tables; TODO: use throw");
        return;
    }
    
    for (int i=0; i<currentTablesCount; i++) {
        if(currentTables[i]==table)
            return;
    }
    currentTables[currentTablesCount++] = table;
    
    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Operator condition, int value)
{

    AddCurrentTable(table);
    
    pyEvaluator.PushCondition(table, attribute, condition, value,
                              isTableAttributeIndexBuilt[table]->at(attribute));

    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Operator condition, float value)
{
    AddCurrentTable(table);
    
    pyEvaluator.PushCondition(table, attribute, condition, value,
                              isTableAttributeIndexBuilt[table]->at(attribute));

    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Operator condition, string value)
{
    AddCurrentTable(table);

    pyEvaluator.PushCondition(table, attribute, condition, value,
                              isTableAttributeIndexBuilt[table]->at(attribute));

    return;
}

void RecordManager::PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2)
{
    AddCurrentTable(table_1);
    AddCurrentTable(table_2);
    
    
    if(getBlockCount(table_1)<getBlockCount(table_2)){
        pyEvaluator.PushCondition(table_2, attribute_2, condition, table_1, attribute_1,
                                  isTableAttributeIndexBuilt[table_2]->at(attribute_2));
    }
    else{
        pyEvaluator.PushCondition(table_1, attribute_1, condition, table_2, attribute_2,
                                  isTableAttributeIndexBuilt[table_1]->at(attribute_1));
    }
    
    return;
}


#define FIRSTUUID 1

vector<vector<UUID>> RecordManager::SelectRecord(uint table, uint *attributes)
{
    vector<Record*> records;
    vector<uint> tables;
    
    UUID underEvaluateUUID[currentTablesCount];
    
    // TODO
    // optimization can be done here, to reorder the table order
    for (int i=0; i<currentTablesCount; i++) {
        underEvaluateUUID[i] = FIRSTUUID;
    }
    
    bool isDone = false;
    while (1) {
        records.clear();
        tables.clear();
        
        for (int i=0; i<currentTablesCount; i++) {
            records.push_back(GetRecord(currentTables[i], underEvaluateUUID[i]));
            tables.push_back(currentTables[i]);
            Debug("record of table "<<currentTables[i]<<" uuid "<<underEvaluateUUID[i]-1<<" transfered");
            
            // no carry
            if((++underEvaluateUUID[i] - FIRSTUUID) < GetRecordCount(currentTables[i])){
                break;
            }
            // carry
            else{
                if(i == currentTablesCount-1) isDone = true; // highest carry
                underEvaluateUUID[i] = FIRSTUUID;
            }
            
        }
        
        if(pyEvaluator.Evaluate(tables, records, tableRecordDataTypes)) {
            Debug("evaluation finished early");
            break;
        }
        
        if(isDone) {
            Debug("evaluation finished by feeding all records");
            break;
        }
    }
    
    return pyEvaluator.GetResult();
}


void RecordManager::DeleteRecord(uint table)
{
//    vector<UUID> toDelete = SelectRecord(table);
//    
//    for (int i=0; i<toDelete.size(); i++) {
//        // TODO
//        //bufferManager.deleteRec(Table *t, toDelete.at(i));
//    }
    
    return;
}



UUID RecordManager::NextUUID(void)
{
    // TODO
    // read record number from table description structure
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
//    if(currentTablesCount != 1){
//        Debug("insert needs exactly one table");
//        return;
//    }
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
    
    
    // TODO
    // call buffer manager's insert function
    // bufferManager.insert(Table *t, Record *record);
    
    #if TEST
    lastRecord->next = record;
    lastRecord = record;
    #endif
    
    
    newRecord.clear(); 
    
    return;
}
