/*
	Block.cpp

 */

#include "Block.h"

/*
 Void Constructor 
 Set the block invaild
 */
Block::Block(){
	is_Valid = is_Dirty = 0;
	Initialize();
}

/* 
 Desctructor
 No need to Flushback the block, because already managed by BufferManager
 */
Block::~Block(){
	;
}

/*
 Void initiate, invaild block
 */
void Block::Initialize(){
	if ( is_Valid && is_Dirty ) Flushback();
	File_Num 		= -1;
	Block_Offset	= 0;
	LRU_Value 		= 0;
	Pinlock 		= false;
	is_Valid 		= false;
	is_Dirty 		= false;
	Fptr 			= NULL;
	recordHandle 	= NULL;
	next 			= -1;
	memset(token, EMPTY, BLOCK_SIZE);
}

/* 
 Initiate the buffer block with file block, vaild
 @param pFi 	- pointer to FileInf 
 @param offset  - blockoffset 
 */
void Block::Initialize(FileInf *pFi, int offset ){
	if ( is_Valid && is_Dirty ) Flushback();
	File_Num 		= pFi->File_id;
	Block_Offset 	= offset;
	Pinlock 		= 0;
	is_Valid 		= true;
	is_Dirty 		= false;
	Fptr 			= pFi;
	next	 		= -1;
	time(&LRU_Value);
	pFi->fd.seekg(BLOCK_SIZE * Block_Offset, ios::beg);
	pFi->fd.read(token, BLOCK_SIZE);

	/* Check the End Of File with gcount smaller than BLOCK_SIZE */
	int i = pFi->fd.gcount();
	if ( i != BLOCK_SIZE ){
		for(; i < BLOCK_SIZE; i++)
			token[i] = EMPTY;
	}

	/*Create record pointers */ 
	recordHandle = new Record[pFi->recordPerBlock];
	unsigned int byteOffset = 0;
	unsigned int rec_num = 0;
	for (; (byteOffset + pFi->recordLen) < BLOCK_SIZE; byteOffset += pFi->recordLen){
		for (unsigned int i = 0; i < pFi->dataVector->size(); i++){
			recordHandle[rec_num].data.push_back(reinterpret_cast<void *>(&token[byteOffset]));
			switch ( pFi->dataVector->at(i).dataType ) {
				Int: {
					byteOffset += sizeof(int);
					break;
				}
				Float: {
					byteOffset += sizeof(float);
					break;
				}
				String: {
					byteOffset += pFi->dataVector->at(i).dataLength * sizeof(char);
					break;
				}
				Uuid: {
					byteOffset += sizeof(UUID);
					break;
				}
			}
		}
		recordHandle[rec_num].next = NULL;
		rec_num++;
	}
}

/*
 Flushback

 */
void Block::Flushback(){
	if (Fptr){
		Fptr->fd.seekp(BLOCK_SIZE * (Block_Offset - 1), ios::beg);
		Fptr->fd.write(token, BLOCK_SIZE);
	}
	if ( recordHandle ) 
		delete [] recordHandle;
	// Exception to throw
}



