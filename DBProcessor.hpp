#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include "CommandProcessor.hpp"
#include "DBStatement.hpp"
#include <stdio.h>
namespace ECE141 {

class DBCmdProcessor : public CommandProcessor {
public:
  DBCmdProcessor(CommandProcessor *aNext = nullptr);
  virtual ~DBCmdProcessor();

  virtual Statement *getStatement(Tokenizer &aTokenizer);
  virtual StatusResult interpret(const Statement &aStatement);
  virtual Database *getActiveDatabase() { return ActiveDatabase; }

private:
  std::string root;
  Database* ActiveDatabase;
  static Statement *createStatement() { return new CreateStatement(); };
  static Statement *dropStatement() { return new DropStatement(); };
  static Statement *useStatement() { return new UseStatement(); };
  static Statement *showStatement() { return new ShowStatement(); };
  static Statement *describeStatement() { return new DescribeStatement(); };
};

} // namespace ECE141

#endif /* DBProcessor_hpp */