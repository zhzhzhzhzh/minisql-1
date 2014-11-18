/*
 	BufferManager.cpp

 */

#include "BufferManager.h"

/*
 Close the file that stores the table described if any,
 	else directly return
 @param FileInf*pFi: file to close 
 */
void BufferManager::closeFile(FileInf *pFi){
	int B_index;
	if ( !pFi ) return;
	Bufferlist[pFi->firstBlock].unlock();
	Bufferlist[pFi->lastBlock].unlock();
	for (B_index = pFi->firstBlock; B_index != -1; B_index = Bufferlist[B_index].next){
		Bufferlist[B_index].Initialize();
		blockCount--;
	}
	pFi->fd.close();				// This is the time when it actually flushback from system io
	delete pFi->dataVector;
	delete pFi;						// get rid of leak
	fileCount--;
}

/*
 Open a file that stores that table described if any, else create a new one.
 After open the file, initialize the first and last block of the table,
 according the information in FileInf
 @param Table 	*pTable
 @return FileInf*file: file descriptor
 */
FileInf* BufferManager::openFile(Table *pTable){
	FileInf *ret;
	ret = new FileInf(pTable);		
	if ( ret->Block_Num != 0){								// In case of new created table
		ret->firstBlock = getFreeBlock();							
		Bufferlist[ret->firstBlock].Initialize(ret, 0);
		Bufferlist[ret->firstBlock].lock();
		if ( ret->Block_Num != 1 ){
			ret->lastBlock = getFreeBlock();
			Bufferlist[ret->lastBlock].Initialize(ret, ret->Block_Num);
			Bufferlist[ret->lastBlock].lock();
			Bufferlist[ret->firstBlock].next = ret->lastBlock;	
		}
		else{
			ret->lastBlock = ret->firstBlock;
		}
	}
	else {
		; 			// Do nothing 
	}
	fileCount++;
	return ret;
}

/*
 Find an available file descriptor if any,
 	else replace the one at the head of the queue(FIFO)
 @param Table 	*pTable
 @return FileInf*file: file descriptor
 */
FileInf* BufferManager::getFile(Table *pTable){
	FileInf *ret = NULL, *former_it = NULL;
	former_it = ret = flistHead;

	/* File exists, maintain the queue */
	for (; ret != NULL && ret->File_id != pTable->tableNum; former_it = ret, ret = ret->next);
	if ( ret != NULL ){
		if ( ret != flistTail ){
			if ( ret == flistHead ){ 					// Move head forward
				flistHead = flistHead->next;
			}
			else {
				former_it->next = ret->next; 			
			}
			flistTail->next = ret; 						// link node to the tail
			flistTail = ret;
			ret->next = NULL;
		}
		return ret;
	}


	/* In case active files exceed MAX_FILE_NUM */
	if ( fileCount > MAX_FILE_NUM ){ 				
		FileInf *pFi = flistHead;
		flistHead = flistHead->next;
		closeFile(pFi); 								// Close the Least Recently used
		//fileCount--;
	}

	ret = openFile(pTable);
	if ( fileCount == 0 ){
		flistHead = flistTail = ret;
	} 
	else {
		flistTail->next = ret;						
		flistTail = ret; 
	}
	ret->next = NULL;
	//fileCount++;
	return ret;
}

/*
 This function actually does nothing but create a file on disk and
 	add the FileInf pointer to the file list
	
 */
void BufferManager::createTable(Table *pTable){
	/*
	FileInf *file;
	file = 
	*/ 
	getFile(pTable);					// Empty file init in openFile as well
	/* Blahblah */
	/* file->firstBlock = -1;
	file->lastBlock = -1; 	*/
}

/*
 Get a block of buffer that is not in use if any,
 	else repalce one with the Least Recently Used Algorithm
 @return int describe the block number in the Bufferlist
 */
