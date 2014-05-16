/*
 * ProtocolException.h
 *
 *  Created on: Feb 16, 2014
 *      Author: viktor
 */

#ifndef PROTOCOLEXCEPTION_H_
#define PROTOCOLEXCEPTION_H_

#include <string>

class ProtocolException {
protected:
   std::string description;
public:
   ProtocolException(std::string d) : description(d){}
   virtual ~ProtocolException(){}

   std::string getDescription(){
      return description;
   }
};

#endif /* PROTOCOLEXCEPTION_H_ */
