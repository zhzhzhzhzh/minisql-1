//
//  PyEvaluator.h
//  record-index-manager
//
//  Created by ying on 11/16/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#ifndef __record_index_manager__PyEvaluator__
#define __record_index_manager__PyEvaluator__

#include <iostream>
#include <string>
#include <set>

#include "typedefs.h"

#if MACOS
#include <Python/Python.h>
#endif
#if WIN
#include <Python.h>
#endif



class PyEvaluator{
    
public:
    PyEvaluator();
    ~PyEvaluator(){Py_Finalize();}
    
    void OnQuit(){
        PyEval_CallObject(pFuncOnQuit,NULL);
    }
    
    void NewEvaluation();
    void PushCondition(uint table, uint attribute, Operator condition, int value, bool withIndex=false);
    void PushCondition(uint table, uint attribute, Operator condition, float value, bool withIndex=false);
    void PushCondition(uint table, uint attribute, Operator condition, string value, bool withIndex=false);
    void PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2, bool withIndex=false);
    void PushLogicalOperation(string op);
    
    bool Evaluate(vector<uint>tables, vector<Record*> records, vector<DataType>* tableRecordDataTypes[]);
    vector<set<UUID>> GetResult(int currentTablesCount);
    
    
    // index manager
    void LoadIndex(uint table, uint attribute);
    void CreateIndex(uint table, uint attribute);
    void DropIndex(uint table, uint attribute);
    
    void InsertIndexNode(uint table, uint attribute, int key, UUID uuid);
    void InsertIndexNode(uint table, uint attribute, float key, UUID uuid);
    void InsertIndexNode(uint table, uint attribute, string key, UUID uuid);
    
    void DeleteIndexNode(uint table, uint attribute, int key, UUID uuid);
    void DeleteIndexNode(uint table, uint attribute, float key, UUID uuid);
    void DeleteIndexNode(uint table, uint attribute, string key, UUID uuid);
    
    

private:
    const char *pBuffer = nullptr;
    int iBufferSize = 0;
    
    PyObject *pModule = nullptr;
    PyObject *pRetValue = nullptr;

    PyObject *pFuncOnQuit = nullptr;
    
    PyObject *pFuncPushCondition = nullptr;
    PyObject *pFuncPushLogicalOperation = nullptr;
    PyObject *pFuncNewEvaluation = nullptr;
    PyObject *pFuncEvaluate = nullptr;
    PyObject *pFuncGetEvaluationResults = nullptr;
    
    
    // index manager
    PyObject *pFuncLoadIndex = nullptr;
    PyObject *pFuncCreateIndex = nullptr;
    PyObject *pFuncDropIndex = nullptr;
    PyObject *pFuncInsertIndexNode = nullptr;
    PyObject *pFuncDeleteIndexNode = nullptr;
    
    
};

    

    
    
    
#endif /* voidined(__record_index_manager__PyEvaluator__) */
