/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#include "common.h"
#include "util.h"

void printArgs(ISData* isData, CodeLine* c, int function);
void printArg(ISData* isData, Parameter* p, int function);
void printLine(ISData* isData, CodeLine* code, int indent, int function);

extern char version[];

void output(int fd, ISData* isData, int indent)
{
  int i;
  int j;
  int strParam, numParam;


  printf("// Generated with isDcc v%s\n", version);
  printf("// (c) 1998 Andrew de Quincey\n\n");
  printf("// original file version = %s\n", isData->compilerVersion);
  printf("\n\n");

  // the declare section
  printf("declare\n");

  // string vars
  if (isData->stringUserVarsCount)
  {
    printIndent(indent);
    printf("// ------------- STRING VARIABLES --------------\n");
  }
  for(i=0; i < isData->stringUserVarsCount; i++)
    if (isData->stringUserVars[i])
    {
      printIndent(indent);
      printf("string string%i;\n", i);
    }
  if (isData->stringUserVarsCount)
    printf("\n");


  // number vars
  if (isData->numberUserVarsCount)
  {
    printIndent(indent);
    printf("// ------------- NUMBER VARIABLES --------------\n");
  }
  for(i=0; i < isData->numberUserVarsCount; i++)
    if (isData->numberUserVars[i])
    {
      printIndent(indent);
      printf("number number%i;\n", i);
    }
  if (isData->numberUserVarsCount)
    printf("\n");
  
  // datatypes
  if (isData->dataTypesCount)
  {
    printIndent(indent);
    printf("// ------------- DATA TYPES --------------\n");
  }
  for(i=0; i < isData->dataTypesCount; i++)
  {
    printIndent(indent);
    printf("typedef %s\n", isData->dataTypes[i]->name);
    printIndent(indent);
    printf("begin\n");

    for(j=0; j< isData->dataTypes[i]->entriesCount; j++)
    {
      printIndent(indent+1); 
      printf("%s ", types[isData->dataTypes[i]->entries[j]->type]);
      printf("%s", isData->dataTypes[i]->entries[j]->name);
      
      if (isData->dataTypes[i]->entries[j]->type == 0)
	printf("[%i]", isData->dataTypes[i]->entries[j]->size);
      printf(";\n");
    }

    printIndent(indent);
    printf("end;\n");
    
    if (i < (isData->dataTypesCount -1))
      printf("\n");
  }
  if (isData->dataTypesCount)
    printf("\n");

  // functions declarations
  if (isData->functionPrototypesCount)
  {
    printIndent(indent);
    printf("// ------------- FUNCTION PROTOTYPES --------------\n");
  }
  for(i=0; i< isData->functionPrototypesCount; i++)
  {
    // if we don't actually know which function body
    // is associated, then skip the function
    if ((isData->functionPrototypes[i]->functionBody == NULL) &&
	(isData->functionPrototypes[i]->type == FUNCTION_USER))
      continue;
    
    strParam=0;
    numParam=0;
    printIndent(indent);
    printf("prototype %s(", isData->functionPrototypes[i]->name);
    
    for(j=0; j< isData->functionPrototypes[i]->paramsCount; j++)
    {
      if (isData->functionPrototypes[i]->params[j] & 0x80000000)
	printf("BYREF ");

      printf("%s", types[isData->functionPrototypes[i]->params[j] & 0xffffff]);
      
      if (j < (isData->functionPrototypes[i]->paramsCount -1))
	printf(", ");
    }

    printf(");\n");
  }

  // the main program code
  printf("\n");
  printIndent(indent);
  printf("// ------------- MAIN PROGRAM CODE --------------\n");
  printf("program\n");
  printf("start:\n");
  for(i=0; i < isData->codeLinesCount; i++)
  {
    // skip any NULLed lines
    if (isData->codeLines[i] == NULL)
      continue;

    printLine(isData, isData->codeLines[i], indent, -1);
  }

  // if last thing in code is a label, then stick an exit in	
  // (can't have a label as the last thing in the code)
  if ((isData->codeLinesCount) &&
      (isData->codeLines[i-1]->type == LABEL))
  {
    printIndent(indent);
    printf("exit;\n");
  }


  if (isData->fileVersion == 5)
    printf("\nendprogram\n");


  // functions
  printf("\n");
  printIndent(indent);
  printf("// ------------- FUNCTION DEFS --------------\n");
  for(i=0; i < isData->functionPrototypesCount; i++)
  {
    // not a user function => no body
    if (isData->functionPrototypes[i]->type != FUNCTION_USER)
      continue;

    // function never called => skip it
    if ((isData->functionPrototypes[i]->functionBody == NULL))
      continue;

    // print function header
    printIndent(indent);
    printf("// ------------- FUNCTION %s --------------\n", isData->functionPrototypes[i]->name);
    printIndent(indent);
    printf("function %s(", isData->functionPrototypes[i]->name);
    
    strParam=0;
    numParam=0;
    for(j=0; j< isData->functionPrototypes[i]->paramsCount; j++)
    {
      switch((isData->functionPrototypes[i]->params[j]) & 0xffffff)
      {
      case TYPE_STRING:
	printf("%s", isData->functionPrototypes[i]->paramStringsNames[strParam++]);
	break;
	
	// a number
      default:
	printf("%s", isData->functionPrototypes[i]->paramNumbersNames[numParam++]);
      }
      
      if (j < (isData->functionPrototypes[i]->paramsCount -1))
	printf(", ");
    }
    printf(")\n");


    // print local variables
    for(j=0; j< isData->functionPrototypes[i]->localNumbersCount; j++)
    {
      printIndent(indent+1);
      printf("number lNumber%i;\n", j);
    }
    for(j=0; j< isData->functionPrototypes[i]->localStringsCount; j++)
    {
      printIndent(indent+1);
      printf("string lString%i;\n", j);
    }

    // print function body
    printIndent(indent);
    printf("begin\n");
    for(j=0; j < isData->functionPrototypes[i]->functionBody->codeLinesCount; j++)
    {
      // skip any NULLed lines
      if (isData->functionPrototypes[i]->functionBody->codeLines[j] == NULL)
	continue;
      
      printLine(isData, isData->functionPrototypes[i]->functionBody->codeLines[j], indent+1, i);
    }

    // if last thing in function is a label, then stick a return in	
    // (can't have a label as the last thing in a function)
    if ((isData->functionPrototypes[i]->functionBody->codeLinesCount) &&
	(isData->functionPrototypes[i]->functionBody->codeLines[j-1]->type == LABEL))
    {
      printIndent(indent+1);
      printf("return;\n");
    }

    // end of function
    printIndent(indent);
    printf("end;\n");
    
    if (i < (isData->functionPrototypesCount -1))
      printf("\n\n");
  }
}   



