//necessary for RTD_NEXT symbol
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>


#define PATH_MAX 512

FILE *fout = NULL;
static int (*origopen)(const char *path, int flags, ...);
static int (*origrmdir)(const char *pathname);
static int (*origunlink)(const char *pathname);
static int (*origrename)(const char *oldpath, const char *newpath);
static int (*origunlinkat)(int dirfd, const char *pathname, int flags);
static int (*origopenat)(int dirfd, const char *pathname, int flags, ...);


/* works like mkdir(1) used as "mkdir -p" */
int mkdirp(const char *pathname, mode_t mode)
{
  char parent[PATH_MAX], *p;
  /* make a parent directory path */
  strncpy(parent, pathname, sizeof(parent));
  parent[sizeof(parent) - 1] = '\0';
  for(p = parent + strlen(parent); *p != '/' && p != parent; p--);
  *p = '\0';
  /* try make parent directory */
  if(p != parent && mkdirp(parent, mode) != 0)
    return -1;
  /* make this one if parent has been made */
  if(mkdir(pathname, mode) == 0)
    return 0;
  /* if it already exists that is fine */
  if(errno == EEXIST)
    return 0;
  return -1;
}



void copyBytes(int ofd, int dfd)
{
  char *buffer = malloc(65536);
  int len;
  while ((len = read(ofd, buffer, 65536)) > 0) {
    if (write(dfd, buffer, len) <= 0)
      break;
  }
  free(buffer);
}

int copyfile(const char *origpathname)
{

  int ofd, dfd;

  const char *bckpath = "/tmp/back";
  char *realp1 = realpath(origpathname, NULL);
  char *realp2 = strdup(realp1);

  char *dirnm = dirname(realp1);
  // Need to check if actually a file. For the moment we don't care
  char *filenm = basename(realp2);
  
  char *destDirPath = malloc( strlen(dirnm) + strlen(bckpath) + 2);
  strcpy(destDirPath, bckpath);
  strcat(destDirPath, dirnm);
  strcat(destDirPath, "/");
  //Create dir  path up to backup file
  mkdirp(destDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  char *destFilePath = malloc( strlen(destDirPath) + strlen(filenm) + 1);
  strcpy(destFilePath, destDirPath);
  strcat(destFilePath, filenm);

  ofd = origopen(origpathname, O_RDONLY);

  //The O_EXCL should in principle protect from multiple process/thread creation attempts to a similar file.
  //Only the first gets through
  dfd = origopen(destFilePath, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  if (dfd <= 0) {
    close(ofd);
    goto release_buffers;
  }

  copyBytes(ofd, dfd);

  close(ofd);
  close(dfd);

  release_buffers:
  free(realp1);
  free(realp2);
  free(destDirPath);
  free(destFilePath);

  return 0;
}


int checkCopiedAlready(const char *path)
{
  return 0;
}

int checkExcludeList(const char *path)
{
  return 0;
}




int fileExist(const char *pathname)
{
  struct stat sb;  
  int ret = stat(pathname, &sb);
  fprintf(fout, "File: %s exist=%d\n", pathname, ret);
  return (ret == 0);
}


#ifndef TEST


__attribute__((constructor)) void init(void) 
{
  fout = fopen("/tmp/open.txt", "a");
  fprintf(fout, "process %u openened\n", getpid());
  origopen = dlsym(RTLD_NEXT, "open");
  origrmdir = dlsym(RTLD_NEXT, "rmdir");
  origunlink = dlsym(RTLD_NEXT, "unlink");
  origrename = dlsym(RTLD_NEXT, "rename");
  origunlinkat = dlsym(RTLD_NEXT, "unlinkat");
  origopenat = dlsym(RTLD_NEXT, "openat");
}

__attribute__((destructor))  void fini(void) 
{
  fclose(fout);
}


int open(const char *path, int flags, ...)
{

  va_list ap;
  mode_t mode;

  if (flags & (O_WRONLY | O_RDWR)) {
    //check if file already exist if yes back it up
    if (fileExist(path)) {
      fprintf(fout, "%s opened for WR. backing up\n", path);
      copyfile(path);
    }
  }

  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
    return origopen(path, flags, mode);
  }
  return origopen(path, flags);
}


int openat(int dirfd, const char *pathname, int flags, ...)
{
  va_list ap;
  mode_t mode;

  if (flags & (O_WRONLY | O_RDWR)) {
    //check if file already exist if yes back it up
    if (fileExist(pathname)) {
      fprintf(fout, "%s openat for WR. backing up\n", pathname);
      copyfile(pathname);
    }
  }

  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
    return origopenat(dirfd, pathname, flags, mode);
  }
  return origopenat(dirfd, pathname, flags); 
}


int unlink(const char *pathname)
{
  fprintf(fout, "%s set for deleting. backing up\n", pathname);
  if (fileExist(pathname))
    copyfile(pathname);
  return origunlink(pathname);
}

int rmdir(const char *pathname)
{
  fprintf(fout, "%s set for deleting. Do not backup yet\n", pathname);
  return origrmdir(pathname);
}

int rename(const char *oldpath, const char *newpath)
{
  fprintf(fout, "%s renaming to %s. backing up both\n", oldpath, newpath);
  copyfile(oldpath);
  if (fileExist(newpath))
    copyfile(newpath);
  else 
    fprintf(fout, "%s does not exists yet no back up\n", newpath);
  return origrename(oldpath, newpath);
}


int unlinkat(int dirfd, const char *pathname, int flags)
{
  fprintf(fout, "%s unlinkat dirfd=%d\n", pathname, dirfd);
  copyfile(pathname);
  return origunlinkat(dirfd, pathname, flags);
}


#else



int main(int argc, char *argv[])
{

  origopen = dlsym(RTLD_NEXT, "open");
  origrmdir = dlsym(RTLD_NEXT, "rmdir");
  origunlink = dlsym(RTLD_NEXT, "unlink");


  copyfile("/etc/hosts");
  return 0;
  
}



#endif
