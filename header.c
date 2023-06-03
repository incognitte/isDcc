/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#include <stdlib.h>
#include "ishield.h"
#include "util.h"
#include "common.h"

static FunctionPrototype** parseFunctions(int fd, int numFuncs, int fileVersion);
static SysVar** parseSysVars(int fd, int numVar);
static DataType** parseUserTypes(int fd, int numTypes);
static int parseType(int fd, int type);

void parseHeader(int fd, ISData* isData)
{
  int tmp;
  int filePos;
  int i;

  // --------------------------------------------------------------------------
  // check main magic 
  lseek(fd, 0, 0);
  if (get4Byte(fd) != ISHIELD_MAGIC)
    error("File is not an installshield file! (Bad Magic)\n");


  // --------------------------------------------------------------------------
  // check version magic
  switch(isData->versionMagic = get4Byte(fd))
  {
  case ISHIELD_MAGIC_v2_20_905:
    isData->fileVersion = 2;
    break;

  case ISHIELD_MAGIC_v3_00_065:
    isData->fileVersion = 3;
    break;

  case ISHIELD_MAGIC_v5_00_000:
    isData->fileVersion = 5;
    break;

  default:
    error("Unknown Installshield version!\n");
  }

  // --------------------------------------------------------------------------
  // print out interesting (ahem) header stuff

  // the CRC
  isData->crc = get4Byte(fd);

  // the info string
  tmp = get1Byte(fd);
  isData->infoString = (char*) mallocX(tmp+1);
  filterLF(getString(fd, isData->infoString, tmp));


  // --------------------------------------------------------------------------
  // get number of labels & make up their memory space
  // added +1 onto labelsCount since it seems label0 IS valid....
  // graaa!
  isData->labelsCount = get2Byte(fd) +1;
  isData->labels = (Label**) mallocX(sizeof(Label*) * isData->labelsCount);
  for(i=0; i< isData->labelsCount; i++)
  {
    isData->labels[i] = (Label*) mallocX(sizeof(Label));
    isData->labels[i]->usage=0;
    isData->labels[i]->position=-1;
  }


  // --------------------------------------------------------------------------
  // next comes the string bytes. I'm not ACTUALLY quite sure what these
  // do, but they're definitely to do with string initialisation
  // we'll just skip over them for now, but keep a note of the length for
  // later
  // there are 0x0002's for each system string variable
  // there are 0xffff's for each user string variable, PLUS 5 EXTRA ones!!!
  // (the 5 extra ones are a bug in version 1 of the files, I think)
  tmp = get2Byte(fd);
  lseek(fd, tmp * 2, 1);

  // process string system variables
  isData->stringSysVarsCount = get2Byte(fd);
  isData->stringSysVars = parseSysVars(fd, isData->stringSysVarsCount);

  // work out number of user string variables
  // this equals (stringStuffLength - stringSysVarsCount) - 5
  // why 5 has to be subtracted, i don't know, but it does
  if (isData->fileVersion == 2)
    isData->stringUserVarsCount = (tmp - isData->stringSysVarsCount)-5;
  else
    isData->stringUserVarsCount = tmp - isData->stringSysVarsCount;

  // --------------------------------------------------------------------------
  // get number of user number variables
  // Needs 0xb subtracted from it in file version 1... bug??

  // get the number of user vars
  tmp = get2Byte(fd);

  // process numerical system variables
  isData->numberSysVarsCount = get2Byte(fd);
  isData->numberSysVars = parseSysVars(fd, isData->numberSysVarsCount);

  // set the number of user number vars
  if (isData->fileVersion == 2)
    isData->numberUserVarsCount = tmp - 0xb;
  else
    isData->numberUserVarsCount = tmp - isData->numberSysVarsCount;


  // --------------------------------------------------------------------------
  // do user defined types
  isData->dataTypesCount = get2Byte(fd);
  isData->dataTypes = parseUserTypes(fd, isData->dataTypesCount);
  

  // --------------------------------------------------------------------------
  // process user function headers.
  isData->functionPrototypesCount= get2Byte(fd);
  isData->functionPrototypes = parseFunctions(fd, 
						   isData->functionPrototypesCount, 
						   isData->fileVersion);

  // --------------------------------------------------------------------------
  // process user function bodies.
  isData->functionBodiesCount= isData->functionPrototypesCount;
  isData->functionBodies= (FunctionBody**) mallocX(sizeof(FunctionBody*) * isData->functionBodiesCount);

  // --------------------------------------------------------------------------
  // the last 2 extra bytes (wonder what goes here?)
  get2Byte(fd);

  // --------------------------------------------------------------------------
  // make up initial memory for the codelines
  isData->codeLines = mallocX(sizeof(CodeLine*));
  isData->codeLinesMax = 1;
  isData->codeLinesCount = 0;


  // make up space for variable usage counters
  isData->stringUserVars = (int*) mallocX(sizeof(int) * isData->stringUserVarsCount);
  for(i=0; i < isData->stringUserVarsCount; i++)
    isData->stringUserVars[i] = 0;
  isData->numberUserVars = (int*) mallocX(sizeof(int) * isData->numberUserVarsCount);
  for(i=0; i < isData->numberUserVarsCount; i++)
    isData->numberUserVars[i] = 0;


  // --------------------------------------------------------------------------
  // work out the actual end of file - could be some crap there

  // move to EOF
  filePos = tell(fd);
  lseek(fd, -1, 2);
  
  // move back over the compiler version string at the EOF
  tmp = get1Byte(fd);
  lseek(fd, -(tmp+1), 1);

  // get the compiler version string
  isData->compilerVersion = (char*) mallocX(tmp+1);
  getString(fd, isData->compilerVersion, tmp);  

  // move back to start of compiler string
  lseek(fd, -tmp, 1);
  
  // move back to where we were
  isData->eofPos = tell(fd);
  lseek(fd, filePos, 0);
}




