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
        UnsetTableDescriptions(i);
    }
    
    // is table chosen
    //isTableChosen = false;
    // clear chosen table
    currentTablesCount = 0;
    isWhereUsed = false;
    
    // clear UUID count
    //currentLastUUID = 0;
    
    // clear new record
    newRecord.clear();
    
    return;
}

void RecordManager::NewQuery(void)
{
    //isTableChosen = false;
    currentTablesCount = 0;
    isWhereUsed = false;

    conditions.clear();
    pyEvaluator.NewEvaluation();

    
    newRecord.clear();
    
    
    return;
}

void RecordManager::LoadTable(struct Table* tableStruct){
    if(tableStructs[tableStruct->tableNum] == tableStruct)
        return;
    
    tableStructs[tableStruct->tableNum] = tableStruct;
    
    vector<DataType> dataType;
    vector<bool> isIndexBuilt;
    for (int i=0; i<tableStruct->attributes.size(); i++) {
        dataType.push_back(tableStruct->attributes.at(i).dataType);
        // TODO agree on there is no index
        isIndexBuilt.push_back(tableStruct->attributes.at(i).indexName != "");
    }
    
    SetTableDescriptions(tableStruct->tableNum, dataType, isIndexBuilt);
    
    return;
}


void RecordManager::SetTableDescriptions(uint table, vector<DataType> dataType, vector<bool> isIndexBuilt)
{
    if(tableRecordDataTypes[table] != nullptr){
        delete tableRecordDataTypes[table];
        Debug("delete former record datatype of table "<<table);
    }
    tableRecordDataTypes[table] = new vector<DataType>(dataType);
    
    
    if(isTableAttributeIndexBuilt[table] != nullptr){
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


void RecordManager::UnsetTableDescriptions(uint table)
{
    // record datatype
    if (tableRecordDataTypes[table] != nullptr) {
        delete tableRecordDataTypes[table];
        tableRecordDataTypes[table] = nullptr;
    }
    
    // is attribute set
    isTableAttributeSet[table] = false;

}


void RecordManager::ChooseTable(uint table)
{
    //Debug("this function is deprecated");
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

    //AddCurrentTable(table);
    isWhereUsed = true;
    
    pyEvaluator.PushCondition(table, attribute, condition, value,
                              isTableAttributeIndexBuilt[table]->at(attribute));

    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Operator condition, float value)
{
    //AddCurrentTable(table);
    isWhereUsed = true;
    
    pyEvaluator.PushCondition(table, attribute, condition, value,
                              isTableAttributeIndexBuilt[table]->at(attribute));

    return;
}

void RecordManager::PushCondition(uint table, uint attribute, Operator condition, string value)
{
    //AddCurrentTable(table);
    isWhereUsed = true;

    pyEvaluator.PushCondition(table, attribute, condition, value,
                              isTableAttributeIndexBuilt[table]->at(attribute));

    return;
}

void RecordManager::PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2)
{
    //AddCurrentTable(table_1);
    //AddCurrentTable(table_2);
    isWhereUsed = true;
    
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


vector<set<UUID>> RecordManager::SelectUUID()
{
    vector<Record*> records;
    vector<uint> tables;
    
    
    UUID underEvaluateUUID[currentTablesCount];
    
    
    // TODO
    // optimization can be done here, to reorder the table order in currentTables
    // at 0 changes frequently most
    
    
    for (int i=0; i<currentTablesCount; i++) {
        underEvaluateUUID[i] = FIRSTUUID;
    }
    
    bool isDone = false;
    while (1) {
        records.clear();
        tables.clear();
        
        Record * r;
        for (int i=0; i<currentTablesCount; i++) {
            r = bufferManager.getRecord(tableStructs[currentTables[i]], underEvaluateUUID[i]);
            if(r!=nullptr){
                records.push_back(r);
                tables.push_back(currentTables[i]);
                Debug("record of table "<<currentTables[i]<<" uuid "<<underEvaluateUUID[i]-1<<" transfered");
            }
 
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

    return pyEvaluator.GetResult(currentTablesCount);
}


vector<vector<Record*>> RecordManager::SelectRecord()
{
    vector<vector<Record*>> results;
    vector<Record*> oneTableResults;
    
    // no where, return all
    if(isWhereUsed == false){
        for (int i=0; i<currentTablesCount; i++) {
            Table * tableStr = tableStructs[currentTables[i]];
            oneTableResults.clear();
            for (UUID uuid=FIRSTUUID; uuid<=tableStr->recordNum; uuid++) {
                Record * r=bufferManager.getRecord(tableStr, uuid);
                if (r!=nullptr) {
                    oneTableResults.push_back(r);
                    Debug("uuid: "<<uuid<<" selected");
                }
            }
            results.push_back(oneTableResults);
        }
        Debug("All selected");
        return results;
    }
    
    
    // save original table order
    uint currentTablesOrigin[currentTablesCount];
    for (int i=0; i<currentTablesCount; i++) {
        currentTablesOrigin[i] = currentTables[i];
    }
    
    vector<set<UUID>> resultsUUID = SelectUUID();
    
    
#if TEST
    if(resultsUUID.at(0).size() == 0)
        Debug("This evaluation got nothing");
#endif
    
    
    // NOTE need to restore to original order
    // can be reduced if a Table struct is included in each set of UUID
    for (int i=0; i<currentTablesCount; i++) {
        Table * tableStr = tableStructs[currentTablesOrigin[i]];
        oneTableResults.clear();
        
        int positionNow = 0;
        for (positionNow=0; positionNow<currentTablesCount; positionNow++) {
            if(currentTables[positionNow] == currentTablesOrigin[i])
                break;
        }

        for (set<UUID>::iterator it=resultsUUID.at(positionNow).begin(); it!=resultsUUID.at(positionNow).end(); it++) {
            oneTableResults.push_back(bufferManager.getRecord(tableStr, *it));
        }
        results.push_back(oneTableResults);
    }
    return results;
}


void RecordManager::DeleteRecord(uint table)
{
    if(currentTablesCount == 0)
        ChooseTable(table);
    
    // call delete all
    if(isWhereUsed == false){
        bufferManager.deleteAll(tableStructs[table]);
    }
    
    
    vector<set<UUID>> toDelete = SelectUUID();
    
    int i = 0;
    for (i = 0; i<currentTablesCount; i++) {
        if(currentTables[i] == table)
            break;
    }
    for (set<UUID>::iterator it = toDelete.at(i).begin(); it!=toDelete.at(i).end(); it++) {
        Debug("try to delete uuid: "<<*it);
        bufferManager.deleteRec(tableStructs[table], *it);
        Debug("uuid: "<<*it<<" deleted");
    }
    
    return;
}



UUID RecordManager::NextUUID(Table* tableStruct)
{
    return UUID(tableStruct->recordNum + 1);
}

bool RecordManager::AppendValue(int recordData)
{
    Debug("value append int: "<<recordData);
    int *value = new int(recordData);
    newRecord.push_back(static_cast<void*>(value));
    
    return true;
}

bool RecordManager::AppendValue(float recordData)
{
    Debug("value append float: "<<recordData);
    float *value = new float(recordData);
    newRecord.push_back(static_cast<void*>(value));
    
    return true;
}

bool RecordManager::AppendValue(string recordData)
{
    Debug("value append string: "<<recordData);
    string *value = new string(recordData);
    newRecord.push_back(static_cast<void*>(value));
   
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

    UUID *puuid = new UUID(NextUUID(tableStructs[table]));
    record->data.push_back(static_cast<void*>(puuid));
    
    for (int i=0; i<newRecord.size(); i++) {
        record->data.push_back(newRecord[i]);
    }
    record->next = nullptr;
    
    
    Debug("insert, uuid: "<<*puuid);
#if TEST
    PrintSingle(tableStructs[table],record);
#endif
    
 	bufferManager.insertRec(tableStructs[table], record);

    
    #if 0
    lastRecord->next = record;
    lastRecord = record;
    #endif
    
    
    newRecord.clear(); 
    
    return;
}
