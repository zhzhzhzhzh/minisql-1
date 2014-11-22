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

#include "BufferManager.h"
#include "PyEvaluator.h"
#include "typedefs.h" 


using std::string;
using namespace std;



//struct FetchedValue{
//    uint table;
//    uint attribute;
//};
//
//
//
//enum ValueType{
//    FetchValue = 0,
//    IntValue = 1,
//    FloatValue = 2,
//    StringValue = 3
//};


//
//union Operand{
//    int intv;
//    float floatv;
//    //string stringv;
//    FetchedValue fetchv;
//};
//
//struct ConditionExpr{
//    //ValueType value1Type;
//    Operand v1;
//    //void* value1;
//    Operator condition;
//    ValueType value2Type;
//    void* value2;
//    
//};


    
class RecordManager{
public:
    RecordManager() {};
    ~RecordManager() {};
    
    void Initialize();
    
    void NewQuery(void);    // reentrant
    
    bool BuildIndex(uint table, uint attribute);
    bool DropIndex(uint table, uint attribute);
    
    void LoadTable(const struct Table* tableStruct);  // reentrant

    
	void CreateTable(const struct Table* tableStruct){
        LoadTable(tableStruct);
        bufferManager.createTable(tableStruct);
    }
    
    void DropTable(const struct Table* tableStruct){
        bufferManager.removeTable(tableStruct);
        UnsetTableDescriptions(tableStruct->tableNum);
    }
    
    void ChooseTable(uint table);   // reentrant
    
    void PushCondition(uint table, uint attribute, Operator condition, int value);  // reentrant
    void PushCondition(uint table, uint attribute, Operator condition, float value);    // reentrant
    void PushCondition(uint table, uint attribute, Operator condition, string value);   // reentrant
    void PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2); // reentrant
    
    void PushLogicOp(string op){
        pyEvaluator.PushLogicalOperation(op);
    }   // reentrant

    bool AppendValue(int recordData);   // reentrant
    bool AppendValue(float recordData); // reentrant
    bool AppendValue(string recordData);    // reentrant
    
    
    void InsertRecord(uint table);
    vector<vector<Record*>> SelectRecord();
    void DeleteRecord(uint table);
    
    void OnQuit(){
        bufferManager.quitDB();
    }

    
    //void Commit(void);  not needed, insert or delete implies this
    
    
    
    
#if TEST
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
    
    void PrintSingle(const Table*t, Record* r)
    {
        for(int i=0;i<r->data.size();i++){
            DataType type = tableRecordDataTypes[t->tableNum]->at(i);
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
    }
    
    Record* GetRecord(uint table, UUID uuid)
    {

        Record* r;
        r = root;
        for (int i = 0; i<uuid; i++) {
            r = r->next;
        }
        return r;
    }
    
    
    
    int getBlockCount(uint table)
    {
        switch (table) {
            case 0:
                return 100;
                break;
            case 1:
                return 200;
                break;
            default:
                Debug("get block returned -1");
                return -1;
                break;
        }
    }
    
    /* no longer used
    bool Eva(){
        vector<Record *> records;
        vector<uint> tables;
        records.push_back(root->next);
        records.push_back(root->next);
        tables.push_back(0);
        tables.push_back(0);
        return pyEvaluator.Evaluate(tables, records, tableRecordDataTypes);
    }
     */

    
#endif // end of if TEST
    
    
private:
    uint currentTables[MAX_CONCURRENT_TABLE];
    int currentTablesCount;
    bool isWhereUsed;
    PyEvaluator pyEvaluator;

    
    // logic control
    bool isTableAttributeSet[MAX_TABLE_NUMBER];
    bool isTableChosen;

    // table description
#if MACOS
    const struct Table *tableStructs[MAX_TABLE_NUMBER] = {nullptr};
    vector<DataType> *tableRecordDataTypes[MAX_TABLE_NUMBER] = {nullptr};  // pointers to data type chain, excluding the first UUID at 0
    vector<bool> *isTableAttributeIndexBuilt[MAX_TABLE_NUMBER] = {nullptr};
#endif
#if WIN
    const struct Table *tableStructs[MAX_TABLE_NUMBER];
    vector<DataType> *tableRecordDataTypes[MAX_TABLE_NUMBER];  // pointers to data type chain, excluding the first UUID at 0
    vector<bool> *isTableAttributeIndexBuilt[MAX_TABLE_NUMBER];
#endif
    
    void SetTableDescriptions(uint table, vector<DataType> dataType, vector<bool> isIndexBuilt);    // reentrant
    void UnsetTableDescriptions(uint table);

    
    //UUID currentLastUUID = 0; 
    //uint currentTables[MAX_CONCURRENT_TABLE];
    //int currentTableCount;
    
    // data
    vector<void*> newRecord;
    
    vector<void*> conditions;
    
    
    // function
    void AddCurrentTable(uint table);
    UUID NextUUID(const struct Table* tableStruct);
    vector<set<UUID>> SelectUUID();
    
    
    int GetRecordCount(const struct Table *table){
        return table->recordNum;
    }

    //bool Evaluate();

    // buffer
public:
    
    BufferManager bufferManager;
    
};

#endif /* defined(__record_index_manager__RecordManager__) */
