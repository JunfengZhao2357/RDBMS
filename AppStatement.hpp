#include "Errors.hpp"
#include "Helpers.hpp"
#include "Statement.hpp"
#include "Tokenizer.hpp"
#include "time.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace ECE141 {

class HelpStatement : public Statement {
public:
  HelpStatement() : Statement(Keywords::help_kw), ToHelpWith(Token()) {
    ToHelpWith.type = TokenType::unknown;
  }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 1) {
      Token firstToken = aTokenizer.peek(0);
      if (firstToken.keyword == Keywords::help_kw) {
        aTokenizer.next(1);
        if (aTokenizer.remaining() >= 1) {
          Token secondToken = aTokenizer.peek(0);
          if (secondToken.type == TokenType::keyword) {
            ToHelpWith = secondToken;
            aTokenizer.next(1);
          }
        }
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Help Statement"; }

  StatusResult run(std::ostream &aStream) const {
    if (ToHelpWith.type == TokenType::unknown) {
      aStream
          << "help\t-- the available list of commands shown below:\n"
          << "\t-- help - shows this list of commands\n"
          << "\t-- version -- shows the current version of this application\n"
          << "\t-- quit  -- terminates the execution of this DB application\n"
          << "\t-- create database <name> -- creates a new database\n"
          << "\t-- drop database <name> -- drops the given database\n"
          << "\t-- use database <name>  -- uses the given database\n"
          << "\t-- describe database <name>  -- describes the given database\n"
          << "\t-- show databases   -- shows the list of databases available\n";
    } else {
      aStream << "help " << ToHelpWith.data << " "
              << HelpInfo[ToHelpWith.keyword] << "\n";
    }
    return StatusResult();
  }

private:
  Token ToHelpWith; // store information about what we are going to help
};

class VersionStatement : public Statement {
public:
  VersionStatement() : Statement(Keywords::version_kw) {}

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 1) {
      Token firstToken = aTokenizer.peek(0);
      if (firstToken.keyword == Keywords::version_kw) {
        aTokenizer.next(1);
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Version Statement"; }

  StatusResult run(std::ostream &aStream) const {
    // use current date and begin data to compute the week number
    time_t now = time(0);
    tm beginTm;
    beginTm.tm_year = 2020 - 1900;
    beginTm.tm_mon = 4 - 1;
    beginTm.tm_mday = 6;
    beginTm.tm_hour = 0;
    beginTm.tm_min = 0;
    beginTm.tm_sec = 0;
    beginTm.tm_isdst = 0;
    time_t begin = mktime(&beginTm);
    int week = (now - begin) / (60 * 60 * 24 * 7) + 1;
    aStream << "version ECE141b-" << week << "\n";
    return StatusResult();
  }
};

class QuitStatement : public Statement {
public:
  QuitStatement() : Statement(Keywords::quit_kw) {}

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 1) {
      Token firstToken = aTokenizer.peek(0);
      if (firstToken.keyword == Keywords::quit_kw) {
        aTokenizer.next(1);
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Quit Statement"; }

  StatusResult run(std::ostream &aStream) const {
    return StatusResult(ECE141::userTerminated);
  }
};
} // namespace ECE141