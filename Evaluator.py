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

    



def PushCondition(table1, attribute1, condition, value=0, withIndex=False, table2=-1, attribute2=-1):
    print 'from eva.py',table1, attribute1, condition, value, withIndex, table2, attribute2
    
    if table2<0 and attribute2<0:
        # second operand direct value
        if withIndex:
            conditionsDirectWithIndex.append((table1, attribute1, condition, value))
        else:
            conditionsDirectWithoutIndex.append((table1, attribute1, condition, value))
    else:
        # second value table value
        if withIndex:
            conditionsIndirectWithIndex.append((table1, attribute1, condition, table2, attribute2))
        else:
            conditionsIndirectWithoutIndex.append((table1, attribute1, condition, table2, attribute2))

    
    print 'from eva.py:PushCondition',conditionsDirectWithIndex
    print 'from eva.py:PushCondition',conditionsDirectWithoutIndex
    print 'from eva.py:PushCondition',conditionsIndirectWithIndex
    print 'from eva.py:PushCondition',conditionsIndirectWithoutIndex
    return
    
    
    

'''    
conditionsDirectWithIndex = []
conditionsDirectWithoutIndex = [] # feed data to evaluate
conditionsIndirectWithIndex = []
conditionsIndirectWithoutIndex = []
currentRecordData = {}
currentRecordDataNew = {}
results = {}
'''

# for feeding data
hasMoreData = True

def NewEvaluation():
    conditionsDirectWithIndex = []
    conditionsDirectWithoutIndex = []
    conditionsIndirectWithIndex = []
    conditionsIndirectWithoutIndex = []
    print 'from eva.py','conditions cleared'
    currentRecordData = {}
    currentRecordDataNew = {}
    results = {}
    print 'from eva.py','currentRecordData cleared'
    currentTables = []
    

# for conditionsDirectWithIndex
def Evaluate(isDone, table, uuid, record): 
    if table not in currentTables:
        currentTables.append(table)
        
    currentRecordDataNew[table] = [data for data in record] # to a list
    currentRecordDataNew[table].append(uuid)
    
    if table not in results.keys():
        results[table] = set([])
    
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
            table,attribute,condition,value = expr
            
            print 'from eva.py:Evaluate table,attribute,condition,value', table,attribute,condition,value
            
            if condition == Equal:
                if currentRecordDataNew[table][attribute] == value:
                    results[table].add(currentRecordDataNew[table][-1])
                    print 'from eva.py:Evaluate','added uuid: ',table,currentRecordDataNew[table][-1]
            elif condition == Less:
                if currentRecordDataNew[table][attribute] < value:
                    results[table].add(currentRecordDataNew[table][-1])
            elif condition == Greater:
                if currentRecordDataNew[table][attribute] > value:
                    results[table].add(currentRecordDataNew[table][-1])
            elif condition == LessEqual:
                if currentRecordDataNew[table][attribute] <= value:
                    results[table].add(currentRecordDataNew[table][-1])
            elif condition == GreaterEqual:
                if currentRecordDataNew[table][attribute] >= value:
                    results[table].add(currentRecordDataNew[table][-1])
            else: # not equal
                if currentRecordDataNew[table][attribute] != value:
                    results[table].add(currentRecordDataNew[table][-1])
      
        
        print 'from eva.py:Evaluate', 'results', results
        return False
        
        
        
        
        
        
        # all expr evaluated in this turn
        
                            
    else:
        return False
        
    
    #return isAllEvaluated
    
# for conditionsDirectWithoutIndex
# for conditionsIndirectWithIndex
# for conditionsIndirectWithoutIndex
def EvaluateFeedingData(isDone, table, *record):
    pass
    
    
    
    
    
    