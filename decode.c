/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#include "common.h"
#include "decodeOps.h"
#include "decodetable.h"
#include "util.h"


static int labelCounter;
static int instructionCounter;


static void addCodeLine(ISData* isData, int inCounter, CodeLine* codeLine);
static int parseArg(int fd, Parameter* param, ISData* isData, int inCounter, int updateUsage);
static int parseOther(int fd, void* buffer);
static int parseComparison(int comparison);
static void decodePass0(int fd, ISData* isData);
static void decodePass1(ISData* isData);
static void decodePass2(ISData* isData);

void initDecode(void)
{
  int i;
  int pos;

  // get memory & wipe decodeTable
  decodeTable = (DecodeEntry**)  mallocX(sizeof(DecodeEntry*)*DECODETABLESIZE);
  for(i =0; i < DECODETABLESIZE; i++)
    decodeTable[i] = NULL;

  for(i=0; i< rawDecodeTableSize; i++)
  {
    // check op isn't out of range
    if ((pos = (int) rawDecodeTable[i][0]) > DECODETABLESIZE)
      error("\n!!!!Decode table entry out of range! (%i >= %i)\n", pos, DECODETABLESIZE);

    // check we don't have duplicates    
    if (decodeTable[pos] != NULL)
      error("Duplicate entries for opcode (0x%x) in decode table!\n", pos);

    // make new memspace for entry
    decodeTable[pos] = (DecodeEntry*) malloc(sizeof(DecodeEntry));

    // add in the useful stuff
    decodeTable[pos]->text = (char*) rawDecodeTable[i][1];
    decodeTable[pos]->info.numParams = (int) rawDecodeTable[i][2];
    decodeTable[pos]->decodeFunction0 = (decodeFunc) rawDecodeTable[i][3];
  }
  /*
  for(i=0; i< DECODETABLESIZE; i++)
  {
    if (decodeTable[i])
      continue;

    printf("%x\n", i);
  }
  exit(1);
  */
  // zero the label & instruction counters
  labelCounter=0;
  instructionCounter=0;
}


void decode(int fd, ISData* isData)
{
  // pass 0 - read the code in from the file
  decodePass0(fd, isData);

  // pass 1 - work out function prototype/body pairings & fix local variable counts
  decodePass1(isData);

  // pass 2 - fix local/parameter variable indicators
  decodePass2(isData);
}


// pass 1 - work out function prototype/body pairings, fix local variable counts,
static void decodePass1(ISData* isData)
{
  int i;
  int j;
  int fn;
  int k;

  // do the main body of the code
  for(i=0; i< isData->codeLinesCount; i++)
  {
    // ah ha! a function call. we can update
    if ((isData->codeLines[i]->type == FUNCTION) &&
	(isData->codeLines[i]->functionNumber >= 0) &&
	(isData->functionPrototypes[isData->codeLines[i]->functionNumber]->type == FUNCTION_USER))
    {
      // get the function number
      fn = isData->codeLines[i]->functionNumber;
      
      // skip if we've already done this one
      if (isData->functionPrototypes[fn]->functionBody != NULL)
	continue;
      
      // check we've not got an invalid function label (i.e. the label is
      // present in the main code, NOT a function
      if (isData->labels[isData->codeLines[i]->destLabel]->functionNumber == -1)
	error("Invalid function label\n");
	
      // update function body pointer
      isData->functionPrototypes[fn]->functionBody = 
	isData->functionBodies[isData->labels[isData->codeLines[i]->destLabel]->functionNumber];
      
      // update local string vars counts
      isData->functionPrototypes[fn]->localStringsCount =
	isData->functionPrototypes[fn]->functionBody->localStringsCount -
	isData->functionPrototypes[fn]->paramStringsCount;
      isData->functionPrototypes[fn]->localStrings = 
	isData->functionPrototypes[fn]->functionBody->localStrings + 
	(sizeof(int) * isData->functionPrototypes[fn]->paramStringsCount);
      
      // update local number vars counts
      isData->functionPrototypes[fn]->localNumbersCount =
	isData->functionPrototypes[fn]->functionBody->localNumbersCount -
	isData->functionPrototypes[fn]->paramNumbersCount;
      isData->functionPrototypes[fn]->localNumbers = 
	isData->functionPrototypes[fn]->functionBody->localNumbers + 
	(sizeof(int) * isData->functionPrototypes[fn]->paramNumbersCount);
    }
  }

  // do the functions
  for(i=0; i< isData->functionBodiesCount; i++)
  {
    for(j=0; j< isData->functionBodies[i]->codeLinesCount; j++)
    {
      // ah ha! a function call. we can update
      if ((isData->functionBodies[i]->codeLines[j]->type == FUNCTION) &&
	  (isData->functionBodies[i]->codeLines[j]->functionNumber >= 0) &&
	  (isData->functionPrototypes[isData->functionBodies[i]->codeLines[j]->functionNumber]->type == FUNCTION_USER))
      {
	// get the function number
	fn = isData->functionBodies[i]->codeLines[j]->functionNumber;

	// skip if we've already done this one	
	if (isData->functionPrototypes[fn]->functionBody != NULL)
	  continue;

	// update function body pointer
	isData->functionPrototypes[fn]->functionBody = 
	  isData->functionBodies[isData->labels[isData->functionBodies[i]->codeLines[j]->destLabel]->functionNumber];
	
	// update local string vars counts
	isData->functionPrototypes[fn]->localStringsCount =
	  isData->functionPrototypes[fn]->functionBody->localStringsCount -
	  isData->functionPrototypes[fn]->paramStringsCount;
	isData->functionPrototypes[fn]->localStrings = 
	  isData->functionPrototypes[fn]->functionBody->localStrings + 
	  (sizeof(int) * isData->functionPrototypes[fn]->paramStringsCount);

	// update local number vars counts
	isData->functionPrototypes[fn]->localNumbersCount =
	  isData->functionPrototypes[fn]->functionBody->localNumbersCount -
	  isData->functionPrototypes[fn]->paramNumbersCount;
	isData->functionPrototypes[fn]->localNumbers = 
	  isData->functionPrototypes[fn]->functionBody->localNumbers + 
	  (sizeof(int) * isData->functionPrototypes[fn]->paramNumbersCount);
      }
    }
  }
}


