/**
 * @file Scheduler.cpp
 *
 * Contains implementation of methods of the class Scheduler.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 12, 2014
 */

#include "Scheduler/Scheduler.h"
#include "Exceptions/ConfigurationException.h"

/**
 * @brief Constructor with filling variants map./
 * Initializes pointer to controller and creates variants map.
 * Variants map is created from two-dimensional static function-variant groups map by selecting
 * only those variant groups that are given.
 * @param c Pointer to controller.
 * @param groups Vector of variant group names that shall be used during analysis.
 * @throws ConfigurationException if one of groups given has invalid syntax or is not supported
 */
Scheduler::Scheduler(Controller *c, std::vector<std::string> groups) :
      controller(c) {
   // Iterate all functions
   for (const auto & fun : variationsMap) {
      // Iterate all given groups
      for (auto grpStr : groups) {
         if (strToGroup.find(grpStr) != strToGroup.end()) {
            varGroups group = strToGroup[grpStr];
            // Copy all variants for the function and group into variants map
            for (auto var : variationsMap[fun.first][group]) {
               variants[fun.first].push_back(var);
            }
         }
         else
            throw ConfigurationException("Invalid variants list");
      }
   }
}

/**
 * @brief Default destructor.
 */
Scheduler::~Scheduler() {
}

/**
 * @brief Function to create variations map.
 * @return Static variations map.
 */
