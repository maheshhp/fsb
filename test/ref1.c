#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "time.h"

int writeTemplateToFile(const char *filePath, const char *templateExt){
  FILE *newFile, *tempFile;
  char *temp, ch;
  asprintf(&temp, "./%s_template.txt", templateExt+1);
  tempFile = fopen(temp, "r");
  newFile = fopen(filePath, "w");

  if (tempFile == NULL) {
    tempFile = fopen("./default_template.txt", "r");
  }
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

int createFile(const char *fileName, const char *fileExt, mode_t filePermission, int injectStatus){
  FILE *fp;
  char *temp;
  asprintf(&temp, "%s%s",fileName, fileExt);
  fp = fopen(temp, "w");
  if(fp == NULL){
    return 0;
  }
  fclose(fp);
  if (injectStatus) {
    writeTemplateToFile(temp, fileExt);
  }
  chmod(temp, filePermission);
  fp = NULL;
  return 1;
}

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

void nDirectories(int ct) {
  struct timeval start, end;
  clock_t t;
  float delta;
  int fnReturn = 0;
  double time_taken;

  char *currDir;
  t = clock();
  gettimeofday(&start, NULL);
  fnReturn = 0;

  // the files are created here
  for (int i = 0; i < ct; i++) {
    sprintf(currDir,"%s%d","./try/Dir",i);
    // printf("%s\n",currDir );
    createDirectory(currDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  }

  t = clock() - t;
  gettimeofday(&end, NULL);

	time_taken = ((double)t) / CLOCKS_PER_SEC;

  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
  printf("Time taken for create file w/o injection is : \n Time of day diff: %f\n Proc time diff: %f\n Function return status: %d\n",delta, time_taken, fnReturn);
}
int main(int argc, char const *argv[]) {
  nDirectories(20);
  return 0;
}
