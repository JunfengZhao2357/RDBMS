#include "Database.hpp"
#include "Errors.hpp"
#include "FolderReader.hpp"
#include "Helpers.hpp"
#include "Statement.hpp"
#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

using namespace std;
namespace ECE141 {

class ShowDB : public ECE141::FolderListener {
public:
  ShowDB(){};

  virtual bool operator()(const std::string &aName) {
    std::cout << aName << std::endl;
    return true;
  }
};

class CreateStatement : public Statement {
public:
  CreateStatement() : Statement(Keywords::create_kw) {
    root = getenv("DB_PATH");
  }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 3) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::database_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          aTokenizer.next(3);
          dbName = secondToken.data;
          if (aTokenizer.more()) {
            if (aTokenizer.current().type != TokenType::keyword &&
                aTokenizer.current().data != ";") {
              dbName.append(" ");
              return StatusResult(); 
              // keep the error and wait for run() to deal with it
            }
          }
          // database = &ActiveDatabase;
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Create Statement"; }

  StatusResult run(std::ostream &aStream, Database* &dbptr) const {
    if (!isValid(dbName))
      return StatusResult(ECE141::illegalIdentifier);
    // create database as dbName.db
    std::string path = root + "/" + dbName + ".db";
    filesystem::path filePath(path);
    // check if database exist
    if (filesystem::exists(filePath)) {
      return StatusResult(ECE141::databaseExists);
    } else {
      Database* db = new Database(dbName, CreateNewStorage());
       db = new Database(dbName,OpenExistingStorage());
      StatusResult status = db->createDatabase(dbName);
      if (status) {
        // (*database) = new Database(dbName, OpenExistingStorage());
        std::cout << "create database " << dbName << " (ok)\n";
      }
      return status;
    }
  }

  bool isValid(std::string aName) const {
    if (aName.length() == 0)
      return true;
    char begin = aName[0];
    if (!isalpha(begin))
      return false;
    for (int i = 1; i < aName.length(); i++) {
      char c = aName[i];
      if (!isalnum(c) && c != '#' && c != '@' && c != '$' && c != '_') {
        return false;
      }
    }
    return true;
  }

private:
  std::string root;
  std::string dbName;
};

class DropStatement : public Statement {
public:
  DropStatement() : Statement(Keywords::drop_kw) { root = getenv("DB_PATH"); }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 3) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::database_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          aTokenizer.next(3);
          dbName = secondToken.data;
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Drop Statement"; }

  StatusResult run(std::ostream &aStream, Database* &dbptr) const {
    std::string path = root + "/" + dbName + ".db";
    filesystem::path filePath(path);
    // check if database exist
    if (!filesystem::exists(filePath)) {
      return StatusResult(ECE141::unknownDatabase);
    } else {
      Database db(dbName, OpenExistingStorage{});
      StatusResult status = db.dropDatabase(dbName);
      if (dbptr != nullptr && dbptr->getName() == dbName) {
        dbptr = nullptr;
      }
      if (status) {
        std::cout << "drop database " << dbName << " (ok)\n";
      }
      return status;
    }
  }

private:
  std::string root;
  std::string dbName;
};

class UseStatement : public Statement {
public:
  UseStatement() : Statement(Keywords::use_kw) { root = getenv("DB_PATH"); }
  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 3) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::database_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          aTokenizer.next(3);
          dbName = secondToken.data;
          // database = &ActiveDatabase;
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Use Statement"; }

  StatusResult run(std::ostream &aStream, Database* &dbptr) const {
    std::string path = root + "/" + dbName + ".db";
    filesystem::path filePath(path);
    // check if database exist
    if (!filesystem::exists(filePath)) {
      return StatusResult(ECE141::unknownDatabase);
    } else {
      if (dbptr != nullptr) {
        dbptr->storage.save(dbptr->MetaBlock, 0);
      }
      dbptr = new Database(dbName, OpenExistingStorage());
      std::cout << "use database " << dbName << " (ok)\n";
      return StatusResult(noError);
    }
  }

private:
  std::string root;
  std::string dbName;
};

class ShowStatement : public Statement {
public:
  ShowStatement() : Statement(Keywords::show_kw) { root = getenv("DB_PATH"); }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 2) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::databases_kw) {
        aTokenizer.next(2);
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Show Statement"; }

  StatusResult run(std::ostream &aStream, Database* &dbptr) const {
    ShowDB show;
    ECE141::FolderReader fr(root.c_str());
    fr.each(show, ".db");
    return StatusResult();
  }

private:
  std::string root;
};

class DescribeStatement : public Statement {
public:
  DescribeStatement() : Statement(Keywords::describe_kw) {
    root = getenv("DB_PATH");
  }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 3) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::database_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          aTokenizer.next(3);
          dbName = secondToken.data;
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "describe Statement"; }

  StatusResult run(std::ostream &aStream, Database* &dbptr) const {
    std::string path = root + "/" + dbName + ".db";
    filesystem::path filePath(path);
    // check if database exist
    if (!filesystem::exists(filePath)) {
      return StatusResult(ECE141::unknownDatabase);
    } else {
      Database db(dbName, OpenExistingStorage{});
      StatusResult status = db.describeDatabase(dbName);
      if (status) {
        std::cout << "describe database " << dbName << " (ok)\n";
      }
      return status;
    }
  }

private:
  std::string root;
  std::string dbName;
};
} // namespace ECE141