// pass 2 - fix local/parameter variable indicators
static void decodePass2(ISData* isData)
{
  int i;
  int j;
  int k;

  for(i=0; i< isData->functionPrototypesCount; i++)
  {
    // function not actually used
    if (isData->functionPrototypes[i]->functionBody == NULL)
      continue;

    for(j=0; j< isData->functionPrototypes[i]->functionBody->codeLinesCount; j++)
    {      
      for(k=0; k< isData->functionPrototypes[i]->functionBody->codeLines[j]->paramsCount; k++)
      {
	//	printf("\t\tP%i\n", isData->functionPrototypes[i]->functionBody->codeLines[j]->paramsCount);

	if (isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->type == PARAM_FNLOCALSTRINGVARIABLE)
	{
	  // this is actually a parameter variable
	  if (isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->data.variableNumber <
	      isData->functionPrototypes[i]->paramStringsCount)
	  {
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->type = PARAM_FNPARAMSTRINGVARIABLE;
	  }
	  
	  // actually a local variable
	  else
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->data.variableNumber -=
	      isData->functionPrototypes[i]->paramStringsCount;
	}
	  
	if (isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->type == PARAM_FNLOCALNUMBERVARIABLE)
	{
	  // this is actually a parameter variable
	  if (isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->data.variableNumber <
	      isData->functionPrototypes[i]->paramNumbersCount)
	  {
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->type = PARAM_FNPARAMNUMBERVARIABLE;
	  }
	  
	  // actually a local variable
	  else
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->params[k]->data.variableNumber -=
	      isData->functionPrototypes[i]->paramNumbersCount;
	}
      }

      // fix the destination
      if (isData->functionPrototypes[i]->functionBody->codeLines[j]->destination != NULL)
      {
	if (isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->type == PARAM_FNLOCALSTRINGVARIABLE)
	{
	  // this is actually a parameter variable
	  if (isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->data.variableNumber <
	      isData->functionPrototypes[i]->paramStringsCount)
	  {
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->type = PARAM_FNPARAMSTRINGVARIABLE;
	  }
	  
	  // actually a local variable
	  else
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->data.variableNumber -=
	      isData->functionPrototypes[i]->paramStringsCount;
	}
	  
	if (isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->type == PARAM_FNLOCALNUMBERVARIABLE)
	{
	  // this is actually a parameter variable
	  if (isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->data.variableNumber <
	      isData->functionPrototypes[i]->paramNumbersCount)
	  {
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->type = PARAM_FNPARAMNUMBERVARIABLE;
	  }
	  
	  // actually a local variable
	  else
	    isData->functionPrototypes[i]->functionBody->codeLines[j]->destination->data.variableNumber -=
	      isData->functionPrototypes[i]->paramNumbersCount;
	}
      }
    } 
  }
}




