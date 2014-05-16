#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/mount.h>

int main(){
   int fd;
   ssize_t size;
   char buf[20];

   fd = open("tst/01/file.txt", O_RDONLY);
   if (fd >= 0){
      size = read(fd, buf, 20);
      if (size >= 0)
         write(1, buf, size);
      close(fd);
   }

   return 0;
}