int BufferManager::getFreeBlock(){
	int ret = 0;
	if ( blockCount < MAX_BLOCK_NUM ){						// Free block avaliable
		blockCount++;
		for (ret = 0; ret < MAX_BLOCK_NUM; ret++){
			if ( !Bufferlist[ret].is_Valid )
				break;
		}
	}
	else{
		time_t min_t = -1; 
		for (int i = 0; i < MAX_BLOCK_NUM; i++){
			if ( Bufferlist[i].Pinlock != true && (Bufferlist[i].LRU_Value < min_t || min_t == -1) ){
				min_t = Bufferlist[i].LRU_Value;
				ret = i;
			}
		}

		/* Remove block ret from the file it belongs to */
	 	int i = Bufferlist[ret].Fptr->firstBlock;
	 	while ( i >= 0 && Bufferlist[i].next != ret && i < MAX_BLOCK_NUM ) 
	 		i = Bufferlist[i].next; 						// Find the former block
	 	if ( i >= 0 && i < MAX_BLOCK_NUM )					// Just in case
			Bufferlist[i].next = Bufferlist[ret].next;		// Remove the block node off the fileblocks	
	}
	return ret;
}

/*
 Find the Block(if exist, else find free one) according to the FileInf pFi and offset
 @param FileInf *pFi: FileInfomation
 @param int 	offset: block offset in the file
 @return int 	block_index in Bufferlist 
 */
int BufferManager::getBlock(FileInf *pFi, int offset){
	if ( !pFi ) return -1;
	if ( offset == 1 ){
		return pFi->firstBlock;
	}
	else {
		int block_index = pFi->firstBlock;
		while ( block_index >= 0 && Bufferlist[block_index].Block_Offset != offset ) 
			block_index = Bufferlist[block_index].next;
		if ( block_index == -1 ){
			block_index = getFreeBlock();
			Bufferlist[block_index].Initialize(pFi, offset);
		}

		/* To link the node into the file-block queue */
		Bufferlist[block_index].next = Bufferlist[pFi->firstBlock].next;
		Bufferlist[pFi->firstBlock].next = block_index;
		return block_index;
	}
}

/*
 Not yet finished, record recover remained
 @param Table* 	pTable Table Information
 @param uuid 	Unique record id for each tuple
 @return Record*  
 */
Record* BufferManager::getRecord(Table *pTable, UUID uuid) 	// Exception to throw
{	
	FileInf *file;
	file = getFile(pTable);
	
	int Block_num = static_cast<int>(ceil(uuid / file->recordPerBlock));
	int block = getBlock(file, Block_num);
	long recordOffset = (int)uuid - (Block_num - 1) * file->recordPerBlock - 1;
	/*
	Record *recordHandle, *recnode;
	recordHandle = new Record;
	recnode = recordHandle;
	recnode->next = NULL;
	recordHandle = new Record[]
	*/
	return &Bufferlist[block].recordHandle[recordOffset];
}





/*
 Insert a new tuple into the table described by pTable
 @param Table 	*pTable: describe the table
 @param Record 	*rec: record to insert 
 */
bool BufferManager::insertRec(Table *pTable, Record* rec){
	FileInf *file;
	file = getFile(pTable);
	// TODO: Update FileInf
	UUID insert_uuid = pTable->recordNum + 1;				// Pre-condition: catalog has already update the record number

	//insert(file, insert_uuid, rec);

	/* Insert a record */
	if ( file->Block_Num == 0 ){
		file->Block_Num = 1;
		file->firstBlock = file->lastBlock = getBlock(file, 1);
		Bufferlist[file->firstBlock].next = -1;
		Bufferlist[file->firstBlock].lock();
	} 
	int BlockOffset = static_cast<int>(ceil(insert_uuid / file->recordPerBlock));
	if ( BlockOffset > file->Block_Num ){
		int block = getBlock(file, BlockOffset);
		Bufferlist[file->lastBlock].next = block;				// add a block to the table
		Bufferlist[file->lastBlock].unlock();
		Bufferlist[block].next = -1;
		Bufferlist[block].lock();
	}

	long recordOffset = insert_uuid - file->recordPerBlock * (file->Block_Num - 1);		// Insert in the last block
	long byteOffset = file->recordLen * (recordOffset - 1);
	for (int i = 0; i < pTable->attributes.size(); i++){
		switch ( pTable->attributes.at(i).dataType ){
			case Int: {
				memcpy(&Bufferlist[file->lastBlock].token[byteOffset], rec->data.at(i), sizeof(int));
				byteOffset += sizeof(int);
				delete static_cast<int *>(rec->data.at(i));
				break;
			}
			case Float: {
				memcpy(&Bufferlist[file->lastBlock].token[byteOffset], rec->data.at(i),  sizeof(float));
				byteOffset += sizeof(float);
				delete static_cast<float *>(rec->data.at(i));
				break;
			}
			case String: {
				memcpy(&Bufferlist[file->lastBlock].token[byteOffset], rec->data.at(i), strlen((char *)rec->data.at(i)) + 1);
				byteOffset += strlen((char *)rec->data.at(i));
				delete [] static_cast<char *>(rec->data.at(i));
				break;
			}
			case Uuid: {
				memcpy(&Bufferlist[file->lastBlock].token[byteOffset], rec->data.at(i), sizeof(UUID));	
				byteOffset += sizeof(UUID);
				delete static_cast<UUID *>(rec->data.at(i));
				break;
			}
			
		} 
		rec->data.clear();
	}
	file->recordNum++;
	delete rec;
	return true;
}

