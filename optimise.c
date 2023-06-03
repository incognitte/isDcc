/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#include "optimise.h"

static int instructionCounter;

static void optIf(ISData* isData, int line);
static void memzero(void* memstart, int length);

void optimise(ISData* isData)
{
  int i;
  
  for(i=0; i< isData->codeLinesCount; i++)
  {
    if (isData->codeLines[i] == NULL)
      continue;

    switch(isData->codeLines[i]->type)
    {
    case IFSTATEMENT:
      //      optIf(isData, i);
      break;
    }
  }
  printf("hi\n");

}


static void optIf(ISData* isData, int line)
{
  CodeLine* curLine;
  int ifCodeStart;
  int nextCodeStart;
  int elseCodeStart;
  int numSubCodeLines;
  int ifSize;
  int elseSize;
  int hasElse = 0;
  
  curLine = isData->codeLines[line];

  // ok, work out stuff for an IF without an ELSE
  ifCodeStart = line+1;
  nextCodeStart = isData->labels[curLine->destLabel]->position;
  numSubCodeLines =1;
  ifSize = nextCodeStart - ifCodeStart;

  // now, check if it's got an ELSE clause, and fix if it does
  if ((isData->codeLines[nextCodeStart-1]->type == GOTO) &&
      (isData->codeLines[nextCodeStart-1]->destLabel >
       curLine->destLabel))
  {
    elseCodeStart = nextCodeStart;
    nextCodeStart = 
      isData->labels[isData->codeLines[nextCodeStart-1]->destLabel]->position;
    hasElse = 1;
    numSubCodeLines =2;
    elseSize = nextCodeStart - elseCodeStart;
  }

  // make up memory & copy 
  curLine->subCodeLinesCount = numSubCodeLines;
  curLine->subCodeLines =
    (CodeLine**) mallocX(sizeof(CodeLine*) * numSubCodeLines);

  // do if lines
  curLine->subCodeLines[0] = 
    (CodeLine*) mallocX(sizeof(CodeLine) * ifSize);
  memcpy(curLine->subCodeLines[0], 
	 isData->codeLines[ifCodeStart],
	 ifSize * sizeof(CodeLine*));
  memzero(curLine->subCodeLines[0],
	  ifSize * sizeof(CodeLine*));
  
  // do else lines if they are there
  if (hasElse)
  {
    curLine->subCodeLines[1] = 
      (CodeLine*) mallocX(sizeof(CodeLine) * elseSize);
    memcpy(curLine->subCodeLines[1], 
	   isData->codeLines[elseCodeStart],
	   elseSize * sizeof(CodeLine*));
    memzero(curLine->subCodeLines[1],
	    elseSize * sizeof(CodeLine*));
  }
  
  /*

  printf("%i\n", ifSize);
  if (hasElse)
    printf("%i\n", elseSize);
  */
}




static void memzero(void* memstart, int length)
{
  int i;
  char* poo = (char*) memstart;

  for(i=0; i< length; i++)
    *(poo + i) = 0;
}





