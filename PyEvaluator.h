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
#include <Python/Python.h>

#include "typedefs.h"


class PyEvaluator{
    
public:
    PyEvaluator();
    ~PyEvaluator(){Py_Finalize();}
    
    void NewEvaluation();
    void PushCondition(uint table, uint attribute, Operator condition, int value, bool withIndex=false);
    void PushCondition(uint table, uint attribute, Operator condition, float value, bool withIndex=false);
    void PushCondition(uint table, uint attribute, Operator condition, string value, bool withIndex=false);
    void PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2, bool withIndex=false);
    void PushLogicalOperation(string op);
    
    bool Evaluate(vector<uint>tables, vector<Record*> records, vector<DataType>* tableRecordDataTypes[]);
    vector<set<UUID>> GetResult(int currentTablesCount);


private:
    const char *pBuffer = nullptr;
    int iBufferSize = 0;
    
    PyObject *pModule = nullptr;
    PyObject *pRetValue = nullptr;

    PyObject *pFuncPushCondition = nullptr;
    PyObject *pFuncPushLogicalOperation = nullptr;
    PyObject *pFuncNewEvaluation = nullptr;
    PyObject *pFuncEvaluate = nullptr;
    PyObject *pFuncGetEvaluationResults = nullptr;

    
    
//    PyObject* pArgs = Py_BuildValue("ii", 12, 14);
//    
//    PyObject* pRet = PyEval_CallObject(pFunc,pArgs);
//    
//    PyArg_ParseTuple(pRet,"ii", &a,&b);
//
//    pRetValue = PyEval_CallObject(pFunc, NULL);
//    
//    PyArg_Parse(pValue, "s", &pBuffer);
//    cout<<pBuffer<<endl;
    
    
    
};

    

    
    
    
#endif /* defined(__record_index_manager__PyEvaluator__) */
