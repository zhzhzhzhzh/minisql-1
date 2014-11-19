/*
 Self-debugging for BufferManager
 */
#include "BufferManager.h"

int main(){
	Table InsT;
	Attribute a;
	BufferManager bufferM;
	InsT.attrNumber = 4;

	a.attrName = "uuid";
	a.dataType = Uuid;
	a.attrType = 1;
	InsT.attributes.push_back(a);

	a.attrName = "att1";
	a.dataType = Int;
	InsT.attributes.push_back(a);

	a.attrName = "att2";
	a.dataType = Int;
	InsT.attributes.push_back(a);

	a.attrName = "att3";
	a.dataType = String;
	a.dataLength = 5;
	InsT.attributes.push_back(a);

	InsT.tableName = "table1";
	InsT.recordNum = 12;
	InsT.tableNum  = 1;

	/*
	Record *rec = new Record;
	UUID  *uuid = new UUID(8);
	int *aaa = new int(0x1111);
	int *bbb = new int(0xabcd);
	string *str = new string("wxyz");

	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	//bufferM.createTable(&InsT);
	
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;

	rec = new Record;
	uuid = new UUID(9);
	aaa = new int(0x2222);
	bbb = new int(0x1234);
	str = new string("zxcv");
	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;

	rec = new Record;
	uuid = new UUID(10);
	aaa = new int(0x2222);
	bbb =new int(0x1234);
	str = new string("zxcv");
	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;
	
	rec = new Record;
	uuid = new UUID(11);
	aaa = new int(0x3333);
	bbb = new int(0x5678);
	str = new string("qwer");
	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;
	rec = bufferM.getRecord(&InsT, 1);
	printf("%u, %d, %x, %s",	*static_cast<UUID *>(rec->data.at(0)), 
								*static_cast<int *>(rec->data.at(1)),
								*static_cast<int *>(rec->data.at(2)),
								static_cast<char *>(rec->data.at(3)));
	rec = new Record;
	uuid = new UUID(12);
	aaa = new int(0x4444);
	bbb = new int(0x5678);
	str = new string("qwer");
	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;


	rec = new Record;
	uuid = new UUID(13);
	aaa = new int(0x5555);
	bbb = new int(0x5678);
	str = new string("qwer");
	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;

	rec = new Record;
	uuid = new UUID(14);
	aaa = new int(0x6666);
	bbb = new int(0x5678);
	str = new string("qwer");
	rec->data.push_back(static_cast<void *>(uuid));
	rec->data.push_back(static_cast<void *>(aaa));
	rec->data.push_back(static_cast<void *>(bbb));
	rec->data.push_back(static_cast<void *>(str));
	rec->next = NULL;
	bufferM.insertRec(&InsT, rec);
	InsT.recordNum++;
	
	*/
	
	bufferM.deleteRec(&InsT, 3);
	InsT.recordNum--;
/*
	bufferM.deleteRec(&InsT, 1);
	InsT.recordNum--;

	bufferM.deleteRec(&InsT, 1);
	InsT.recordNum--;
	
	
	bufferM.deleteRec(&InsT, 1);
	InsT.recordNum--;

	bufferM.deleteRec(&InsT, 1);
	InsT.recordNum--;

	bufferM.deleteRec(&InsT, 1);
	InsT.recordNum--;
	*/
	/*
	*/
	bufferM.quitDB();
	getchar();

	return 0;
}
