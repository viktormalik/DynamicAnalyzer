/**
 * @file Trace.cpp
 *
 * Contains implementation of methods of the class Trace.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Apr 2, 2014
 */

#include "Trace.h"

/**
 * @brief Default constructor.
 * Creates empty trace.
 */
Trace::Trace() {
}

/**
 * @brief Default destructor.
 */
Trace::~Trace() {
}

/**
 * @brief Add call to the end of the trace.
 * @param call Pointer to the call to be added.
 */
void Trace::nextCall(Call *c) {
   calls.push_back(c);
}

/**
 * @brief Gets the size of the trace (number of calls).
 * @return Size of the trace.
 */
unsigned Trace::getSize() {
   return calls.size();
}

/**
 * @brief Gets the pointer to the call at given index.
 * @param index Index of the call to get.
 * @return Pointer to the call or NULL if index is beyond size of the trace.
 */
Call *Trace::getCall(unsigned index) {
   return calls[index];
}

/**
 * @brief Gets the vector of calls in the trace.
 * @return Vector of calls.
 */
std::vector<Call *> Trace::getCalls() {
   return calls;
}
