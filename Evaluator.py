# coding: utf-8

import sys
import os

indicies = {} # (T,A):root

'''
def BuildIndex(table, attribute):
    pass
    indices[(table, attribute)] = BPlusTree(table, attribute)
    
    IndexManager.IndexInsertNode(table, attribute)
    

    
def DeleteIndex(table, attribute):
    indices[(table, attribute)].DeleteAll()
    indices.pop((table, attribute))
    pass
    
# need to modify the last uuid
def DeleteIndexNode(table, attribute, uuid, key):
    pass
    
def InsertIndexNode(table, attribute, uuid, key):
    pass
    
    
def OnQuit():
    indices[(table, attribute)].WriteToFile()
    pass
    
    
'''

    
conditionsDirectWithIndex = []
conditionsDirectWithoutIndex = []
conditionsIndirectWithIndex = []
conditionsIndirectWithoutIndex = []
conditionsAll = []
currentRecordData = {}
isDataNew = {}
results = {}
finalResults = set([])
currentTables = []


def PushCondition(table1, attribute1, condition, value=0, withIndex=False, table2=-1, attribute2=-1):
    print 'from eva.py',table1, attribute1, condition, value, withIndex, table2, attribute2
    
    if table2<0 and attribute2<0:
        # second operand direct value
        if withIndex:
            conditionsDirectWithIndex.append((table1, attribute1, condition, value))
            conditionsAll.append((table1, attribute1, condition, value))
        else:
            conditionsDirectWithoutIndex.append((table1, attribute1, condition, value))
            conditionsAll.append((table1, attribute1, condition, value))
    else:
        # second value table value
        if withIndex:
            conditionsIndirectWithIndex.append((table1, attribute1, condition, table2, attribute2))
            conditionsAll.append((table1, attribute1, condition, table2, attribute2))
        else:
            conditionsIndirectWithoutIndex.append((table1, attribute1, condition, table2, attribute2))
            conditionsAll.append((table1, attribute1, condition, table2, attribute2))

    
    print 'from eva.py:PushCondition',conditionsDirectWithIndex
    print 'from eva.py:PushCondition',conditionsDirectWithoutIndex
    print 'from eva.py:PushCondition',conditionsIndirectWithIndex
    print 'from eva.py:PushCondition',conditionsIndirectWithoutIndex
    return
    
    
def PushLogicalOperation(operator):
    operator = operator.lower()
    if operator not in ['(',')','and','or']:
        print 'from eva.py:PushLogicalOperation',operator,'is not right operator'
        
    conditionsAll.append(operator)
        
    

def NewEvaluation():
    conditionsDirectWithIndex = []
    conditionsDirectWithoutIndex = []
    conditionsIndirectWithIndex = []
    conditionsIndirectWithoutIndex = []
    conditionsAll = []
    print 'from eva.py','conditions cleared'
    currentRecordData = {}
    isDataNew = {}
    print 'from eva.py','currentRecordData cleared'
    results = {}
    finalResults = set([])
    currentTables = []
    


def GetCurrentUUIDTuple():
    uuidTuple = []
    for i in range(0,len(currentTables)):
        uuidTuple.append(currentRecordData[currentTables[i]][-1])
    return uuidTuple
    



# for conditionsDirectWithIndex
def Evaluate(isDone, table, uuid, record): 
    if table not in currentTables:
        currentTables.append(table)
    
    
    isDataNew[table] = True    
    currentRecordData[table] = [data for data in record] # to a list
    currentRecordData[table].append(uuid)
    
    
    print 'from eva.py:Evaluate', 'isDone',isDone
    print 'from eva.py:Evaluate', 'table',table
    print 'from eva.py:Evaluate', 'record',record
    
    if isDone:
        Equal = 0
        Less = 1
        Greater = 2
        LessEqual = 3
        GreaterEqual = 4
        NotEqual = 5
        
        
        # format: [table1, attribute1, condition, value]
        for expr in conditionsDirectWithoutIndex:
            if expr not in results.keys():
                results[expr] = []
                
            table,attribute,condition,value = expr
            
            print 'from eva.py:Evaluate table,attribute,condition,value', table,attribute,condition,value
            
            # TODO decide from where to fetch data
            if condition == Equal:
                if currentRecordData[table][attribute] == value:
                    results[expr].append(GetCurrentUUIDTuple())
            elif condition == Less:
                if currentRecordData[table][attribute] < value:
                    results[expr].append(GetCurrentUUIDTuple())
            elif condition == Greater:
                if currentRecordData[table][attribute] > value:
                    results[expr].append(GetCurrentUUIDTuple())
            elif condition == LessEqual:
                if currentRecordData[table][attribute] <= value:
                    results[expr].append(GetCurrentUUIDTuple())
            elif condition == GreaterEqual:
                if currentRecordData[table][attribute] >= value:
                    results[expr].append(GetCurrentUUIDTuple())
            else: # not equal
                if currentRecordData[table][attribute] != value:
                    results[expr].append(GetCurrentUUIDTuple())
      
        
        print 'from eva.py:Evaluate', 'results', results
        return False
        
    
        # all expr evaluated in this turn
        for key in isDataNew.keys():
            isDataNew[key] = False
                       
    else:
        return False
        
    
    #return isAllEvaluated
    
# for conditionsDirectWithoutIndex
# for conditionsIndirectWithIndex
# for conditionsIndirectWithoutIndex
def EvaluateFeedingData(isDone, table, *record):
    pass
   

def LogicalEvaluation():
    for expr in conditionsAll:
        for res in results[expr]:
            finalResults.add(tuple(res))
    
    print 'from eva.py:LogicalEvaluation', 'finalResults', finalResults


def GetEvaluationResults(isFirstTime):
    if isFirstTime>0:
        LogicalEvaluation()
        return 
    
    try:  
        res = finalResults.pop() # a list
    except:
        res = []
        for i in range(0,len(currentTables)):
            res.append(-1)
        res = tuple(res)
    
    print 'from eva.py:GetEvaluationResults return', res
    return res

    
    
if __name__ == '__main__':
    GetEvaluationResults(True)
    pass  
    