// pass 0 => decode stuff from the file
void decodePass0(int fd, ISData* isData)
{
  int tmp;
  char buffer[256];
  int opcode;
  int i;
  int expectFunctionStart;
  int curFunction;
  int pos;


  // decode the main code section
  get2Byte(fd);
  while(1)
  {
    // need to check this as well, 'cos you don't always have an _EWQ section
    // this needs to be checked FIRST, otherwise we'll interfere with opcode
    // reading
    if (tell(fd) >= isData->eofPos)
    {
      isData->functionBodiesCount = 0;
      return;
    }

    // get opcode
    opcode = get2Byte(fd);

    // check if the opcode is the start of the (debug?) area (i.e. _EWQ, or
    // 0x455f
    if (opcode == 0x455f)
    {
      lseek(fd, -2, 1);
      isData->functionBodiesCount = 0;
      return;
    }

    // check opcode in range & in table
    if ((opcode > DECODETABLESIZE) || (opcode < 0))
      error("\n!!!!Opcode out of range(0x%x) at (0x%x)\n", opcode, tell(fd));
    if (decodeTable[opcode] == NULL)
      error("\n!!!!Unknown opcode (0x%x) at (0x%x)\n", opcode, tell(fd));

    // if it's a LABEL, then skip forward over the labels to the next opcode
    if (opcode == 0)
    {
      pos = tell(fd);
    
      // skip crap
      get2Byte(fd);
      
      // keep reading until we get something other than a label
      while((i = get2Byte(fd)) == 0)
	get2Byte(fd);

      // if it's a function start, then we've hit the end of the main code
      if (i == 0xb6)
      {
	lseek(fd, pos-2, 0);
	break;
      }

      // otherwise, move back to where we were
      lseek(fd, pos, 0);
    }

    // if we get a function start in the main code => end of main code
    if (opcode == 0xb6)
    {
      lseek(fd, -2, 1);
      break;
    }

    //    printf("%x -- %x\n", tell(fd), opcode);
    

    // if we get a function end in the main code => summat's up
    if (opcode == 0xb8)
      error("Got unexpected function end in main code\n");
    
    // run the function if there is one
    if (decodeTable[opcode]->decodeFunction0 != NULL)
      (*decodeTable[opcode]->decodeFunction0)(fd, opcode, -1, isData);

    //    printf("%i\n", isData->codeLines[0]->type);
  }


  // decode the functions/ subroutines
  curFunction = 0;
  while(1)
  {
    // need to check this as well, 'cos you don't always have an _EWQ section
    // this needs to be checked FIRST, otherwise we'll interfere with opcode
    // reading
    if (tell(fd) >= isData->eofPos)
    {
      isData->functionBodiesCount = curFunction;
      return;
    }

    // check for EOF (read next opcode, move back & check it's not 0x455f)
    opcode = get2Byte(fd);
    lseek(fd, -2, 1);
    if (opcode == 0x455f)
    {
      isData->functionBodiesCount = curFunction;
      return;
    }

    // check we've not got too many functions
    if (curFunction == isData->functionBodiesCount)
      error("Too many functions found!\n");

    // make up space for the codelines in this function
    isData->functionBodies[curFunction] = (FunctionBody*) mallocX(sizeof(FunctionBody));
    isData->functionBodies[curFunction]->codeLines = (CodeLine**) mallocX(sizeof(CodeLine*));
    isData->functionBodies[curFunction]->codeLinesCount = 0;
    isData->functionBodies[curFunction]->codeLinesMax = 1;

    // zero instruction counter, so label offsets will be relative to the
    // start of the function they are in
    instructionCounter=0;

    //    printf("------------\n");	

    // right, process the code for real now
    expectFunctionStart=1;
    while(1)
    {
      // need to check this as well, 'cos you don't always have an _EWQ section
      // this needs to be checked FIRST, otherwise we'll interfere with opcode
      // reading
      if (tell(fd) >= isData->eofPos)
      {
	isData->functionBodiesCount = curFunction;
	return;
      }

      // get opcode
      opcode = get2Byte(fd);

      // check if the opcode is the start of the (debug?) area (i.e. "_EWQ", or
      // 0x455f	
      if (opcode == 0x455f)
      {
	lseek(fd, -2, 1);
	isData->functionBodiesCount = curFunction;
	return;
      }

      // if it's a LABEL, then skip forward over the labels to the next opcode
      if (opcode == 0)
      {
	pos = tell(fd);
    
	// skip crap
	get2Byte(fd);
      
	// keep reading until we get something other than a label
	while((i = get2Byte(fd)) == 0)
	  get2Byte(fd);

	// if it's a function start, then we've hit the end of the main code
	if ((i == 0xb6) && (!expectFunctionStart))
	{
	  lseek(fd, pos-2, 0);
	  break;
	}
	
	// otherwise, move back to where we were
	lseek(fd, pos, 0);
      }

      // hit a function start here? => end of function
      if ((opcode == 0xb6) && (!expectFunctionStart))
      {
	lseek(fd, -2, 1);
	break;
      }

      // hit a function start here? => ignore it
      if ((opcode == 0xb6) && expectFunctionStart)
	expectFunctionStart=0;
      
      //      printf("%x -- %x\n", tell(fd), opcode);

      // check opcode in range & in table
      if ((opcode > DECODETABLESIZE) || (opcode < 0))
	error("\n!!!!Opcode out of range(0x%x) at (0x%x)\n", opcode, tell(fd));
      if (decodeTable[opcode] == NULL)
	error("\n!!!!Unknown opcode (0x%x) at (0x%x)\n", opcode, tell(fd));

      // run the function if there is one
      if (decodeTable[opcode]->decodeFunction0 != NULL)
	(*decodeTable[opcode]->decodeFunction0)(fd, opcode, curFunction, isData);
    }

    // keep function indicator up to date
    curFunction++;
  }
} 


// ----------------------------------------------------------------------------
// function parsing for system functions
static void parseSystemFunction(int fd, int opcode, int inCounter, ISData* isData)
{
  int count;
  char buffer[256];
  int argType;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->destination = NULL;
  codeLine->functionNumber = -1;

  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text)+1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // setup it's parameters
  codeLine->paramsCount = decodeTable[opcode]->info.numParams;
  codeLine->params = (Parameter**) mallocX(decodeTable[opcode]->info.numParams * sizeof(Parameter*));

  // go through each parameter
  for(count =0; count < decodeTable[opcode]->info.numParams; count++)
  {
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == PARAM_UNKNOWN)
      error("Unknown function arg at (0x%x)\n", tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// function parsing for system structure functions
static void parseStructFunction(int fd, int opcode, int inCounter, ISData* isData)
{
  int count;
  char buffer[256];
  int argType;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->destination = NULL;
  codeLine->functionNumber = -1;

  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text)+1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // setup it's parameters
  codeLine->paramsCount = decodeTable[opcode]->info.numParams;
  codeLine->params = (Parameter**) mallocX(decodeTable[opcode]->info.numParams * sizeof(Parameter*));

  // go through each parameter
  for(count =0; count < decodeTable[opcode]->info.numParams; count++)
  {
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == PARAM_UNKNOWN)
      error("Unknown function arg at (0x%x)\n", tell(fd));
  }

  if (codeLine->paramsCount == 5)
  {
    // check we got a longconst as parameter 2	
    if (codeLine->params[1]->type != PARAM_LONGCONST)
      error("\n!!!!Unexpected type at (0x%x) in parseStructFunction0\n",
	    tell(fd));

    // OK, parameter 2's REAL type is a PARAM_DATATYPENUM 
    // (i.e. the number of the data structure concerned)
    codeLine->params[1]->type = PARAM_DATATYPENUM;
  }
  else
  {
    // check we got a longconst as parameter 1
    if (codeLine->params[0]->type != PARAM_LONGCONST)
      error("\n!!!!Unexpected type at (0x%x) in parseStructFunction0\n",
	    tell(fd));

    // OK, parameter 2's REAL type is a PARAM_DATATYPENUM 
    // (i.e. the number of the data structure concerned)
    codeLine->params[0]->type = PARAM_DATATYPENUM;
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}