/*
 Delete a record off the table described via pTable
 @param Table 	*pTable
 @param UUID 	delete_uuid
 */
int BufferManager::deleteRec(Table *pTable, UUID delete_uuid){
	FileInf *file;
	file = getFile(pTable);

	/* In case of block withdraw i.e. delete the last tuple */
	int CurBlkNum_in_File = static_cast<int>(ceil(pTable->recordNum / file->recordPerBlock));
	if ( CurBlkNum_in_File != Bufferlist[file->lastBlock].Block_Offset ){
		Bufferlist[file->lastBlock].unlock();
		Bufferlist[file->lastBlock].is_Valid = false;
		//Bufferlist[file->lastBlock].Initialize(); 					// redundent operation
		int block = getBlock(file, CurBlkNum_in_File);
		if ( block != file->firstBlock ){
			int i = file->firstBlock;
			for (; i != -1 && Bufferlist[i].next != file->lastBlock; i = Bufferlist[i].next);
			if ( i != -1 ){
				Bufferlist[i].next = block;	
			}
			Bufferlist[block].next = -1;
			Bufferlist[block].lock();
		}
	}


	if ( delete_uuid != pTable->recordNum ){						// Pre-condition: the catalog has already decrease the number of records
																	// In case of the deletion of the last one
		/* delete */
		int blockNum = static_cast<int>(ceil(delete_uuid / file->recordPerBlock));		// 
		long recordOffset = delete_uuid - file->recordPerBlock * (blockNum - 1);
		long byteOffset = file->recordLen * (recordOffset - 1);
		int deleteblockIndex = getBlock(file, blockNum);				// The block to delete 

		/* Cover the deteled record with the last one */
		// Todo calculate the byteoffset of the origin
		memcpy(&Bufferlist[deleteblockIndex].token[byteOffset], &Bufferlist[file->lastBlock].token[byteOffset], file->recordLen);
	} 
	file->recordNum--;
	return pTable->recordNum;
}

/*
 Get the maximum uuid in the table described, i.e. recordNum
 @param Table *pTable
 @return UUID the recordNum
 */
UUID BufferManager::getMaxuuid(Table *pTable){
	if ( pTable ){
		FileInf *file;
		file = getFile(pTable);
		return file->recordNum;
	}
	else {
		return 0;
	}
}	

/* 
 remove the table directly
 @param Table *pTable
 */
void BufferManager::removeTable(Table *pTable){
	if ( pTable ){
		FileInf *file = getFile(pTable);
		char s[20];
		sprintf(s, "rm %s.table", file->File_id);
		system(s);	
	}
}

void BufferManager::quitProgram(){
	FileInf *fit, *nextfit;
	for (fit = flistHead; fit != NULL; fit = nextfit){
		nextfit = fit->next;
		closeFile(fit); 						// FileInf deleted in closeFile();
		/*
		Bufferlist[fit->firstBlock].unlock();
		Bufferlist[fit->lastBlock].unlock();
		for (int i = fit->firstBlock; i != -1 && i < MAX_BLOCK_NUM; i = Bufferlist[i].next){
			Bufferlist[i].Flushback();
			Bufferlist[i].is_Valid = false;
		}
		delete fit;								// To get rid of mem-leak
		*/
	}
		
	// There is no need to destruct the block because it is done in the destructor	
}
