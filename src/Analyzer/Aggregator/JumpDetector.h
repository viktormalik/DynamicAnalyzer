/**
 * @file JumpDetector.h
 *
 * @brief JumpDetector class.
 *
 * Contains definition of the class JumpDetector, which serves for detecting jumps in the
 * control-flow graph.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 27.03.2014
 *
 * Created on: Mar 19, 2014
 */

#ifndef JUMPDETECTOR_H_
#define JUMPDETECTOR_H_

#include "HashTable.h"

#include <string>

// Forward declaration of class Aggregator for the linker
class Aggregator;

/**
 * @brief Class serving for detecting jumps in the graph.
 *
 * Searches jumps in control-flow graph and modifies it.
 * Can directly modify graph in Aggregator class because it is a friend class of Aggregator.
 */
class JumpDetector {
public:
   /**
    * @brief Constructor with members initialization.
    * Sets minimal subroutine size and pointer to aggregator.
    * @param s Minimal subroutine size.
    * @param a Aggregator.
    */
   JumpDetector(int s, Aggregator *a);
   /**
    * @brief Default destructor.
    */
   virtual ~JumpDetector();
   /**
    * @brief Finds jumps in the graph in aggregator and modifies it.
    * At first finds last trace of minimal subroutine size. Then searches this trace in hash table
    * and if it has already been in the table, creates new link from trace beginning to found
    * trace beginning.
    */
   void findJumps();
protected:
   HashTable table; /**< Hash table for saving and searching traces */
   unsigned minSize; /**< Minimal subroutine size */
   Aggregator *aggregator; /**< Aggregator containing graph to search jumps in */

   std::string traceStr; /**< Auxiliary member used for trace string when searching traces in graph */
   std::vector<unsigned> indexes; /**< Auxiliary member containing indexes of found trace */
private:
   /**
    * @brief Get the trace of size equal to minimal subroutine size ending at given index.
    * @param node Ending node of the trace requested.
    */
   void getTrace(unsigned node);
};

#endif /* JUMPDETECTOR_H_ */
