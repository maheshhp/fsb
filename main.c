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
static mode_t gPermission = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

struct fileTree{
  char name[' '];
  int nodeType;
  char ext[12];
  mode_t permission;
  int numberOfChildren;
  struct fileTree *next[' '];
  struct fileTree *parent;
};

typedef struct{
  char extension[5];
  char format[' '];
}fileMap;

struct queueNode{
  struct fileTree *node;
  TAILQ_ENTRY(queueNode) nextNode;
};
struct fileTree *fs;
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

int isVerbose(const char *argument){
  if(strcmp(argument,"--v") ==0){
    return 1;
  }
  else{
    return 0;
  }
}

int isInjectTemplate(const char *argument) {
  if (strcmp(argument,"--t") ==0) {
    return 1;
  }
  else{
    return 0;
  }

}

int isFilePermission(const char* argument){
  char tPermissionString[' '];
  memmove(tPermissionString, argument + 2, strlen(argument) - 1);
  if (isdigit(tPermissionString[0])) {
    return 1;
  }
  return 0;
}

void setUpVerbose(){
  gIsVerbose = 1;
}

void enableInjection(){
  gInjectTemplate = 1;
}

int getVerboseStatus(){
  return gIsVerbose;
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

void rollUpDirectory(char *argument){
  int len = strlen(argument);
  char tPath[' '];
  for (int i = len-1; i >= 0; i--) {
    if (argument[i] == '/') {
      memcpy(tPath, argument, i+1);
      break;
    }
  }
  printf("The value of the rolled up directory is %s\n",tPath );
  strcpy(argument, tPath);
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

char* buildExtensionFromArgument(const char *argument){
  return &argument[1];
}

char* getExtensionFromArgument(const char *argument){
  int i = 0;
  for (i = 0; i < G_TOTAL_SUPPORTED_FORMATS; i++) {
    if(!strcmp(argument,extensions[i].extension)){
      return extensions[i].format;
    }
  }
  return buildExtensionFromArgument(argument);
}

mode_t getFilePermission(const char *argument){
  char tPermissionString[' '];
  mode_t tPermission;
  memmove(tPermissionString, argument + 2, strlen(argument) - 1);
  tPermission = strtol(tPermissionString, NULL, 10);
  if (tPermission > 777 || tPermission < 000) {
    printf("Invalid permissions, setting default permission\n");
    return gPermission;
  }
  return tPermission;
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
  memset(dst,0,strlen(dst));
  strcpy(dst,returnFormat);
}

void separateFileNameAndFormat(const char* argument, char* fileName, char* fileFormat){
  int i=0, j=0;
  char tFormat[' '];
  while (argument[i] != '.') {
    fileName[i] = argument[i];
    i++;
  }
  while (i < strlen(argument)) {
    fileFormat[j++] = argument[i++];
  }
}

struct fileTree* getNode(const char *fileName, const char *fileExt){
  struct fileTree *temp;
  temp = (struct fileTree*)malloc(sizeof(struct fileTree));
  // temp->next = NULL;
  temp->numberOfChildren = 0;
  strcpy(temp->name, fileName);
  strcpy(temp->ext, fileExt);
  temp->permission = gPermission; //default permission
  return temp;
}

struct queueNode* getQueueNode(struct fileTree* dataNode){
  struct queueNode *temp;
  temp = (struct queueNode*)malloc(sizeof(struct queueNode));

  temp->node = dataNode;

  return temp;
}
struct fileTree* buildNode(const char *fileName, const char *fileExt, mode_t filePermission, int nodeType){
    struct fileTree *temp;
    temp = getNode(fileName, fileExt);
    temp->permission = filePermission;

    /*
            node type 1: for directory
                 type 2: for file
                 type -1: if any error in the node type
    */
    if (nodeType ==1) {
      temp->nodeType = 1;
    } else if(nodeType ==2){
      temp->nodeType = 2;
    }else{
      temp->nodeType = -1;
    }

    return temp;
}
void add(struct fileTree* parentNode, struct fileTree* childNode){

  parentNode->next[parentNode->numberOfChildren] = childNode;
  parentNode->next[parentNode->numberOfChildren]->parent = parentNode;
  parentNode->numberOfChildren += 1;

  // printf("val of num child is %d\n", parentNode->numberOfChildren);
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

  struct queueNode *currentNode = NULL, *tNode = NULL;

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
  mode_t currentPermission;
  int i = 0;

  /* first would be to put an empty root node incase the fs to be built by the user does not have a root*/
  fs = buildNode("root","",gPermission,1);
  fs->parent = NULL;

  /*Now we need a pointer to keep track of the location of the parser in teh file system*/
  struct fileTree *currentNode = fs;

  /*setting up the default values for the format, directory and the permission*/
  strcpy(currentFormat,"");
  strcpy(currentDirectory, "./");
  currentPermission = gPermission;

  for (i = 1; i < argc; i++) {
    printf("\n\nIteration: %d | Current format: %s | Current Directory: %s\n\n", i, currentFormat, currentDirectory);
    if (containsDoubleMinus(argv[i])) {
      if (getVerboseStatus()) {
        printf("\nSetting up tags like verbose, help and template injection\n" );
      }

      if (isHelp(argv[i])) {
        printHelp(); //Printing the manual
      }

      if (isVerbose(argv[i])) {
        setUpVerbose();
      }

      if (isInjectTemplate(argv[i])) {
        enableInjection();
      }

      if (isFilePermission(argv[i])) {
        currentPermission = getFilePermission(argv[i]);
      }
    }
    else if (containsMinus(argv[i])) {
      if (getVerboseStatus()) {
        printf("Resolving the file format\n");
      }
      /* remove any exisitng extensions and set it to empty (implementation of the -d option)*/
      if (isDir(argv[i])) {
        if (getVerboseStatus()) {
          printf("Checking for directory and truncating the format context \n");
        }
        memset(currentFormat,0,strlen(currentFormat));
        strcpy(currentFormat, ""); //Adding a directory to the tree
      }
      else if (isMultipleExtension(argv[i])) {
        getMultipleExtensionFromArgument(argv[i],currentFormat);
        if (getVerboseStatus()) {
          printf("The multiple extension value is %s\n", currentFormat);
        }

      }
      else {
        memset(currentFormat,0,strlen(currentFormat));
        strcpy(currentFormat, getExtensionFromArgument(argv[i]));
        printf("the value of current format is %s\n", currentFormat);
        printf("The single extension value is %s\n", currentFormat);//For debug

      }
    }
    else if(isRollUp(argv[i])){
      printf("Rolling up a directory because the ^ argument was given\n");
      memset(currentFormat, 0, strlen(currentFormat));
      rollUpDirectory(currentDirectory);
      currentNode = currentNode->parent;

    }
    else if(isDrillDown(argv[i])){
      printf("Drilling down a directory because the / argument was given\n");
      memset(currentFormat, 0, strlen(currentFormat));
      strcat(currentDirectory,argv[i-1]);
      strcat(currentDirectory,"/");
      currentNode = currentNode->next[(currentNode->numberOfChildren)-1];

    }
    else if(strcmp(currentFormat, "") == 0 && !(containsFormat(argv[i]))){
      char tPath[' '];
      memset(tPath, 0, strlen(tPath));
      strcat(tPath, currentDirectory);
      strcat(tPath, argv[i]);
      printf("The value of permission for the current directory is %o\n", currentPermission);
      printf("Adding this directory to the trees --> %s\n", tPath);
      currentPermission = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
      add(currentNode, buildNode(tPath, currentFormat, currentPermission, 1));
    }
    else if(containsFormat(argv[i])){
      printf("Checking if the argument has a format or not\n");//For debug
      char tPath[' '], tFileName[' '], tFormat[' '];
      memset(tFormat, 0, strlen(tFormat));
      memset(tPath, 0, strlen(tPath));
      memset(tFileName, 0, strlen(tFileName));
      strcpy(tPath, currentDirectory);
      separateFileNameAndFormat(argv[i], tFileName, tFormat);
      if (strcmp(currentFormat, "") != 0) {
        strcat(tFormat, currentFormat);
      }
      strcat(tPath, tFileName);
      printf("Adding this file to the tree --> %s%s\n", tPath, tFormat);
      printf("Current permission value for the file is %o\n", currentPermission);
      currentPermission = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
      add(currentNode, buildNode(tPath, tFormat, currentPermission, 2));
    }
    else {

      char tPath[' '];
      strcpy(tPath,"");
      strcat(tPath, currentDirectory);
      strcat(tPath, argv[i]);
      printf("current permission value for the file with extension is %o\n", currentPermission); //For debug
      printf("Adding this file to the tree --> %s%s\n", tPath, currentFormat); //For debug
      currentPermission = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
      add(currentNode, buildNode(tPath, currentFormat, currentPermission, 2));
    }
  }
  // return printFileSystem(fs);
  return 0;
}


int main(int argc, const char *argv[]) {

  if(argc <= 1){
    printf("\nNo Arguments !\n Refer manual for usage. (--help)\n");
    return -1;
  }

  struct queueNode *currentNode = NULL, *tNode = NULL;

  int execCode = parseBuildCommand(argc, argv);

  printf("\n   Start the n-ary tree traversal \n" );

  TAILQ_HEAD(head_s, queueNode) head;
  // Initialize the head before use
  TAILQ_INIT(&head);
  currentNode = getQueueNode(fs);
  TAILQ_INSERT_TAIL(&head, currentNode, nextNode);
  currentNode = NULL;
  while (!TAILQ_EMPTY(&head)) {
    currentNode = TAILQ_FIRST(&head);
    printf("%s%s\n",currentNode->node->name, currentNode->node->ext );
    printf("Number of children is  %d\n", currentNode->node->numberOfChildren);

    for (int i = 0; i < currentNode->node->numberOfChildren; i++) {
      tNode = getQueueNode(currentNode->node->next[i]);
      TAILQ_INSERT_TAIL(&head, tNode, nextNode);
      tNode = NULL;
    }
    TAILQ_REMOVE(&head, currentNode, nextNode);
    free(currentNode);
    currentNode = NULL;

  }

  return execCode;
}

/*

  ERROR/BUGS:
  5. separateFileNameAndFormat has a bug does not prepend the '.' (still pending)

*/