void printArgs(ISData* isData, CodeLine* c, int function)
{
  int i;
  
  for(i=0; i< c->paramsCount; i++)
  {
    printArg(isData, c->params[i], function);
    
    if (i < (c->paramsCount-1))
      printf(", ");
  }
}


void printArg(ISData* isData, Parameter* p, int function)
{
  switch(p->type)
  {
  case PARAM_SYSTEMSTRINGVARIABLE:
    printf("%s", isData->stringSysVars[p->data.variableNumber]->name);
    break;

  case PARAM_USERSTRINGVARIABLE:
    printf("string%i", p->data.variableNumber);
    break;

  case PARAM_FNPARAMSTRINGVARIABLE:
    if (function == -1)
      error("Unexpected function parameter found (not in a function)\n");

    if (p->data.variableNumber >= isData->functionPrototypes[function]->paramStringsCount)     
	error("Invalid function parameter found\n");
      
    printf("%s", isData->functionPrototypes[function]->paramStringsNames[p->data.variableNumber]);
    break;

  case PARAM_FNLOCALSTRINGVARIABLE:
    if (function == -1)
      error("Unexpected function local found (not in a function)\n");

    printf("lString%i", p->data.variableNumber);
    break;

  case PARAM_SYSTEMNUMBERVARIABLE:
    printf("%s", isData->numberSysVars[p->data.variableNumber]->name);
    break;

  case PARAM_USERNUMBERVARIABLE:
    printf("number%i", p->data.variableNumber);
    break;

  case PARAM_FNPARAMNUMBERVARIABLE:
    if (function == -1)
      error("Unexpected function parameter found (not in a function)\n");

    if (p->data.variableNumber >= isData->functionPrototypes[function]->paramNumbersCount)     
	error("Invalid function parameter found\n");
      
    printf("%s", isData->functionPrototypes[function]->paramNumbersNames[p->data.variableNumber]);
    break;

  case PARAM_FNLOCALNUMBERVARIABLE:
    if (function == -1)
      error("Unexpected function local found (not in a function)\n");

    printf("lNumber%i", p->data.variableNumber);
    break;

  case PARAM_STRINGCONST:
    printf("\"%s\"", p->data.string);
    break;
      
  case PARAM_LONGCONST:
    if (p->data.intVal < 256)
      printf("%i", p->data.intVal);
    else
      printf("0x%x", p->data.intVal);
    break;

  case PARAM_DATATYPENUM:
    printf("%s", isData->dataTypes[p->data.intVal]->name);
    break;

  default:
    error("poo\n");
  }	
}


