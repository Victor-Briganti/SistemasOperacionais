/*
 * Lê os status de um arquivo e trata o SIGHUP
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <unistd.h>

bool readInt = true;

void handleInt(int signo, siginfo_t *info, void *context) {
  printf("\nReelendo\n\n");
  readInt = true;
}

void print_stat(struct stat &sb) {
  printf("ID of containing device:  [%x,%x]\n", major(sb.st_dev),
         minor(sb.st_dev));

  printf("File type:                ");

  switch (sb.st_mode & S_IFMT) {
  case S_IFBLK:
    printf("block device\n");
    break;
  case S_IFCHR:
    printf("character device\n");
    break;
  case S_IFDIR:
    printf("directory\n");
    break;
  case S_IFIFO:
    printf("FIFO/pipe\n");
    break;
  case S_IFLNK:
    printf("symlink\n");
    break;
  case S_IFREG:
    printf("regular file\n");
    break;
  case S_IFSOCK:
    printf("socket\n");
    break;
  default:
    printf("unknown?\n");
    break;
  }

  printf("I-node number:            %ju\n", (uintmax_t)sb.st_ino);

  printf("Mode:                     %jo (octal)\n", (uintmax_t)sb.st_mode);

  printf("Link count:               %ju\n", (uintmax_t)sb.st_nlink);
  printf("Ownership:                UID=%ju   GID=%ju\n", (uintmax_t)sb.st_uid,
         (uintmax_t)sb.st_gid);

  printf("Preferred I/O block size: %jd bytes\n", (intmax_t)sb.st_blksize);
  printf("File size:                %jd bytes\n", (intmax_t)sb.st_size);
  printf("Blocks allocated:         %jd\n", (intmax_t)sb.st_blocks);

  printf("Last status change:       %s", ctime(&sb.st_ctime));
  printf("Last file access:         %s", ctime(&sb.st_atime));
  printf("Last file modification:   %s", ctime(&sb.st_mtime));
}

int main(int argc, char *argv[]) {
  struct stat sb;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sigaction actHup = {0};
  actHup.sa_sigaction = &handleInt;

  if (sigaction(SIGHUP, &actHup, NULL) < 0) {
    perror("sigaction SIGHUP");
    return EXIT_FAILURE;
  }

  while (true) {
    if (readInt) {
      if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
      }

      print_stat(sb);
      readInt = false;
    }
  }

  return 0;
}