/**
 * @file Scheduler.h
 *
 * @brief Scheduler class.
 *
 * Contains definition of the class Scheduler, which is responsible for scheduling the behavior
 * of the calls in tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 12, 2014
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "Parser/OutboundMsg.h"
#include "Call.h"
#include "Run.h"

/**
 * Different groups of variants used for changing the flow of tested program.
 */
enum varGroups {
   INVAL, /**< Errors of invalid values as parameters of the call (bad file descriptor - EBADF,
           * invalid value - EINVAL) */
   IO, /**< Input-output error (EIO) */
   ACCESSRIGHTS, /**< Error in access rights (EACCES) */
   MEMORY, /**< Errors connected to memory (invalid pointer - EFAULT, unsufficent memory - ENOMEM) */
   INTERRUPT, /**< Interruption of the call by external signal (EINTR) */
   PATH, /**< Errors in file path given as parameter of the call (directory is not in fact
          * a directory - ENOTDIR, directory does not exist - ENOENT) */
   LIMITS, /**< Errors with unsufficent limits (maximum files open, file too large, ...) */
   PERMISSIONS, /**< Errors with permissions of the filesystem (read-only filesystem - EROFS,
                 * effect of the call is not supported - EPERM */
   DESTFILE /**< Different errors with file given as parameter (too many symbolic links,
             * file busy, ...) */
};

/**
 * Type representing two-dimensional map, mapping function type and variant groups into vector
 * of variants (integers) corresponding to the couple function-variant group.
 */
typedef std::map<functions, std::map<varGroups, std::vector<int> > > TVarMap;

// Forward declaration of class Controller (for linker)
class Controller;

/**
 * @brief Class scheduling the behavior of the calls executed in tested program.
 *
 * Class is responsible for creating responses to the messages with calls coming from tested
 * program. It alternates call's normal behavior in order to explore different branches of the
 * tested program.
 * Class is abstract, defines interface for inherited classes, so each of them can use different
 * scheduling strategy.
 * Class is also responsible for creating map of variants used, depending on variant groups
 * specified in configuration file.
 */
class Scheduler {
public:
   /**
    * @brief Constructor with filling variants map./
    * Initializes pointer to controller and creates variants map.
    * Variants map is created from two-dimensional static function-variant groups map by selecting
    * only those variant groups that are given.
    * @param c Pointer to controller.
    * @param groups Vector of variant group names that shall be used during analysis.
    */
   Scheduler(Controller *c, std::vector<std::string> groups);
   /**
    * @brief Default destructor.
    */
   virtual ~Scheduler();

   /**
    * @brief Create reaction to call that can be controlled by analyzer.
    * Used for calls that arrived in message of type CONTROL.
    * @param call Call to be scheduled.
    * @param run Current run of tested program.
    * @param callNum Number of call in current run.
    * @return Outbound message containing variant of the call that is to be run by tested program.
    */
   virtual OutboundMsg *scheduleCall(Call *call, Run *run, unsigned callNum) = 0;
   /**
    * @brief Nofitication about a call that cannot be controlled.
    * Used for calls that arrived in message of type NOTIFY.
    * @param callNum Number of the call in current run of tested program.
    */
   virtual void notifCall(unsigned callNum) = 0;
   /**
    * @brief Informs scheduler that next call will be starting new run.
    */
   virtual void nextRun() = 0;
protected:
   Controller *controller; /**< Pointer to the controller where the scheduler has been created */
   /**
    * Map mapping functions to the list of variants that will be used in current analysis.
    */
   std::map< functions, std::vector<int> > variants;
private:
   /**
    * Static map containing all posible variants for all possible calls separated by groups.
    */
   static TVarMap variationsMap;
   /**
    * @brief Function to create variations map.
    * @return Static variations map.
    */
   static TVarMap createVarMap();
   /**
    * Dictionary mapping variant groups names to their values from ::varGroups.
    */
   static std::map<std::string, varGroups> strToGroup;
};

#endif /* SCHEDULER_H_ */
