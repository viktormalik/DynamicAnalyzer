/**
 * @file Run.cpp
 *
 * Contains implementation of methods of the class Run.
 *
 * @author Viktor Malik
 *
 * @date 02.04.2014
 *
 * Created on: Feb 15, 2014
 */

#include "Run.h"

#include <iostream>

/**
 * @brief Default constructor.
 * Creates empty run with depth 0.
 */
Run::Run() :
      depth(0) {
}

/**
 * @brief Default destructor.
 * As each call belongs exactly to one run, this is the place where all calls are destroyed.
 */
Run::~Run() {
   for (auto call : calls) {
      delete call;
   }
}

/**
 * @brief Sets depth of the run.
 * @param d Depth value to be set.
 */
void Run::setDepth(unsigned d) {
   depth = d;
}

/**
 * @brief Gets depth of the run
 * @return Value of the depth.
 */
unsigned Run::getDepth() {
   return depth;
}

/**
 * @brief Prints the run.
 */
void Run::print() {
   std::cout << std::endl;
   for (auto call : calls) {
      std::cout << call->getName() << "(";
      call->printParams();
      std::cout << ") = " << call->getReturnVal() << std::endl;
   }
   std::cout << "Depth: " << depth << std::endl;
   std::cout << std::endl;
}

