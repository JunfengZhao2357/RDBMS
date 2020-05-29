//
//  SQLProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "SQLProcessor.hpp"
#include <iostream>

namespace ECE141 {

// STUDENT: Implement the SQLProcessor class here...
SQLCmdProcessor::SQLCmdProcessor(CommandProcessor *aNext)
    : CommandProcessor(aNext) {
  root = getenv("DB_PATH");
}

SQLCmdProcessor::~SQLCmdProcessor() {}

StatusResult SQLCmdProcessor::interpret(const Statement &aStatement) {
  // STUDENT: write code related to given statement
  ActiveDatabase = getActiveDatabase();
  if (ActiveDatabase == nullptr) {
    return StatusResult(noDatabaseSpecified);
  }
  std::cout << "[INFO] Using database " << ActiveDatabase->getName() << std::endl;
  return aStatement.run(std::cout, this->ActiveDatabase);
}

// USE: factory to create statement based on given tokens...
Statement *SQLCmdProcessor::getStatement(Tokenizer &aTokenizer) {
  // STUDENT: Analyze tokens in tokenizer, see if they match one of the
  //         statements you are supposed to handle. If so, create a
  //         statement object of that type on heap and return it.
  static std::map<Keywords, std::function<Statement *(void)>> factories = {
      {Keywords::create_kw, createTableStatement},
      {Keywords::drop_kw, dropTableStatement},
      {Keywords::show_kw, showTableStatement},
      {Keywords::describe_kw, describeTableStatement},
      {Keywords::insert_kw, insertStatement},
      {Keywords::delete_kw, deleteStatement},
      {Keywords::select_kw, selectStatement},
      {Keywords::update_kw, updateStatement},
      {Keywords::indexes_kw, showIndexStatement},
  };
  Statement *statement;
  Token currToken = aTokenizer.current();
  if (factories.count(currToken.keyword) > 0) {
    if (currToken.keyword == Keywords::show_kw && aTokenizer.peek(1).keyword == Keywords::indexes_kw) {
      statement = factories[Keywords::indexes_kw]();
    } else {
      statement = factories[currToken.keyword]();
    }
  } else {
    return nullptr;
  }
  if (statement->parse(aTokenizer)) {
    return statement;
  } else {
    return nullptr;
  }
}

} // namespace ECE141
