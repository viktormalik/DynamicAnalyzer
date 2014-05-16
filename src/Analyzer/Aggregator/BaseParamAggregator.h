/**
 * @file BaseParamAggregator.h
 *
 * @brief BaseParamAggregator class.
 *
 * Contains definition of the class BaseParamAggregator which aggregates calls by function name
 * and base parameter value.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date Apr 7, 2014
 *
 * Created on: Apr 7, 2014
 */

#ifndef BASEPARAMAGGREGATOR_H_
#define BASEPARAMAGGREGATOR_H_

#include "Aggregator/Aggregator.h"
#include "Call.h"

#include <string>
#include <map>

/**
 * @brief Aggregator distinguishing calls not only by function name, but also by base parameter
 * value.
 * Contains map specifying for each call, which parameter is considered the base one.
 * Reimplements function Aggregator::toCall.
 */
class BaseParamAggregator: public Aggregator {
public:
   /**
    * @brief Constructor with same parameter as parent constructor.
    * Only calls parent constructor Aggregator::Aggregator.
    * @param subroutineSize Minimal subroutine size.
    * @param path Output file destination.
    */
   BaseParamAggregator(unsigned subroutineSize, std::string path);
   /**
    * @brief Default destructor.
    */
   virtual ~BaseParamAggregator();
   /**
    * @brief Converts call to string.
    * The string is created from function name and base parameter value. Other parameters are not
    * inserted (but commas between parameters are).
    * @param call Call to be converted.
    * @return String representation of the call.
    */
   std::string toString(Call *call);
private:
   /**
    * Static dictionary containing number (index) of base parameter of each function.
    * If value for a function is not specified, no parameter is considered base.
    */
   static std::map<functions, int> baseParamMap;
};

#endif /* BASEPARAMAGGREGATOR_H_ */
