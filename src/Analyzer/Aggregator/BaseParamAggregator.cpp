/**
 * @file BaseParamAggregator.cpp
 *
 * Contains implementation of methods of the class BaseParamAggregator.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date Apr 7, 2014
 *
 * Created on: Apr 7, 2014
 */

#include "Aggregator/BaseParamAggregator.h"
#include "Param.h"

/**
 * @brief Constructor with same parameter as parent constructor.
 * Only calls parent constructor Aggregator::Aggregator.
 * @param subroutineSize Minimal subroutine size.
 * @param path Output file destination.
 */
BaseParamAggregator::BaseParamAggregator(unsigned subroutineSize, std::string path) :
      Aggregator(subroutineSize, path) {
}

/**
 * @brief Default destructor.
 */
BaseParamAggregator::~BaseParamAggregator() {
}

/**
 * @brief Converts call to string.
 * The string is created from function name and base parameter value. Other parameters are not
 * inserted (but commas between parameters are).
 * @param call Call to be converted.
 * @return String representation of the call.
 */
std::string BaseParamAggregator::toString(Call *call) {
   std::string result(call->getName());
   result += "(";
   // Get function from call
   functions fun = call->getFunction();
   // If base parameter of function exists, find it
   if (baseParamMap.find(fun) != baseParamMap.end()) {
      int paramNum = baseParamMap[fun];
      // Write parameters list, writing only main parameter value
      for (int i = 0; i < call->paramsCnt(); ++i) {
         if (paramNum == i)
            result += call->getParam(i)->getValue();
         if (i != call->paramsCnt() - 1)
            result += ",";
      }
   }
   result += ")";
   return result;
}

/**
 * Static dictionaty initialization.
 */
std::map<functions, int> BaseParamAggregator::baseParamMap = {
      { READ, 0 },
      { WRITE, 0 },
      { OPEN, 0 },
      { OPEN64, 0 },
      { CLOSE, 0 },
      { LSEEK, 0 },
      { CREAT, 0 },
      { CREAT64, 0 },
      { LINK, 1 },
      { SYMLINK, 1 },
      { UNLINK, 0 },
      { STAT, 0 },
      { LSTAT, 0 },
      { FSTAT, 0 },
      { ACCESS, 0 },
      { CHMOD, 0 },
      { FCHMOD, 0 },
      { FLOCK, 0 },
      { OPENDIR, 0 },
      { MKDIR, 0 },
      { RMDIR, 0 },
      { FSYNC, 0 },
      { MMAP, 1 },
      { MUNMAP, 1 },
      { MLOCK, 1 },
      { MUNLOCK, 1 },
      { MLOCKALL, 0 },
      { DUP, 0 },
      { DUP2, 0 },
      { SHMGET, 1 },
      { CHOWN, 0 },
      { FCHOWN, 0 },
      { LCHOWN, 0 },
      { MOUNT, 0 },
      { UMOUNT, 0 },
      { UMOUNT2, 0 },
      { UMASK, 0 } };

