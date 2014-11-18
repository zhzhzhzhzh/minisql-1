# coding: utf-8
 
import types
  
class Node(object):
    def __init__(self, nodeID, parent=0, sibling=0):
            self.nodeID = nodeID
            self.parent = parent
            self.sibling = sibling
            # large data here
            self.pointers = []
            self.keys = []
    
    def addKey(self, pointer, key):
        for index in range(len(keys)):
            if key > keys[index]:
                break
        pointers.insert(index, pointer)      
        keys.insert(index, key)
        
    def isLeaf(self):
        return not type(pointers[0]) == type(self)
        
    def size(self):
        return len(self.keys)


# id and uuid are treated as 64 bits long
class BPlusTree(object):
    def __init__(self, table, attribute, dataType=types.IntType, keySize=8, blockSize=4000):
        self.table = table
        self.attribute = attribute
        self.dataType = dataType
        # (blocksize - id)/(uuid/id+key)
        self.N = (blockSize - 8)/(8+keySize)
        self.root = Node(0)
        self.idCount = 0


    def Insert(self, key):
        n = FindElement(key)
        pass
        
        
    def Delete(self, key):
        pass
        
        
    
    def Search(self, condition, value):
        Equal = 0
        Less = 1
        Greater = 2
        #LessEqual = 3
        #GreaterEqual = 4
        NotEqual = 5
        
        result = []
        if condition == Equal:
            n = FindElement(value)
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
            pass
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
                
                
        
    # find the first node in
    def FindElement(self, key):
        n = self.root
        while not n.isLeaf():
            for index in range(len(n.keys)):
                if key < n.keys[index]:
                    break
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
        
        
    
    def ReadFromFile(self):
        pass
        
    
    # need to instantiate a buffer manager
    def WriteToFile(self):
        f = open(str(table)+'.'+str(attribute)+'.index', 'w')
        f.write(self.dataType)
        
        f.close()

        
    def DeleteAll(self):
        # remove file
        pass
        
        
if __name__ == '__main__':
    bp = BPlusTree('0', types.IntType)
    bp.WriteToFile()
    
    