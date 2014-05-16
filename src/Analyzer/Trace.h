/**
 * @file Trace.h
 *
 * @brief Trace class.
 *
 * Contains definition of Trace class, representing a trace in tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.02.2014
 *
 * Created on: Apr 2, 2014
 */

#ifndef TRACE_H_
#define TRACE_H_

#include "Call.h"

#include <vector>

/**
 * @brief Trace in the tested program.
 *
 * Class representing a trace, which means a sequence of successive calls as they are called by
 * tested program.
 */
class Trace {
public:
   /**
    * @brief Default constructor.
    * Creates empty trace.
    */
   Trace();
   /**
    * @brief Default destructor.
    */
   virtual ~Trace();

   /**
    * @brief Add call to the end of the trace.
    * @param call Pointer to the call to be added.
    */
   void nextCall(Call *call);
   /**
    * @brief Gets the size of the trace (number of calls).
    * @return Size of the trace.
    */
   unsigned getSize();
   /**
    * @brief Gets the pointer to the call at given index.
    * @param index Index of the call to get.
    * @return Pointer to the call or NULL if index is beyond size of the trace.
    */
   Call *getCall(unsigned index);
   /**
    * @brief Gets the vector of calls in the trace.
    * @return Vector of calls.
    */
   std::vector<Call *> getCalls();

protected:
   std::vector<Call *> calls; /**< Vector of calls in the trace */
};

#endif /* TRACE_H_ */
