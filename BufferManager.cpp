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
	int B_index, nextblock;
	if ( !pFi ) return;
	if ( pFi->firstBlock >= 0 ) Bufferlist[pFi->firstBlock].unlock();
	if ( pFi->lastBlock >= 0 )	Bufferlist[pFi->lastBlock].unlock();
	for (B_index = pFi->firstBlock; B_index != -1; B_index = nextblock){
		nextblock = Bufferlist[B_index].next;
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
FileInf* BufferManager::openFile(const Table *pTable){
	FileInf *ret;
	ret = new FileInf(pTable);		
	if ( ret->Block_Num != 0){								// In case of new created table
		ret->firstBlock = getFreeBlock();							
		Bufferlist[ret->firstBlock].Initialize(ret, 1);		// Bug fixed, should init first block with 1 instead of 0
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
FileInf* BufferManager::getFile(const Table *pTable){
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
	if ( fileCount == 1 ){								// Because the fileCount has increased in open
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
bool BufferManager::createTable(const Table *pTable){
	/*
	FileInf *file;
	file = 
	*/ 
	if ( getFile(pTable) != NULL ){ 					// Empty file init in openFile as well
		return true;
	}			
	else {
		return false;
	}
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
	 	int former_block = Bufferlist[ret].Fptr->firstBlock;
	 	while ( former_block >= 0 && Bufferlist[former_block].next != ret && former_block < MAX_BLOCK_NUM ) 
	 		former_block = Bufferlist[former_block].next; 						// Find the formerBlock
	 	if ( former_block >= 0 && former_block < MAX_BLOCK_NUM )					// Just in case
			Bufferlist[former_block].next = Bufferlist[ret].next;		// Remove the block node off the fileblocks	
	}
	return ret;
}

/*
 Find the Block(if exist, else find free one) according to the FileInf pFi and offset
 @param FileInf *pFi: FileInfomation
 @param int 	offset: block offset in the file
 @return int 	block_index in Bufferlist 
 TODO: revise...
 */
int BufferManager::getBlock(FileInf *pFi, int offset){
	if ( !pFi ) return -1;
	int block_index = pFi->firstBlock;
	if ( offset == 1 && pFi->firstBlock != -1 ){
		return pFi->firstBlock;
	}
	else if ( offset == pFi->Block_Num && pFi->lastBlock != -1 && Bufferlist[pFi->lastBlock].Block_Offset == offset ){
		// lastBlock stays as is
		return pFi->lastBlock;
	}
	else {
		/* In case of the first block in a new file*/ 
		if ( offset == 1 ){
			block_index = getFreeBlock();
			Bufferlist[block_index].Initialize(pFi, offset);
			Bufferlist[block_index].lock();
			Bufferlist[block_index].next = -1;
		}
		else{
			int former_block = -1;
			while ( block_index >= 0 && Bufferlist[block_index].Block_Offset != offset ){
				former_block = block_index;
				block_index = Bufferlist[block_index].next;
			}
			if ( block_index == -1 ){
				block_index = getFreeBlock();
				Bufferlist[block_index].Initialize(pFi, offset);

				/*  
					To link the node into the file-block queue, except for the case of 
					the lastBlock when lastblock move forward
				 */
				if ( offset != pFi->Block_Num ){				
					Bufferlist[block_index].next = Bufferlist[pFi->firstBlock].next;
					Bufferlist[pFi->firstBlock].next = block_index;
				}
				else {

				}
			}
			else if ( offset == pFi->Block_Num ){								// lastBlock not in the list, i.e. lastBlock withdraw
				Bufferlist[former_block].next = Bufferlist[block_index].next;	// remove the node inside 
				Bufferlist[block_index].next = -1;								// linked in later
			}					
		}
		return block_index;
	}
}

/*
 Not yet finished, record recover remained
 @param Table* 	pTable Table Information
 @param uuid 	Unique record id for each tuple
 @return Record*  
 */
Record* BufferManager::getRecord(const Table *pTable, UUID uuid) 	// Exception to throw
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
 Almost bugless, report ASAP, if any.
 */
bool BufferManager::insertRec(const Table *pTable, Record* rec){
	FileInf *file;
	file = getFile(pTable);
	// TODO: Update FileInf
	UUID insert_uuid = pTable->recordNum + 1;				// Pre-condition: catalog has already update the record number

	//insert(file, insert_uuid, rec);

	/* Insert a record */
	if ( file->Block_Num == 0 ){
		file->Block_Num++;
		file->firstBlock = file->lastBlock = getBlock(file, 1);
		Bufferlist[file->firstBlock].next = -1;
		Bufferlist[file->firstBlock].lock();
	} 
	else { 															
		int BlockOffset = static_cast<int>(ceil((float)insert_uuid / file->recordPerBlock));
		if ( BlockOffset > file->Block_Num ){						// In case of block overflow
			file->Block_Num++;
			int block = getBlock(file, BlockOffset);
			Bufferlist[file->lastBlock].next = block;				// add a block to the table
			if ( file->lastBlock != file->firstBlock )				// Equals when one-block evolve two
				Bufferlist[file->lastBlock].unlock();
			Bufferlist[block].next = -1;
			Bufferlist[block].lock();
			file->lastBlock = block;
		}
	}
	

	long recordOffset = insert_uuid - file->recordPerBlock * (file->Block_Num - 1);		// Insert in the last block
	long byteOffset = file->recordLen * (recordOffset - 1);
	for (unsigned int i = 0; i < pTable->attributes.size(); i++){
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
				memcpy(&Bufferlist[file->lastBlock].token[byteOffset], static_cast<string *>(rec->data.at(i))->c_str(), strlen((char *)rec->data.at(i)) + 1);
				byteOffset += strlen(static_cast<string *>(rec->data.at(i))->c_str());
				delete static_cast<string *>(rec->data.at(i));
				break;
			}
			case Uuid: {
				memcpy(&Bufferlist[file->lastBlock].token[byteOffset], rec->data.at(i), sizeof(UUID));	
				byteOffset += sizeof(UUID);
				delete static_cast<UUID *>(rec->data.at(i));
				break;
			}
			
		} 
		
	}
	rec->data.clear();
	file->recordNum++;
	Bufferlist[file->lastBlock].is_Dirty = true;
	delete rec;
	return true;
}

/*
 Delete a record off the table described via pTable
 @param Table 	*pTable
 @param UUID 	delete_uuid
 */
int BufferManager::deleteRec(const Table *pTable, UUID delete_uuid){
	FileInf *file;
	file = getFile(pTable);
	long last_recOff = file->recordNum - file->recordPerBlock * (file->Block_Num - 1);
	long last_byteOff = file->recordLen * (last_recOff - 1);
	if ( delete_uuid != file->recordNum ){							// The record to delete is not the last one

		/* delete */
		int del_blockNum = static_cast<int>(ceil(delete_uuid / file->recordPerBlock));		// 
		long del_recordOffset = delete_uuid - file->recordPerBlock * (del_blockNum - 1);
		long del_byteOffset = file->recordLen * (del_recordOffset - 1);
		int del_blkIndex = getBlock(file, del_blockNum);				// The block to delete 

		/* Cover the deteled record with the last one */
		memcpy(&Bufferlist[del_blkIndex].token[del_byteOffset + sizeof(UUID)], 
				&Bufferlist[file->lastBlock].token[last_byteOff + sizeof(UUID)], 
				file->recordLen - sizeof(UUID));
		Bufferlist[del_blkIndex].is_Dirty = true;
	} 

	/* To delete the last block */
	memset(&Bufferlist[file->lastBlock].token[last_byteOff], EMPTY, file->recordLen);
	Bufferlist[file->lastBlock].is_Dirty = true;

	/* 	
	  	In case of block withdraw i.e. the last tuple 
	   	unluckily the tuple being the first in its the block
	 */   
	int CurBlkNum_in_File = static_cast<int>(ceil((float)(file->recordNum - 1) / file->recordPerBlock));
	if ( CurBlkNum_in_File < file->Block_Num ){
		file->Block_Num--;
		blockCount--;
		Bufferlist[file->lastBlock].unlock();
		Bufferlist[file->lastBlock].Initialize();
		if ( CurBlkNum_in_File == 0 ){
			file->firstBlock = file->lastBlock = -1;
		}
		else {
			int block = getBlock(file, CurBlkNum_in_File);					
			if ( block != file->firstBlock ){								// Equals when two-blocks reduce to one
				int i = file->firstBlock;
				while ( i >= 0 && i < MAX_BLOCK_NUM && Bufferlist[i].next != file->lastBlock )
					i = Bufferlist[i].next;
				if ( i != -1 ){
					Bufferlist[i].next = block;								// Link the lastBlock back to the list
				}
			}
			Bufferlist[block].next = -1;
			Bufferlist[block].lock();
			file->lastBlock = block;	
		}
	}

	/* 
	 	Descrease the number in FileInf while 
	 	pTable is maintained by the CatalogManager 
	 */ 
	file->recordNum--;
	return pTable->recordNum;
}

/*
 Get the maximum uuid in the table described, i.e. recordNum
 @param Table *pTable
 @return UUID the recordNum
 */
UUID BufferManager::getMaxuuid(const Table *pTable){
	if ( pTable ){
		FileInf *file;
		file = getFile(pTable);
		return file->recordNum;
	}
	else {
		return 0;
	}
}	


bool BufferManager::deleteAll(const Table *pTable ){
	return removeTable(pTable) && createTable(pTable);
}

/* 
 remove the table directly
 @param Table *pTable
 TODO: IF THE FILE IS CURRENTLY IN THE FILELIST, UPDATE THE LIST 
 */
bool BufferManager::removeTable(const Table *pTable){
	if ( pTable ){

}

void BufferManager::quitDB(){
	FileInf *fit, *nextfit;
	for (fit = flistHead; fit != NULL; fit = nextfit){
		nextfit = fit->next;
		closeFile(fit); 						// FileInf deleted in closeFile();
	}	
	// There is no need to destruct the block because it is done in the destructor	
}
