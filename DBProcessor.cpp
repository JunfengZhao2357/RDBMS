#include "DBProcessor.hpp"
#include "Tokenizer.hpp"
#include <iostream>
#include <memory>

namespace ECE141 {
//.....................................

DBCmdProcessor::DBCmdProcessor(CommandProcessor *aNext)
    : CommandProcessor(aNext) {
  root = getenv("DB_PATH");
  ActiveDatabase = nullptr;
}

DBCmdProcessor::~DBCmdProcessor() {}

// USE: -----------------------------------------------------
StatusResult DBCmdProcessor::interpret(const Statement &aStatement) {
  // STUDENT: write code related to given statement
  StatusResult theResult = aStatement.run(std::cout, this->ActiveDatabase);
  return theResult;
}

// USE: factory to create statement based on given tokens...
Statement *DBCmdProcessor::getStatement(Tokenizer &aTokenizer) {
  // STUDENT: Analyze tokens in tokenizer, see if they match one of the
  //         statements you are supposed to handle. If so, create a
  //         statement object of that type on heap and return it.
  static std::map<Keywords, std::function<Statement *(void)>> factories = {
      {Keywords::create_kw, createStatement},
      {Keywords::drop_kw, dropStatement},
      {Keywords::use_kw, useStatement},
      {Keywords::show_kw, showStatement},
      {Keywords::describe_kw, describeStatement},
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