// ----------------------------------------------------------------------------
// function parsing for system functions
static void doComponentTotalSize(int fd, int opcode, int inCounter, ISData* isData)
{
  int count;
  char buffer[256];
  int argType;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->functionNumber = -1;
  codeLine->destination = NULL;
  
  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text)+1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // setup it's parameters
  codeLine->paramsCount = 2;
  codeLine->params = (Parameter**) mallocX(4 * sizeof(Parameter*));

  // do first 2 parameters
  for(count =0; count < 2; count++)
  {
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == PARAM_UNKNOWN)
      error("Unknown function arg at (0x%x)\n", tell(fd));
  }

  // if param 2 is a string, then there are another 2 params in there
  switch(codeLine->params[1]->type)
  {
  case PARAM_STRINGCONST:
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
    codeLine->paramsCount = 4;
    for(count =2; count < 4; count++)
    {
      codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
      if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == PARAM_UNKNOWN)
	error("Unknown function arg at (0x%x)\n", tell(fd));
    }
    break;
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// function parsing for system functions
static void doComponentListItems(int fd, int opcode, int inCounter, ISData* isData)
{
  int count;
  char buffer[256];
  int argType;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->functionNumber = -1;
  codeLine->destination = NULL;

  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text)+1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // setup it's parameters
  codeLine->paramsCount = 2;
  codeLine->params = (Parameter**) mallocX(3 * sizeof(Parameter*));

  // do first 2 parameters
  for(count =0; count < 2; count++)
  {
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == PARAM_UNKNOWN)
      error("Unknown function arg at (0x%x)\n", tell(fd));
  }

  // if file version = 5, there's an extra list parameter
  if (isData->fileVersion == 5)
  {
    codeLine->params[2] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[2], isData, inCounter, 1) == PARAM_UNKNOWN)
      error("Unknown function arg at (0x%x)\n", tell(fd));
    codeLine->paramsCount = 3;    
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// function parsing for user functions
static void parseUserFunction(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  int count;
  int pos;
  int fnNumber;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->destination = NULL;

  // get function number
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_USERFUNCTION)
    error("\n!!!!Unexpected type at (0x%x) in parseUserFunction0\n",
	  tell(fd));
  fnNumber = *((int*) buffer);
  codeLine->functionNumber = fnNumber;

  // just check it's in range
  if (fnNumber > isData->functionPrototypesCount)
    error("Out of range function encountered\n");

  // make up it's text name
  codeLine->name = (char*) mallocX(strlen(isData->functionPrototypes[fnNumber]->name)+1);
  strcpy(codeLine->name, isData->functionPrototypes[fnNumber]->name);

  // get label number of function
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_LABEL)
    error("\n!!!!Unexpected type at (0x%x) in parseUserFunction0\n",
	  tell(fd));
  codeLine->destLabel = *((int*) buffer) -1;

  // setup its parameters
  codeLine->paramsCount = isData->functionPrototypes[fnNumber]->paramsCount;
  codeLine->params = (Parameter**) mallocX(codeLine->paramsCount * sizeof(Parameter*));

  // go through each param
  for(count =0; count < codeLine->paramsCount; count++)
  {
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == PARAM_UNKNOWN)
      error("Unknown variable type at (0x%x)\n", tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// function parsing for DLL functions
static void parseDLLFunction(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  int count;
  int pos;
  int fnNumber;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));


  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->destination = NULL;

  // get function number
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_USERFUNCTION)
    error("\n!!!!Unexpected type at (0x%x) in parseUserFunction0\n",
	  tell(fd));
  fnNumber = *((int*) buffer);
  codeLine->functionNumber = fnNumber;

  // just check it's in range
  if (fnNumber > isData->functionPrototypesCount)
    error("Out of range function encountered\n");
  
  // make up it's text name
  codeLine->name = (char*) mallocX(strlen(isData->functionPrototypes[fnNumber]->name)+1);
  strcpy(codeLine->name, isData->functionPrototypes[fnNumber]->name);

  // setup its parameters
  codeLine->paramsCount = isData->functionPrototypes[fnNumber]->paramsCount;
  codeLine->params = (Parameter**) mallocX(codeLine->paramsCount * sizeof(Parameter*));

  // go through each param
  for(count =0; count < codeLine->paramsCount; count++)
  {
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    if (parseArg(fd, codeLine->params[count], isData, inCounter, 1) == OTHER_UNKNOWN)
      error("Unknown variable type at (0x%x)\n", tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}

  

// ----------------------------------------------------------------------------
// function for parsing labels
static void label(int fd, int opcode, int inCounter, ISData* isData)
{
  int tmp;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = LABEL;
  codeLine->opcode = opcode;
  codeLine->labelNumber = labelCounter;
  codeLine->paramsCount =0;
  codeLine->destination = NULL;

  // record where this label is (relative to the main code, or the
  // current function start, since instructionCounter is reset at the
  // start of each new function
  isData->labels[labelCounter]->position = instructionCounter;
  isData->labels[labelCounter]->functionNumber = inCounter;

  // just dump the no. of instructions to next label
  get2Byte(fd);

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;

  // move onto the next label
  labelCounter++;
}


// ----------------------------------------------------------------------------
// function for equating things
static void equate(int fd, int opcode, int inCounter, ISData* isData)
{
  char buffer[512];
  int argType;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = OPERATION;
  codeLine->opcode = opcode;
  codeLine->operationNumber = OP_EQUATE;
  codeLine->paramsCount = 1;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*));
  strcpy(codeLine->string, "=");

  //  printf("XX%i\n", isData->codeLines[0]->type);

  // get the destination
  codeLine->destination = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->destination, isData, inCounter, 1))
  {
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
  case PARAM_FNPARAMSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in strCopy\n", 
	  tell(fd));
  }


  //  printf("YY%i\n", isData->codeLines[0]->type);


  // get the source
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter));
  switch(argType = parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
  case PARAM_FNPARAMSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
  case PARAM_STRINGCONST:
  case PARAM_LONGCONST:
    break;

  default:
    printf("%i\n", argType);
    error("\n!!!!Unexpected variable type at (0x%x) in strCopy\n", 
	  tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}


// ----------------------------------------------------------------------------
// function for an if statement
static void ifStmt(int fd, int opcode, int inCounter, ISData* isData)
{
  int comp;
  int label;
  int argType;
  char buffer[512];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = IFSTATEMENT;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 2;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*) * 2);
  codeLine->destination = NULL;

  // get jump destination
  if (parseOther(fd, (void*) buffer) != OTHER_LABEL)
    error("\n!!!!Unexpected variable type at (0x%x) in ifStmt\n", 
	  tell(fd));

  // removed -1 since label0 can exist
  codeLine->destLabel = *((int*) buffer);

  // update label's usage count
  // removed -1 since label0 can exist
  isData->labels[*((int*) buffer)]->usage++;

  // parse comparison
  if ((codeLine->comparisonType = parseComparison(get1Byte(fd))) == OP_UNKNOWN)
    error("\n!!!!Unknown comparison type at (0x%x) in compare\n", tell(fd));

  // get first operand 
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in ifStmt\n", 
	  tell(fd));
  }

  // get second operand 
  codeLine->params[1] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[1], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in ifStmt\n", 
	  tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}
  


