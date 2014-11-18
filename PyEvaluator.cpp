//
//  PyEvaluator.cpp
//  record-index-manager
//
//  Created by ying on 11/16/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#include "PyEvaluator.h"


PyEvaluator::PyEvaluator()
{
    Py_Initialize();
    
    PyRun_SimpleString("import sys\nsys.path.append('/Users/ying/Documents/DEV/record-index-manager/record-index-manager')");

    pModule = PyImport_ImportModule("Evaluator");
    
    pFuncPushCondition = PyObject_GetAttrString(pModule, "PushCondition");
    pFuncPushLogicalOperation = PyObject_GetAttrString(pModule, "PushLogicalOperation");
    pFuncNewEvaluation = PyObject_GetAttrString(pModule, "NewEvaluation");
    pFuncEvaluate = PyObject_GetAttrString(pModule, "Evaluate");
    pFuncGetEvaluationResults = PyObject_GetAttrString(pModule, "GetEvaluationResults");

}


void PyEvaluator::NewEvaluation()
{
    PyEval_CallObject(pFuncNewEvaluation, NULL);
}

void PyEvaluator::PushCondition(uint table, uint attribute, Operator condition, int value, bool withIndex)
{
    Debug("in py pushcondition:"<<table<<attribute<<condition<<value<<withIndex);

    //def PushCondition(table1, attribute1, condition, value=0, withIndex=False, table2=-1, attribute2=-1):

    PyObject* pArgs = Py_BuildValue("iiiib", table, attribute, condition, value, withIndex);
    PyEval_CallObject(pFuncPushCondition,pArgs);
    
    return;
}

void PyEvaluator::PushCondition(uint table, uint attribute, Operator condition, float value, bool withIndex)
{
    PyObject* pArgs = Py_BuildValue("iiifb", table, attribute, condition, value, withIndex);
    PyEval_CallObject(pFuncPushCondition,pArgs);

    
    return;
}

void PyEvaluator::PushCondition(uint table, uint attribute, Operator condition, string value, bool withIndex)
{
    PyObject* pArgs = Py_BuildValue("iiisb", table, attribute, condition, value.c_str(), withIndex);
    PyEval_CallObject(pFuncPushCondition,pArgs);

    return;
}

void PyEvaluator::PushCondition(uint table_1, uint attribute_1, Operator condition, uint table_2, uint attribute_2, bool withIndex)
{
    //def PushCondition(table1, attribute1, condition, value=0, withIndex=False, table2=-1, attribute2=-1):

    PyObject* pArgs = Py_BuildValue("iiiibii", table_1, attribute_1, condition, 0/*unused value*/, withIndex,
                                    table_2, attribute_2);
    PyEval_CallObject(pFuncPushCondition,pArgs);
    
    return;
}

void PyEvaluator::PushLogicalOperation(string op)
{
    PyObject* pyParams = PyTuple_New(1);
    PyObject* pArgs = Py_BuildValue("s", op.c_str());
    PyTuple_SetItem(pyParams, 0, pArgs);
    PyEval_CallObject(pFuncPushLogicalOperation, pyParams);
    
    return;
}


// return true if all expr are evaluated
// always feed data, if return true, just stop
bool PyEvaluator::Evaluate(vector<uint>tables, vector<Record*> records, vector<DataType>* tableRecordDataTypes[])
{
    // only changed records are pushed
    
    Debug("PyEvaluator::Evaluate:changed value "<<records.size());
    
    bool isDone;
    for (int i=0; i<records.size(); i++) {
        isDone = (i==records.size()-1);
        
        /*  for reference
         struct Record{
         vector<void *> data;
         struct Record * next;
         };
         */
        uint table = tables.at(i);
        vector<void *> recordData = records.at(i)->data;
        unsigned long paramCount = recordData.size() - 1; //minus the first UUID, plus the first leading isDone, table
        string paramsPattern = "";
        DataType type;
        
        Debug("PyEvaluator::Evaluate:recordData.size() "<<paramCount);
        // def EvaluateFeedingData(isDone, table, *record):
        
        PyObject* pyParamsRecordData = PyTuple_New(paramCount);
        PyObject* pyParam = nullptr;
        
        for (int i=1; i<=paramCount; i++) {
            type = tableRecordDataTypes[table]->at(i-1);
            
            switch (type) {
                case Int:
                    pyParam = Py_BuildValue("i", *((int*)recordData.at(i)));
                    PyTuple_SetItem(pyParamsRecordData, i-1, pyParam);
                    break;
                    
                case Float:
                    pyParam = Py_BuildValue("f", *((float*)recordData.at(i)));
                    PyTuple_SetItem(pyParamsRecordData, i-1, pyParam);
                    break;
                    
                case String:
                    pyParam = Py_BuildValue("s", (*((string*)recordData.at(i))).c_str());
                    PyTuple_SetItem(pyParamsRecordData, i-1, pyParam);
                    break;
                    
                default:
                    break;
            }
        }
        
        // def Evaluate(isDone, table, uuid, record)
        PyObject* pArgs = Py_BuildValue("biiO", isDone, table, *((UUID*)recordData.at(0)), pyParamsRecordData);
        
        pRetValue = PyEval_CallObject(pFuncEvaluate, pArgs);
        
        bool pBuffer;
        PyArg_Parse(pRetValue, "b", &pBuffer);
        
        Debug("PyEvaluator::Evaluate:return "<<(pBuffer ? "true" : "false"));
    }

    return false;
}

vector<vector<UUID>> PyEvaluator::GetResult(int currentTablesCount)
{
    bool isFirstTime = true;
    
    int uuid = 0;
    vector<vector<UUID>> results;
    vector<UUID> oneTableRes;
    
    {
        PyObject* pyParams = PyTuple_New(1);
        PyObject* pArgs = Py_BuildValue("b", isFirstTime);
        PyTuple_SetItem(pyParams, 0, pArgs);
        PyEval_CallObject(pFuncGetEvaluationResults, pyParams);
    }
    
    isFirstTime = false;
    PyObject* pyParams = PyTuple_New(1);
    PyObject* pArgs = Py_BuildValue("b", isFirstTime);
    PyTuple_SetItem(pyParams, 0, pArgs);
    PyObject* pBuffer = PyTuple_New(currentTablesCount);
    while(1){
        pRetValue = PyEval_CallObject(pFuncGetEvaluationResults, pyParams);
        
        PyArg_Parse(pRetValue, "O", &pBuffer);
        
        oneTableRes.clear();
        for (int i=0; i<currentTablesCount; i++) {
            PyArg_Parse(PyTuple_GetItem(pBuffer, i), "i", &uuid);
            oneTableRes.push_back((UUID)uuid);
            if (uuid<=0) {
                for (int i=0; i<results.size(); i++) {
                    for (int j=0; j<results.at(i).size(); j++) {
                        DebugS("Get result: "<<results.at(i).at(j)<<" ");
                    }
                    DebugS(endl);
                }
                return results;
            }
        }
        results.push_back(oneTableRes);
    }
    

    return results;
}

