/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include "common.h"
#include "util.h"
#include "ishield.h"
#include "header.h"
#include "decode.h"
#include "output.h"
#include "optimise.h"


char version[] = "1.22";


int main(int argc, char* argv[])
{
  int fd;
  int i,j;
  int tmp;
  ISData isData;
 

  fprintf(stderr, "isDcc v%s, (c) 1998 Andrew de Quincey\n\n", version);

  // --------------------------------------------------------------------------
  // check params
  if (argc != 2)
    error("Syntax: isDcc <scriptfile>\n");


  // --------------------------------------------------------------------------
  // open file
  if ((fd = open(argv[1], O_RDONLY | O_BINARY)) == -1)
    error("Cannot open file %s for reading\n", argv[1]);


  // --------------------------------------------------------------------------
  // initialise the opTable stuff
  initDecode();


  // --------------------------------------------------------------------------
  // parse the header
  parseHeader(fd, &isData);

  // decode the code
  decode(fd, &isData);

  // do any extra conversion on the code
  optimise(&isData);

  // output it
  output(fd, &isData, 1);

  close(fd);
}

