#C Compiler
CFLAGS = -Wall -std=c99
CLIBS = 
CC = gcc
SRC_FOLDER = src
INC_FOLDER = include
LIB_DESTINY_FOLDER = lib
TEST_FOLDER = teste
BIN_DESTINY_FOLDER = bin
#Arquiver
AR = ar
AR_OPT = crs
LIB_NAME = t2fs


all: lib testes apps

DirectoryBlock: $(INC_FOLDER)/DirectoryBlock.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/DirectoryBlock.c -o $(LIB_DESTINY_FOLDER)/DirectoryBlock.o

DiscAccessManager: $(INC_FOLDER)/DiscAccessManager.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/DiscAccessManager.c -o $(LIB_DESTINY_FOLDER)/DiscAccessManager.o

FilePath: $(INC_FOLDER)/FilePath.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/FilePath.c -o $(LIB_DESTINY_FOLDER)/FilePath.o

FileSystem: $(INC_FOLDER)/FileSystem.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/FileSystem.c -o $(LIB_DESTINY_FOLDER)/FileSystem.o

FreeSpaceManager: $(INC_FOLDER)/FreeSpaceManager.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/FreeSpaceManager.c -o $(LIB_DESTINY_FOLDER)/FreeSpaceManager.o

IndirectionBlock: $(INC_FOLDER)/IndirectionBlock.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/IndirectionBlock.c -o $(LIB_DESTINY_FOLDER)/IndirectionBlock.o

OpenFile: $(INC_FOLDER)/OpenFile.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/OpenFile.c -o $(LIB_DESTINY_FOLDER)/OpenFile.o

OpenRecord: $(INC_FOLDER)/OpenRecord.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/OpenRecord.c -o $(LIB_DESTINY_FOLDER)/OpenRecord.o

t2fs: $(INC_FOLDER)/t2fs.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/t2fs.c -o $(LIB_DESTINY_FOLDER)/t2fs.o

t2fs_record: $(INC_FOLDER)/t2fs_record.h
	$(CC) -c $(CFLAGS) $(CLIBS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/t2fs_record.c -o $(LIB_DESTINY_FOLDER)/t2fs_record.o

apidisk: $(LIB_DESTINY_FOLDER)/libapidisk.a
	(cd $(LIB_DESTINY_FOLDER); ls; $(AR) x libapidisk.a)

lib: DirectoryBlock DiscAccessManager FilePath FileSystem FreeSpaceManager IndirectionBlock OpenFile OpenRecord t2fs t2fs_record apidisk
	$(AR) $(AR_OPT) $(LIB_DESTINY_FOLDER)/lib$(LIB_NAME).a $(LIB_DESTINY_FOLDER)/DirectoryBlock.o $(LIB_DESTINY_FOLDER)/DiscAccessManager.o $(LIB_DESTINY_FOLDER)/FilePath.o $(LIB_DESTINY_FOLDER)/FileSystem.o $(LIB_DESTINY_FOLDER)/FreeSpaceManager.o $(LIB_DESTINY_FOLDER)/IndirectionBlock.o $(LIB_DESTINY_FOLDER)/OpenFile.o $(LIB_DESTINY_FOLDER)/OpenRecord.o $(LIB_DESTINY_FOLDER)/t2fs.o $(LIB_DESTINY_FOLDER)/t2fs_record.o $(LIB_DESTINY_FOLDER)/apidisk.o

testes: lib	
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(TEST_FOLDER)/FilePathTest.c -o $(BIN_DESTINY_FOLDER)/FilePathTest -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME) $(CLIBS)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(TEST_FOLDER)/DirectoryBlockTest.c -o $(BIN_DESTINY_FOLDER)/DirectoryBlockTest -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME) $(CLIBS)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(TEST_FOLDER)/DiscAccessManagerTest.c -o $(BIN_DESTINY_FOLDER)/DiscAccessManagerTest -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME) $(CLIBS)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(TEST_FOLDER)/FileSystemTest.c -o $(BIN_DESTINY_FOLDER)/FileSystemTest -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME) $(CLIBS)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(TEST_FOLDER)/FreeSpaceManagerTest.c -o $(BIN_DESTINY_FOLDER)/FreeSpaceManagerTest -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME) $(CLIBS)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(TEST_FOLDER)/IndirectionBlockTest.c -o $(BIN_DESTINY_FOLDER)/IndirectionBlockTest -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME) $(CLIBS)


apps: lib
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/mkdirt2.c -o $(BIN_DESTINY_FOLDER)/mkdirt2 -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME)
	#$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/copy2t2.c -o $(BIN_DESTINY_FOLDER)/copy2t2 -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/rmdirt2.c -o $(BIN_DESTINY_FOLDER)/rmdirt2 -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME)
	$(CC) $(CFLAGS) -I $(INC_FOLDER)/ $(SRC_FOLDER)/dirt2.c -o $(BIN_DESTINY_FOLDER)/dirt2 -L $(LIB_DESTINY_FOLDER) -l$(LIB_NAME)

clean:
	rm -rf $(BIN_DESTINY_FOLDER)/* $(LIB_DESTINY_FOLDER)/*.o $(LIB_DESTINY_FOLDER)/lib$(LIB_NAME).a


	

 