// ----------------------------------------------------------------------------
// function for a comparison
static void comparison(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));
  Parameter tmpParam;

  // setup start of structure
  codeLine->type = OPERATION;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 2;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*) * 2);


  // get the destination
  codeLine->destination = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->destination, isData, inCounter, 1))
  {
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in strCopy\n", 
	  tell(fd));
  }


  // get first operand 
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  // get the comparison type
  if (parseArg(fd, &tmpParam, isData, inCounter, 0) != PARAM_LONGCONST)
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", tell(fd));
  if ((codeLine->operationNumber = parseComparison(tmpParam.data.intVal)) == OP_UNKNOWN)
    error("\n!!!!Unknown comparison type at (0x%x) in compare\n", tell(fd));
  strcpy(codeLine->string, operations[codeLine->operationNumber]);

  // get second operand 
  codeLine->params[1] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[1], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// function for a generic operation decode
static void decodeOp(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));


  // setup start of structure
  codeLine->type = OPERATION;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 2;
  codeLine->operationNumber = decodeTable[opcode]->info.operationNumber;
  strcpy(codeLine->string, operations[decodeTable[opcode]->info.operationNumber]);
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*) * 2);

  // get the destination
  codeLine->destination = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->destination, isData, inCounter, 1))
  {
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
  case PARAM_FNPARAMSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in strCopy\n", 
	  tell(fd));
  }

  // get first operand 
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_STRINGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
  case PARAM_FNPARAMSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }


  // get second operand 
  codeLine->params[1] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[1], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_STRINGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
  case PARAM_FNPARAMSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }


  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}


// ----------------------------------------------------------------------------
// function for a single variable operation
static void singleVarOp(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = OPERATION;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 2;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*)*2);
  codeLine->operationNumber = decodeTable[opcode]->info.operationNumber;
  strcpy(codeLine->string, operations[decodeTable[opcode]->info.operationNumber]);


  // get the destination
  codeLine->destination = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->destination, isData, inCounter, 1))
  {
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in strCopy\n", 
	  tell(fd));
  }

  // copy destination into 1st param
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter)); 
  memcpy(codeLine->params[0], codeLine->destination, sizeof(Parameter));

  // get operand 
  codeLine->params[1] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[1], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}


// ----------------------------------------------------------------------------
// function for a NOT operation
static void notOp(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = OPERATION;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 1;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*));
  codeLine->operationNumber = decodeTable[opcode]->info.operationNumber;
  strcpy(codeLine->string, operations[decodeTable[opcode]->info.operationNumber]);


  // get the destination
  codeLine->destination = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->destination, isData, inCounter, 1))
  {
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in strCopy\n", 
	  tell(fd));
  }


  // ignore this. it is a copy of the other operand 
  // (it seems to put this in, even though BIT NOT takes only one operand!)
  parseArg(fd, (void*) buffer, isData, inCounter, 0);


  // get operand 
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter)); 
  switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// function for the AskOptions(...) dialogue
static void askOptions(int fd, int opcode, int inCounter, ISData* isData)
{
  int numPairs;
  char buffer[512];
  int argType;
  int count;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->functionNumber = -1;
  codeLine->destination = NULL;

  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text)+1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // get the number of pairs
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_NUMPARAMS)
    error("\n!!!!Unexpected variable type at (0x%x) in askOptions\n", 
	  tell(fd));
  numPairs = *((int*) buffer);
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*) * (2 + (numPairs * 2)));
  codeLine->paramsCount = 2 + (numPairs * 2);

  // first two args definitely there
  // get first operand 
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter)); 
  switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  // get second operand  (string)
  codeLine->params[1] = (Parameter*) mallocX(sizeof(Parameter)); 
  switch(parseArg(fd, codeLine->params[1], isData, inCounter, 1))
  {
  case PARAM_STRINGCONST:
  case PARAM_SYSTEMSTRINGVARIABLE:
  case PARAM_USERSTRINGVARIABLE:
  case PARAM_FNPARAMSTRINGVARIABLE:
  case PARAM_FNLOCALSTRINGVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  // extract (variable number of) args
  for(count =0; count < numPairs; count++)
  {
    // get first operand (string)
    codeLine->params[2+(count*2)] = (Parameter*) mallocX(sizeof(Parameter)); 
    switch(parseArg(fd, codeLine->params[2 + (count*2)], isData, inCounter, 1))
    {
    case PARAM_STRINGCONST:
    case PARAM_SYSTEMSTRINGVARIABLE:
    case PARAM_USERSTRINGVARIABLE:
    case PARAM_FNPARAMSTRINGVARIABLE:
    case PARAM_FNLOCALSTRINGVARIABLE:
      break;
      
    default:
      error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	    tell(fd));
    }

    // get second operand (number)	
    codeLine->params[2+1+(count*2)] = (Parameter*) mallocX(sizeof(Parameter)); 
    switch(parseArg(fd, codeLine->params[2 + 1 +(count*2)], isData, inCounter, 1))
    {
    case PARAM_LONGCONST:
    case PARAM_SYSTEMNUMBERVARIABLE:
    case PARAM_USERNUMBERVARIABLE:
    case PARAM_FNPARAMNUMBERVARIABLE:
    case PARAM_FNLOCALNUMBERVARIABLE:
      break;
      
    default:
      error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	    tell(fd));
    }
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}    



