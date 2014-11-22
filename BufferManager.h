/*
	BufferManager.h

 */

#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_
#include "Block.h"
#define MAX_BLOCK_NUM 	256
#define MAX_FILE_NUM 	MAX_CONCURRENT_TABLE
//#include "Exceptions.h"
using namespace std;
//struct FileInf;
enum readType {  
	singleRec = 0,
	linkedRec = 1,
	lockedRec = 2
};
class BufferManager
{

public:
	/* Void Constructor */
	BufferManager(){
		fileCount 	= 0;
		blockCount 	= 0;
		Bufferlist  = new Block[MAX_BLOCK_NUM];
		//Tablelist 	= new FileInf[MAX_FILE_NUM];
		flistHead = NULL;
		flistTail = NULL;
		//BufferVector.clear();
		//TableVector.clear();
	};
	//BufferManager(arguments);
	~BufferManager(){
		for (int i = 0; i < MAX_BLOCK_NUM; i++)
			Bufferlist[i].Initialize();
		delete [] Bufferlist;
		//delete [] Tablelist;
	};

	/* function */
	Record* 	getRecord(const Table*, UUID );
	bool 		createTable(const Table* );
	bool 		insertRec(const Table* , Record* );
	bool 		deleteAll(const Table* );
	int   		deleteRec(const Table* , UUID );
	UUID 		getMaxuuid(const Table* );
	int 		getBlockCount(const Table* );
	bool 		removeTable(const Table *);
	void 		quitDB();

private:
	/* data */
	Block* 		Bufferlist;
	FileInf* 	flistHead;
	FileInf* 	flistTail;
	int 		blockCount;
	int 		fileCount;

	/* function */
	FileInf*	getFile(const Table* );
	FileInf*	openFile(const Table* );						// On open a table
	void		closeFile(FileInf* );					// On close a table
	int  		getBlock(FileInf* , int);
	int 		getFreeBlock();

};

#endif /* _BUFFERMANAGER_H_ */
