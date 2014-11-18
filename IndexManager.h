//
//  index-manager.h
//  record-index-manager
//
//  Created by ying on 10/27/14.
//  Copyright (c) 2014 ying. All rights reserved.
//

#ifndef __record_index_manager__index_manager__
#define __record_index_manager__index_manager__

#include <iostream>
#include "typedefs.h"

#include <Python/Python.h>
#include <iostream>


using namespace std;


class IndexManager{
public:
    IndexManager();
    ~IndexManager(){};
    
    
    void BuildIndex(uint table, uint attribute);
    void DropIndex(uint table, uint attribute);
    
    
private:
    
};



#endif /* defined(__record_index_manager__index_manager__) */
