/**
 * @file lib_filesystem.cpp
 *
 * @brief Shared library
 *
 * File containing implementation of the shared library used by analyser. The library contains 
 * implementation of functions to be monitored and controlled by analyser.
 *
 * @author Viktor Malik, <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 01.04.2014
 * 
 * Created on: Feb 16, 2014
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mount.h>
#include <stdarg.h>
#include <fcntl.h>

#define SOCKET_PATH "/tmp/analyserSocket"
#define FUNCTIONS_CNT 45

/**
 * Enumeration type representing all supported functions.
 */
enum functions {
   READ, WRITE, OPEN, OPEN64, CLOSE, LSEEK, CREAT, CREAT64, LINK, SYMLINK, UNLINK, STAT, LSTAT,
   FSTAT, ACCESS, CHMOD, FCHMOD, FLOCK, OPENDIR, READDIR, CLOSEDIR, MKDIR, RMDIR, FSYNC,

   MMAP, MUNMAP, MLOCK, MUNLOCK, MLOCKALL, MUNLOCKALL, BRK, SELECT, POLL, DUP, DUP2, SHMGET, CHOWN,
   FCHOWN, LCHOWN, MOUNT, UMOUNT, UMOUNT2, UMASK, REWINDDIR, SYNC
};

// Global variable containing socket file desriptor
int socketFd;

// Array mapping functions to strings with their names
const char *funName[FUNCTIONS_CNT] = {
      [READ] = "read",
      [WRITE] = "write",
      [OPEN] = "open",
      [OPEN64] = "open64",
      [CLOSE] = "close",
      [LSEEK] = "lseek",
      [CREAT] = "creat",
      [CREAT64] = "creat64",
      [LINK] = "link",
      [SYMLINK] = "symlink",
      [UNLINK] = "unlink",
      [STAT] = "stat",
      [LSTAT ] = "lstat",
      [FSTAT] = "fstat",
      [ACCESS] = "access",
      [CHMOD] = "chmod",
      [FCHMOD] = "fchmod",
      [FLOCK] = "flock",
      [OPENDIR] = "opendir",
      [READDIR ] = "readdir",
      [CLOSEDIR] = "closedir",
      [MKDIR] = "mkdir",
      [RMDIR] = "rmdir",
      [FSYNC] = "fsync",

      [MMAP] = "mmap",
      [MUNMAP] = "munmap",
      [MLOCK ] = "mlock",
      [MUNLOCK] = "munlock",
      [MLOCKALL] = "mlockall",
      [MUNLOCKALL] = "munlockall",
      [BRK] = "brk",
      [SELECT ] = "select",
      [POLL] = "poll",
      [DUP] = "dup",
      [DUP2] = "dup2",
      [SHMGET] = "shmget",
      [CHOWN] = "chown",
      [FCHOWN] = "fchown",
      [LCHOWN] = "lchown",
      [MOUNT] = "mount",
      [UMOUNT] = "umount",
      [UMOUNT2] = "umount2",
      [UMASK ] = "umask",
      [REWINDDIR] = "rewinddir",
      [SYNC] = "sync" };

// Array with information what type of message to send about each function
//    0 - send no message (default)
//    1 - send NOTIFY message
//    2 - send CONTROL message
int funList[FUNCTIONS_CNT];

/**
 * Enumeration with all inbound message types - EXEC and ACK.
 */
enum inMsgType {
   EXEC, ACK
};

/**
 * Type representing inbound message (message from controlling program).
 */
typedef struct inMsg {
   enum inMsgType type; /**< Message type */
   char function[11]; /**< Function name */
   int variant; /**< Variant to be used for the call */
} TInMsg;

/**
 * @brief Parses message OPTION.
 * Parses message of type OPTION coming in initial phase of the communication and sets control and
 * notify function lists.
 * @param  msg Strgin with message to be parsed.
 * @return -1 if an error occured, otherwise 0.
 */
int parseOptionMsg(char *msg) {
   char *pch;
   pch = strtok(msg, "\r\n");
   if ((strcmp(pch, "OPTION")) != 0)
      return -1;
   // Get rid of NOTIFICATION line
   pch = strtok(NULL, "\r\n");
   // Clear functions list
   for (int i = 0; i < FUNCTIONS_CNT; ++i) {
      funList[i] = 0;
   }
   // Determine NOTIFICATION list type
   pch = strtok(NULL, "\r\n");
   if ((strcmp(pch, "ALL")) == 0) {
      // ALL
      for (int i = 0; i < FUNCTIONS_CNT; ++i) {
         funList[i] = 1;
      }
      // Get rid of CONTROL line
      pch = strtok(NULL, "\r\n");
   }
   else if ((strcmp(pch, "INCLUDE")) == 0) {
      // INCLUDE
      pch = strtok(NULL, "\r\n");
      // Parse functions for NOTIFICATION list
      while ((strcmp(pch, "CONTROL")) != 0) {
         for (int i = 0; i < FUNCTIONS_CNT; ++i) {
            if ((strcmp(pch, funName[i])) == 0)
               funList[i] = 1;
         }
         pch = strtok(NULL, "\r\n");
      }
   }
   else {
      // Get rid of CONTROL line
      pch = strtok(NULL, "\r\n");  
   }

   // Determine CONTROL list type
   pch = strtok(NULL, "\r\n");
   if ((strcmp(pch, "ALL")) == 0) {
      // ALL
      for (int i = 0; i < FSYNC; ++i) {
         funList[i] = 2;
      }
      // Get rid of CONTROL line
      pch = strtok(NULL, "\r\n");
   }
   else if ((strcmp(pch, "INCLUDE")) == 0) {
      // INCLUDE
      // Parse functions for CONTROL list
      while ((pch = strtok(NULL, "\r\n")) != NULL) {
         for (int i = 0; i < FSYNC; ++i) {
            if ((strcmp(pch, funName[i])) == 0)
               funList[i] = 2;
         }
      }
   }

   return 0;
}

