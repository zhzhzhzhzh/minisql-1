/*
	Block.h 
 */

#ifndef _BLOCK_H_
#define _BLOCK_H_
#include <ctime>
#include <cmath>
#include <cstring>
#include "typedefs.h"
#define EMPTY 		0x00
#define BLOCK_SIZE 	4096


using namespace std;
struct FileInf;
class Block
{
	/* Friend class */ 		// Too lazy a coder ^_^
	friend class BufferManager;
	char 		token[BLOCK_SIZE];
	int     	next;			// Pointer to the next block
	int 		File_Num;		// Associated file
	bool 		Pinlock;		// Pinlocked to prevent blcok from replacing 
	bool 		is_Valid;		// Block in use
	bool		is_Dirty;		// Write Back required
	time_t 		LRU_Value;		// To implement Least Recent Used Algorithm
	int 		Block_Offset;	// Offset in the file 
	FileInf 	*Fptr;			// The file this block belongs to
	Record 		*recordHandle;
	
public:
		
	/* Void Constructor, initiate the block with empty value */
	Block();
	~Block();
	void Initialize();
	void Initialize(FileInf *, int);
	inline void lock(){			// Lock and unlock to special for first and last blocks of a file
		Pinlock = true;
	};			
	inline void unlock(){
		Pinlock = false;
	};

private:
	void Flushback();
};

struct FileInf{
public:
	fstream 	fd;
	int			File_id;
	UUID 		recordNum; 				
	int			recordLen; 		
	int 		Block_Num;
	int 		Attr_Num;
	int  		recordPerBlock;
	int 		firstBlock;
	int  		lastBlock;
	std::vector<Attribute> *dataVector;
	FileInf		*next;

	FileInf(const Table* pTable):
		File_id(pTable->tableNum), recordNum(pTable->recordNum), Attr_Num(pTable->attrNumber)
	{
		next = NULL;
		firstBlock = -1;
		lastBlock = -1;
		if ( !pTable ){
			return;
		}

		/* To do: mind the mem-leak */
		dataVector = new std::vector<Attribute>(pTable->attributes);
		
		/* Create or/and Open the file *.table */
		char *fileName;
		fileName = new char[50];
		#if WIN
		sprintf(fileName, "./%s/%d.table", pTable->dbName.c_str(), File_id);
		#endif
		#if MACOS
		sprintf(fileName, "./%s/%d.table", pTable->dbName, File_id);
		#endif
		fd.open(fileName, ios::in | ios::out | ios::binary);	
		if ( !fd ){
			fd.open(fileName, ios::in | ios::out | ios::binary | ios::trunc);
			fd.close();
			// fd.write("aaaa", 10);
			fd.open(fileName, ios::in | ios::out | ios::binary);
		}

		/* Calculating recordLen and blockNum */
		long len = pTable->attributes.size();
		recordLen = 0;
		for (int i = 0; i < len; i++){
			switch( pTable->attributes.at(i).dataType ){
				case Int: {
					recordLen += sizeof(int);
					break;
				}
				case Float: {
					recordLen += sizeof(float);
					break;
				}
				case String: {
					recordLen += pTable->attributes.at(i).dataLength * sizeof(char);
					break;
				}
				case Uuid: {
					recordLen += sizeof(UUID);
					break;
				}
			}
		}
		recordPerBlock = BLOCK_SIZE / recordLen;
		Block_Num = static_cast<int>(ceil((float)recordNum / recordPerBlock));

		/* Pointer to Blocks remains unsolved */
		delete [] fileName; 				
	}	
};

#endif /* _BLOCK_H_ */


