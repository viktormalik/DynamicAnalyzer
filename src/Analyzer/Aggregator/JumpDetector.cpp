/**
 * @file JumpDetector.cpp
 *
 * Contains implementation of methods of the class JumpDetector.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 19, 2014
 */

#include "Aggregator/JumpDetector.h"
#include "Aggregator.h"
#include "Trace.h"

#include <vector>
#include <algorithm>

/**
 * @brief Constructor with members initialization.
 * Sets minimal subroutine size and pointer to aggregator.
 * @param s Minimal subroutine size.
 * @param a Aggregator.
 */
JumpDetector::JumpDetector(int s, Aggregator *a) :
      minSize(s), aggregator(a), traceStr() {
}

/**
 * @brief Default destructor.
 */
JumpDetector::~JumpDetector() {
}

/**
 * @brief Finds jumps in the graph in aggregator and modifies it.
 * At first finds last trace of minimal subroutine size. Then searches this trace in hash table
 * and if it has already been in the table, creates new link from trace beginning to found
 * trace beginning.
 */
void JumpDetector::findJumps() {
   // Find last trace of minimal subroutine size
   getTrace(aggregator->currentNode);
   if (!indexes.empty()) {
      // Find trace in hash table
      std::vector<unsigned> jumpDest = table.findTrace(traceStr, indexes);
      if (!jumpDest.empty()) {
         // Trace was found in the hash table, jumpDest contains indexes of found trace

         // Last trace must be all created in current program run
         if (std::equal(indexes.begin(), indexes.end(), aggregator->insertedNodes.end() - minSize)
               && indexes.front() - jumpDest.front() >= minSize) {

            // Delete the trace from graph and change corresponding traces in hash table
            for (unsigned i = 0; i < minSize; ++i) {

               // If a trace has been created when the node has been inserted, change the trace to
               // correspond with the new jump
               getTrace(aggregator->currentNode);
               if (!indexes.empty())
                  table.changeTrace(traceStr, indexes, i, jumpDest);

               // Delete node from graph
               aggregator->currentNode =
                     aggregator->graph[aggregator->currentNode]->getPredecessor();
               aggregator->deleteLastNode();
               aggregator->insertedNodes.pop_back();
            }
            // Delete successor that has been replaced by jump (it is always last successor because
            // the trace has been created in current run)
            aggregator->graph[aggregator->currentNode]->deleteSuccessor();
            // Create jump
            aggregator->graph[aggregator->currentNode]->addSuccessor(jumpDest.front());
            // Set current node to the end of destination trace
            aggregator->currentNode = jumpDest.back();
         }
      }
   }
}

/**
 * @brief Get the trace of size equal to minimal subroutine size ending at given index.
 * @param node Ending node of the trace requested.
 */
void JumpDetector::getTrace(unsigned startNode) {
   indexes.clear();
   unsigned nodeIndex = startNode;
   // Array of indexes
   unsigned path[minSize];
   // Fill the array of indexes from back to front
   for (int i = minSize - 1; i >= 0; --i) {
      path[i] = nodeIndex;
      nodeIndex = aggregator->graph[nodeIndex]->getPredecessor();
   }
   // Trace must contain correct number of nodes (must not start at node 0)
   if (path[0] != 0) {
      // Create vector from array of indexes and store it into object variable
      for (unsigned i = 0; i < minSize; ++i) {
         indexes.push_back(path[i]);
      }
      // Create trace from vector of indexes and store its string representation into object
      // variable
      Trace trace;
      for (auto i : indexes) {
         trace.nextCall(aggregator->graph[i]->getCall());
      }
      traceStr = aggregator->toString(&trace);
   }
}
