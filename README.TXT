isDcc v1.22
(c) 1998 Andrew de Quincey
http://www.tardis.ed.ac.uk/~adq
adq@tardis.ed.ac.uk


----------------------------------
Info:

This is an installshield decompiler. It supports all versions of 
installshield up to v5.5 (as far as I know).


----------------------------------
Usage:

To use it, type: 
       isdcc scriptfile.ins

The decompiled script will be output on stdout, so you can redirect it,
for example:
       isdcc setup.ins > setup.rul


----------------------------------
Problems:

You may have problems if you decompile a script compiled by one
installshield compiler, and attempt to recompile it with a different 
version of the compiler. For example, some functions in compiler version
5.00.00 have extra parameters than those in version 3.00.77. Some functions
have been removed from version 5.00.00 (for example CompressGet).

Since this program does not attempt to reconstruct the original higher-level
langauge operators from the ones in the file, what you get out will not
be exactly the same as the original code. The compiler appears to convert
the more complex langauge functions into (usually) a number of goto/comparison
statements. (e.g. "switch" is converted to a load of gotos/compares)

This may cause problems with functions, since in earlier versions of 
installshield, the compiler prevented you from have goto statements in 
functions, and this decompiler will generate these. 

However, installshield compiler 5.00.00 doesn't appear to check for this,
so generated scripts are usable.


----------------------------------
Bugs:

None that I know of, but I'm sure they're there. If you find one, please tell
me about it!


----------------------------------
Changes:

v1.1	Couple of output syntax bugfixes (call, DLL functions in IS3)
v1.2	Lots of bugfixes to do with labels, accessing invalid memory locations, and
	about 10 new opcodes added... all this from user feedback!
v1.21	Fix for falling off the end of file caused by changes introduced in v1.2
	Makefile added for using with gcc (various fixes in the progam for this too)
v1.22	Fix for crashing sometime (buggy alpha high level recovery code),
	and for missing last line of last function in the file

----------------------------------
Next Version:

Hopefully some kind of convertor to solve the problems with incompatibilities
between different versions of the installscript language.

Ability to reconstruct the higher-level language facilities from the compiled
file.


See also:
----------------------------------

http://www.installshield.com/


Acknowledgements:
----------------------------------

NaTzGUL/SiraX for the original Installshield decompiler (Wisdec). Without 
this excellent program, it would not have been possible to write this, since 
I would not have been able to fiddle with the compiled files and see what
did what.

freejazz@hotpop.com for his patches to fix labels properly...
croock@friko.onet.pl for another scipr with an unknown opcode...

And.. various anonymous donators who supplied me with other scripts, and patches
to fix memory problems.....

----------------------------------
Legal:

* Permission is granted to any individual or institution to use, copy, or 
  redistribute this software so long as all of the original files are 
  included unmodified, that it is not sold for profit, and that this copyright
  notice is retained.

* If you wish to use any code from this package in another product,
  or wish to sell it as a standalone product, you *must* contact me.

* Feel free to develop the code, but remember that you are not allowed to
  distribute the modified copies. Send me a copy of your additions, and
  I shall include them in the next version (indicating which parts of
  the code are whose, naturally).

* This software is provided "as is". The author annot be held responsible
  for any loss of data, or other misfortunes resulting from the use of this
  program.

* Likewise, the author cannot be held responsible for the use, or misuse of
  this software.
