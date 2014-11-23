# coding: utf-8

import sys
import os
import BPlusTree as BPT
import random


class Index():
    def __init__(self):
        self.indices = {} # (T,A):root
        
  
    


DEBUG = False
indices = {} # (T,A):root
conditionsDirectWithIndex = []
conditionsDirectWithoutIndex = []
conditionsIndirectWithIndex = []
conditionsIndirectWithoutIndex = []
conditionsAll = []
currentRecordData = {}
isDataNew = {}
results = {}
finalResults = []
currentTables = []


def PushCondition(table1, attribute1, condition, value=0, withIndex=False, table2=-1, attribute2=-1):
    if DEBUG:
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

    if DEBUG:
        print 'from eva.py:PushCondition',conditionsDirectWithIndex
        print 'from eva.py:PushCondition',conditionsDirectWithoutIndex
        print 'from eva.py:PushCondition',conditionsIndirectWithIndex
        print 'from eva.py:PushCondition',conditionsIndirectWithoutIndex
    return
    
    
def PushLogicalOperation(operator):
    operator = operator.lower()
    if DEBUG:
        if operator not in ['(',')','and','or']:
            print 'from eva.py:PushLogicalOperation',operator,'is not right operator'
        
    conditionsAll.append(operator.strip())
    if DEBUG:
        print 'from eva.py:PushLogicalOperation',operator,'pushed'
        
    

def NewEvaluation():
    global DEBUG
    global indices
    global conditionsDirectWithIndex
    global conditionsDirectWithoutIndex
    global conditionsIndirectWithIndex
    global conditionsIndirectWithoutIndex
    global conditionsAll
    global currentRecordData
    global isDataNew
    global results
    global finalResults
    global currentTables

    
    DEBUG = False
    indices = {}
    conditionsDirectWithIndex = []
    conditionsDirectWithoutIndex = []
    conditionsIndirectWithIndex = []
    conditionsIndirectWithoutIndex = []
    conditionsAll = []
    if DEBUG:
        print 'from eva.py','conditions cleared'
    currentRecordData = {}
    isDataNew = {}
    if DEBUG:
        print 'from eva.py','currentRecordData cleared'
    results = {}
    finalResults = []
    currentTables = []
    


def GetCurrentUUIDTuple():
    uuidTuple = []
    for i in range(0,len(currentTables)):
        uuidTuple.append(currentRecordData[currentTables[i]][-1])
    return tuple(uuidTuple)
    



# for conditionsDirectWithIndex
def Evaluate(isDone, table, uuid, record): 
    global DEBUG
    
    if DEBUG:
        print 'from eva.py:Evaluate', 'isDone',isDone
        print 'from eva.py:Evaluate', 'table',table
        print 'from eva.py:Evaluate', 'record',record

    if table not in currentTables:
        currentTables.append(table)
    
    isDataNew[table] = True    
    currentRecordData[table] = [data for data in record] # to a list
    currentRecordData[table].append(uuid)
    
    

    
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
            
            if DEBUG:
                print 'from eva.py:Evaluate table,attribute,condition,value', table,attribute,condition,value
                print 'from eva.py:Evaluate',currentRecordData[table][attribute], 'compares to', value
            
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
            elif condition == NotEqual:
                if currentRecordData[table][attribute] != value:
                    results[expr].append(GetCurrentUUIDTuple())
      
        if DEBUG:
            print 'from eva.py:Evaluate', 'results', results
        return True
        
    
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
   

def EvaluateExpr(expr):
    lop = expr[2]
    rop = expr[0]
    if expr[1] == 'and':
        return lop.intersection(rop)
    elif expr[1] == 'or':
        return lop.union(rop)
    else:
        return
        
