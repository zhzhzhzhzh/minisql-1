# coding: utf-8
 
import types
import sys
import random
import math
  
class Node(object):
    def __init__(self, nodeID, parent=None, sibling=None):
            self.nodeID = nodeID
            self.parent = parent
            self.sibling = sibling
            # large data here
            self.pointers = []
            self.keys = []
        
    def isLeaf(self):
        res = (len(self.pointers) == 0 or type(self.pointers[0]) != type(self))
        #print 'isleaf',res,
        return res
        
    def size(self):
        return len(self.keys)



# TODO no duplicate key, use buckets to store uuid set
# id and uuid are treated as 64 bits long
class BPlusTree(object):
    def __init__(self, table, attribute, dataType=types.IntType, keySize=8, blockSize=4000):
        # table info
        self.table = table
        self.attribute = attribute
        self.dataType = dataType
        # (blocksize - id)/(uuid/id+key)
        #self.N = (blockSize - 8)/(8+keySize)
        self.N = 3
        self.root = None
        self.idCount = 0
        self.isEmpty = True
     

    def InsertInNonLeaf(self, node, key, pointer):
        # root split
        if node == None:
            newRoot = Node(self.idCount)
            self.idCount = self.idCount + 1
            newRoot.pointers.append(self.root)
            newRoot.keys.append(key)
            newRoot.pointers.append(pointer)
            self.root.parent = newRoot
            self.root = newRoot
            
            print 'root left:',
            for i in self.root.pointers[0].keys:
                print i,
            print 'root right:',
            for i in self.root.pointers[1].keys:
                print i,
                     
            return newRoot
            
        for index in range(len(node.keys)):
            if node.keys[index] > key:
                break
        if key > node.keys[index]:
            index = index + 1
        insertPosition = index # for later use
        node.keys.insert(index, key)
        node.pointers.insert(index+1, pointer)
                   
        # other non leaf split
        if len(node.pointers) > self.N:
            newNonLeaf = Node(self.idCount, sibling=node.sibling)
            node.sibling = newNonLeaf
            self.idCount = self.idCount + 1
            
            index = int(math.ceil(1.0*(self.N+1)/2)) - 1
            while len(node.keys) > index:
                newNonLeaf.keys.append(node.keys.pop(index))
                popPointer = node.pointers.pop(index+1)
                popPointer.parent = newNonLeaf
                newNonLeaf.pointers.append(popPointer)
                
            popKey = newNonLeaf.keys.pop(0)
            
            newNonLeaf.parent = self.InsertInNonLeaf(node.parent, popKey, newNonLeaf)
            
            if insertPosition < len(node.keys):
                return node
            else:
                return newNonLeaf        
        return node
        
     
        
    # pointer points to records
    def Insert(self, key, pointer):
        if self.isEmpty:
            self.root = Node(0)
            self.idCount = self.idCount + 1
            self.isEmpty = False
            self.root.keys.append(key)
            self.root.pointers.append(pointer)
        else:
            n = self.FindElement(key)
            
            print 'insert Node: %d'%n.nodeID,
           
            for index in range(len(n.keys)):
                if n.keys[index] > key:
                    break
            if key > n.keys[index]:
                index = index + 1
            print 'insert index: %d'%index
            n.keys.insert(index, key)
            n.pointers.insert(index, pointer)
                       
            # split
            if len(n.keys) >= self.N:
                newLeaf = Node(self.idCount, sibling=n.sibling)
                n.sibling = newLeaf
                self.idCount = self.idCount + 1
                
                index = int(math.ceil(1.0*self.N/2))
                while len(n.keys) > index:
                    newLeaf.keys.append(n.keys.pop(index))
                    newLeaf.pointers.append(n.pointers.pop(index))
                newLeaf.parent = self.InsertInNonLeaf(n.parent, newLeaf.keys[0], newLeaf)
                
                print 'split left:',
                for i in n.keys:
                    print i,
                print 'split right:',
                for i in newLeaf.keys:
                    print i,
        
        
    def Delete(self, key):
        # TODO need to check uuid
        
        n = self.FindElement(key)
        print 'delete Node: %d'%n.nodeID,
       
        if not key in n.keys:
            return
        index = n.keys.index(key)
        
        print 'delete index: %d'%index
        
        
        n.keys.pop(index, key)
        n.pointers.pop(index, pointer)
                   
        # underfull
        if len(n.keys) < int(math.ceil(1.0*(self.N-1)/2)):
            



            
            print 'split left:',
            for i in n.keys:
                print i,
            print 'split right:',
            for i in newLeaf.keys:
                print i,
                
                
        
        if n == self.root and len(self.root.keys) == 0:
            self.isEmpty = True
            
        
        
    
        
    # find the first node in
    def FindElement(self, key):
        n = self.root
        while not n.isLeaf():
            for index in range(len(n.keys)):
                if key < n.keys[index]:
                    break
            if key >= n.keys[index]:
                index = index + 1
            n = n.pointers[index]
        return n
    
    def FindLeftEnd(self):
        n = self.root
        while not n.isLeaf():
            n = n.pointers[0]
        return n
            
    def FindRightEnd(self):
        n = self.root
        while not n.isLeaf():
            n = n.pointers[-1]
        return n
        
        
    def Search(self, condition, value):
        Equal = 0
        Less = 1
        Greater = 2
        #LessEqual = 3
        #GreaterEqual = 4
        NotEqual = 5
        
        result = []
        if condition == Equal:
            n = self.FindElement(value)
            for index in range(len(n.keys)):
                if key == n.keys[index]:
                    break
            while key == n.keys[index]:
                if index >= len(n.keys):
                    if type(n.sibling)!=type(n):
                        break
                    n = n.sibling
                    index = 0
                    continue
                result.append(n.pointers[index]) # add uuid
                index = index + 1
            return result       
                 
        elif condition == Less:
            n = self.FindLeftEnd()
            for index in range(len(n.keys)):
                if key == n.keys[index]:
                    break
            while key == n.keys[index]:
                if index >= len(n.keys):
                    if type(n.sibling)!=type(n):
                        break
                    n = n.sibling
                    index = 0
                    continue
                result.append(n.pointers[index]) # add uuid
                index = index + 1
            return result
            
            
            
            
        elif condition == Greater:
            pass

        else: # not equal
            n = FindLeftEnd()
            while type(n) == type(n.parent):
                for index in range(len(n.keys)):
                    if n.keys[index] != value:
                        result.append(n.pointers[index])
                n = n.sibling
            return result
                
                
    
        
    
    def ReadFromFile(self):
        pass
        
    
    # need to instantiate a buffer manager
    def WriteToFile(self):
        pass
        '''
        f = open(str(table)+'.'+str(attribute)+'.index', 'w')
        f.write(self.dataType)
        
        f.close()
        '''

        
    def DeleteAll(self):
        # remove file
        pass
        
        
def PrintTree(tree):
    tmp = Node(-1)
    first = tree.root
    
    level = 0
    while type(first) == type(tmp):
        print '\nlevel',level,'================'
        level = level + 1
        node = 0
        nex = first
        while type(nex) == type(tmp):
            parent = -1
            if type(nex.parent) == type(tmp):
                parent=nex.parent.nodeID

            print '\tnode[%d]{%d}'%(nex.nodeID,parent),
            node = node + 1
            
            
            if not nex.isLeaf():
                i = 0
                for i in range(len(nex.keys)):
                    print '(%d)'%nex.pointers[i].nodeID,
                    print nex.keys[i],
                i = i + 1
                print '(%d)'%nex.pointers[i].nodeID,
            else:
                for k in nex.keys:
                    print k,
            nex = nex.sibling
        first = first.pointers[0]
    
class Record(object):
    def __init__(self, uuid):
        self.uuid = uuid
        
if __name__ == '__main__':
    bp = BPlusTree('0', type(Record))
    r = Record(0)
    
    for i in range(20):
        ii = random.randint(0,100)
        bp.Insert(ii,r)
        PrintTree(bp)
        print '\n'

    
    