/*
 * ConfgiruationException.h
 *
 *  Created on: Mar 28, 2014
 *      Author: viktor
 */

#ifndef CONFIGURATIONEXCEPTION_H_
#define CONFIGURATIONEXCEPTION_H_

#include <string>

class ConfigurationException {
protected:
   std::string description;
public:
   ConfigurationException(std::string d) : description(d){}
   virtual ~ConfigurationException(){}

   std::string getDescription(){
      return description;
   }
};

#endif /* CONFIGURATIONEXCEPTION_H_ */
