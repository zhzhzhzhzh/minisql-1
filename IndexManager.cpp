//
//  index-manager.cpp
//  record-index-manager
//
//  Created by ying on 10/27/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#include "IndexManager.h"



IndexManager::IndexManager()
{
}


void IndexManager::BuildIndex(uint table, uint attribute)
{
    cout<<"table"<<table<<endl;
    cout<<"table,attr"<<table<<" "<<attribute<<endl;
}

void IndexManager::DropIndex(uint table, uint attribute)
{
}









/*
PyObject* pyWrapper(PyObject* self, PyObject* args)
{
    int n;
    
    if (! PyArg_ParseTuple(args, "i:fact", &n))
        return NULL;
    
    
    return Py_BuildValue("i", n);
}

static PyMethodDef IndexManagerMethods[] =
{
    {"pyWrapper", pyWrapper, METH_VARARGS, "IndexManager"},
    {NULL, NULL}
};

extern "C"              //不加会导致找不到initexample
void initIndexManager()
{
    PyObject* m;
    m = Py_InitModule("IndexManager", IndexManagerMethods);
    
    IndexManager indexManager;
}
*/



/* 
  compile
 
 
  g++ -fPIC IndexManager.cpp -o IndexManager.so -shared   -F/Library/Frameworks -framework Python
 
  //g++ -fPIC index-manager.cpp -o index-manager.so -shared -I/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 -I/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/config  -F/Library/Frameworks -framework Python

*/
