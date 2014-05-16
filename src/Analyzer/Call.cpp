/**
 * @file Call.cpp
 *
 * File containing implementation of methods of the class Call.
 *
 * @author Viktor Malik
 *
 * @date 01.04.2014
 *
 * Created on: Feb 14, 2014
 */

#include "Call.h"

#include <algorithm>

/**
 * @brief Default constructor.
 * Creates empty call.
 */
Call::Call() :
      function(), functionName(""), params(), returnValue(""), usedVariant(0) {
}

/**
 * @brief Default destructor.
 * Is responsible for deleting all call parameters.
 */
Call::~Call() {
   for (auto param : params) {
      delete param;
   }
}

/**
 * @brief Sets function by name.
 * Sets the function name and also the function value using dictionary ::functionsMap.
 * @param name Name of the function to be set to.
 */
void Call::setFunctionName(std::string name) {
   functionName = name;
   function = functionsMap[name];
}

/**
 * @brief Gets the function.
 * @return The function value of type ::functions.
 */
functions Call::getFunction() {
   return function;
}

/**
 * @brief Gets the function name.
 * @return Name of the function.
 */
std::string Call::getName() {
   return functionName;
}

/**
 * @brief Adds new parameter to the call.
 * @param p Pointer to the new parameter.
 */
void Call::addParam(Param *p) {
   params.push_back(p);
}

/**
 * @brief Gets parameter on given index.
 * @param index Index of parameter.
 * @return Pointer to parameter object.
 */
Param *Call::getParam(unsigned index) {
   if (index >= 0 && index < params.size())
      return params[index];
   else
      return NULL;
}

/**
 * @brief Gets parameters count.
 * @return Number of parameters of the call.
 */
int Call::paramsCnt() {
   return params.size();
}

/**
 * @brief Sets the return value of the call.
 * @param val String containing return value of the call.
 */
void Call::setReturnVal(std::string val) {
   returnValue = val;
}

/**
 * @brief Gets the return value of the call.
 * @return String with the return value of the function.
 */
std::string Call::getReturnVal() {
   return returnValue;
}

/**
 * @brief Sets the variant of the call.
 * Sets which variant has been used to the call in the flow it belongs to.
 * @param v The number of variant.
 */
void Call::setVariant(int v) {
   usedVariant = v;
}

/**
 * @brief Gets the variant used for the call.
 * @return Number of variant.
 */
int Call::getVariant() {
   return usedVariant;
}

/**
 * @brief Checks if the function of given name is valid.
 * Tries to find the function in ::functionsMap.
 * @param String with function name.
 * @return True if function was found, otherwise false.
 */
bool Call::functionExists(std::string fun){
   return (functionsMap.find(fun) != functionsMap.end()) ? true : false;
}
/**
 * @brief Checks if function can be controlled.
 * Tries to find the function in ::controlFunctions
 * @param fun String with function name.
 * @return True if function can be controlled, otherwise false.
 */
bool Call::isControlFunction(std::string fun){
   return (std::find(controlFunctions.begin(), controlFunctions.end(), fun)
      != controlFunctions.end()) ? true : false;
}

/**
 * Initialization of static dictionary used for mapping function names to function values from
 * enum ::functions.
 */
TFunMap Call::functionsMap = {
      { "read", READ },
      { "write", WRITE },
      { "open", OPEN },
      { "open64", OPEN64 },
      { "close", CLOSE },
      { "lseek", LSEEK },
      { "creat", CREAT },
      { "creat64", CREAT64 },
      { "link", LINK },
      { "symlink", SYMLINK },
      { "unlink", UNLINK },
      { "stat", STAT },
      { "lstat", LSTAT },
      { "fstat", FSTAT },
      { "access", ACCESS },
      { "chmod", CHMOD },
      { "fchmod", FCHMOD },
      { "flock", FLOCK },
      { "opendir", OPENDIR },
      { "readdir", READDIR },
      { "closedir", CLOSEDIR },
      { "mkdir", MKDIR },
      { "rmdir", RMDIR },
      { "fsync", FSYNC },
      { "mmap", MMAP },
      { "munmap", MUNMAP },
      { "mlock", MLOCK },
      { "munlock", MUNLOCK },
      { "mlockall", MLOCKALL },
      { "munlockall", MUNLOCKALL },
      { "brk", BRK },
      { "select", SELECT },
      { "poll", POLL },
      { "dup", DUP },
      { "dup2", DUP2 },
      { "shmget", SHMGET },
      { "chown", CHOWN },
      { "fchown", FCHOWN },
      { "lchown", LCHOWN },
      { "mount", MOUNT },
      { "umount", UMOUNT },
      { "umount2", UMOUNT2 },
      { "umask", UMASK },
      { "rewinddir", REWINDDIR },
      { "sync", SYNC } };

std::vector<std::string> Call::controlFunctions = {
      "read",
      "write",
      "open",
      "open64",
      "close",
      "lseek",
      "creat",
      "creat64",
      "link",
      "symlink",
      "unlink",
      "stat",
      "lstat",
      "fstat",
      "access",
      "chmod",
      "fchmod",
      "flock",
      "opendir",
      "readdir",
      "closedir",
      "mkdir",
      "rmdir",
      "fsync"
};
