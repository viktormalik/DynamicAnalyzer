/**
 * @file Run.h
 *
 * @brief Run class
 *
 * Contains definition of class Run, representing one program run (from beginning to the end
 * of the tested program).
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Feb 15, 2014
 */

#ifndef RUN_H_
#define RUN_H_

#include "Trace.h"

#include <vector>

/**
 * @brief A run of one instance of the tested program.
 *
 * Class representing single run of tested program (calls from beginning of the program to its end.
 * Extends class Trace.
 * Contains vector of calls inherited from Trace and adds information about the call number where
 * the program run has been affected by the controller.
 */
class Run: public Trace {
public:
   /**
    * @brief Default constructor.
    * Creates empty run with depth 0.
    */
   Run();
   /**
    * @brief Default destructor.
    * As each call belongs exactly to one call, this is the place where all calls are destroyed.
    */
   virtual ~Run();

   /**
    * @brief Sets depth of the run.
    * @param d Depth value to be set.
    */
   void setDepth(unsigned d);
   /**
    * @brief Gets depth of the run
    * @return Value of the depth.
    */
   unsigned getDepth();

   /**
    * @brief Prints the run.
    */
   void print();

protected:
   unsigned depth; /**< Index of first call after the call in which the expansion has been done
                    * during the program run (run of tested program has been afected by
                    * the controller). */
};

#endif /* RUN_H_ */
