/* This Kernel Module was started by Kaushik N S Iyer (@KaushikIyer16) on 14th-Aug-2016
   named fsbuilder for providing the power to the user to build directory structures
   with the help of a single command. The module can be used as a secure API for file
   creation and management by applications and also as a command based tool for file
   system building by users.

   The module is being built and improved by Mahesh H P (@maheshhp).

   The file system can be easily built by level order traversal of the tree -
   a 'n-ary tree' - data structure.
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/queue.h"
#include "unistd.h"

static int gFileCount = 0, gTypeIsSet = 0, G_TOTAL_SUPPORTED_FORMATS = 19, gIsVerbose = 0, gInjectTemplate=0;

struct fileTree{
  char name[' '];
  char ext[12];
  mode_t permission;
  struct fileTree **next;
};

typedef struct{
  char extension[5];
  char format[' '];
}fileMap;

/* The file formats are mapped to a shortened string, allowing for shorter commands.
The same map will be used for creating files with multiple extensions.*/

fileMap extensions[]=
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
  if(argument[1] == 'd' && argument[0] == '-') {
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
  for (i = 0; i < G_TOTAL_SUPPORTED_FORMATS; i++) {
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

int isRollUp(const char *argument){
  if (argument[0] == '^') {
    return 1;
  }
  return 0;
}

int isDrillDown(const char *argument){
  if (argument[0] == '/') {
    return 1;
  }
  return 0;
}

int containsFormat(const char* argument){
  int length = 0, i = 0;
  length = strlen(argument);
  for (i = 0; i < length; i++) {
    if (argument[i] == '.') {
      return 1;
    }
  }
  return 0;
}

void printHelp() {
  // Under construction
  printf("\nWelcome to File System Builder (F.S.B.)\n");

  printf("\nUsage : [arg1 arg2]*\n");

  printf("\nSUPPORTED FILE FORMATS:\n");
  for (int i = 0; i < G_TOTAL_SUPPORTED_FORMATS; i++) {
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
  for (i = 0; i < G_TOTAL_SUPPORTED_FORMATS; i++) {
    if(!strcmp(argument,extensions[i].extension)){
      return extensions[i].format;
    }
  }
  return ""; // resolve -- Not used!
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

void separateFileNameAndFormat(const char* argument, char* fileName, char* fileFormat){
  int i=0, j=0;
  char tempFormat[' '];
  while (argument[i] != '.') {
    fileName[i] = argument[i];
    i++;
  }
  i++;
  while (i < strlen(argument)) {
    fileFormat[j++] = argument[i++];
  }
}

struct fileTree* getNode(const char *fileName, const char *fileExt){
  struct fileTree *temp;
  temp = (struct fileTree*)malloc(sizeof(struct fileTree));
  temp->next = NULL;
  strcpy(temp->name, fileName);
  strcpy(temp->ext, fileExt);
  temp->permission = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; //default permission
  return temp;
}

struct fileTree* addChild(struct fileTree* node, const char *fileName, const char *fileExt){
  struct fileTree *temp;
  temp = node;
  if (temp == NULL) {
    temp = getNode(fileName, fileExt);
  }
  else{
    if (temp->next == NULL) {
      temp->next[0] = getNode(fileName, fileExt);
    }
    else{
      int i = 0;
      while (temp->next[i] != NULL) {
        i++;
      }
      temp->next[i] = getNode(fileName, fileExt);
    }
  }
  return temp;
}



int createFile(const char *fileName, const char *fileExt, const char *contextPath, mode_t filePermission){
  FILE *fp;
  char *temp;
  asprintf(&temp, "%s/%s.%s", contextPath, fileName, fileExt);
  fp = fopen(temp, "w");
  free(temp);
  if(fp == NULL){
    return 0;
  }
  fclose(fp);
  chmod(temp, filePermission);
  fp = NULL;
  return 1;
}

int createDirectory(char *folderName, const char *contextPath, mode_t dirPermission){
  int dirStatus = -1; //Coz 0 denotes success in case of mkdir
  char *temp;
  asprintf(&temp, "%s/%s", contextPath, folderName);
  dirStatus = mkdir(temp, dirPermission);
  //dirStatus = mkdir(temp, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  free(temp);
  if (dirStatus != 0) {
    return 0;
  }
  dirStatus = -1;
  return 1;
}

int writeTemplateToFile(const char *filePath, const char *templateExt){
  FILE *newFile, *tempFile;
  char *temp, ch;
  asprintf(&temp, "%s_template.txt", templateExt);
  tempFile = fopen(temp, "r");
  free(temp);
  newFile = fopen(filePath, "w");
  if(newFile == NULL || tempFile == NULL){
    if (tempFile != NULL) {
      fclose(tempFile);
    }
    if (newFile != NULL) {
      fclose(newFile);
    }
    return 0;
  }
  ch = fgetc(tempFile);
  while (ch != EOF) {
    fputc(ch, newFile);
    ch = fgetc(tempFile);
  }
  fclose(tempFile);
  fclose(newFile);
  return 1;
}

int injectFileTemplate(struct fileTree* node){
  //Under construction
  return 1;
}

int createFileSystem(struct fileTree* node){
  //Under construction
  return 1;
}

int printFileSystem(struct fileTree* tree){
  //Under construction
  if (gIsVerbose == 1) {
    if (!tree) {
      return -1;
    }
    printf("Continue with file system building?(y, n)\n");
    char userAnswer;
    scanf("%c\n", &userAnswer);
    if (userAnswer == 'y') {
      return createFileSystem(tree);
    }
    else{
      return -1;
    }
  }
  else{
    return createFileSystem(tree);
  }
}

int parseBuildCommand(int argc, const char *argv[]) {
  char currentFormat[' '], currentDirectory[' '];
  int i = 0;
  struct fileTree *buildTree;

  strcpy(currentFormat,"");
  strcpy(currentDirectory, "");

  for (i = 1; i < argc; i++) {
    if (containsDoubleMinus(argv[i])) {
      printf("Coming to contains double minus\n"); //For debug
      if (isHelp(argv[i])) {
        printHelp(); //Printing the manual
      }
      if (strcmp(argv[i], "--v")) {
        gIsVerbose = 1;
      }
      if (strcmp(argv[i], "--t")) {
        gInjectTemplate = 1;
      }
    }
    else if (containsMinus(argv[i])) {
      printf("Coming to contains minus\n"); //For debug
      if (isDir(argv[i])) {
        strcpy(currentFormat,""); //Adding a directory to the tree
      }
      else if (isMultipleExtension(argv[i])) {
        getMultipleExtensionFromArgument(argv[i],currentFormat);
      }
      else {
        strcpy(currentFormat, getExtensionFromArgument(argv[i]));
        printf("the value of current format is %s\n", currentFormat);
      }
    }
    else if(isRollUp(argv[i])){
      printf("Coming to rollup\n"); //For debug
      // Under construction
      //Put navigating to the previous node in the tree
    }
    else if(isDrillDown(argv[i])){
      printf("Coming to drilldown\n"); //For debug
      // Under construction
      //Put navigating to the next node in the tree
    }
    else if(strcmp(currentFormat, "") == 0 && !(containsFormat(argv[i]))){

      /*ISSUE: a node must be added and not a direct call to the directory*/


      printf("Coming to create dir\n"); //For debug
      if (i>0 && (isDrillDown(argv[i-1]) || isRollUp(argv[i-1]) || isDir(argv[i-1])) ) {
        if (strcmp(currentDirectory, "") == 0) {
          strcpy(currentDirectory, argv[i]);
        }
        else{
          strcat(currentDirectory, " ");
          strcat(currentDirectory, argv[i]);
        }
      }
      else {
        //currentDirectory = RollUpDirectory(currentDirectory); -- Implement!!
        if (strcmp(currentDirectory, "") == 0) {
          strcpy(currentDirectory, argv[i]);
        }
        else{
          strcat(currentDirectory, argv[i]);
        }
      }
      printf("Adding this to the tree --> %s\n", currentDirectory); //For debug
      addChild(buildTree, currentDirectory, currentFormat);
    }
    else {
      printf("Coming to create file\n"); //For debug
      if (!containsFormat(argv[i])) {
        printf("Adding this to the tree %s, %s\n", argv[i], currentFormat);
      }
      else{
        char tempPath[' '], tempFileName[' '], tempFormat[' '];
        strcpy(tempPath, currentDirectory);
        separateFileNameAndFormat(argv[i], tempFileName, tempFormat);
        printf("Adding this to the tree --> %s, %s\n", tempFileName, tempFormat); //For debug
        strcat(tempPath, tempFileName);
        addChild(buildTree, tempPath, tempFormat);
      }

    }
  }
  // return printFileSystem(buildTree);
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
