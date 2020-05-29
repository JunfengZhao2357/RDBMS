//
//  AppProcessor.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef AppProcessor_hpp
#define AppProcessor_hpp

#include "AppStatement.hpp"
#include "CommandProcessor.hpp"
#include <stdio.h>

namespace ECE141 {

class AppCmdProcessor : public CommandProcessor {
public:
  AppCmdProcessor(CommandProcessor *aNext = nullptr);
  virtual ~AppCmdProcessor();

  virtual Statement *getStatement(Tokenizer &aTokenizer);
  virtual StatusResult interpret(const Statement &aStatement);

private:
  std::string root;

  static Statement *helpStatement() { return new HelpStatement(); };
  static Statement *versionStatement() { return new VersionStatement(); };
  static Statement *quitStatement() { return new QuitStatement(); };
};

} // namespace ECE141

#endif /* AppProcessor_hpp */
