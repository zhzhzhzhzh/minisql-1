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
    pFuncClearConditions = PyObject_GetAttrString(pModule, "ClearConditions");
    pFuncEvaluate = PyObject_GetAttrString(pModule, "Evaluate");
}


void PyEvaluator::ClearConditions()
{
    PyEval_CallObject(pFuncClearConditions, NULL);
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
        PyObject* pyParam = NULL;
        
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

vector<vector<UUID>> PyEvaluator::GetResult()
{
    vector<vector<UUID>> results;
    vector<UUID> onetablere;
    onetablere.push_back(0);
    
    results.push_back(onetablere);
    
    return results;
}

