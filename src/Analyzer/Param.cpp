/**
 * @file Param.cpp
 *
 * File containing implementation of the methods of class Param.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 07.03.2014
 *
 * Created on: Feb 14, 2014
 */

#include "Param.h"

/**
 * @brief Default constructor.
 * Creates empty parameter.
 */
Param::Param() :
      typeName(""), value("") {
}

/**
 * @brief Constructor with value initialization.
 * Type name is set to unknown.
 * @param val Value of parameter to be set.
 */
Param::Param(std::string val) :
      typeName("unknown"), value(val) {
}

/**
 * @brief Gets value of parameter.
 * @return String with parameter value.
 */
std::string Param::getValue(){
   return value;
}
