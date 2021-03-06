#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/queue.h"
#include "unistd.h"
#include "time.h"
#include <vector>
using namespace std;
static int gFileCount = 0, gTypeIsSet = 0, G_TOTAL_SUPPORTED_FORMATS = 19, gIsVerbose = 0, gInjectTemplate=0;
static mode_t gPermission = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; //Default permission

//File tree data structure - helps represent the file system structure being built
struct fileTree{
  char name[' '];
  int nodeType;
  char ext[12];
  mode_t permission;
  int numberOfChildren;
  struct fileTree *parent;
  vector<struct fileTree*> childrenNodes;

};

//File map data structure - mintains option to extension mapping
typedef struct{
  char extension[5];
  char format[' '];
}fileMap;

//Traversal queue data structure - used for File tree traversal
struct queueNode{
  struct fileTree *node;
  TAILQ_ENTRY(queueNode) nextNode;
};

/* The file formats are mapped to a shortened string, allowing for shorter commands.
The same map will be used for creating files with multiple extensions. */
fileMap extensions[]=
            {
                  {"-p"  , ".php"  },
                  {"-ht" , ".html" },
                  {"-ja"  , ".java" },
                  {"-c"  , ".c"    },
                  {"-cp" , ".cpp"  },
                  {"-j" , ".js"   },
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

struct fileTree *fs; //Root node of File tree

//Options parsing functions start -->
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
  }

  else {
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
//Options parsing functions start <--

//Features setup functions start -->
void setUpVerbose(){
  gIsVerbose = 1;
}

void enableInjection(){
  gInjectTemplate = 1;
}
//Features setup functions start -->

//Features status getter functions start -->
int getVerboseStatus(){
  return gIsVerbose;
}

int getInjectStatus(){
  return gInjectTemplate;
}
//Features status getter functions end <--

//Option parsing methods start -->
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
//Option parsing methods end <--

//Roll-up execution function
void rollUpDirectory(char *argument){

  int len = strlen(argument);
  int count = 0;
  char tPath[' '];
  for (int i = len-2; i >= 0; i--) {

    if (argument[i] == '/') {
      memcpy(tPath, argument, i+1);
      tPath[i+1]='\0';
      break;
    }

  }

  if (getVerboseStatus()) {
    printf("The value of the rolled up directory is %s\n",tPath );
  }

  strcpy(argument, tPath);
}

//Manual print function
void printHelp() {

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

}

char* buildExtensionFromArgument(char *argument){

  argument[0] = '.';
  return argument;
}

//Extension resolution function
char* getExtensionFromArgument(const char *argument){

  int i = 0;
  for (i = 0; i < G_TOTAL_SUPPORTED_FORMATS; i++) {

    if(!strcmp(argument,extensions[i].extension)){
      return extensions[i].format;
    }

  }
  return buildExtensionFromArgument((char*)argument);
}

//File permission resolution function
mode_t getFilePermission(const char *argument){

  char tPermissionString[' '];
  mode_t tPermission;
  memmove(tPermissionString, argument + 2, strlen(argument) - 1);
  tPermission = strtol(tPermissionString, NULL, 8);

  if (tPermission > 777 || tPermission < 000) {

    if (getVerboseStatus()) {
      printf("Invalid permissions, setting default permission\n");
    }

    return gPermission;
  }

  return tPermission;
}

//Multiple extension resolution helper function
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

//Extension extraction function
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

//New File tree node creator function
struct fileTree* getNode(const char *fileName, const char *fileExt){

  struct fileTree *temp;
  temp = (struct fileTree*)malloc(sizeof(struct fileTree));
  temp->numberOfChildren = 0;
  strcpy(temp->name, fileName);
  strcpy(temp->ext, fileExt);
  temp->permission = gPermission; //Default permission
  return temp;
}

//New Traversal Queue node creator function
struct queueNode* getQueueNode(struct fileTree* dataNode){

  struct queueNode *temp;
  temp = (struct queueNode*)malloc(sizeof(struct queueNode));
  temp->node = dataNode;
  return temp;
}

//File tree construction function
struct fileTree* buildNode(const char *fileName, const char *fileExt, mode_t filePermission, int nodeType){

    struct fileTree *temp;
    temp = getNode(fileName, fileExt);
    temp->permission = filePermission;
    /*
            Node type 1: for directory
                 type 2: for file
                 type -1: if any error in the node type
    */
    if (nodeType ==1) {
      temp->nodeType = 1;
    }

    else if(nodeType ==2){
      temp->nodeType = 2;
    }

    else if(nodeType == -1){
      temp->nodeType = -1;
    }

    else{
      /* This is a special node type to handle the root folder getting created */
      temp->nodeType = 0;
    }

    return temp;
}

//File tree node addition function
void add(struct fileTree* parentNode, struct fileTree* childNode){

  parentNode->numberOfChildren += 1;

  childNode->parent = parentNode;
  parentNode->childrenNodes.push_back(childNode);

}

//Directory creation function - Native API access
int createDirectory(char *folderName, mode_t dirPermission){

  int dirStatus = -1; //Coz 0 denotes success in case of mkdir
  char *temp;
  asprintf(&temp, "%s",folderName);
  dirStatus = mkdir(temp, dirPermission);
  free(temp);

  if (dirStatus != 0) {
    return 0;
  }

  dirStatus = -1;
  return 1;
}

//Template injector function
int writeTemplateToFile(const char *filePath, const char *templateExt){

  FILE *newFile, *tempFile;
  char *temp, ch;
  asprintf(&temp, "./%s_template.txt", templateExt+1);
  tempFile = fopen(temp, "r");
  newFile = fopen(filePath, "w");

  if (getVerboseStatus()) {
    printf("Opening %s file and injecting the file %s.\n", filePath, temp);
  }

  if (tempFile == NULL) {
    tempFile = fopen("./default_template.txt", "r");
  }

  if(newFile == NULL || tempFile == NULL){

    if (getVerboseStatus()) {
      printf("Error Opening File!\n");
    }

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

//File creation function - Native API access
int createFile(const char *fileName, const char *fileExt, mode_t filePermission){

  FILE *fp;
  char *temp;
  asprintf(&temp, "%s%s",fileName, fileExt);
  fp = fopen(temp, "w");

  if(fp == NULL){
    return 0;
  }

  fclose(fp);

  if (getInjectStatus()) {
    writeTemplateToFile(temp, fileExt);
  }

  if (getVerboseStatus()) {

    printf("Changing file permission to %o\n", filePermission);
    printf("%d\n", chmod(temp, filePermission));
  }

  chmod(temp, filePermission);
  fp = NULL;
  return 1;
}

//Main command parser function
int createFileSystem(struct fileTree* node){

  struct queueNode *currentNode = NULL, *tNode = NULL;
  TAILQ_HEAD(head_s, queueNode) head;
  // Initialize the head before use
  TAILQ_INIT(&head);
  currentNode = getQueueNode(fs);
  TAILQ_INSERT_TAIL(&head, currentNode, nextNode);
  currentNode = NULL;
  while (!TAILQ_EMPTY(&head)) {
    currentNode = TAILQ_FIRST(&head);

    if (currentNode->node->nodeType == 1) {
      createDirectory(currentNode->node->name,currentNode->node->permission);
    }

    else if(currentNode->node->nodeType == 2){
      createFile(currentNode->node->name,currentNode->node->ext,currentNode->node->permission);
    }

    else if(currentNode->node->nodeType == -1){

      if (getVerboseStatus()) {
        printf("This File/Directory has some errors in it. Please review it.\nName of the node: %s\nExtension of the Node: %s\nPermission for the node: %o\n",currentNode->node->name,currentNode->node->ext,currentNode->node->permission );
      }

    }

    for (int i = 0; i < currentNode->node->numberOfChildren; i++) {

      tNode = getQueueNode(currentNode->node->childrenNodes[i]);
      TAILQ_INSERT_TAIL(&head, tNode, nextNode);
      tNode = NULL;
    }

    TAILQ_REMOVE(&head, currentNode, nextNode);
    free(currentNode);
    currentNode = NULL;
  }

  return 1;
}

//File system display function
int printFileSystem(struct fileTree* tree){

  if (!tree) {
    return -1;
  }

  struct queueNode *currentNode = NULL, *tNode = NULL;

  TAILQ_HEAD(head_s, queueNode) head;
  // Initialize the head before use
  TAILQ_INIT(&head);
  currentNode = getQueueNode(fs);
  TAILQ_INSERT_TAIL(&head, currentNode, nextNode);
  currentNode = NULL;
  while (!TAILQ_EMPTY(&head)) {
    currentNode = TAILQ_FIRST(&head);

    if (getVerboseStatus()) {

      printf("%s%s\n",currentNode->node->name, currentNode->node->ext );
      printf("%d\n", currentNode->node->permission);
      printf("Number of children is  %d\n", currentNode->node->numberOfChildren);
    }

    for (int i = 0; i < currentNode->node->numberOfChildren; i++) {

      tNode = getQueueNode(currentNode->node->childrenNodes[i]);
      TAILQ_INSERT_TAIL(&head, tNode, nextNode);
      tNode = NULL;
    }

    TAILQ_REMOVE(&head, currentNode, nextNode);
    free(currentNode);
    currentNode = NULL;

  }

  int createFileSystemStatus = -1;

  if (getVerboseStatus()) {

    printf("Continue with file system building?(y, n)\n");
    char userAnswer;
    userAnswer = getchar();

    if (userAnswer == 'y') {
      createFileSystemStatus = createFileSystem(tree);
    }

    else{
      return -1;
    }

  }

  createFileSystemStatus = createFileSystem(tree);
  return createFileSystemStatus;
}

int parseBuildCommand(int argc, const char *argv[]) {

  char currentFormat[' '], currentDirectory[' '];
  mode_t currentPermission;
  int i = 0;

  /* First would be to put an empty root node incase the fs to be built by the user does not have a root. */
  fs = buildNode("root","",gPermission,3);
  fs->parent = NULL;

  /* Now we need a pointer to keep track of the location of the parser in teh file system. */
  struct fileTree *currentNode = fs;

  /* Setting up the default values for the format, directory and the permission. */
  strcpy(currentFormat,"");
  strcpy(currentDirectory, "./");
  currentPermission = gPermission;

  for (i = 1; i < argc; i++) {

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

      /* Remove any exisitng extensions and set it to empty (implementation of the -d option). */
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

        if (getVerboseStatus()) {
          printf("the value of current format is %s\n", currentFormat);
          printf("The single extension value is %s\n", currentFormat);
        }
      }
    }
    else if(isRollUp(argv[i])){

      if (getVerboseStatus()) {
        printf("Rolling up a directory because the ^ argument was given\n");
      }

      memset(currentFormat, 0, strlen(currentFormat));
      rollUpDirectory(currentDirectory);
      currentNode = currentNode->parent;
    }
    else if(isDrillDown(argv[i])){

      if (getVerboseStatus()) {
        printf("Drilling down a directory because the / argument was given\n");
      }

      memset(currentFormat, 0, strlen(currentFormat));
      strcat(currentDirectory,argv[i-1]);
      strcat(currentDirectory,"/");
      currentNode = currentNode->childrenNodes[(currentNode->numberOfChildren)-1];

    }

    else if(strcmp(currentFormat, "") == 0 && !(containsFormat(argv[i]))){

      char tPath[' '];
      memset(tPath, 0, strlen(tPath));
      strcat(tPath, currentDirectory);
      strcat(tPath, argv[i]);

      if (getVerboseStatus()) {
        printf("The value of permission for the current directory is %o\n", currentPermission);
        printf("Adding this directory to the trees --> %s\n", tPath);
      }

      add(currentNode, buildNode(tPath, currentFormat, currentPermission, 1));
      currentPermission = gPermission;
    }

    else if(containsFormat(argv[i])){

      if (getVerboseStatus()) {
        printf("Checking if the argument has a format or not\n");
      }

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

      if (getVerboseStatus()) {
        printf("Adding this file to the tree --> %s%s\n", tPath, tFormat);
        printf("Current permission value for the file is %o\n", currentPermission);
      }

      add(currentNode, buildNode(tPath, tFormat, currentPermission, 2));
      currentPermission = gPermission;
    }

    else {

      char tPath[' '];
      strcpy(tPath,"");
      strcat(tPath, currentDirectory);
      strcat(tPath, argv[i]);

      if (getVerboseStatus()) {
        printf("Current permission value for the file with extension is %o\n", currentPermission); //For debug
        printf("Adding this file to the tree --> %s%s\n", tPath, currentFormat);
      }

      add(currentNode, buildNode(tPath, currentFormat, currentPermission, 2));
      currentPermission = gPermission;
    }
  }
  return printFileSystem(fs);
}


int main(int argc, const char *argv[]) {

  struct timeval start, end;
  float delta;
  clock_t t;
  double time_taken;

  if(argc <= 1){
    printf("\nNo Arguments !\n Refer manual for usage. (--help)\n");
    return -1;
  }

  int execCode = parseBuildCommand(argc, argv);
  return execCode;
}
