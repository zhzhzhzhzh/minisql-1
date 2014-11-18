//
//  main.cpp
//  record-index-manager
//
//  Created by ying on 10/27/14.
//  Copyright (c) 2014 ying. All rights reserved.
//


#include <iostream>
#include <string>

#include "RecordManager.h"


int main(int argc, const char * argv[])
{
    RecordManager recordManager;
    
    // must initialize
    recordManager.Initialize();
    
    
    
    recordManager.root = new Record;
    recordManager.root->next = NULL;
    recordManager.lastRecord = recordManager.root;
    
    
    
    
    struct Table tableStruct;
    
    {
        tableStruct.tableName = "test";
        tableStruct.attrNumber = 3;
        tableStruct.recordNum = 3;
        tableStruct.tableNum = 0;
        
        vector<Attribute> attributes;
        struct Attribute attr;
        attr.attrName = "id";
        attr.dataType = String;
        attr.dataLength = 10;
        attr.attrType = 0;
        attr.indexName = "null";
        attributes.push_back(attr);
        attr.attrName = "num";
        attr.dataType = Int;
        attr.dataLength = 8;
        attr.attrType = 0;
        attr.indexName = "null";
        attributes.push_back(attr);
        attr.attrName = "name";
        attr.dataType = String;
        attr.dataLength = 5;
        attr.attrType = 0;
        attr.indexName = "null";
        attributes.push_back(attr);
        
        tableStruct.attributes = attributes;
    }

    // create a table
    //recordManager.CreateTable(&tableStruct);
    
 
    
    
    // must load tables
    recordManager.LoadTable(&tableStruct);

    // drop a table
    //recordManager.DropTable(&tableStruct);


    
    
    
    uint T = tableStruct.tableNum;
    
    string id = "qwerty12";
    int num = 10;
    string name = "abcde";
    recordManager.NewQuery();
    recordManager.AppendValue(id);
    recordManager.AppendValue(num);
    recordManager.AppendValue(name);
    //recordManager.ChooseTable(T);
    recordManager.InsertRecord(T);
    
    num = 11;
    recordManager.NewQuery();
    recordManager.AppendValue(id);
    recordManager.AppendValue(num);
    recordManager.AppendValue(name);
    //recordManager.ChooseTable(T);
    recordManager.InsertRecord(T);
    
    
    num = 12;
    recordManager.NewQuery();
    recordManager.AppendValue(id);
    recordManager.AppendValue(num);
    recordManager.AppendValue(name);
    //recordManager.ChooseTable(T);
    recordManager.InsertRecord(T);
    
    recordManager.PrintRecord(T);
    recordManager.PrintSingle(recordManager.GetRecord(0, 0));
    
    recordManager.NewQuery();
    recordManager.PushCondition(T, 1, NotEqual, 11);
    recordManager.SelectRecord(T);
    
    

    
    return 0;
    
    
    
    /* no longer used test codes
     uint T = 0;
     vector<DataType> dataT;
     dataT.push_back(String);
     dataT.push_back(Int);
     dataT.push_back(String);
     vector<bool> isIndexBuilt;
     isIndexBuilt.push_back(true);
     isIndexBuilt.push_back(false);
     isIndexBuilt.push_back(false);
     recordManager.SetTableDescriptions(T, dataT, isIndexBuilt);
     */
    
    

    
    /*
     a table is created as follows:
     
     create table T (
        id varchar(10) 
        num int,
        name char(5)
     );
     
     create table S (
     key char(5)
     value int,
     );
     
     */

    
    /*
    uint T = 2;
    
    // set the data types of a table
    // data types can be read from catalog manager
    vector<DataType> dataType;
    dataType.clear();
    dataType.push_back(String);
    dataType.push_back(Int);
    dataType.push_back(String);
    recordManager.SetTableAttributeDataType(T, dataType);
     

    // insert into T values ("qwerty12", 10, "abcde");
    string id = "qwerty12";
    int num = 10;
    string name = "abcde";
    recordManager.NewQuery();
    recordManager.ChooseTable(T);
    recordManager.AppendValue(id);
    recordManager.AppendValue(num);
    recordManager.AppendValue(name);
    
    
    // select T.num,S.key from T,S where T.num<100 and S.key!="k0001" or T.name=S.key
    recordManager.NewQuery();
    uint T_index=2;
    uint S_index=3;
    recordManager.ChooseTable(T_index);
    recordManager.ChooseTable(S_index);
    recordManager.PushCondition(T_index, 1, Less, 100);
    recordManager.PushLogicOp(And);
    recordManager.PushCondition(S_index, 0, NotEqual, "k0001");
    recordManager.PushLogicOp(Or);
    recordManager.PushCondition(T_index, 2, Equal, S_index, 0);
     */
    
    // delete is much like select, only difference is...
    
    return 0;
}

