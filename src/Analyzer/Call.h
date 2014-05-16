/**
 * @file Call.h
 *
 * @brief Call class.
 *
 * Contains definition of the class Call, representing a single call of a function in tested
 * program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 07.03.2014
 *
 * Created on: Feb 14, 2014
 */

#ifndef CALL_H_
#define CALL_H_

#include "Param.h"

#include <string>
#include <vector>
#include <map>

/**
 * Enumeration type representing all supported functions.
 */
enum functions {
   READ, WRITE, OPEN, OPEN64, CLOSE, LSEEK, CREAT, CREAT64, LINK, SYMLINK, UNLINK, STAT, LSTAT,
   FSTAT, ACCESS, CHMOD, FCHMOD, FLOCK, OPENDIR, READDIR, CLOSEDIR, MKDIR, RMDIR, FSYNC,

   MMAP, MUNMAP, MLOCK, MUNLOCK, MLOCKALL, MUNLOCKALL, BRK, SELECT, POLL, DUP, DUP2, SHMGET, CHOWN,
   FCHOWN, LCHOWN, MOUNT, UMOUNT, UMOUNT2, UMASK, REWINDDIR, SYNC
};

/**
 * Type representing a map mapping string into a function.
 * Used for dictionary converting function name into value from ::functions.
 */
typedef std::map<std::string, functions> TFunMap;

/**
 * @brief Call of a function in tested program.
 *
 * Class representing one single call of a library function in tested program.
 * Provides information about called function and specified parameters.
 */
class Call {
public:
   /**
    * @brief Default constructor.
    * Creates empty call.
    */
   Call();
   /**
    * @brief Default destructor.
    * Is responsible for deleting all call parameters.
    */
   virtual ~Call();
   /**
    * @brief Sets function by name.
    * Sets the function name and also the function value using dictionary ::functionsMap.
    * @param name Name of the function to be set to.
    */
   void setFunctionName(std::string name);
   /**
    * @brief Gets the function.
    * @return The function value of type ::functions.
    */
   functions getFunction();
   /**
    * @brief Gets the function name.
    * @return Name of the function.
    */
   std::string getName();
   /**
    * @brief Adds new parameter to the call.
    * @param p Pointer to the new parameter.
    */
   void addParam(Param *p);
   /**
    * @brief Gets parameter on given index.
    * @param index Index of parameter.
    * @return Pointer to parameter object.
    */
   Param *getParam(unsigned index);
   /**
    * @brief Gets parameters count.
    * @return Number of parameters of the call.
    */
   int paramsCnt();
   /**
    * @brief Sets the return value of the call.
    * @param val String containing return value of the call.
    */
   void setReturnVal(std::string val);
   /**
    * @brief Gets the return value of the call.
    * @return String with the return value of the function.
    */
   std::string getReturnVal();
   /**
    * @brief Sets the variant of the call.
    * Sets which variant has been used to the call in the flow it belongs to.
    * @param v The number of variant.
    */
   void setVariant(int v);
   /**
    * @brief Gets the variant used for the call.
    * @return Number of variant.
    */
   int getVariant();

   /**
    * Prints the parameters of the call to standard output.
    */
   void printParams() {
      for (std::vector<Param *>::iterator it = params.begin(); it != params.end(); ++it) {
         (*it)->print();
      }
   }

   /**
    * @brief Checks if the function of given name is valid.
    * Tries to find the function in ::functionsMap.
    * @param fun String with function name.
    * @return True if function was found, otherwise false.
    */
   static bool functionExists(std::string fun);

   /**
    * @brief Checks if function can be controlled.
    * Tries to find the function in ::controlFunctions
    * @param fun String with function name.
    * @return True if function can be controlled, otherwise false.
    */
   static bool isControlFunction(std::string fun);

protected:
   functions function; /**< The function called */
   std::string functionName; /**< Name of the function called */
   std::vector<Param *> params; /**< Vector of parameters of the call */
   std::string returnValue; /**< Return value of the call */
   int usedVariant; /**< A variant used for the call in the flow that the call belongs to */

private:
   /**
    * A dictionary mapping function names to the values of enum ::functions
    */
   static TFunMap functionsMap;
   /**
    * Vector containing names of functions that can be both controlled and monitored.
    */
   static std::vector<std::string> controlFunctions;
};

#endif /* CALL_H_ */
