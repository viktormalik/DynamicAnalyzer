/**
 * @file HashTable.cpp
 *
 * Contains implementation of methods of the class HashTable.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 27.03.2014
 *
 * Created on: Mar 18, 2014
 */

#include "Aggregator/HashTable.h"

#include <algorithm>

/**
 * @brief Default constructor.
 * Creates empty hashing table.
 */
HashTable::HashTable() :
      table() {
}

/**
 * @brief Default destructor.
 */
HashTable::~HashTable() {
}

/**
 * @brief Finds the trace in table and if it is not there, inserts it.
 * Method can be used as insert into table if the return value is not handled.
 * @param traceStr String representing trace.
 * @param indexes Vector of indexes of the trace. Inserted to table if there has been no such
 * trace yet.
 * @return Found vector of indexes or empty vector in case new trace has been inserted.
 */
std::vector<unsigned> HashTable::findTrace(std::string traceStr, std::vector<unsigned> indexes) {
   // Search the trace in the table
   auto iterator = table.find(traceStr);
   if (iterator == table.end()) {
      // If trace has not been found, insert it to the table
      table[traceStr] = indexes;
      // Return empty vector
      std::vector<unsigned> result;
      return result;
   }
   else {
      // If trace already is in the table, return found indexes vector
      return table[traceStr];
   }
}

/**
 * @brief Change the indexes of the trace.
 * Searches the trace in hashing table. If obtained indexes are same as the indexes of the trace
 * given, modifies vector of old indexes to new values.
 * Only last n values can be replaced, then ::startIndex marks the position in vector
 * where the replacement begins.
 * @param traceStr String representing trace.
 * @param indexes Old indexes of the trace (for checking that found trace is really the one that
 * is needed to be modified.
 * @param startIndex Index of item in vector of indexes where the replacement begins.
 * @param newValues New indexes to be put into trace.
 */
void HashTable::changeTrace(std::string traceStr, std::vector<unsigned> indexes,
      unsigned startIndex, std::vector<unsigned> newValues) {
   // Search the trace in the table
   std::vector<unsigned> foundIndexes = table[traceStr];
   // If the trace is the same one as the one found in table, replace indexes from position given
   // in startIndex by new values
   if (std::equal(foundIndexes.begin(), foundIndexes.end(), indexes.begin())) {
      unsigned j = 0;
      for (unsigned i = startIndex; i < foundIndexes.size(); ++i) {
         table[traceStr][i] = newValues[j++];
      }
   }
}