static FunctionPrototype** parseFunctions(int fd, 
					  int numFuncs, int fileVersion)
{
  int numParams;
  int number, nameLen;
  int i, j, tmp;
  FunctionPrototype** tmpFuncs;
  FunctionPrototype* tmpFunc;
  int type;
  char buffer[512];
  int paramCounts[10];
  int strParam, numParam;

  // setup memory
  tmpFuncs = (FunctionPrototype**) mallocX((sizeof(FunctionPrototype*) * numFuncs));

  // read all functions
  for(i=0; i < numFuncs ; i++)
  {
    // make & store structure
    tmpFunc = (FunctionPrototype*) mallocX(sizeof(FunctionPrototype));
    tmpFunc->functionBody = NULL;
    tmpFuncs[i] = tmpFunc;

    // perform appropriate processing depending on FN type
    switch(get2Byte(fd))
    {
    case FUNCTION_USER: // USER function

      // indicate it's a user function
      tmpFunc->type = FUNCTION_USER;

      // setup return type & skip some guff
      lseek(fd, 6, 1);
      tmpFunc->returnType = TYPE_LONG;

      // setup name
      sprintf(buffer, "function%i", i);
      tmpFunc->name = (char*) mallocX(strlen(buffer)+1);
      strcpy(tmpFunc->name, buffer);

      // get number of parameters
      tmpFunc->paramsCount = get2Byte(fd);

      // get memory for parameter list
      tmpFunc->params = (int*) mallocX(sizeof(int) * tmpFunc->paramsCount);

      // setup count of string & number params (and locals)
      tmpFunc->paramStringsCount =0;
      tmpFunc->paramNumbersCount =0;
      tmpFunc->localStringsCount =0;
      tmpFunc->localNumbersCount =0;

      // extract all the parameter types	
      for(j =0; j< tmpFunc->paramsCount; j++)
      {
	// get param type
	tmpFunc->params[j] = parseType(fd, get2Byte(fd));

	// is it BYREF?
	if (get2Byte(fd) & 2)
	  tmpFunc->params[j] |= 0x80000000;
	
	if (((tmpFunc->params[j]) & 0xffffff) == TYPE_STRING)
	  tmpFunc->paramStringsCount++;
	else
	  tmpFunc->paramNumbersCount++;
      }


      // get memory for parameter names
      tmpFunc->paramStringsNames = (char**) mallocX(sizeof(char*) * tmpFunc->paramStringsCount);
      tmpFunc->paramNumbersNames = (char**) mallocX(sizeof(char*) * tmpFunc->paramNumbersCount);

      // make up the names
      for(j=0; j< 8; j++)
	paramCounts[j] =0;
      strParam=0;
      numParam=0;
      for(j=0; j< tmpFunc->paramsCount; j++)
      {
	switch(tmpFunc->params[j] & 0xffffff)
	{
	case TYPE_STRING:
	  sprintf(buffer, "pString%i", paramCounts[0]++);
	  break;

	case TYPE_CHAR:
	  sprintf(buffer, "pChar%i", paramCounts[1]++);
	  break;

	case TYPE_LONG:
	  sprintf(buffer, "pLong%i", paramCounts[2]++);
	  break;

	case TYPE_INT:
	  sprintf(buffer, "pInt%i", paramCounts[3]++);
	  break;

	case TYPE_NUMBER:
	  sprintf(buffer, "pNumber%i", paramCounts[4]++);
	  break;

	case TYPE_LIST:
	  sprintf(buffer, "pList%i", paramCounts[5]++);
	  break;

	case TYPE_BOOL:
	  sprintf(buffer, "pBool%i", paramCounts[6]++);
	  break;

	case TYPE_HWND:
	  sprintf(buffer, "pHwnd%i", paramCounts[7]++);
	  break;
	  
	default:
	  error("Unknown parameter type\n");
	}

	// put the name in the correct list of parameters
	if ((tmpFunc->params[j] & 0xffffff) == TYPE_STRING)
	{	
	  tmpFunc->paramStringsNames[strParam] = (char*) mallocX(strlen(buffer) +1);
	  strcpy(tmpFunc->paramStringsNames[strParam], buffer);
	  strParam++;
	}
	else
	{	
	  tmpFunc->paramNumbersNames[numParam] = (char*) mallocX(strlen(buffer) +1);
	  strcpy(tmpFunc->paramNumbersNames[numParam], buffer);
	  numParam++;
	}

	// dunno which body this prototype should be paired with
	tmpFunc->functionBody = NULL;
      }

      break;


    case FUNCTION_DLL: // DLL IMPORT function

      // indicate the function type
      tmpFunc->type = FUNCTION_DLL;

      // get the function's return type
      tmpFunc->returnType = parseType(fd, get2Byte(fd));

      // get the function name
      getString(fd, buffer, get2Byte(fd));
      tmp = strlen(buffer);
      getString(fd, buffer + tmp + 1, get2Byte(fd));
      buffer[strlen(buffer)] = '.';
      tmpFunc->name = (char*) mallocX(strlen(buffer)+1);
      strcpy(tmpFunc->name, buffer);

      // get number of params (move on if ther aren't any)
      tmpFunc->paramsCount = get2Byte(fd);

      // make space for params
      tmpFunc->params = (int*) mallocX(sizeof(int) * tmpFunc->paramsCount);

      // setup count of string & number params (and locals, even though they
      // will not be used.. just for completeness)
      tmpFunc->paramStringsCount =0;
      tmpFunc->paramNumbersCount =0;
      tmpFunc->localStringsCount =0;
      tmpFunc->localNumbersCount =0;

      // extract all the parameter types	
      for(j =0; j< tmpFunc->paramsCount; j++)
      {
	// get param type
	tmpFunc->params[j] = parseType(fd, get2Byte(fd));

	// is it BYREF?
	if (get2Byte(fd) & 2)
	  tmpFunc->params[j] |= 0x80000000;
	
	if ((tmpFunc->params[j] & 0xffffff) == TYPE_STRING)
	  tmpFunc->paramStringsCount++;
	else
	  tmpFunc->paramNumbersCount++;
      }

      // dunno which body this prototype should be paired with
      tmpFunc->functionBody = NULL;
      break;

    default:
      error("Unknown function type at 0x%x\n", tell(fd));
    }
  }

  return(tmpFuncs);
}