// ----------------------------------------------------------------------------
// function for goto
static void doGoto(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  CodeLine* codeLine;
  
  codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = GOTO;
  codeLine->opcode = opcode;
  codeLine->paramsCount =0;
  codeLine->destination = NULL;

  // get the destination
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_LABEL)
    error("\n!!!!Unexpected variable type at (0x%x) in askOptions\n", 
	  tell(fd));
  // removed -1 since label0 can exist
  codeLine->destLabel = *((int*) buffer);

  // update it's usage count
  isData->labels[*((int*) buffer)]->usage++;

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



// ----------------------------------------------------------------------------
// a function start
static void functionStart(int fd, int opcode, int inCounter, ISData* isData)
{
  int i;
  int curStrParam =0;
  int curNumParam =0;
  int numLocalStr =0;
  int numLocalNum =0;

  // check we're expecting a function
  if (inCounter < 0)
    error("Got unexpected function start at (0x%x)\n", tell(fd));

  // OK, done the function, now do it's variable declarations

  // this tells us the length of the function parameter/variable
  // definition. We can just skip it since we need to parse this anyway
  get2Byte(fd);

  // this tells us the number of local strings
  numLocalStr = get2Byte(fd);

  // skip the string flags
  lseek(fd, numLocalStr*2, 1);

  // get number of local strings (incl params)
  isData->functionBodies[inCounter]->localStringsCount = numLocalStr;
  isData->functionBodies[inCounter]->localStrings = (int*) mallocX(sizeof(int) * numLocalStr);
  for(i=0; i < numLocalStr; i++)
    isData->functionBodies[inCounter]->localStrings[i] = 0;

  // skip this
  get2Byte(fd);
  
  // get number of local numbers (incl params)
  numLocalNum = get2Byte(fd);
  isData->functionBodies[inCounter]->localNumbersCount = numLocalNum;
  isData->functionBodies[inCounter]->localNumbers = (int*) mallocX(sizeof(int) * numLocalNum);
  for(i=0; i < numLocalNum; i++)
    isData->functionBodies[inCounter]->localNumbers[i] = 0;

  // dunno which prototype matches this function yet
  isData->functionBodies[inCounter]->prototype = -1;

  // skip this
  get2Byte(fd);

  // we don't add this in as a line of code, since it isn't necessary
}


// ----------------------------------------------------------------------------
// a function end
static void functionEnd(int fd, int opcode, int inCounter, ISData* isData)
{


  // I used not to do anything here, until someone pointed out that
  // this can occur in the middle of a function, necessitating this
  // extra bit of code to slap a RETURN in.
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));
  
  // setup start of structure
  codeLine->type = RETURN;
  codeLine->opcode = opcode;
  codeLine->paramsCount =0;
  codeLine->destination = NULL;

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);
  
  // next instruction
  instructionCounter++;
}


// ----------------------------------------------------------------------------
// a function return function
static void funcReturn(int fd, int opcode, int inCounter, ISData* isData)
{
  int argType;
  char buffer[512];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCRETURN;
  codeLine->opcode = opcode;
  codeLine->destination = NULL;

  // not sure what this does. seems to be always 0xb7
  if ((opcode != 0xb7) && (get2Byte(fd) != 0xb7))
  {
    codeLine->paramsCount =0;
    lseek(fd, -2, 1);
  }

  else
  {
    codeLine->paramsCount =1;
    codeLine->params = (Parameter**) mallocX(sizeof(Parameter*));
    codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter)); 
    switch(argType = parseArg(fd, codeLine->params[0], isData, inCounter, 1))
    {
    case PARAM_LONGCONST:
    case PARAM_SYSTEMNUMBERVARIABLE:
    case PARAM_USERNUMBERVARIABLE:
    case PARAM_FNPARAMNUMBERVARIABLE:
    case PARAM_FNLOCALNUMBERVARIABLE:
      break;
      
    default:
      error("\n!!!!Unexpected variable type at (0x%x) in bitOp\n", 
	    tell(fd));
    }
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;  
}


static void doExit(int fd, int opcode, int inCounter, ISData* isData)
{
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = EXIT;
  codeLine->opcode = opcode;
  codeLine->paramsCount =0;
  codeLine->destination = NULL;

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}


static void doAbort(int fd, int opcode, int inCounter, ISData* isData)
{
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = ABORT;
  codeLine->opcode = opcode;
  codeLine->paramsCount =0;
  codeLine->destination = NULL;

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}


