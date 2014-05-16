/**
 * @file BFSScheduler.cpp
 *
 * Contains implementation of methods of the class BFSScheduler.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 12, 2014
 */

#include "Scheduler/BFSScheduler.h"
#include "Controller.h"

/**
 * @brief Constructor with members initialization.
 * Calls parent constructor from Scheduler. All indexes are set to 0.
 * @param c Controller pointer.
 * @param groups List of group variants used.
 */
BFSScheduler::BFSScheduler(Controller *c, std::vector<std::string> groups) :
      Scheduler(c, groups), currentRun(0), currentCall(0), currentVariant(0), currentCallDepth(0), nextCall(
            false) {
}

/**
 * @brief Default destructor.
 */
BFSScheduler::~BFSScheduler() {
}

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
OutboundMsg *BFSScheduler::scheduleCall(Call *call, Run *run, unsigned callNum) {
   OutboundMsg *outMsg;
   if (callNum == currentCall) {
      // Call where expansion is being done

      // Get next variant (if no variant is available for current function, set it to 0)
      int variant =
            currentVariant != variants[call->getFunction()].size() ?
                  variants[call->getFunction()][currentVariant++] : 0;
      // Set flow depth to next call (call after expanded one)
      run->setDepth(currentCall + 1);
      // Set call variant
      call->setVariant(variant);
      // Return message with next variant
      outMsg = new OutboundMsg(EXEC, call->getName(), variant);
      if (currentVariant == variants[call->getFunction()].size()) {
         // After last variant, go to next call in flow
         nextCall = true;
      }
   }
   else if (callNum < controller->getRun(currentRun)->getDepth()) {
      // Calls before expansion call

      // Get variant of corresponding call in current run
      int variant = controller->getRun(currentRun)->getCall(callNum)->getVariant();
      // Set call variant
      call->setVariant(variant);
      // Send message with same variant as was used in corresponding call
      outMsg = new OutboundMsg(EXEC, call->getName(), variant);

      //  If the call is last in run, go to next run
      if (callNum == controller->getRun(currentRun)->getSize() - 1) {
         goToNextRun();
      }
   }
   else {
      // Calls after expansion call

      // Normal behavior
      outMsg = new OutboundMsg(EXEC, call->getName(), 0);
   }
   return outMsg;
}

/**
 * @brief Inform about call that cannot be controlled.
 * Implements pure virtual method Scheduler::notifCall.
 * Simply goes to the next call (incremets ::currentCall), eventually switches to next run.
 * @param callNum
 */
void BFSScheduler::notifCall(unsigned callNum) {
   // Flow must not be the first run
   if (controller->getRunsCnt() != 0) {
      // In case the call should be expanded, simply go to next call
      if (callNum == currentCall) {
         if (++currentCall == controller->getRun(currentRun)->getSize()) {
            goToNextRun();
         }
      }
   }
}

/**
 * @brief Informs scheduler that next call will be starting new run.
 * Increments ::currentCall in case ::nextCall is true (meaning that all variants for current
 * call have already been tried).
 */
void BFSScheduler::nextRun() {
   // If all variants for current call have been used, switch to next call
   if (nextCall) {
      nextCall = false;
      // Next call will start at variant on index 0
      currentVariant = 0;
      ++currentCall;
      // After last call in the run, go to next run
      if (currentCall == controller->getRun(currentRun)->getSize()) {
         goToNextRun();
      }
   }
}

/**
 * @brief Take next run as model.
 * If there is no next run, end the analysis.
 */
void BFSScheduler::goToNextRun() {
   if (++currentRun == controller->getRunsCnt()) {
      // After last run, end analysis
      controller->setCompleted();
   }
   else {
      // Switch to next run
      currentVariant = 0;
      // In next run, expansion starts after last expanded call (run depth)
      currentCall = controller->getRun(currentRun)->getDepth();
   }
}

