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
#include <Python/Python.h>

#include "typedefs.h"


class PyEvaluator{
    
public:
    PyEvaluator();
    ~PyEvaluator(){Py_Finalize();}
    
    void PushCondition(uint table, uint attribute, Operator condition, int value, bool withIndex=false);
    void PushCondition(uint table, uint attribute, Operator condition, float value, bool withIndex=false);
    void PushCondition(uint table, uint attribute, Operator condition, string value, bool withIndex=false);
    void PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2, bool withIndex=false);
    void ClearConditions();
    
    bool Evaluate(vector<uint>tables, vector<Record*> records, vector<DataType>* tableRecordDataTypes[]);
    vector<vector<UUID>> GetResult();

private:
    const char *pBuffer = NULL;
    int iBufferSize = 0;
    
    PyObject *pModule = NULL;
    
    PyObject *pFuncPushCondition = NULL;
    PyObject *pFuncClearConditions = NULL;
    PyObject *pFuncEvaluate = NULL;

    
    PyObject *pRetValue = NULL;
    
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
