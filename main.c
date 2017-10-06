/* This Kernel Module was started by Kaushik N S Iyer (@KaushikIyer16) on 14th-Aug-2016
   named fsbuilder for providing the power to the user to build directory structures
   with the help of a single command. The module can be used as a secure API for file
   creation and management by applications. The module can also be used as a command
   based tool for file system building by users.

   The file system can be easily built by level order traversal of the tree -
   a 'first child next sibling' tree - data structure.
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

static int fileCount = 0, typeIsSet = 0, TOTAL_SUPPORTED_FORMATS = 19, isVerbose = 0;

struct file_tree{
  char name[' '];
  char ext[12];
  struct file_tree *child;
  struct file_tree *sibling;
};

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

int containsDoubleMinus(const char *argument){
   if(argument[0] == '-' && argument[1] == '-') {
      return 1;
   }
   else {
      return 0;
   }
}

int containsMinus(const char *argument){
   if(argument[0] == '-') {
      return 1;
   }
   else {
      return 0;
   }
}

int containsUp(const char *argument){
  if (argument[0] == '^') {
    return 1;
  }
  return 0;
}

int containsDown(const char *argument){
  if (argument[0] == '/') {
    return 1;
  }
  return 0;
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
  return " "; // resolve -- Not used!
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

struct file_tree* getNode(const char *file_name, const char *file_ext){
  struct file_tree *temp;
  temp = (struct file_tree*)malloc(sizeof(struct file_tree));
  temp->child = NULL;
  temp->sibling = NULL;
  strcpy(temp->name, file_name);
  strcpy(temp->ext, file_ext);
  return temp;
}

struct file_tree* addSibling(struct file_tree* node, const char *file_name, const char *file_ext){
  if (node == NULL) {
    return NULL;
  }
  while (node->sibling != NULL) {
    node = node->sibling;
  }
  return getNode(file_name, file_ext);
}

struct file_tree* addChild(struct file_tree* node, const char *file_name, const char *file_ext){
  if (node == NULL) {
    return NULL;
  }
  else if (node->child != NULL) {
    return addSibling(node->child, file_name, file_ext);
  }
  else{
    return getNode(file_name, file_ext);
  }
}

void printFileSystem(struct file_tree* node){
  if (node == NULL) {
    return;
  }
  while (node != NULL) {
    printf("%s.%s ", node->name, node->ext);
    if (node->child != NULL) {
      printf("\n|__");
      printFileSystem(node->child);
    }
    node = node->sibling;
  }
}
//Will be mostly removed in final build
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
//Will be mostly removed in final build
int createDirectories(char *folders[]){
  int numOfDirectories = sizeof(folders) / sizeof(folders[0]);
  int dirStatus = -1; //Coz 0 denotes success in case of mkdir
  for (int i = 0; i < numOfDirectories; i++) {
    dirStatus = mkdir(folders[i], S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (dirStatus != 0) {
      return 0;
    }
    dirStatus = -1;
  }
  return 1;
}

int createFile(const char *file_name, const char *file_ext, const char *context_path){
  FILE *fp;
  char *temp;
  asprintf(&temp, "%s/%s.%s", context_path, file_name, file_ext);
  fp = fopen(temp, "w");
  free(temp);
  if(fp == NULL){
    return 0;
  }
  fclose(fp);
  fp = NULL;
  return 1;
}

int createDirectory(char *folder_name, const char *context_path){
  int dirStatus = -1; //Coz 0 denotes success in case of mkdir
  char *temp;
  asprintf(&temp, "%s/%s", context_path, folder_name);
  dirStatus = mkdir(temp, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  free(temp);
  if (dirStatus != 0) {
    return 0;
  }
  dirStatus = -1;
  return 1;
}

int createFileSystem(struct file_tree* node){
  //Under construction
  return 1;
}

int parseBuildCommand(int argc, const char *argv[]) {
  char current_format[' '], current_directory[' '];
  int i = 0;
  int required_formats[' '];
  struct file_tree *build_tree;

  strcpy(current_format,"");

  for (i = 1; i < argc; i++) {
    if (containsDoubleMinus(argv[i])) {
      if (isHelp(argv[i])) {
        printHelp(); //Printing the manual
      }
      if (strcmp(argv[i], "--v")) {
        isVerbose = 1;
      }
    }
    else if (containsMinus(argv[i])) {
      if (isDir(argv[i])) {
        strcpy(current_format,"/"); //Adding a directory to the tree
      }
      else if (isMultipleExtension(argv[i])) {
        getMultipleExtensionFromArgument(argv[i],current_format);
      }
      else {
        // Under construction
      }
    }
    else if(containsUp(argv[i])){
      // Under construction
      //Put navigating to the previous node in the tree
    }
    else if(containsDown(argv[i])){
      // Under construction
      //Put navigating to the next node in the tree
    }
    else {
      // Under construction
      //Put node creation code here
      char *a = argv[i];
      int isFile = 0;
      for (int i = 0; i < strlen(argv[i]); i++) {
        if (a[i] == '.') {
          printf("Filename with extension...\n", );
          isFile = 1;
          break;
        }
      }
      if (isFile == 0) {
        build_tree = addChild();

      }
    }
  }
  return 0;
}


int main(int argc, const char *argv[]) {

  if(argc <= 1){
    printf("No Arguments !\n Refer manual for usage. (--help)");
    return -1;
  }

  int execCode = parseBuildCommand(argc, argv);

  return execCode;
}
