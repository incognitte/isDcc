/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/


#ifndef DECODEOPS_H
#define DECODEOPS_H

#include "common.h"

static void parseSystemFunction(int fd, int opcode, int inFunction, ISData* isData);
static void parseStructFunction(int fd, int opcode, int inFunction, ISData* isData);
static void parseUserFunction(int fd, int opcode, int inFunction, ISData* isData);
static void parseDLLFunction(int fd, int opcode, int inFunction, ISData* isData);
static void doComponentTotalSize(int fd, int opcode, int inFunction, ISData* isData);
static void doComponentListItems(int fd, int opcode, int inFunction, ISData* isData);
static void label(int fd, int opcode, int inFunction, ISData* isData);
static void equate(int fd, int opcode, int inFunction, ISData* isData);
static void ifStmt(int fd, int opcode, int inFunction, ISData* isData);
static void comparison(int fd, int opcode, int inFunction, ISData* isData);
static void decodeOp(int fd, int opcode, int inFunction, ISData* isData);
static void plusValue(int fd, int opcode, int inFunction, ISData* isData);
static void notOp(int fd, int opcode, int inFunction, ISData* isData);
static void askOptions(int fd, int opcode, int inFunction, ISData* isData);
static void doGoto(int fd, int opcode, int inFunction, ISData* isData);
static void functionStart(int fd, int opcode, int inFunction, ISData* isData);
static void functionEnd(int fd, int opcode, int inFunction, ISData* isData);
static void funcReturn(int fd, int opcode, int inFunction, ISData* isData);
static void doExit(int fd, int opcode, int inFunction, ISData* isData);
static void doAbort(int fd, int opcode, int inFunction, ISData* isData);
static void sPrintf(int fd, int opcode, int inFunction, ISData* isData);
static void sPrintfBox(int fd, int opcode, int inFunction, ISData* isData);
static void doHandler(int fd, int opcode, int inFunction, ISData* isData);
static void doCall(int fd, int opcode, int inFunction, ISData* isData);
static void doReturn(int fd, int opcode, int inFunction, ISData* isData);
static void singleVarOp(int fd, int opcode, int inFunction, ISData* isData);


#endif
