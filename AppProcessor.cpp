//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "AppProcessor.hpp"
#include "Tokenizer.hpp"
#include <iostream>
#include <memory>

namespace ECE141 {
//.....................................

AppCmdProcessor::AppCmdProcessor(CommandProcessor *aNext)
    : CommandProcessor(aNext) {
  root = getenv("DB_PATH");
}

AppCmdProcessor::~AppCmdProcessor() {}

// USE: -----------------------------------------------------
StatusResult AppCmdProcessor::interpret(const Statement &aStatement) {
  // STUDENT: write code related to given statement
  return aStatement.run(std::cout);
}

// USE: factory to create statement based on given tokens...
Statement *AppCmdProcessor::getStatement(Tokenizer &aTokenizer) {
  // STUDENT: Analyze tokens in tokenizer, see if they match one of the
  //         statements you are supposed to handle. If so, create a
  //         statement object of that type on heap and return it.

  // map key words to statements
  static std::map<Keywords, std::function<Statement *(void)>> factories = {
      {Keywords::help_kw, helpStatement},
      {Keywords::version_kw, versionStatement},
      {Keywords::quit_kw, quitStatement},
  };
  
  Statement *statement;
  Token currToken = aTokenizer.current();
  if (factories.count(currToken.keyword) > 0) {
    statement = factories[currToken.keyword]();
    if (statement->parse(aTokenizer)) {
      return statement;
    } else {
      return nullptr;
    }
  } else {
    return nullptr;
  }
}
} // namespace ECE141
