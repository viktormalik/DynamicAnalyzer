/**
 * @file HashTable.h
 *
 * @brief HashTable class.
 *
 * Contains definition of class HashTable, which serves as hashing table for traces.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 27.03.2013
 *
 * Created on: Mar 18, 2014
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <unordered_map>
#include <vector>
#include <string>

/**
 * @brief Hash table for traces.
 *
 * Contains hash table and function for searching, adding and modifying traces.
 * The table serves for mapping trace into vector of indexes in graph the trace lies on.
 * Used during jump detection for finding if given trace has already been present in the tested
 * program and might be considered a subroutine.
 */
class HashTable {
public:
   /**
    * @brief Default constructor.
    * Creates empty hashing table.
    */
   HashTable();
   /**
    * @brief Default destructor.
    */
   virtual ~HashTable();

   /**
    * @brief Finds the trace in table and if it is not there, inserts it.
    * Method can be used as insert into table if the return value is not handled.
    * @param traceStr String representing trace.
    * @param indexes Vector of indexes of the trace. Inserted to table if there has been no such
    * trace yet.
    * @return Found vector of indexes or empty vector in case new trace has been inserted.
    */
   std::vector<unsigned> findTrace(std::string traceStr, std::vector<unsigned> indexes);
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
   void changeTrace(std::string traceStr, std::vector<unsigned> indexes, unsigned startIndex,
         std::vector<unsigned> newValues);
protected:
   /**
    * Hash table mapping traces (strings representing traces) into their position in the graph.
    * The structure used is unordered_map from C++11 standard, which is implemented as hash table.
    */
   std::unordered_map<std::string, std::vector<unsigned>> table;
};

#endif /* HASHTABLE_H_ */
