//
//  SQLProcessor.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include "CommandProcessor.hpp"
#include "SQLStatement.hpp"
#include "Schema.hpp"
#include "Tokenizer.hpp"
#include <stdio.h>

class Statement;
class Database; // define this later...

namespace ECE141 {

class SQLCmdProcessor : public CommandProcessor {
public:
  SQLCmdProcessor(CommandProcessor *aNext = nullptr);
  virtual ~SQLCmdProcessor();

  virtual Statement *getStatement(Tokenizer &aTokenizer);
  virtual StatusResult interpret(const Statement &aStatement);

  /*  do these in next assignment
      StatusResult insert();
      StatusResult update();
      StatusResult delete();
  */

protected:
  // do you need other data members?
  std::string root;
  Database *ActiveDatabase;

  static Statement *createTableStatement() {
    return new CreateTableStatement();
  };
  static Statement *dropTableStatement() { return new DropTableStatement(); }
  static Statement *showTableStatement() { return new ShowTableStatement(); }
  static Statement *describeTableStatement() {
    return new DescribeTableStatement();
  }

  static Statement *insertStatement() { return new InsertStatement(); }
  static Statement *deleteStatement() { return new DeleteStatement(); }
  static Statement *selectStatement() { return new SelectStatement(); }
  static Statement *updateStatement() { return new UpdateStatement(); }

  static Statement *showIndexStatement() { return new ShowIndexStatement(); }
};

} // namespace ECE141
#endif /* SQLProcessor_hpp */
