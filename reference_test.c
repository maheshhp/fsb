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



int main(int argc, char const *argv[]) {

  struct timeval start, end;
  clock_t t;
  float delta;
  int fnReturn = 0;

	t = clock();
  gettimeofday(&start, NULL);
  fnReturn = 0;

  fnReturn = createFile("first_test", ".html", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 0);

	t = clock() - t;
  gettimeofday(&end, NULL);

	double time_taken = ((double)t) / CLOCKS_PER_SEC;

  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
  printf("Time taken for create file w/o injection is : \n Time of day diff: %f\n Proc time diff: %f\n Function return status: %d\n",delta, time_taken, fnReturn);

  // End of first calculation

  t = clock();
  gettimeofday(&start, NULL);
  fnReturn = 0;

  fnReturn = createFile("second_test", ".html", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);

	t = clock() - t;
  gettimeofday(&end, NULL);

	double time_taken = ((double)t) / CLOCKS_PER_SEC;

  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
  printf("Time taken for create file with injection is : \n Time of day diff: %f\n Proc time diff: %f\n Function return status: %d\n",delta, time_taken, fnReturn);

  // End of second calculation

  t = clock();
  gettimeofday(&start, NULL);
  fnReturn = 0;

  fnReturn = createDirectory("third_test", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	t = clock() - t;
  gettimeofday(&end, NULL);

	double time_taken = ((double)t) / CLOCKS_PER_SEC;

  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
  printf("Time taken for create directory is : \n Time of day diff: %f\n Proc time diff: %f\n Function return status: %d\n",delta, time_taken, fnReturn);

  // End of third calculation

  t = clock();
  gettimeofday(&start, NULL);

  createDirectory("web", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  createFile("./web/1", ".php", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createFile("./web/2", ".php", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createDirectory("./web/css", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  createDirectory("./web/html", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  createDirectory("./web/js", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  createFile("./web/css/1", ".css", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createFile("./web/css/2", ".css", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createFile("./web/js/1", ".js", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createFile("./web/js/2", ".js", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createFile("./web/html/1", ".html", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);
  createFile("./web/html/2", ".html", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, 1);

	t = clock() - t;
  gettimeofday(&end, NULL);

	double time_taken = ((double)t) / CLOCKS_PER_SEC;

  delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
  printf("Time taken for create reference structure with injection is : \n Time of day diff: %f\n Proc time diff: %f\n",delta, time_taken);

  // End of fourth calculation

	return 0;
}
