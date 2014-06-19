#include "FileSystem.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

FileSystem fileSystem;

t2fs_file FS_create(FilePath* filePath)
{
    //Directory Path until folde to create new file
	FilePath dirPath;
    FP_withoutLastNode(filePath, &dirPath);
    //Record of the parent who will get new child
    OpenRecord parentOpenRecord;
    BYTE parentBlock[fileSystem.superBlock.BlockSize];
    
    //Find parent, starting from the root
    Record* parentRecord = TR_find(fileSystem.superBlock.RootDirReg, dirPath, &parentOpenRecord, parentBlock, NULL, NULL, NULL);
    FP_destroy(&dirPath);
    
    Record newRecord;
    TR_t2fs_record(&newRecord, TYPEVAL_REGULAR, FP_getLastNode(filePath), 0, 0);
    
    OpenRecord newOpenRecord;
    int addRecordSignal = TR_addRecord(parentRecord, newRecord, newOpenRecord);
    //Update open record too
    parentOpenRecord.record = parentRecord;
    
    int writingSignal;
    if (addRecordSignal == TR_RECORD_MODIFIED){
        //If modified a directory and it is not the root directory
        if (parentBlock != NULL){
            writingSignal = DAM_write(parentOpenRecord.blockAddress, parentBlock, fileSystem.superBlock.BlockSize);
        }else{
            //If modified the root directory then save it. Actually save the superblock that has the root directory
            //Supose SuperBlock doesn't have alligment paddings, i.e., sizeof(SuperBlock) <= SuperBlock.SuperBlockSize
            writingSignal = DAM_write(FS_SUPERBLOCK_ADDRESS, SuperBlock, sizeof(SuperBlock));
        }
    }else if(addRecordSignal != TR_ADDRECORD_SUCCESS){
        return addRecordSignal;
    }
    
    t2fs_file newHandle = FS_createHandle(filePath, newOpenRecord);
    if (newHandle < 0){
        return FS_CREATESUCCESS_BUT_OPENPROBLEM;
    }else{
        return newHandle;
    }
}

t2fs_file FS_createHandle(OpenRecord openRecord)
{
    assert openRecord.record.TypeVal != TYPEVAL_INVALIDO;
    assert openRecord.record.name != NULL;
    assert openRecord.blockAddress != 0;
    
    //Verify if there is place in fileSytem open records and if this record is already open
    int i;
    BOOL alreadyOpen = FALSE;
    int possiblePlaceRecords = -1;
    for (i = 0; i < FS_OPENRECORDS_MAXSIZE; i++){
        if (fileSystem.openRecords[i].record.TypeVal != TYPEVAL_INVALIDO){
            if (OR_equals(fileSystem.openRecords[i], openRecord)){
                alreadyOpen = TRUE;
                possiblePlaceRecords = i;
                break;
            }
        }else{
            possiblePlaceRecords = i;
        }
    }
    
    //Verify if there is place in fileSytem open files
    int possiblePlaceFiles = -1;
    for (i = 0; i < FS_OPENFILES_MAXSIZE; i++){
        if (fileSystem.openFiles[i].recordIndex < 0){
            possiblePlaceFiles = i;
            break;
        }
    }
    
    if (alreadyOpen){
        assert possiblePlaceRecords >= 0;
        if (possiblePlaceFiles >= 0){
            //New openfile will point this record
            assert fileSystem.openRecords[possiblePlaceRecords].count > 0;
            fileSystem.openRecords[possiblePlaceRecords].count++;

            OpenFile newOpenFile;
            OF_OpenFile(&newOpenFile, possiblePlaceRecords);
            fileSystem.openFiles[possiblePlaceFiles] = newOpenFile;
            //The address of the handle just created
            return possiblePlaceFiles;
        }else{
            return FS_OPENPROBLEM_FILEPOINTER_SPACE_UNAVAILABLE;
        } 
    }else{
        //If file is not open yet and there is place in fileSystem open record then...
        if (possiblePlaceRecords >= 0){
            if (possiblePlaceFiles >= 0){
                //Copies the new record into fileSystem's openRecords
                fileSystem.openRecords[possiblePlaceRecords] = openRecord;
                fileSystem.openRecords[possiblePlaceRecords].count = 1;
                
                OpenFile newOpenFile;
                OF_OpenFile(&newOpenFile, possiblePlaceRecords);
                fileSystem.openFiles[possiblePlaceFiles] = newOpenFile;
                //The address of the handle just created
                return possiblePlaceFiles;
                
            }else{
                return FS_OPENPROBLEM_FILEPOINTER_SPACE_UNAVAILABLE;
            } 
        }else{
            return FS_OPENPROBLEM_RECORDPOINTER_SPACE_UNAVAILABLE;
        }
    }
}

t2fs_record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, char* name, int count)
{
	return 0;
}

int FS_findEmptyInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, int count)
{
	return 0;
}

int FS_delete(FilePath* filePath)
{
	return 0;
}

t2fs_file FS_open(FilePath* filePath)
{
	return 0;
}