static void doHandler(int fd, int opcode, int inCounter, ISData* isData)
{
  char buffer[256];
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = HANDLER;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 2;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*)*2);
  codeLine->destination = NULL;

  // get first operand 
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
  case PARAM_SYSTEMNUMBERVARIABLE:
  case PARAM_USERNUMBERVARIABLE:
  case PARAM_FNPARAMNUMBERVARIABLE:
  case PARAM_FNLOCALNUMBERVARIABLE:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }


  // get first operand 
  codeLine->params[1] = (Parameter*) mallocX(sizeof(Parameter));
  switch(parseArg(fd, codeLine->params[1], isData, inCounter, 1))
  {
  case PARAM_LONGCONST:
    break;

  default:
    error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	  tell(fd));
  }

  /*
    NOT ANYMORE... Apparently label 0 CAN exist in the file
  // my labels start at 0
  (codeLine->params[1]->data.intVal--);
  */

  // GRAAAA. If you pass -1 to Handler(), it wipes the current
  // handler. Need to check for this, and not try and update
  // the labelcount ('cos it'll crash)
  if (codeLine->params[1]->data.intVal != -1)
  {
    // update label's usage count
    isData->labels[codeLine->params[1]->data.intVal]->usage++;
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



static void doCall(int fd, int opcode, int inCounter, ISData* isData)
{
  char buffer[256];
  int argType;
  int pos;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = CALL;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 1;
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*));
  codeLine->params[0] = (Parameter*) mallocX(sizeof(Parameter));
  codeLine->destination = NULL;
    
  // can have a label or an int here
  pos = tell(fd);
  if (parseOther(fd, (void*) buffer) == OTHER_LABEL)
  {
    codeLine->params[0]->type = PARAM_LONGCONST;
    codeLine->params[0]->data.intVal = *((int*) buffer);
  }
  else
  {
    lseek(fd, pos, 0);

    // get destination
    switch(parseArg(fd, codeLine->params[0], isData, inCounter, 1))
    {
    case PARAM_LONGCONST:
      break;
      
    default:
      error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	    tell(fd));
    }
  }

  /*
    NOT ANYMORE.. APPARENTLY label0 CAN exist in the file
  // my labels start at 0
  (codeLine->params[0]->data.intVal)--;
  */

  // update it's usage count
  isData->labels[codeLine->params[0]->data.intVal]->usage++;

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}



static void doReturn(int fd, int opcode, int inCounter, ISData* isData)
{
  char buffer[256];
  int argType;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = RETURN;
  codeLine->opcode = opcode;
  codeLine->paramsCount = 0;
  codeLine->destination = NULL;

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}

  

static void sPrintf(int fd, int opcode, int inCounter, ISData* isData)
{
  int numParams;
  char buffer[512];
  int argType;
  int count;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->functionNumber = -1;
  codeLine->destination = NULL;

  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text) + 1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // get the number of params
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_NUMPARAMS)
    error("\n!!!!Unexpected variable type at (0x%x) in askOptions\n", 
	  tell(fd));
  numParams = *((int*) buffer);
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*) * numParams);
  codeLine->paramsCount = numParams;

  // extract (variable number of) args
  for(count =0; count < numParams; count++)
  {
    // get operand
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    switch(parseArg(fd, codeLine->params[count], isData, inCounter, 1))
    {
    case PARAM_STRINGCONST:
    case PARAM_SYSTEMSTRINGVARIABLE:
    case PARAM_USERSTRINGVARIABLE:
    case PARAM_LONGCONST:
    case PARAM_SYSTEMNUMBERVARIABLE:
    case PARAM_USERNUMBERVARIABLE:
    case PARAM_FNPARAMNUMBERVARIABLE:
    case PARAM_FNLOCALNUMBERVARIABLE:
    case PARAM_FNPARAMSTRINGVARIABLE:
    case PARAM_FNLOCALSTRINGVARIABLE:
      break;
      
    default:
      error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	    tell(fd));
    }
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}




static void sPrintfBox(int fd, int opcode, int inCounter, ISData* isData)
{
  int numParams;
  char buffer[512];
  int argType;
  int count;
  CodeLine* codeLine = (CodeLine*) mallocX(sizeof(CodeLine));

  // setup start of structure
  codeLine->type = FUNCTION;
  codeLine->opcode = opcode;
  codeLine->functionNumber = -1;
  codeLine->destination = NULL;

  // copy function's name into it
  codeLine->name = (char*) mallocX(strlen(decodeTable[opcode]->text)+1);
  strcpy(codeLine->name, decodeTable[opcode]->text);

  // get the number of params
  if ((argType = parseOther(fd, (void*) buffer)) != OTHER_NUMPARAMS)
    error("\n!!!!Unexpected variable type at (0x%x) in askOptions\n", 
	  tell(fd));
  numParams = *((int*) buffer);
  codeLine->params = (Parameter**) mallocX(sizeof(Parameter*) * numParams);
  codeLine->paramsCount = numParams;

  // extract (variable number of) args
  for(count =0; count < numParams; count++)
  {
    // get operand
    codeLine->params[count] = (Parameter*) mallocX(sizeof(Parameter));
    switch(parseArg(fd, codeLine->params[count], isData, inCounter, 1))
    {
    case PARAM_STRINGCONST:
    case PARAM_SYSTEMSTRINGVARIABLE:
    case PARAM_USERSTRINGVARIABLE:
    case PARAM_LONGCONST:
    case PARAM_SYSTEMNUMBERVARIABLE:
    case PARAM_USERNUMBERVARIABLE:
    case PARAM_FNPARAMNUMBERVARIABLE:
    case PARAM_FNLOCALNUMBERVARIABLE:
    case PARAM_FNPARAMSTRINGVARIABLE:
    case PARAM_FNLOCALSTRINGVARIABLE:
      break;
      
    default:
      error("\n!!!!Unexpected variable type at (0x%x) in compare\n", 
	    tell(fd));
    }
  }

  // add it into the appropriate list of codelines
  addCodeLine(isData, inCounter, codeLine);

  // next instruction
  instructionCounter++;
}





