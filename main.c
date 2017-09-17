/* This Kernel Module was started by Kaushik N S Iyer (@KaushikIyer16) on 14th-Aug-2016
   named fsbuilder for providing the power to the user to build directory structures
   with the help of a single command. The algorithm used in the process is based on
   the 'first child next parent' tree - data structure.
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

static int fileCount = 0, typeIsSet = 0, TOTAL_SUPPORTED_FORMATS=18;

typedef struct{
  char extension[5];
  char format[' '];
}file_map;

/* The file formats are mapped to a shortened string, allowing for shorter commands.
The same map will be used for creating files with multiple extensions.*/

file_map extensions[]=
            {
                  {"-p"  , ".php"  },
                  {"-ht" , ".html" },
                  {"-j"  , ".java" },
                  {"-c"  , ".c"    },
                  {"-cp" , ".cpp"  },
                  {"-js" , ".js"   },
                  {"-css" , ".css" },
                  {"-s"  , ".scala"},
                  {"-r"  , ".r"    },
                  {"-py" , ".py"   },
                  {"-x"  , ".xml"  },
                  {"-xh" , ".xhtml"},
                  {"-cu" , ".cu"   },
                  {"-jp" , ".jsp"  },
                  {"-sq" , ".sql"  },
                  {"-pd" , ".pdf"  },
                  {"-sc" , ".scss" },
                  {"-t"  , ".txt"  },
                  {"-jo" , ".json" },
                  {"-cs" , ".cs"   }
            };

int containsMinus(const char *argument) {
   if(argument[0] == '-') {
      return 1;
   }
   else {
      return 0;
   }
}

int isDir(const char *argument){
  if(argument[1] == 'd') {
    return 1;
  }
  else {
    return 0;
  }
}

int isHelp(const char *argument) {
  if (!strcmp(argument,"--help")) {
    return 1;
  } else {
    return 0;
  }
}

void printHelp() {
  // Under construction
  printf("\nWelcome to File System Builder (F.S.B)\n");

  printf("\nUsage : [arg1 arg2]*\n");

  printf("\nSUPPORTED FILE FORMATS:\n");
  for (int i = 0; i < TOTAL_SUPPORTED_FORMATS; i++) {
    printf("%5s ---------- %s\n", extensions[i].extension, extensions[i].format);
  }

  printf("\nIn order to build a directory use the -d option.\n");

  printf("\nSome special characters used to build the file system or the directory structure are:\n" );
  printf(" / -> Move to a child directory with the name as that of the previous argument\n" );
  printf(" ^ -> Move to the parent directory of the current directory\n" );

  // Under construction
}

int buildFileSystem(int argc, const char *argv[]) {

  char current_format[' '], current_directory[' '];
  int i=0;
  int required_formats[' '];

  strcpy(current_format,"");

  for (i = 1; i < argc; i++) {
    if (containsMinus(argv[i])) {
      if (isHelp(argv[i])) {
        printHelp();
      }
      else if (isDir(argv[i])) {
        strcpy(current_format,"");
      }
      else {
        // Under construction
      }
    }
    else {
      // Under construction
    }
  }
  return 0;
}

int main(int argc, const char *argv[]) {

  if(argc <= 1){
    printf("No Arguments !\n Refer manual for usage. (--help)");
    return -1;
  }

  int execCode = buildFileSystem(argc, argv);

  return execCode;
}
