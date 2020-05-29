//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include "Filters.hpp"
#include "Schema.hpp"
#include "Storage.hpp"
#include "Index.hpp"
#include <filesystem>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

namespace ECE141 {

class Database {
public:
  Database(const std::string aName, CreateNewStorage);
  Database(const std::string aName, OpenExistingStorage);
  ~Database();

  Storage &getStorage() { return storage; }
  std::string &getName() { return dbName; }

  StatusResult createDatabase(const std::string &aName = "");
  StatusResult describeDatabase(const std::string &aName = "");
  StatusResult dropDatabase(const std::string &aName = "");

  StatusResult createTable(const Schema &aSchema);
  StatusResult dropTable(const std::string &aName);
  StatusResult describeTable(const std::string &aName);
  StatusResult showTables();

  StatusResult insertRow(Schema *schemaPtr,
                         std::vector<std::map<std::string, ValueType>> vec);
  StatusResult deleteFrom(std::string aName);
  StatusResult selectFrom(Schema *schemaPtr, std::vector<std::string> fieldList,
                          int limitNum, std::string orderBy,
                          Expressions expressionList);
  StatusResult updateRecords(Schema *schemaPtr, Expression *setExpr,
                             Expressions whereExprs, Operators op);
  StatusResult showIndexes();
  
public:
  std::string rootPath;
  std::string dbName;
  Storage storage;
  MetaData MetaBlock;
  Schema schema;

  friend class UseStatement;
  friend class CreateStatement;
  friend class DropTableStatement;
  friend class DescribeTableStatement;
  friend class CreateTableStatement;
};

// static Database* ActiveDatabase;

} // namespace ECE141

#endif /* Database_hpp */
