/*
 * SocketException.h
 *
 *  Created on: Feb 15, 2014
 *      Author: viktor
 */

#ifndef SOCKETEXCEPTION_H_
#define SOCKETEXCEPTION_H_

#include <string>

class SocketException {
protected:
   std::string description;
public:
   SocketException(std::string d) : description(d){}
   virtual ~SocketException(){}

   std::string getDescription(){
      return description;
   }
};

#endif /* SOCKETEXCEPTION_H_ */
