/*
 Operation eg.h
 */

 /* To Create Table: */
 	Table *pTable;
 	BufferManager.createTable(pTable);


 /* To find a record: */
 	UUID uuid;
 	Table *pTable;
 	BufferManager.getRecord(pTable, uuid);


 /* To insert a record: */
 	Record *pRecord;
 	Table pTable;
 	BufferManager.insertRec(pTable, pRecord);


 /* To delete a record: */
	Table pTable;
	UUID delete_uuid;
 	UUID ret = BufferManager.insertRec(pTable, delete_uuid); 	
 	/* ret to swap */

 /* To quit: */
 	BufferManager.quitProgram();