def LogicalEvaluation():
    global finalResults

    operator = ['or', 'and']
    parenthesis = ['(', ')']
    priority = {'or':1, 'and':2}
    #expr = ['(',1, 'or', 0,')','and','(',0,'or',0,')', 'or','(',1,'or',0,')']
    expr = []

    for op in conditionsAll:
        if op in operator or op in parenthesis:
            expr.append(op)
        else:
            expr.append(set(results[op]))
    
    
    
    if DEBUG:
        print 'from eva.py:LogicalEvaluation: expr ',expr
    stack = []
    priorityHighestNow = 0
    for op in expr:
        
        if op == ')':
            res = EvaluateExpr([stack.pop() for i in range(3)])
            stack.pop()
            stack.append(res)
            if len(stack)>2:
                for index in range(-1,-(len(stack)+1),-1):
                    topOperatorNow = stack[index]
                    if topOperatorNow in operator:
                        break
                priorityHighestNow = priority[topOperatorNow]
        else:
            if op not in operator:
                stack.append(op)
                if op == '(':
                    priorityHighestNow = 0
            else:
                if priority[op] > priorityHighestNow:
                    stack.append(op)
                    priorityHighestNow = priority[op]
                else:
                    while priority[op] <= priorityHighestNow:
                        res = EvaluateExpr([stack.pop() for i in range(3)])
                        stack.append(res)
                    
                        priorityHighestNow = 0
                        if len(stack)>2:
                            for index in range(-1,-(len(stack)+1),-1):
                                topOperatorNow = stack[index]
                                if topOperatorNow in operator:
                                    break
                            priorityHighestNow = priority[topOperatorNow]
                    
                    # if more priority levels, need to loop
                    stack.append(op)
        if DEBUG:
            print 'stack now',stack
        
    if len(stack)>1:
        res = EvaluateExpr([stack.pop() for i in range(3)])
        stack.append(res)
    
    if DEBUG:
        print 'stack final ',stack    
    finalResults = list(stack[0])
    
    if DEBUG:
        print 'from eva.py:LogicalEvaluation', 'finalResults', finalResults


def GetEvaluationResults(isFirstTime):
    if isFirstTime>0:
        LogicalEvaluation()
        return 
            
    try:  
        res = finalResults.pop() # a tuple
    except:
        res = []
        for i in range(0,len(currentTables)):
            res.append(-1)
        res = tuple(res)
    
    if DEBUG:
        print 'from eva.py:GetEvaluationResults return', res
    return res

    
    
    
def LoadIndex(table, attribute):
    indices[(table, attribute)] = ReadFromFile(table, attribute)

def CreateIndex(table, attribute):
    if (table, attribute) not in indices.keys():
        indices[(table, attribute)] = BPT.BPlusTree(table, attribute)

def DropIndex(table, attribute):
    #indices[(table, attribute)].DeleteAll()
    #indices.pop((table, attribute))
    pass
        

def InsertIndexNode(table, attribute, key, uuid):
    indices[(table, attribute)].Insert(key, uuid)


# need to modify the last uuid
def DeleteIndexNode(table, attribute, key, uuid):
    pass


def Search(table, attribute, condition, value):
    return indices[(table, attribute)].Search(condition, value)


def PrintTree(table, attribute):
    indices[(table, attribute)].PrintTree()


# serves as quit for this module
def OnQuit():
    for k in indicies.keys:
        table, attribute = k
        indices[k].WriteToFile()

    
    
''''    
if __name__ == '__main__': 
    index = Index()
    
    table = 0
    attribute = 1
    index.BuildIndex(table,attribute)
    for i in range(10):
        ii = random.randint(0,100)
        index.InsertIndexNode(table, attribute, ii, ii)
        index.PrintTree(table, attribute)
        print '\n'
        
        
    raw_input()
        
    table = 1
    attribute = 1
    index.BuildIndex(table,attribute)
    for i in range(10):
        ii = random.randint(0,100)
        index.InsertIndexNode(table, attribute, ii, ii)
        index.PrintTree(table, attribute)
        print '\n'
        
        
        
    #def Search(self, condition, value):
    Equal = 0
    Less = 1
    Greater = 2
    #LessEqual = 3
    #GreaterEqual = 4
    NotEqual = 5
             
    
    raw_input()
    
    table = 0
    attribute = 1
    res = index.Search(table, attribute, Less, 12)
    print res
    res = index.Search(table, attribute, Equal, 20)
    print res
    res = index.Search(table, attribute, Greater, 35)
    print res
    
    raw_input()
    
    table = 1
    attribute = 1
    res = index.Search(table, attribute, Less, 12)
    print res
    res = index.Search(table, attribute, Equal, 20)
    print res
    res = index.Search(table, attribute, Greater, 35)
    print res
    

'''    
    
    
    
    
    
    