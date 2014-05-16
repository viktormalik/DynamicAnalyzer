/**
 * @file BFSScheduler.h
 *
 * @brief BFSScheduler class.
 *
 * Contains definition of the class BFSScheduler, a scheduler using strategy similar to
 * Breadth-first search for exploring all branches of tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 12, 2014
 */

#ifndef BFSSCHEDULER_H_
#define BFSSCHEDULER_H_

#include "Scheduler/Scheduler.h"

/**
 * @brief Scheduler using strategy similar to Breadth-first search.
 * The strategy is based on selecting runs from controller and then for all calls in the run,
 * trying all possible variants. That causes creating new runs, which are then selected.
 * Class keeps indexes of current run, current call in that run and current variant being used
 * for that call. By incrementing those indexes, it tries all possibilities for all calls in all
 * runs.
 * Algorithm ends when there is no new run left.
 * @see Scheduler
 */
class BFSScheduler: public Scheduler {
public:
   /**
    * @brief Constructor with members initialization.
    * Calls parent constructor from Scheduler. All indexes are set to 0.
    * @param c Controller pointer.
    * @param groups List of group variants used.
    */
   BFSScheduler(Controller *c, std::vector<std::string> groups);
   /**
    * @brief Default destructor.
    */
   virtual ~BFSScheduler();

   /**
    * @brief Schedule call that can be controlled by analyzer.
    * Implements pure virtual method Scheduler::scheduleCall.
    * If call number is same as ::currentCall, does call expansion, which means that it returns
    * outbound message with first variant that has not been tried yet (given by ::currentVariant).
    * If call number is less than ::currentCall (call is before the expanded one), used variant is
    * copied from the run given by ::currentRun.
    * For other calls, use default variant 0 (meaning standard behavior of the call).
    * @param call Call to be scheduled.
    * @param run Actual run of the tested program.
    * @param callNum Number of call in actual run.
    * @return Outbound message with variant number for the call.
    */
   OutboundMsg *scheduleCall(Call *call, Run *run, unsigned callNum);
   /**
    * @brief Inform about call that cannot be controlled.
    * Implements pure virtual method Scheduler::notifCall.
    * Simply goes to the next call (incremets ::currentCall), eventually switches to next run.
    * @param callNum
    */
   void notifCall(unsigned callNum);
   /**
    * @brief Informs scheduler that next call will be starting new run.
    * Increments ::currentCall in case ::nextCall is true (meaning that all variants for current
    * call have already been tried).
    */
   void nextRun();
protected:
   unsigned currentRun; /**< Number of run already received used as model for expansion
                          * (index into vector Controller::runs */
   unsigned currentCall; /**< Index of call in current run that shall be expanded */
   unsigned currentVariant; /**< Number of variant that will be used for expansion as next
                             * (index into map ::variants */
   unsigned currentCallDepth; /**< Depth of current run, @see Run::depth */
   bool nextCall; /**< Boolean meaning that all variants for current call have been used and next
                   * call should be set in new run */
private:
   /**
    * @brief Take next run as model.
    * If there is no next run, end the analysis.
    */
   void goToNextRun();
};

#endif /* BFSSCHEDULER_H_ */
