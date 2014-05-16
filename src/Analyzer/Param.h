/**
 * @file Param.h
 *
 * @brief Param class.
 *
 * Contains definiton of class Param, which represents a parameter of a call in tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 07.03.2014
 *
 * Created on: Feb 14, 2014
 */

#ifndef PARAM_H_
#define PARAM_H_

#include <string>
#include <iostream>

/**
 * @brief One parameter of a call in tested program.
 *
 * Class representing parameter of a call.
 * Provides information about type and value of the parameter.
 *
 * @todo Providing type is not implemented.
 */
class Param {
public:
   /**
    * @brief Default constructor.
    * Creates empty parameter.
    */
   Param();
   /**
    * @brief Constructor with value initialization.
    * Type name is set to unknown.
    * @param val Value of parameter to be set.
    */
   Param(std::string val);
   /**
    * @brief Default destructor.
    */
   virtual ~Param(){}
   /**
    * @brief Gets value of parameter.
    * @return String with parameter value.
    */
   std::string getValue();
   /**
    * @brief Method to print the parameter.
    */
   void print(){
      std::cout << value << ", ";
   }

protected:
   std::string typeName; /**< String with name of the type of parameter */
   std::string value; /**< String with value of the parameter */
};

#endif /* PARAM_H_ */