void printLine(ISData* isData, CodeLine* code, int indent, int function)
{
  switch(code->type)
  {
  case FUNCTION:
    printIndent(indent);

    if (isData->fileVersion == 5)
      printf("%s(", code->name);

    else  // fix for installshield < 5
    {
      if ((code->functionNumber != -1) &&
	  (isData->functionPrototypes[code->functionNumber]->type == FUNCTION_DLL) &&
      	  strchr(code->name, '.'))
	printf("%s(", strchr(code->name, '.') +1);
      else
	printf("%s(", code->name);
    }
     
    printArgs(isData, code, function);
    printf(");\n");
    break;

  case OPERATION:
    printIndent(indent);
    printArg(isData, code->destination, function);
    printf(" = ");
    
    switch(code->operationNumber)
    {
    case OP_BITNOT:
      printf(" %s ", code->string);
      printArg(isData, code->params[0], function);
      break;
      
    case OP_EQUATE:
      printArg(isData, code->params[0], function);
	break;
	
    default:
      printArg(isData, code->params[0], function);
      printf(" %s ", code->string);
      printArg(isData, code->params[1], function);
      break;
    }
    printf(";\n");
    break;
    
  case EXIT:
    printIndent(indent);
    printf("exit;\n");
    break;
    
  case ABORT:
    printIndent(indent);
    printf("abort;\n");
    break;
    
  case LABEL:
    if (isData->labels[code->labelNumber]->usage)
      printf("\nlabel%i:\n", code->labelNumber);
    break;
    
  case GOTO:
    printIndent(indent);
    printf("goto label%i;\n", code->destLabel);
    break;
    
  case IFSTATEMENT:
    printIndent(indent);
    printf("if (");
    printArg(isData, code->params[0], function);
    printf(" %s ", operations[code->comparisonType]);
    printArg(isData, code->params[1], function);
    printf(") then\n");
    printIndent(indent+1);
    printf("goto label%i;\n", code->destLabel);
    printIndent(indent);
    printf("endif;\n");
    break;
    
  case HANDLER:
    printIndent(indent);
    printf("Handler(");
    printArg(isData, code->params[0], function);
    printf(", label%i", code->params[1]->data.intVal);
    printf(");\n");
      break;
      
  case RETURN:
    printIndent(indent);
    printf("return;\n");
    break;
    
  case CALL:
    printIndent(indent);
    printf("call ");
    printf("label%i", code->params[0]->data.intVal);
    printf(";\n");
    break;

  case FUNCRETURN:
    printIndent(indent);
    printf("return");
    if (code->paramsCount == 1)
    {
      printf("(");
      printArg(isData, code->params[0],function);
      printf(")");
    }
    printf(";\n");
    break;

  default:
    error("unknown code type %i\n", code->type);
  }
}