// ----------------------------------------------------------------------------
// copy a line of code from the file into the correct place in the structur
static void addCodeLine(ISData* isData, int inCounter, CodeLine* codeLine)
{
  CodeLine** tmpCodeLines;

  // add into main code
  if (inCounter == -1)
  {
    if (isData->codeLinesCount >= isData->codeLinesMax)
    {
      tmpCodeLines = (CodeLine**) malloc(sizeof(CodeLine*) * isData->codeLinesMax*2);
      memcpy(tmpCodeLines, isData->codeLines, sizeof(CodeLine*) * isData->codeLinesMax);
      free(isData->codeLines);
      isData->codeLines = tmpCodeLines;
      isData->codeLinesMax *=2;
    }
    
    isData->codeLines[isData->codeLinesCount++] = codeLine;
  }

  // add into a function
  if (inCounter >= 0)
  {
    if (isData->functionBodies[inCounter]->codeLinesCount >= isData->functionBodies[inCounter]->codeLinesMax)
    {
      tmpCodeLines =(CodeLine**) malloc(sizeof(CodeLine*) * isData->functionBodies[inCounter]->codeLinesMax*2);
      memcpy(tmpCodeLines, 
	     isData->functionBodies[inCounter]->codeLines, 
	     sizeof(CodeLine*) * isData->functionBodies[inCounter]->codeLinesMax);
      free(isData->functionBodies[inCounter]->codeLines);
      isData->functionBodies[inCounter]->codeLines = tmpCodeLines;
      isData->functionBodies[inCounter]->codeLinesMax *=2;
    }

    isData->functionBodies[inCounter]->codeLines[isData->functionBodies[inCounter]->codeLinesCount++] = codeLine;
  }
}



static int parseArg(int fd, Parameter* param, ISData* isData, int inCounter, int updateUsage)
{
  int argType;
  int tmp;
  char buffer[1024];

  switch(argType = get1Byte(fd))
  {
  case 0x61: // static string
    param->type = PARAM_STRINGCONST;
    tmp = get2Byte(fd);
    getString(fd, buffer, tmp);
    escapeString(buffer);
    param->data.string = (char*) mallocX(strlen(buffer)+1);
    strcpy(param->data.string, buffer);
    return(PARAM_STRINGCONST);
      
  case 0x41: // static long
    param->type = PARAM_LONGCONST;
    param->data.intVal = get4Byte(fd);
    return(PARAM_LONGCONST);
  
  case 0x62: // source string variable
  case 0x52: // destination string variable
    tmp = get2Byte(fd);

    // a global system variable
    if ((tmp >= 0) && (tmp < isData->stringSysVarsCount))
    {
      param->type = PARAM_SYSTEMSTRINGVARIABLE;
      param->data.variableNumber = tmp;
      return(PARAM_SYSTEMSTRINGVARIABLE);
    }
    
    // maybe a function local string variable?
    if ((0xff9b - tmp) < 0x8000)
    {
      param->type = PARAM_FNLOCALSTRINGVARIABLE;
      param->data.variableNumber = 0xff9b - tmp;

      return(PARAM_FNLOCALSTRINGVARIABLE);
    }

    // a global user variable
    if ((tmp - isData->stringSysVarsCount) >= 0)
    {
      param->type = PARAM_USERSTRINGVARIABLE;
      param->data.variableNumber = tmp - isData->stringSysVarsCount;
      if (updateUsage)
      	(isData->stringUserVars[param->data.variableNumber])++;
      return(PARAM_USERSTRINGVARIABLE);
    }

    // out of range
    error("Out-of-range string variable (0x%x) encountered at 0x%x\n",
	  tmp, tell(fd));

  case 0x32: // destination number variable
  case 0x42: // source number variable
    tmp = get2Byte(fd);

    // a global system variable
    if ((tmp >=0) && (tmp < isData->numberSysVarsCount))
    {
      param->type = PARAM_SYSTEMNUMBERVARIABLE;
      param->data.variableNumber = tmp;
      return(PARAM_SYSTEMNUMBERVARIABLE);
    }

    // maybe a function variable?
    if ((0xff9b - tmp) < 0x8000)
    {
      param->type = PARAM_FNLOCALNUMBERVARIABLE;
      param->data.variableNumber = 0xff9b - tmp;
      return(PARAM_FNLOCALNUMBERVARIABLE);
    }

    // a global user variable
    if ((tmp - isData->numberSysVarsCount) >= 0)
    {
      param->type = PARAM_USERNUMBERVARIABLE;
      param->data.variableNumber = tmp - isData->numberSysVarsCount;
      if (updateUsage)	
	(isData->numberUserVars[param->data.variableNumber])++;
      return(PARAM_USERNUMBERVARIABLE);
    }
    
    error("Out-of-range number variable (0x%x) encountered at 0x%x\n",
	  tmp, tell(fd));

  default:
    return(PARAM_UNKNOWN);
  }
}  

static int parseOther(int fd, void* buffer)
{
  int argType;
  int tmp;

  *((int*) buffer) =0;
  switch(argType = get1Byte(fd))
  {
  case 0x20: // number of variable pairs following
    *((int*) buffer) = get2Byte(fd);
    return(OTHER_NUMPARAMS);

  case 0x80: // function number
    *((int*) buffer) = get2Byte(fd);
    return(OTHER_USERFUNCTION);

  case 0x70: // label
    *((int*) buffer) = get2Byte(fd);
    return(OTHER_LABEL);

  default:
    return(OTHER_UNKNOWN);
  }
}


static int parseComparison(int comparison)
{
  switch(comparison & 0x0f)
  {
  case 1:    
    return(OP_LESSTHAN);
    break;

  case 2:
    return(OP_GREATERTHAN);
    break;

  case 3:
    return(OP_LESSTHANEQUAL);
    break;

  case 4:
    return(OP_GREATERTHANEQUAL);
    break;

  case 5:
    return(OP_EQUAL);
    break;

  case 6:
    return(OP_NOTEQUAL);
    break;
    
  default:
    return(OP_UNKNOWN);
  }
}
