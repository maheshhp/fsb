/* This Kernel Module was started by Kaushik N S Iyer (@KaushikIyer16) on 14th-Aug-2016
   named fsbuilder for providing the power to the user to build directory structures
   with the help of a single command. The algorithm used in the process is based on
   the 'first child next sibling' tree - data structure.
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

static int fileCount = 0, typeIsSet = 0, TOTAL_SUPPORTED_FORMATS = 19;

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
                  {"-cs" , ".css" },
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
                  {"-sp" , ".cs"   }
            };

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

int isMultipleExtension(const char *argument){
  int extensionCount = 0;
  for (int i = 0; argument[i] != '\0'; i++) {
    if(argument[i] == '-' && isalnum(argument[i+1])){
      extensionCount++;
    }
    if (extensionCount > 1) {
      return 1;
    }
  }
  return 0;
}

int isValidExtension(const char *argument){
  int i = 0;
  for (i = 0; i < TOTAL_SUPPORTED_FORMATS; i++) {
    if(!strcmp(argument,extensions[i].extension)){
      return 1;
    }
  }
  return 0;
}

int containsMinus(const char *argument) {
   if(argument[0] == '-') {
      return 1;
   }
   else {
      return 0;
   }
}

void printHelp() {
  // Under construction
  printf("\nWelcome to File System Builder (F.S.B.)\n");

  printf("\nUsage : [arg1 arg2]*\n");

  printf("\nSUPPORTED FILE FORMATS:\n");
  for (int i = 0; i < TOTAL_SUPPORTED_FORMATS; i++) {
    printf("%5s ---------- %s\n", extensions[i].extension, extensions[i].format);
  }

  printf("\nIn order to build a directory use the -d option.\n");

  printf("\nSome special characters used to build the file system or the directory structure are:\n" );
  printf("\n / -> Move to a child directory with the name as that of the previous argument\n" );
  printf("\n ^ -> Move to the parent directory of the current directory\n" );
  //-----
  //Needs some serious rework to change to the current command format and explain all features
  //-----
}

char* getExtensionFromArgument(const char *argument){
  int i = 0;
  for (i = 0; i < TOTAL_SUPPORTED_FORMATS; i++) {
    if(!strcmp(argument,extensions[i].extension)){
      return extensions[i].format;
    }
  }
  return " "; // resolve
}

void getMultipleExtensionFromArgument(const char *argument, char *dst){

  int location = 0, currentLocation = 0;
  char returnFormat[' '], currentFormat[' '];

  returnFormat[0] = '\0';
  currentFormat[0] = '\0';

  while(argument[location] != '\0'){
      if (argument[location] == '-') {
        if(currentLocation != 0 && isValidExtension(currentFormat)){
          strcat(returnFormat,getExtensionFromArgument(currentFormat));
          returnFormat[strlen(returnFormat)] = '\0';
        }
        else{
          //Do nothing
        }
        currentFormat[0] = '-';
        currentLocation =1 ;
      }
      else{
        currentFormat[currentLocation++] = argument[location];
        currentFormat[currentLocation] = '\0';
      }
      location++;
  }
  strcat(returnFormat,getExtensionFromArgument(currentFormat));
  strcpy(dst,returnFormat);
}
//Execution testing pending
int createFiles(char *files[]){
  int numOfFiles = sizeof(files) / sizeof(files[0]);
  FILE *fp;
  for (int i = 0; i < numOfFiles; i++) {
    fp = fopen(files[i], "w");
    if(fp == NULL){
      return 0;
    }
    fclose(fp);
    fp = NULL;
  }
  return 1;
}
//Execution testing pending
int createDirectory(char *folders[]){
  int numOfDirectories = sizeof(folders) / sizeof(folders[0]);
  int dirStatus = -1; //Coz 0 denotes success in  case of mkdir
  for (int i = 0; i < numOfDirectories; i++) {
    dirStatus = mkdir(folders[i], "S_IRWXG");
    if (dirStatus != 0) {
      return 0;
    }
    dirStatus = -1;
  }
  return 1;
}

int buildFileSystem(int argc, const char *argv[]) {

  char current_format[' '], current_directory[' '];
  int i = 0;
  int required_formats[' '];

  strcpy(current_format,"");

  for (i = 1; i < argc; i++) {
    if (containsMinus(argv[i])) {
      if (isHelp(argv[i])) {
        printHelp(); //Printing the manual
      }
      else if (isDir(argv[i])) {
        strcpy(current_format,""); //Adding a directory to the tree
      }
      else if (isMultipleExtension(argv[i])) {
        getMultipleExtensionFromArgument(argv[i],current_format);
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

  FILE *fd;
  fd = fopen("./foo.txt", "w");
  printf("%d\n", fd);
  fclose(fd);
  return execCode;
}