TVarMap Scheduler::createVarMap() {
   TVarMap varMap;

   varMap[READ][INVAL].push_back(10);
   varMap[READ][INVAL].push_back(11);
   varMap[READ][IO].push_back(20);
   varMap[READ][MEMORY].push_back(40);
   varMap[READ][INTERRUPT].push_back(50);
   varMap[READ][DESTFILE].push_back(90);

   varMap[WRITE][INVAL].push_back(10);
   varMap[WRITE][INVAL].push_back(11);
   varMap[WRITE][IO].push_back(20);
   varMap[WRITE][MEMORY].push_back(40);
   varMap[WRITE][INTERRUPT].push_back(50);
   varMap[WRITE][LIMITS].push_back(70);
   varMap[WRITE][LIMITS].push_back(71);
   varMap[WRITE][LIMITS].push_back(72);

   varMap[OPEN][ACCESSRIGHTS].push_back(30);
   varMap[OPEN][MEMORY].push_back(40);
   varMap[OPEN][MEMORY].push_back(41);
   varMap[OPEN][PATH].push_back(60);
   varMap[OPEN][PATH].push_back(61);
   varMap[OPEN][PATH].push_back(62);
   varMap[OPEN][LIMITS].push_back(70);
   varMap[OPEN][LIMITS].push_back(72);
   varMap[OPEN][LIMITS].push_back(73);
   varMap[OPEN][LIMITS].push_back(74);
   varMap[OPEN][PERMISSIONS].push_back(80);
   varMap[OPEN][PERMISSIONS].push_back(81);
   varMap[OPEN][DESTFILE].push_back(90);
   varMap[OPEN][DESTFILE].push_back(91);
   varMap[OPEN][DESTFILE].push_back(92);

   varMap[OPEN64][ACCESSRIGHTS].push_back(30);
   varMap[OPEN64][MEMORY].push_back(40);
   varMap[OPEN64][MEMORY].push_back(41);
   varMap[OPEN64][PATH].push_back(60);
   varMap[OPEN64][PATH].push_back(61);
   varMap[OPEN64][PATH].push_back(62);
   varMap[OPEN64][LIMITS].push_back(70);
   varMap[OPEN64][LIMITS].push_back(72);
   varMap[OPEN64][LIMITS].push_back(73);
   varMap[OPEN64][LIMITS].push_back(74);
   varMap[OPEN64][PERMISSIONS].push_back(80);
   varMap[OPEN64][PERMISSIONS].push_back(81);
   varMap[OPEN64][DESTFILE].push_back(90);
   varMap[OPEN64][DESTFILE].push_back(91);
   varMap[OPEN64][DESTFILE].push_back(92);

   varMap[CLOSE][INVAL].push_back(10);
   varMap[CLOSE][IO].push_back(20);
   varMap[CLOSE][INTERRUPT].push_back(50);

   varMap[LSEEK][INVAL].push_back(10);
   varMap[LSEEK][INVAL].push_back(11);

   varMap[CREAT][ACCESSRIGHTS].push_back(30);
   varMap[CREAT][MEMORY].push_back(40);
   varMap[CREAT][MEMORY].push_back(41);
   varMap[CREAT][PATH].push_back(60);
   varMap[CREAT][PATH].push_back(61);
   varMap[CREAT][PATH].push_back(62);
   varMap[CREAT][LIMITS].push_back(70);
   varMap[CREAT][LIMITS].push_back(72);
   varMap[CREAT][LIMITS].push_back(73);
   varMap[CREAT][LIMITS].push_back(74);
   varMap[CREAT][PERMISSIONS].push_back(81);
   varMap[CREAT][DESTFILE].push_back(90);
   varMap[CREAT][DESTFILE].push_back(92);

   varMap[CREAT64][ACCESSRIGHTS].push_back(30);
   varMap[CREAT64][MEMORY].push_back(40);
   varMap[CREAT64][MEMORY].push_back(41);
   varMap[CREAT64][PATH].push_back(60);
   varMap[CREAT64][PATH].push_back(61);
   varMap[CREAT64][PATH].push_back(62);
   varMap[CREAT64][LIMITS].push_back(70);
   varMap[CREAT64][LIMITS].push_back(72);
   varMap[CREAT64][LIMITS].push_back(73);
   varMap[CREAT64][LIMITS].push_back(74);
   varMap[CREAT64][PERMISSIONS].push_back(81);
   varMap[CREAT64][DESTFILE].push_back(90);
   varMap[CREAT64][DESTFILE].push_back(92);

   varMap[LINK][IO].push_back(20);
   varMap[LINK][ACCESSRIGHTS].push_back(30);
   varMap[LINK][MEMORY].push_back(40);
   varMap[LINK][MEMORY].push_back(41);
   varMap[LINK][PATH].push_back(60);
   varMap[LINK][PATH].push_back(61);
   varMap[LINK][PATH].push_back(62);
   varMap[LINK][LIMITS].push_back(70);
   varMap[LINK][LIMITS].push_back(72);
   varMap[LINK][LIMITS].push_back(75);
   varMap[LINK][PERMISSIONS].push_back(80);
   varMap[LINK][PERMISSIONS].push_back(81);
   varMap[LINK][DESTFILE].push_back(91);
   varMap[LINK][DESTFILE].push_back(92);

   varMap[SYMLINK][IO].push_back(20);
   varMap[SYMLINK][ACCESSRIGHTS].push_back(30);
   varMap[SYMLINK][MEMORY].push_back(40);
   varMap[SYMLINK][MEMORY].push_back(41);
   varMap[SYMLINK][PATH].push_back(60);
   varMap[SYMLINK][PATH].push_back(61);
   varMap[SYMLINK][PATH].push_back(62);
   varMap[SYMLINK][LIMITS].push_back(70);
   varMap[SYMLINK][LIMITS].push_back(72);
   varMap[SYMLINK][PERMISSIONS].push_back(80);
   varMap[SYMLINK][PERMISSIONS].push_back(81);
   varMap[SYMLINK][DESTFILE].push_back(91);
   varMap[SYMLINK][DESTFILE].push_back(92);

   varMap[UNLINK][IO].push_back(20);
   varMap[UNLINK][ACCESSRIGHTS].push_back(30);
   varMap[UNLINK][MEMORY].push_back(40);
   varMap[UNLINK][MEMORY].push_back(41);
   varMap[UNLINK][PATH].push_back(60);
   varMap[UNLINK][PATH].push_back(61);
   varMap[UNLINK][PATH].push_back(62);
   varMap[UNLINK][LIMITS].push_back(70);
   varMap[UNLINK][LIMITS].push_back(72);
   varMap[UNLINK][PERMISSIONS].push_back(80);
   varMap[UNLINK][DESTFILE].push_back(90);
   varMap[UNLINK][DESTFILE].push_back(91);
   varMap[UNLINK][DESTFILE].push_back(92);

   varMap[STAT][ACCESSRIGHTS].push_back(30);
   varMap[STAT][MEMORY].push_back(40);
   varMap[STAT][MEMORY].push_back(41);
   varMap[STAT][PATH].push_back(60);
   varMap[STAT][PATH].push_back(61);
   varMap[STAT][PATH].push_back(62);
   varMap[STAT][DESTFILE].push_back(92);

   varMap[LSTAT][ACCESSRIGHTS].push_back(30);
   varMap[LSTAT][MEMORY].push_back(40);
   varMap[LSTAT][MEMORY].push_back(41);
   varMap[LSTAT][PATH].push_back(60);
   varMap[LSTAT][PATH].push_back(61);
   varMap[LSTAT][PATH].push_back(62);
   varMap[LSTAT][DESTFILE].push_back(92);

   varMap[FSTAT][INVAL].push_back(10);
   varMap[FSTAT][MEMORY].push_back(40);
   varMap[FSTAT][MEMORY].push_back(41);

   varMap[ACCESS][IO].push_back(20);
   varMap[ACCESS][ACCESSRIGHTS].push_back(30);
   varMap[ACCESS][MEMORY].push_back(40);
   varMap[ACCESS][MEMORY].push_back(41);
   varMap[ACCESS][PATH].push_back(60);
   varMap[ACCESS][PATH].push_back(61);
   varMap[ACCESS][PATH].push_back(62);
   varMap[ACCESS][PERMISSIONS].push_back(81);
   varMap[ACCESS][DESTFILE].push_back(92);

   varMap[CHMOD][IO].push_back(20);
   varMap[CHMOD][ACCESSRIGHTS].push_back(30);
   varMap[CHMOD][MEMORY].push_back(40);
   varMap[CHMOD][MEMORY].push_back(41);
   varMap[CHMOD][PATH].push_back(60);
   varMap[CHMOD][PATH].push_back(61);
   varMap[CHMOD][PATH].push_back(62);
   varMap[CHMOD][PERMISSIONS].push_back(80);
   varMap[CHMOD][PERMISSIONS].push_back(81);
   varMap[CHMOD][DESTFILE].push_back(92);

   varMap[FCHMOD][INVAL].push_back(10);
   varMap[FCHMOD][IO].push_back(20);
   varMap[FCHMOD][PERMISSIONS].push_back(80);
   varMap[FCHMOD][PERMISSIONS].push_back(81);

   varMap[FLOCK][INVAL].push_back(10);
   varMap[FLOCK][INVAL].push_back(11);
   varMap[FLOCK][INTERRUPT].push_back(50);
   varMap[FLOCK][LIMITS].push_back(76);

   varMap[OPENDIR][INVAL].push_back(10);
   varMap[OPENDIR][ACCESSRIGHTS].push_back(30);
   varMap[OPENDIR][MEMORY].push_back(41);
   varMap[OPENDIR][PATH].push_back(61);
   varMap[OPENDIR][PATH].push_back(62);
   varMap[OPENDIR][LIMITS].push_back(73);
   varMap[OPENDIR][LIMITS].push_back(74);

   varMap[READDIR][INVAL].push_back(10);

   varMap[CLOSEDIR][INVAL].push_back(10);

   varMap[MKDIR][ACCESSRIGHTS].push_back(30);
   varMap[MKDIR][MEMORY].push_back(40);
   varMap[MKDIR][MEMORY].push_back(41);
   varMap[MKDIR][PATH].push_back(60);
   varMap[MKDIR][PATH].push_back(61);
   varMap[MKDIR][PATH].push_back(62);
   varMap[MKDIR][LIMITS].push_back(70);
   varMap[MKDIR][LIMITS].push_back(72);
   varMap[MKDIR][LIMITS].push_back(75);
   varMap[MKDIR][PERMISSIONS].push_back(80);
   varMap[MKDIR][PERMISSIONS].push_back(81);
   varMap[MKDIR][DESTFILE].push_back(91);
   varMap[MKDIR][DESTFILE].push_back(92);

   varMap[RMDIR][ACCESSRIGHTS].push_back(30);
   varMap[RMDIR][MEMORY].push_back(40);
   varMap[RMDIR][MEMORY].push_back(41);
   varMap[RMDIR][PATH].push_back(60);
   varMap[RMDIR][PATH].push_back(61);
   varMap[RMDIR][PATH].push_back(62);
   varMap[RMDIR][PERMISSIONS].push_back(80);
   varMap[RMDIR][PERMISSIONS].push_back(81);
   varMap[RMDIR][DESTFILE].push_back(92);
   varMap[RMDIR][DESTFILE].push_back(93);

   varMap[FSYNC][INVAL].push_back(10);
   varMap[FSYNC][IO].push_back(20);
   varMap[FSYNC][PERMISSIONS].push_back(81);

   return varMap;
}

TVarMap Scheduler::variationsMap = createVarMap();

/**
 * Static dictionary initialization.
 */
std::map<std::string, varGroups> Scheduler::strToGroup = { 
   { "inval", INVAL }, 
   { "io", IO }, 
   { "access", ACCESSRIGHTS }, 
   { "memory", MEMORY }, 
   { "interrupt", INTERRUPT }, 
   { "path", PATH }, 
   { "limits", LIMITS }, 
   { "permissions", PERMISSIONS }, 
   { "file", DESTFILE } 
};