/**
 * @brief Parses inbound message.
 * Parses message coming from controlling program (EXEC or ACK).
 * @param  msg String with message to be parsed.
 * @return     Pointer to the structure containing parsed message.
 */
TInMsg *parseInMsg(char *msg) {
   char *pch;
   TInMsg *result = malloc(sizeof(struct inMsg));
   // Get message type
   pch = strtok(msg, "\r\n");
   if ((strcmp(pch, "ACK")) == 0) {
      result->type = ACK;
      return result;
   }
   else if ((strcmp(pch, "EXEC")) == 0) {
      result->type = EXEC;
      // Get function name
      pch = strtok(NULL, "\r\n");
      strcpy(result->function, pch);

      // Get call variant
      pch = strtok(NULL, "\r\n");
      result->variant = atoi(pch);

      return result;
   }
   else
      return NULL;
}

/**
 * @brief Create socket connection with controller.
 * Connection is created only if global variable socketFd contains 0 (no connectino has been 
 * created yet).
 */
void socketConnection() {
   if (!socketFd) {
      // Create socket connection

      // Create new socket
      if ((socketFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
         fprintf(stderr, "Socket error\n");
      }
      // Create structure with server socket destination
      struct sockaddr_un saddr;
      saddr.sun_family = AF_UNIX;
      strcpy(saddr.sun_path, SOCKET_PATH);
      // Connect to server socket
      if ((connect(socketFd, (struct sockaddr*) &saddr, sizeof(saddr))) == -1) {
         fprintf(stderr, "Connect error\n");
      }

      // Send INIT message
      send(socketFd, "INIT\r\n\r\n", 8, 0);

      // Receive and parse option message
      char buf[250];
      recv(socketFd, buf, 250, 0);
      if ((parseOptionMsg(buf)) < 0) {
         fprintf(stderr, "Protocol error\n");
      }
   }
}

/**
 * @brief Compose outbound message.
 * Message is of type NOTIFY or CONTROL depending on settings from OPTION message.
 * @param  function  Function which the message informs about.
 * @param  paramsCnt Number of function parameters.
 * @param  params    Function parameters values converted to strings.
 * @return           Message composed.
 */
char *composeMsg(enum functions function, int paramsCnt, char *params[]) {
   // Alloc space for message
   char *resultMsg;
   if ((resultMsg = malloc(250 * sizeof(char))) == NULL) {
      fprintf(stderr, "Malloc error\n");
      return NULL;
   }
   // Write message type
   strcpy(resultMsg, funList[function] == 1 ? "NOTIFY\r\n" : "CONTROL\r\n");

   // Write function name
   strcat(resultMsg, funName[function]);
   strcat(resultMsg, "\r\n");
   // Write parameters
   for (int i = 0; i < paramsCnt; ++i) {
      strcat(resultMsg, params[i]);
      strcat(resultMsg, "\r\n");
   }
   // Write ending CRLF
   strcat(resultMsg, "\r\n");

   return resultMsg;
}

/**
 * @brief Initial part of every wrapped call.
 * Contains socket connection, creating and sending informal message (NOTIFY or CONTROL) and 
 * receiving and parsing answer.
 * @param  function  Function from which the initialisation is called.
 * @param  paramsCnt Function parameters count.
 * @param  params    Function parameters values converted to strings.
 * @return           Structure containing parsed incoming message.
 */
TInMsg *initFunction(enum functions function, int paramsCnt, char *params[]) {
   // Connect to socket if this is first call in program
   socketConnection();
   if (funList[function]) {
      // Compose and send message
      char *outMsg = composeMsg(function, paramsCnt, params);
      send(socketFd, outMsg, strlen(outMsg), 0);
      free(outMsg);

      // Receive and parse answer
      char inMsgBuf[100];
      recv(socketFd, inMsgBuf, 100, 0);
      TInMsg *inMsg = parseInMsg(inMsgBuf);
      if (inMsg->type == EXEC) {
         if (strcmp(inMsg->function, funName[function]) == 0) {
            return inMsg;
         }
         else {
            fprintf(stderr, "Protocol error.\n");
            return NULL;
         }
      }
      else {
         return NULL;
      }
   }
   else
      return NULL;
}

/**
 * @brief Send return message.
 * Send message of type RETURN containing return value of the call.
 * @param returnVal String with return value.
 */
void sendReturnMsg(char *returnVal) {
   char *msg;
   if ((msg = malloc(20 * sizeof(char))) == NULL) {
      fprintf(stderr, "Malloc error");
      return;
   }
   // Create message
   strcpy(msg, "RETURN\r\n");
   strcat(msg, returnVal);
   strcat(msg, "\r\n\r\n");
   // Send message
   send(socketFd, msg, strlen(msg), 0);
   free(msg);
   // Receive answer
   char inMsgBuf[10];
   recv(socketFd, inMsgBuf, 10, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Wrapping functions for filesystem calls (controllable functions)
////////////////////////////////////////////////////////////////////////////////

ssize_t read(int fd, void *buf, size_t count) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%p", buf);
   sprintf(paramsStr[2], "%zu", count);

   TInMsg *inMsg = initFunction(READ, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // bad file descriptor or not open for reading
         errno = EBADF;
         break;
      case 11: // fd attached to object not suitable for reading
         errno = EINVAL;
         break;
      case 20: // I/O error
         errno = EIO;
         break;
      case 40: // buf is outside accessible address space
         errno = EFAULT;
         break;
      case 50: // call interrupted by a signal
         errno = EINTR;
         break;
      case 90: // fd referes to a directory
         errno = EISDIR;
         break;
      default:
         normal = 1;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   ssize_t (*read_orig)(int, void *, size_t);
   read_orig = dlsym(RTLD_NEXT, "read");
   // Run normal function and save return value
   ssize_t returnVal = read_orig(fd, buf, count);
   if (funList[READ]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%zd", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

ssize_t write(int fd, const void *buf, size_t count) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%p", buf);
   sprintf(paramsStr[2], "%zu", count);

   TInMsg *inMsg = initFunction(WRITE, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // bad file descriptor or not open for writing
         errno = EBADF;
         break;
      case 11: // fd attached to an object not suitable for writing
         errno = EINVAL;
         break;
      case 20: // I/O error
         errno = EIO;
         break;
      case 40: // buf outside accessible address space
         errno = EFAULT;
         break;
      case 50: // call interrupted by a signal
         errno = EINTR;
         break;
      case 70: // quota of disk blocks has been exhausted
         errno = EDQUOT;
         break;
      case 71: // attempt to write to a file that exceeds maximum file size limit
         errno = EFBIG;
         break;
      case 72: // device containing the file has no room for the data
         errno = ENOSPC;
         break;
      default:
         normal = 1;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   ssize_t (*write_orig)(int, const void *, size_t);
   write_orig = dlsym(RTLD_NEXT, "write");
   // Run normal function and save return value
   ssize_t returnVal = write_orig(fd, buf, count);
   if (funList[WRITE]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%zd", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int open(const char *pathname, int flags, ...) {
   int paramsCnt = 2;
   mode_t mode = 0;
   if (flags & O_CREAT) {
      paramsCnt = 3;
      // Get value of last parameter
      va_list args;
      va_start(args, flags);
      mode = va_arg(args, mode_t);
   }

   // Convert parameters to strings
   char *paramsStr[paramsCnt];
   for (int i = 0; i < paramsCnt; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], pathname);
   sprintf(paramsStr[1], "%d", flags);
   if (paramsCnt == 3) {
      sprintf(paramsStr[2], "%lo", (unsigned long) mode);
   }

   TInMsg *inMsg = initFunction(OPEN, paramsCnt, paramsStr);
   for (int i = 0; i < paramsCnt; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // requested access to the file is not allowed
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname was too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // a component used as a dircetory in pathname is not a directory
         errno = ENOTDIR;
         break;
      case 70: // O_CREAT is specified, file does not exist and quota of disk blocks has been exhausted
         if (flags & O_CREAT) {
            errno = EDQUOT;
            break;
         }
         normal = 1;
         break;
      case 72: // device containing pathname has no room for the new file
         if (flags & O_CREAT) {
            errno = ENOSPC;
            break;
         }
         normal = 1;
         break;
      case 73: // process has maximum of files open
         errno = EMFILE;
         break;
      case 74: // system has maximum of files open
         errno = ENFILE;
         break;
      case 80: // O_NOATIME was specified, but caller UID did not match owner of the file
         if (flags & O_NOATIME) {
            errno = EPERM;
            break;
         }
         normal = 1;
         break;
      case 81: // write access requested, but pathname refers to a file on read-only system
         if ((flags & O_RDWR) || (flags & O_WRONLY)) {
            errno = EROFS;
            break;
         }
         normal = 1;
         break;
      case 90: // write access requested and pathname refers a directory
         if ((flags & O_RDWR) || (flags & O_WRONLY)) {
            errno = EISDIR;
            break;
         }
         normal = 1;
         break;
      case 91: // O_CREAT and O_EXCL used and file already exists
         if ((flags & O_CREAT) && (flags & O_EXCL)) {
            errno = EEXIST;
            break;
         }
         normal = 1;
         break;
      case 92: // too many symobilc links encountered in resolving pathname
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*open_orig)(const char *, int, ...);
   open_orig = dlsym(RTLD_NEXT, "open");
   // Run normal function and save return value
   int returnVal = open_orig(pathname, flags, mode);
   if (funList[OPEN]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int open64(const char *pathname, int flags, ...) {
   int paramsCnt = 2;
   mode_t mode = 0;
   if (flags & O_CREAT) {
      paramsCnt = 3;
      // Get value of last parameter
      va_list args;
      va_start(args, flags);
      mode = va_arg(args, mode_t);
   }

   // Convert parameters to strings
   char *paramsStr[paramsCnt];
   for (int i = 0; i < paramsCnt; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], pathname);
   sprintf(paramsStr[1], "%d", flags);
   if (paramsCnt == 3) {
      sprintf(paramsStr[2], "%lo", (unsigned long) mode);
   }

   TInMsg *inMsg = initFunction(OPEN64, paramsCnt, paramsStr);
   for (int i = 0; i < paramsCnt; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // requested access to the file is not allowed
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname was too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // a component used as a dircetory in pathname is not a directory
         errno = ENOTDIR;
         break;
      case 70: // O_CREAT is specified, file does not exist and quota of disk blocks has been exhausted
         if (flags & O_CREAT) {
            errno = EDQUOT;
            break;
         }
         normal = 1;
         break;
      case 72: // device containing pathname has no room for the new file
         if (flags & O_CREAT) {
            errno = ENOSPC;
            break;
         }
         normal = 1;
         break;
      case 73: // process has maximum of files open
         errno = EMFILE;
         break;
      case 74: // system has maximum of files open
         errno = ENFILE;
         break;
      case 80: // O_NOATIME was specified, but caller UID did not match owner of the file
         if (flags & O_NOATIME) {
            errno = EPERM;
            break;
         }
         normal = 1;
         break;
      case 81: // write access requested, but pathname refers to a file on read-only system
         if ((flags & O_RDWR) || (flags & O_WRONLY)) {
            errno = EROFS;
            break;
         }
         normal = 1;
         break;
      case 90: // write access requested and pathname refers a directory
         if ((flags & O_RDWR) || (flags & O_WRONLY)) {
            errno = EISDIR;
            break;
         }
         normal = 1;
         break;
      case 91: // O_CREAT and O_EXCL used and file already exists
         if ((flags & O_CREAT) && (flags & O_EXCL)) {
            errno = EEXIST;
            break;
         }
         normal = 1;
         break;
      case 92: // too many symobilc links encountered in resolving pathname
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*open64_orig)(const char *, int, ...);
   open64_orig = dlsym(RTLD_NEXT, "open64");
   int returnVal = open64_orig(pathname, flags, mode);
   if (funList[OPEN64]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int close(int fd) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%d", fd);

   TInMsg *inMsg = initFunction(CLOSE, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // fd isn't a valid open file descriptor
         errno = EBADF;
         break;
      case 20: // I/O error
         errno = EIO;
         break;
      case 50: // call interrupted by a signal
         errno = EINTR;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*close_orig)(int);
   close_orig = dlsym(RTLD_NEXT, "close");
   int returnVal = close_orig(fd);
   if (funList[CLOSE]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

off_t lseek(int fd, off_t offset, int whence) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%jd", offset);
   sprintf(paramsStr[2], "%d", whence);

   TInMsg *inMsg = initFunction(LSEEK, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // fd is not an open file descriptor
         errno = EBADF;
         break;
      case 11: // resulting file offset will be beyond the end of seekable device
         errno = EINVAL;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*lseek_orig)(int, off_t, int);
   lseek_orig = dlsym(RTLD_NEXT, "lseek");
   off_t returnVal = lseek_orig(fd, offset, whence);
   if (funList[LSEEK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%jd", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int creat(const char *pathname, mode_t mode) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], pathname);
   sprintf(paramsStr[1], "%lo", (unsigned long) mode);

   TInMsg *inMsg = initFunction(CREAT, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // requested access to the file is not allowed
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname was too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // a component used as a directory in pathname is not a directory
         errno = ENOTDIR;
         break;
      case 70: // quota of disk blocks has been exhausted
         errno = EDQUOT;
         break;
      case 72: // device containing pathname has no room for the new file
         errno = ENOSPC;
         break;
      case 73: // process has maximum of files open
         errno = EMFILE;
         break;
      case 74: // system has maximum of files open
         errno = ENFILE;
         break;
      case 81: // write access requested, but pathname refers to a file on read-only system
         errno = EROFS;
         break;
      case 90: // write access requested and pathname refers a directory
         errno = EISDIR;
         break;
      case 92: // too many symobilc links encountered in resolving pathname
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*creat_orig)(const char *, mode_t);
   creat_orig = dlsym(RTLD_NEXT, "creat");
   int returnVal = creat_orig(pathname, mode);
   if (funList[CREAT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int creat64(const char *pathname, mode_t mode) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], pathname);
   sprintf(paramsStr[1], "%lo", (unsigned long) mode);

   TInMsg *inMsg = initFunction(CREAT64, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // requested access to the file is not allowed
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname was too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // a component used as a directory in pathname is not a directory
         errno = ENOTDIR;
         break;
      case 70: // quota of disk blocks has been exhausted
         errno = EDQUOT;
         break;
      case 72: // device containing pathname has no room for the new file
         errno = ENOSPC;
         break;
      case 73: // process has maximum of files open
         errno = EMFILE;
         break;
      case 74: // system has maximum of files open
         errno = ENFILE;
         break;
      case 81: // write access requested, but pathname refers to a file on read-only system
         errno = EROFS;
         break;
      case 90: // write access requested and pathname refers a directory
         errno = EISDIR;
         break;
      case 92: // too many symobilc links encountered in resolving pathname
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*creat64_orig)(const char *, mode_t);
   creat64_orig = dlsym(RTLD_NEXT, "creat64");
   int returnVal = creat64_orig(pathname, mode);
   if (funList[CREAT64]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int link(const char *oldpath, const char *newpath) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], oldpath);
   strcpy(paramsStr[1], newpath);

   TInMsg *inMsg = initFunction(LINK, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 20: // I/O error occured
         errno = EIO;
         break;
      case 30: // write access to the directory containing newpath is denied
         errno = EACCES;
         break;
      case 40: // oldpath or newpath point outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // oldpath or newpath is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // direcotry component in oldpath or newpath does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in oldpath or newpath is not a  directory
         errno = ENOTDIR;
         break;
      case 70: // quota of disk blocks has been exhausted
         errno = EDQUOT;
         break;
      case 72: // device containng the file has no room for new directory entry
         errno = ENOSPC;
         break;
      case 75: // file reffered to in oldpath has maximum number of links on it
         errno = EMLINK;
         break;
      case 80: // filesystem containing oldpath and newpath does not support creating hard links
         errno = EPERM;
         break;
      case 81: // file is on read-only filesystem
         errno = EROFS;
         break;
      case 91: // newpath already exists
         errno = EEXIST;
         break;
      case 92: // too many symbolic links encountered in resolving oldpath or newpath
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*link_orig)(const char *, const char *);
   link_orig = dlsym(RTLD_NEXT, "link");
   int returnVal = link_orig(oldpath, newpath);
   if (funList[LINK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int symlink(const char *oldpath, const char *newpath) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], oldpath);
   strcpy(paramsStr[1], newpath);

   TInMsg *inMsg = initFunction(SYMLINK, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 20: // I/O error occured
         errno = EIO;
         break;
      case 30: // write access to the directory containing newpath is denied
         errno = EACCES;
         break;
      case 40: // oldpath or newpath point outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // oldpath or newpath is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // direcotry component in oldpath or newpath does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in oldpath or newpath is not a  directory
         errno = ENOTDIR;
         break;
      case 70: // quota of disk blocks has been exhausted
         errno = EDQUOT;
         break;
      case 72: // device containng the file has no room for new directory entry
         errno = ENOSPC;
         break;
      case 80: // filesystem containing oldpath and newpath does not support creating symbolic links
         errno = EPERM;
         break;
      case 81: // file is on read-only filesystem
         errno = EROFS;
         break;
      case 91: // newpath already exists
         errno = EEXIST;
         break;
      case 92: // too many symbolic links encountered in resolving newpath
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*symlink_orig)(const char *, const char *);
   symlink_orig = dlsym(RTLD_NEXT, "symlink");
   int returnVal = symlink_orig(oldpath, newpath);
   if (funList[SYMLINK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int unlink(const char *pathname) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   strcpy(paramsStr[0], pathname);

   TInMsg *inMsg = initFunction(UNLINK, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 20: // I/O error occured
         errno = EIO;
         break;
      case 30: // write access to the directory containing pathname is denied
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in pathname is not a  directory
         errno = ENOTDIR;
         break;
      case 81: // file is on read-only filesystem
         errno = EROFS;
         break;
      case 90: // pathname refers to a directory
         errno = EISDIR;
         break;
      case 92: // too many symbolic links encountered in resolving pathname
         errno = ELOOP;
         break;
      case 93: // file pathname is being used by another process
         errno = EBUSY;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*unlink_orig)(const char *);
   unlink_orig = dlsym(RTLD_NEXT, "unlink");
   int returnVal = unlink_orig(pathname);
   if (funList[UNLINK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int __xstat(int var, const char *path, struct stat *buf) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], path);
   sprintf(paramsStr[1], "%p", buf);

   TInMsg *inMsg = initFunction(STAT, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // search permission is denied for one of the directories in path
         errno = EACCES;
         break;
      case 40: // bad address
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // path is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in path does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in path is not a directory
         errno = ENOTDIR;
         break;
      case 92: // too many symbolic links when traversing path
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*xstat_orig)(int, const char *, struct stat *);
   xstat_orig = dlsym(RTLD_NEXT, "__xstat");
   int returnVal = xstat_orig(var, path, buf);
   if (funList[STAT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int __lxstat(int var, const char *path, struct stat *buf) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], path);
   sprintf(paramsStr[1], "%p", buf);

   TInMsg *inMsg = initFunction(LSTAT, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // search permission is denied for one of the directories in path
         errno = EACCES;
         break;
      case 40: // bad address
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // path is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in path does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in path is not a directory
         errno = ENOTDIR;
         break;
      case 92: // too many symbolic links when traversing path
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*lxstat_orig)(int, const char *, struct stat *);
   lxstat_orig = dlsym(RTLD_NEXT, "__lxstat");
   int returnVal = lxstat_orig(var, path, buf);
   if (funList[LSTAT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int __fxstat(int var, int fd, struct stat *buf) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%p", buf);

   TInMsg *inMsg = initFunction(FSTAT, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // bad file descriptor
         errno = EBADF;
         break;
      case 40: // bad address
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*fxstat_orig)(int, int, struct stat *);
   fxstat_orig = dlsym(RTLD_NEXT, "__fxstat");
   int returnVal = fxstat_orig(var, fd, buf);
   if (funList[FSTAT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int access(const char *pathname, int mode) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], pathname);
   sprintf(paramsStr[1], "%d", mode);

   TInMsg *inMsg = initFunction(ACCESS, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 20: // I/O error
         errno = EIO;
         break;
      case 30: // requested access is denied for file
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in pathname is not a directory
         errno = ENOTDIR;
         break;
      case 81: // write permission requested on read-only filesystem
         if (mode & W_OK) {
            errno = EROFS;
            break;
         }
         normal = 1;
         break;
      case 92: // too many symbolic links when resolving pathname
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*access_orig)(const char *, int);
   access_orig = dlsym(RTLD_NEXT, "access");
   int returnVal = access_orig(pathname, mode);
   if (funList[ACCESS]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int chmod(const char *path, mode_t mode) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], path);
   sprintf(paramsStr[1], "%lo", (unsigned long) mode);

   TInMsg *inMsg = initFunction(CHMOD, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 20: // I/O error
         errno = EIO;
         break;
      case 30: // search permission denied on a path prefix
         errno = EACCES;
         break;
      case 40: // path points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // path is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // file does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in path is not a directory
         errno = ENOTDIR;
         break;
      case 80: // effective UID does not match owner of the file
         errno = EPERM;
         break;
      case 81: // file is on read-only filesystem
         errno = EROFS;
         break;
      case 92: // too many symbolic links when resolving path
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*chmod_orig)(const char *, mode_t);
   chmod_orig = dlsym(RTLD_NEXT, "chmod");
   int returnVal = chmod_orig(path, mode);
   if (funList[CHMOD]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int fchmod(int fd, mode_t mode) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%lo", (unsigned long) mode);

   TInMsg *inMsg = initFunction(FCHMOD, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // file descriptor not valid
         errno = EBADF;
         break;
      case 20: // I/O error
         errno = EIO;
         break;
      case 80: // effective UID does not match file owner
         errno = EPERM;
         break;
      case 81: // name file is on read-only filesystem
         errno = EROFS;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*fchmod_orig)(int, mode_t);
   fchmod_orig = dlsym(RTLD_NEXT, "fchmod");
   int returnVal = fchmod_orig(fd, mode);
   if (funList[FCHMOD]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int flock(int fd, int operation) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%d", operation);

   TInMsg *inMsg = initFunction(FLOCK, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // fd is not an open file descriptor
         errno = EBADF;
         break;
      case 11: // operation is invalid
         errno = EINVAL;
         break;
      case 50: // call interrupted by a signal
         errno = EINTR;
         break;
      case 76: // kernel ran out of memory for allocating lock records
         errno = ENOLCK;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*flock_orig)(int, int);
   flock_orig = dlsym(RTLD_NEXT, "flock");
   int returnVal = flock_orig(fd, operation);
   if (funList[FLOCK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

DIR *opendir(const char *name) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   strcpy(paramsStr[0], name);

   TInMsg *inMsg = initFunction(OPENDIR, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // fd is not valid descriptor opened for reading
         errno = EBADF;
         break;
      case 30: // permission denied
         errno = EACCES;
         break;
      case 41: // insufficent memory
         errno = ENOMEM;
         break;
      case 61: // directory does not exist
         errno = ENOENT;
         break;
      case 62: // name is not a directory
         errno = ENOTDIR;
         break;
      case 73: // too many file descriptors in use by process
         errno = EMFILE;
         break;
      case 74: // too many files are currently open in the system
         errno = ENFILE;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("NULL");
         return NULL;
      }
   }

   // Normal behavior
   DIR *(*opendir_orig)(const char *);
   opendir_orig = dlsym(RTLD_NEXT, "opendir");
   DIR *returnVal = opendir_orig(name);
   if (funList[OPENDIR]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%p", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

struct dirent *readdir(DIR *dirp) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%p", dirp);

   TInMsg *inMsg = initFunction(READDIR, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // invalid directory stream descriptor dirp
         errno = EBADF;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("NULL");
         return NULL;
      }
   }

   // Normal behavior
   struct dirent *(*readdir_orig)(DIR *);
   readdir_orig = dlsym(RTLD_NEXT, "readdir");
   struct dirent *returnVal = readdir_orig(dirp);
   if (funList[READDIR]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%p", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int closedir(DIR *dirp) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%p", dirp);

   TInMsg *inMsg = initFunction(CLOSEDIR, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // invalid directory stream descriptor dirp
         errno = EBADF;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*closedir_orig)(DIR *);
   closedir_orig = dlsym(RTLD_NEXT, "closedir");
   int returnVal = closedir_orig(dirp);
   if (funList[CLOSEDIR]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int mkdir(const char *pathname, mode_t mode) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], pathname);
   sprintf(paramsStr[1], "%lo", (unsigned long) mode);

   TInMsg *inMsg = initFunction(MKDIR, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // parent directory does not allow write permission
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in pathname is not a  directory
         errno = ENOTDIR;
         break;
      case 70: // quota of disk blocks has been exhausted
         errno = EDQUOT;
         break;
      case 72: // user disk quota is exhausted
         errno = ENOSPC;
         break;
      case 75: // number of links to the parent directory is already maximal
         errno = EMLINK;
         break;
      case 80: // filesystem containing pathname does not support creating directories
         errno = EPERM;
         break;
      case 81: // file is on read-only filesystem
         errno = EROFS;
         break;
      case 91: // pathname already exists
         errno = EEXIST;
         break;
      case 92: // too many symbolic links encountered in resolving pathname
         errno = ELOOP;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*mkdir_orig)(const char *, mode_t);
   mkdir_orig = dlsym(RTLD_NEXT, "mkdir");
   int returnVal = mkdir_orig(pathname, mode);
   if (funList[MKDIR]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int rmdir(const char *pathname) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   strcpy(paramsStr[0], pathname);

   TInMsg *inMsg = initFunction(RMDIR, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 30: // write access to the directory containing pathname was not allowed
         errno = EACCES;
         break;
      case 40: // pathname points outside accessible address space
         errno = EFAULT;
         break;
      case 41: // insuficent kernel memory available
         errno = ENOMEM;
         break;
      case 60: // pathname is too long
         errno = ENAMETOOLONG;
         break;
      case 61: // directory component in pathname does not exist
         errno = ENOENT;
         break;
      case 62: // component used as directory in pathname is not a  directory
         errno = ENOTDIR;
         break;
      case 80: // filesystem containing pathname does not support removal of directories
         errno = EPERM;
         break;
      case 81: // directory is on read-only filesystem
         errno = EROFS;
         break;
      case 92: // too many symbolic links encountered in resolving pathname
         errno = ELOOP;
         break;
      case 93: // pathname is currently in use by other process
         errno = EBUSY;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*rmdir_orig)(const char *);
   rmdir_orig = dlsym(RTLD_NEXT, "rmdir");
   int returnVal = rmdir_orig(pathname);
   if (funList[RMDIR]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int fsync(int fd) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%d", fd);

   TInMsg *inMsg = initFunction(FSYNC, 1, paramsStr);
   free(paramsStr[0]);
   if (inMsg) {
      int normal = 0;
      switch (inMsg->variant) {
      case 10: // fd is not a valid open file descriptor
         errno = EBADF;
         break;
      case 20: // I/O error
         errno = EIO;
         break;
      case 81: // fd is bound to a special file which does not support sync
         errno = EROFS;
         break;
      default:
         normal = 1;
         break;
      }
      if (!normal) {
         sendReturnMsg("-1");
         return -1;
      }
   }

   // Normal behavior
   int (*fsync_orig)(int);
   fsync_orig = dlsym(RTLD_NEXT, "fsync");
   int returnVal = fsync_orig(fd);
   if (funList[FSYNC]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

////////////////////////////////////////////////////////////////////////////////
// Notification-only calls
////////////////////////////////////////////////////////////////////////////////

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
   // Convert parameters to strings
   char *paramsStr[6];
   for (int i = 0; i < 6; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%p", addr);
   sprintf(paramsStr[1], "%zu", length);
   sprintf(paramsStr[2], "%d", prot);
   sprintf(paramsStr[3], "%d", flags);
   sprintf(paramsStr[4], "%d", fd);
   sprintf(paramsStr[5], "%jd", length);

   initFunction(MMAP, 6, paramsStr);
   for (int i = 0; i < 6; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   void *(*mmap_orig)(void *, size_t, int, int, int, off_t);
   mmap_orig = dlsym(RTLD_NEXT, "mmap");
   void *returnVal = mmap_orig(addr, length, prot, flags, fd, offset);
   if (funList[MMAP]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%p", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int munmap(void *addr, size_t length) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%p", addr);
   sprintf(paramsStr[1], "%zu", length);

   initFunction(MUNMAP, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*munmap_orig)(void *, size_t);
   munmap_orig = dlsym(RTLD_NEXT, "munmap");
   int returnVal = munmap_orig(addr, length);
   if (funList[MUNMAP]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int mlock(const void *addr, size_t length) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%p", addr);
   sprintf(paramsStr[1], "%zu", length);

   initFunction(MLOCK, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*mlock_orig)(const void *, size_t);
   mlock_orig = dlsym(RTLD_NEXT, "mlock");
   int returnVal = mlock_orig(addr, length);
   if (funList[MLOCK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int munlock(const void *addr, size_t length) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%p", addr);
   sprintf(paramsStr[1], "%zu", length);

   initFunction(MUNLOCK, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*munlock_orig)(const void *, size_t);
   munlock_orig = dlsym(RTLD_NEXT, "munlock");
   int returnVal = munlock_orig(addr, length);
   // Send return value to the controller
   char *returnValStr = malloc(20 * sizeof(char));
   sprintf(returnValStr, "%d", returnVal);
   sendReturnMsg(returnValStr);
   free(returnValStr);
   // Return value
   return returnVal;
}

int mlockall(int flags) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%d", flags);

   initFunction(MLOCKALL, 1, paramsStr);
   free(paramsStr[0]);

   // Normal behavior
   int (*mlockall_orig)(int);
   mlockall_orig = dlsym(RTLD_NEXT, "mlockall");
   int returnVal = mlockall_orig(flags);
   if (funList[MLOCKALL]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int munlockall(void) {
   initFunction(MUNLOCKALL, 0, NULL);

   // Normal behavior
   int (*munlockall_orig)();
   munlockall_orig = dlsym(RTLD_NEXT, "munlockall");
   int returnVal = munlockall_orig();
   if (funList[MUNLOCKALL]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int brk(void *addr) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%p", addr);

   initFunction(BRK, 1, paramsStr);
   free(paramsStr[0]);

   // Normal behavior
   int (*brk_orig)(void *);
   brk_orig = dlsym(RTLD_NEXT, "brk");
   int returnVal = brk_orig(addr);
   if (funList[BRK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
   // Convert parameters to strings
   char *paramsStr[5];
   for (int i = 0; i < 5; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", nfds);
   sprintf(paramsStr[1], "%p", readfds);
   sprintf(paramsStr[2], "%p", writefds);
   sprintf(paramsStr[3], "%p", exceptfds);
   sprintf(paramsStr[4], "%p", timeout);

   initFunction(SELECT, 5, paramsStr);
   for (int i = 0; i < 5; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*select_orig)(int, fd_set *, fd_set *, fd_set *, struct timeval *);
   select_orig = dlsym(RTLD_NEXT, "select");
   int returnVal = select_orig(nfds, readfds, writefds, exceptfds, timeout);
   if (funList[SELECT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%p", fds);
   sprintf(paramsStr[1], "%d", (int) nfds);
   sprintf(paramsStr[2], "%d", timeout);

   initFunction(POLL, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*poll_orig)(struct pollfd *, nfds_t, int);
   poll_orig = dlsym(RTLD_NEXT, "poll");
   int returnVal = poll_orig(fds, nfds, timeout);
   if (funList[POLL]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int dup(int oldfd) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%d", oldfd);

   initFunction(DUP, 1, paramsStr);
   free(paramsStr[0]);

   // Normal behavior
   int (*dup_orig)(int);
   dup_orig = dlsym(RTLD_NEXT, "dup");
   int returnVal = dup_orig(oldfd);
   if (funList[DUP]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int dup2(int oldfd, int newfd) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", oldfd);
   sprintf(paramsStr[1], "%d", newfd);

   initFunction(DUP2, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*dup2_orig)(int, int);
   dup2_orig = dlsym(RTLD_NEXT, "dup2");
   int returnVal = dup2_orig(oldfd, newfd);
   if (funList[DUP2]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int shmget(key_t key, size_t size, int shmflg) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", key);
   sprintf(paramsStr[1], "%zu", size);
   sprintf(paramsStr[2], "%d", shmflg);

   initFunction(SHMGET, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*shmget_orig)(key_t, ssize_t, int);
   shmget_orig = dlsym(RTLD_NEXT, "shmget");
   int returnVal = shmget_orig(key, size, shmflg);
   if (funList[SHMGET]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int chown(const char *path, uid_t owner, gid_t group) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], path);
   sprintf(paramsStr[1], "%d", owner);
   sprintf(paramsStr[2], "%d", group);

   initFunction(CHOWN, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*chown_orig)(const char *, uid_t, gid_t);
   chown_orig = dlsym(RTLD_NEXT, "chown");
   int returnVal = chown_orig(path, owner, group);
   if (funList[CHOWN]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int fchown(int fd, uid_t owner, gid_t group) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   sprintf(paramsStr[0], "%d", fd);
   sprintf(paramsStr[1], "%d", owner);
   sprintf(paramsStr[2], "%d", group);

   initFunction(FCHOWN, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*fchown_orig)(int, uid_t, gid_t);
   fchown_orig = dlsym(RTLD_NEXT, "fchown");
   int returnVal = fchown_orig(fd, owner, group);
   if (funList[FCHOWN]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int lchown(const char *path, uid_t owner, gid_t group) {
   // Convert parameters to strings
   char *paramsStr[3];
   for (int i = 0; i < 3; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], path);
   sprintf(paramsStr[1], "%d", owner);
   sprintf(paramsStr[2], "%d", group);

   initFunction(LCHOWN, 3, paramsStr);
   for (int i = 0; i < 3; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*lchown_orig)(const char *, uid_t, gid_t);
   lchown_orig = dlsym(RTLD_NEXT, "lchown");
   int returnVal = lchown_orig(path, owner, group);
   if (funList[LCHOWN]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int mount(const char *source, const char *target, const char *filesystemtype,
      unsigned long mountflags, const void *data) {
   // Convert parameters to strings
   char *paramsStr[5];
   for (int i = 0; i < 5; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], source);
   strcpy(paramsStr[1], target);
   strcpy(paramsStr[2], filesystemtype);
   sprintf(paramsStr[3], "%lu", mountflags);
   sprintf(paramsStr[4], "%p", data);

   initFunction(MOUNT, 5, paramsStr);
   for (int i = 0; i < 5; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*mount_orig)(const char *, const char *, const char *, unsigned long, const void *);
   mount_orig = dlsym(RTLD_NEXT, "mount");
   int returnVal = mount_orig(source, target, filesystemtype, mountflags, data);
   if (funList[MOUNT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int umount(const char *target) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   strcpy(paramsStr[0], target);

   initFunction(UMOUNT, 1, paramsStr);
   free(paramsStr[0]);

   // Normal behavior
   int (*umount_orig)(const char *);
   umount_orig = dlsym(RTLD_NEXT, "umount");
   int returnVal = umount_orig(target);
   if (funList[UMOUNT]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

int umount2(const char *target, int flags) {
   // Convert parameters to strings
   char *paramsStr[2];
   for (int i = 0; i < 2; ++i) {
      paramsStr[i] = malloc(100 * sizeof(char));
   }
   strcpy(paramsStr[0], target);
   sprintf(paramsStr[1], "%d", flags);

   initFunction(UMOUNT2, 2, paramsStr);
   for (int i = 0; i < 2; ++i) {
      free(paramsStr[i]);
   }

   // Normal behavior
   int (*umount2_orig)(const char *, int);
   umount2_orig = dlsym(RTLD_NEXT, "umount2");
   int returnVal = umount2_orig(target, flags);
   if (funList[UMOUNT2]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%d", returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

mode_t umask(mode_t mask) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%lo", (unsigned long) mask);

   initFunction(UMASK, 1, paramsStr);
   free(paramsStr[0]);

   // Normal behavior
   mode_t (*umask_orig)(mode_t);
   umask_orig = dlsym(RTLD_NEXT, "umask");
   mode_t returnVal = umask_orig(mask);
   if (funList[UMASK]) {
      // If call is monitored or controlled, send return value to the controller
      char *returnValStr = malloc(20 * sizeof(char));
      sprintf(returnValStr, "%lo", (unsigned long) returnVal);
      sendReturnMsg(returnValStr);
      free(returnValStr);
   }
   // Return value
   return returnVal;
}

void rewinddir(DIR *dirp) {
   // Convert parameters to strings
   char *paramsStr[1];
   paramsStr[0] = malloc(100 * sizeof(char));
   sprintf(paramsStr[0], "%p", dirp);

   initFunction(REWINDDIR, 1, paramsStr);
   free(paramsStr[0]);

   // Normal behavior
   void (*rewinddir_orig)(DIR *);
   rewinddir_orig = dlsym(RTLD_NEXT, "rewinddir");
   if (funList[REWINDDIR]) {
      // If call is monitored or controlled, send return value to the controller
      sendReturnMsg("");
   }
   return rewinddir_orig(dirp);
}

void sync(void) {
   initFunction(SYNC, 0, NULL);

   // Normal behavior
   void (*sync_orig)(void);
   sync_orig = dlsym(RTLD_NEXT, "sync");
   if (funList[SYNC]) {
      // If call is monitored or controlled, send return value to the controller
      sendReturnMsg("");
   }
   return sync_orig();
}