static SysVar** parseSysVars(int fd, int numVars)
{
  int number, nameLen;
  int i, tmp;
  SysVar** tmpVars;
  SysVar* tmpVar;

  // setup memory
  tmpVars = (SysVar**) mallocX(sizeof(SysVar*) * numVars);

  // read all strings
  for(i=0; i < numVars; i++)
  {
    // make & store structure
    tmpVar = (SysVar*) mallocX(sizeof(SysVar));
    tmpVars[i] = tmpVar;

    // ignore string number in file
    get2Byte(fd);

    // get name
    nameLen = get2Byte(fd);
    tmpVar->name = (char*) mallocX(nameLen+1);
    getString(fd, tmpVar->name, nameLen);
  }

  return(tmpVars);
}


static DataType** parseUserTypes(int fd, int numTypes)
{
  int i, j;
  char buffer[256];
  int tmp, tmp2;
  DataType** tmpVars;
  DataType* tmpVar;
  TypeEntry** tmpEntries;
  TypeEntry* tmpEntry;

  // setup memory
  tmpVars = (DataType**) mallocX(sizeof(DataType*) * numTypes);


  for(i=0; i< numTypes; i++)
  {
    // setup memory for this type
    tmpVar = (DataType*) mallocX(sizeof(DataType));
    tmpVars[i] = tmpVar;

    // skip two bytes
    get2Byte(fd);

    // get name
    tmp = get2Byte(fd);
    tmpVar->name = (char*) mallocX(tmp + 1);
    getString(fd, tmpVar->name, tmp);
    
    // get number of members & setup memory
    tmpVar->entriesCount = get2Byte(fd);
    tmpEntries = (TypeEntry**) mallocX(sizeof(TypeEntry*) * tmpVar->entriesCount);
    tmpVar->entries = tmpEntries;

    // process them
    for(j=0; j < tmpVar->entriesCount; j++)
    {
      // setup memory for this entry
      tmpEntry = (TypeEntry*) mallocX(sizeof(TypeEntry));
      tmpEntries[j] = tmpEntry;

      // get the data type of entry
      tmpEntry->type = parseType(fd, get2Byte(fd));

      // get the size of it
      tmpEntry->size = get2Byte(fd);

      // get name of entry
      tmp = get2Byte(fd);
      tmpEntry->name = (char*) mallocX(tmp+1);
      getString(fd, tmpEntry->name, tmp);
    }
  }
  
  return(tmpVars);
}


static int parseType(int fd, int type)
{
  switch(type)
  {
  case 0:
    return(TYPE_STRING);

  case 4:
    return(TYPE_NUMBER);

  case 6:
    return(TYPE_BOOL);
     
  case 1:
    return(TYPE_CHAR);
    
  case 7:
    return(TYPE_HWND);

  case 3:
    return(TYPE_INT);
    
  case 5:
    return(TYPE_LIST);

  case 2:
    return(TYPE_LONG);

  }
  
  error("Unknown type (0x%x) at 0x%x\n", type, tell(fd));
}
