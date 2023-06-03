/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>


typedef struct
{
  int type;
  int byref;
  
  union
  {
    int intVal;
    char* string;
    int variableNumber;
    int userFunction;
    int label;
  } data;
} Parameter;

typedef struct CodeLine CodeLine;
struct CodeLine
{
  int type;
  int opcode;
  char* name;
  
  Parameter* destination;
  Parameter** params;
  int paramsCount;

  CodeLine** subCodeLines;
  int subCodeLinesCount;

  int functionNumber;

  char string[10];
  int operationNumber;
  int destLabel;
  int comparisonType;
  int labelNumber;

};

#define FUNCTION 0
#define OPERATION 1
#define IFSTATEMENT 2
#define COMPARISON 3
#define LABEL 4
#define GOTO 5
#define EXIT 6
#define ABORT 7
#define FUNCRETURN 8
#define HANDLER 9
#define CALL 10
#define RETURN 11

#define OP_UNKNOWN 0
#define OP_EQUATE 1
#define OP_LESSTHAN 2
#define OP_GREATERTHAN 3
#define OP_LESSTHANEQUAL 4
#define OP_GREATERTHANEQUAL 5
#define OP_EQUAL 6
#define OP_NOTEQUAL 7
#define OP_PLUS 8
#define OP_MINUS 9
#define OP_MULT 10
#define OP_DIV 11
#define OP_BITAND 12
#define OP_BITOR 13
#define OP_BITEOR 14
#define OP_BITNOT 15
#define OP_SHIFTL 16
#define OP_SHIFTR 17
#define OP_LOGICAND 18
#define OP_LOGICOR 19
#define OP_STRCAT 20
#define OP_PATHCAT 21
#define OP_MOD 22


static char* operations[] = {NULL, "=", "<", ">", "<=", ">=", "=", "!=",
			     "+", "-", "*", "/", "&", "|", "^", "!", "<<",
			     ">>", "&&", "||", "+", "^"};

typedef struct
{
  char* name;
} SysVar;


typedef struct
{
  int usage;
  int position;
  
  int functionNumber;
} Label;


typedef struct
{
  CodeLine** codeLines;
  int codeLinesCount;
  int codeLinesMax;

  int* localStrings;
  int localStringsCount;

  int* localNumbers;
  int localNumbersCount;
  
  int prototype;
} FunctionBody;


typedef struct
{
  int type;
  char *name;
  int label;
  int returnType;
  FunctionBody* functionBody;
  


  // top bit set => byref
  int* params;
  int paramsCount;

  char** paramStringsNames;
  int paramStringsCount;

  char** paramNumbersNames;
  int paramNumbersCount;

  int* localStrings;
  int localStringsCount;

  int* localNumbers;
  int localNumbersCount;
} FunctionPrototype;




#define FUNCTION_USER 2
#define FUNCTION_DLL 1

typedef struct
{
  unsigned int type;
  unsigned int size;
  char* name;
} TypeEntry;

typedef struct
{
  char* name;
  unsigned int entriesCount;
  TypeEntry** entries;
} DataType;

typedef struct
{

  unsigned long versionMagic;
  unsigned char fileVersion;
  unsigned long crc;
  char* infoString;
  char* compilerVersion;
  long eofPos;

  FunctionPrototype** functionPrototypes;
  int functionPrototypesCount;

  FunctionBody** functionBodies;
  int functionBodiesCount;

  SysVar** stringSysVars;
  int stringSysVarsCount;

  SysVar** numberSysVars;
  int numberSysVarsCount;

  int* stringUserVars;
  int stringUserVarsCount;

  int* numberUserVars;
  int numberUserVarsCount;

  DataType** dataTypes;
  int dataTypesCount;

  CodeLine** codeLines;
  int codeLinesCount;
  int codeLinesMax;

  Label** labels;
  int labelsCount;
} ISData;



#define PARAM_UNKNOWN 0
#define PARAM_STRINGCONST 1
#define PARAM_LONGCONST 2
#define PARAM_SYSTEMSTRINGVARIABLE 3
#define PARAM_USERSTRINGVARIABLE 4
#define PARAM_SYSTEMNUMBERVARIABLE 5
#define PARAM_USERNUMBERVARIABLE 6
#define PARAM_FNPARAMSTRINGVARIABLE 7
#define PARAM_FNPARAMNUMBERVARIABLE 8
#define PARAM_FNLOCALSTRINGVARIABLE 9
#define PARAM_FNLOCALNUMBERVARIABLE 10
#define PARAM_LABEL 11
#define PARAM_DATATYPENUM 12

#define OTHER_UNKNOWN 0
#define OTHER_USERFUNCTION 1
#define OTHER_LABEL 2
#define OTHER_NUMPARAMS 3

#define TYPE_STRING 0
#define TYPE_CHAR 1
#define TYPE_LONG 2
#define TYPE_INT 3
#define TYPE_NUMBER 4
#define TYPE_LIST 5
#define TYPE_BOOL 6
#define TYPE_HWND 7

static char* types[] = {"string", "char", "long", "int", "number", 
			"LIST", "BOOL", "HWND"};

#endif





