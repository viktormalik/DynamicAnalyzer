/*
 * main.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: viktor
 */

#include <string>
#include <iostream>

#include "Controller.h"
#include "Exceptions/ProtocolException.h"
#include "Exceptions/SocketException.h"
#include "Exceptions/ConfigurationException.h"
#include "Configuration.h"

int main(int argc, char *argv[]) {
   try {
      Configuration config;
      config.parse(argc, argv);
      if (config.checkHelp())
         config.printHelp();
      else {
         Controller controller(&config);
         controller.startAnalysis();
         controller.printTraces();
      }
   } catch (SocketException &e) {
      std::cerr << "Socket error:" << std::endl << e.getDescription() << std::endl;
   } catch (ProtocolException &e) {
      std::cerr << "Communication protocol error" << std::endl << e.getDescription() << std::endl;
   } catch (ConfigurationException &e) {
      std::cerr << "Configuration error:" << std::endl << e.getDescription() << std::endl;
   }

   return 0;
}

