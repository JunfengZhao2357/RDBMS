#include "Database.hpp"
#include "Errors.hpp"
#include "Filters.hpp"
#include "Helpers.hpp"
#include "Schema.hpp"
#include "Statement.hpp"
#include "Storage.hpp"
#include "Timer.hpp"
#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <string>
#include <unordered_set>

using namespace std;
namespace ECE141 {
class CreateTableStatement : public Statement {
public:
  CreateTableStatement() : Statement(Keywords::create_kw) {}
  const char *getStatementName() const { return "Create Table Statement"; }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 4) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::table_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          aTokenizer.next(3);
          tableName = secondToken.data;
          if (aTokenizer.more()) {
            if (aTokenizer.current().type != TokenType::keyword &&
                aTokenizer.current().type != TokenType::punctuation) {
              tableName.append(" ");
              return StatusResult();
              // keep the error and let run() deal with it
            }
          }
          schema.setName(tableName);
          StatusResult theResult = parseAttributes(aTokenizer);
          return theResult;
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseAttributes(Tokenizer &aTokenizer) {
    if (aTokenizer.current().type != TokenType::punctuation ||
        aTokenizer.current().data != "(") {
      return StatusResult(syntaxError);
    }

    static std::map<std::string, DataType> strToDataType = {
        {"boolean", DataType::bool_type},       {"float", DataType::float_type},
        {"timestamp", DataType::datetime_type}, {"int", DataType::int_type},
        {"varchar", DataType::varchar_type},
    };

    int pos = 1;
    while (aTokenizer.peek(pos).data != ")") {
      Attribute attr;
      std::string nullstr = "NULL";
      attr.setDefault(nullstr);
      // find and set Name of Attribute
      Token aName = aTokenizer.peek(pos++);
      if (aName.type != TokenType::identifier)
        return StatusResult(syntaxError);
      attr.setName(aName.data);

      // find and set type of Attribute
      Token aType = aTokenizer.peek(pos++);
      if (aType.type != TokenType::keyword &&
          aType.data.compare("TIMESTAMP") != 0)
        return StatusResult(syntaxError);
      std::string type = aType.data;
      std::transform(type.begin(), type.end(), type.begin(), ::tolower);
      attr.setType(strToDataType[type]);

      // if type=varchar, find and set size
      if (strToDataType[type] == DataType::varchar_type) {
        // find and set size of Attribute
        Token firstToken = aTokenizer.peek(pos++);
        if (firstToken.data != "(") {
          std::cout << "[ERROR] Missing '(' after VARCHAR\n";
          return StatusResult(syntaxError);
        }
        Token aSizeToken = aTokenizer.peek(pos++);
        if (aSizeToken.type != TokenType::number) {
          std::cout << "[ERROR] No valid size after VARCHAR\n";
          return StatusResult(syntaxError);
        }
        std::string aSize = aSizeToken.data;
        try {
          attr.setSize(stoi(aSize));
        } catch (std::invalid_argument) {
          std::cout << "[ERROR] No valid size after VARCHAR\n";
          return StatusResult(syntaxError);
        }
        Token lastToken = aTokenizer.peek(pos++);
        if (lastToken.data != ")") {
          std::cout << "[ERROR] Missing ')' after VARCHAR\n";
          return StatusResult(syntaxError);
        }
      }

      // find and set autoIncrement, nullable, primary key, default
      // ',' marks the end of the definition for an attribute
      while (aTokenizer.peek(pos).data != ",") {
        if (aTokenizer.peek(pos).data == ")") {
          pos--;
          break;
        }
        Token currToken = aTokenizer.peek(pos);
        if (currToken.type != TokenType::keyword &&
            currToken.data.compare("DEFAULT") != 0) {
          std::cout << "[ERROR] Missing a Keyword in argument\n";
          return StatusResult(syntaxError);
        }
        switch (currToken.keyword) {
        case Keywords::auto_increment_kw: // AUTO_INCREMENT
          pos++;
          attr.setAutoIncrement(1);
          break;
        case Keywords::not_kw:
          pos++;
          if (aTokenizer.peek(pos).keyword != Keywords::null_kw)
            return StatusResult(syntaxError);
          pos++;
          attr.setNullable(0);
          break;
        case Keywords::primary_kw:
          pos++;
          if (aTokenizer.peek(pos).keyword != Keywords::key_kw)
            return StatusResult(syntaxError);
          pos++;
          attr.setPrimary(1);
          break;
        default:
          std::transform(currToken.data.begin(), currToken.data.end(),
                         currToken.data.begin(), ::tolower);
          if (currToken.data != "default") {
            return StatusResult(syntaxError);
          }
          pos++;
          attr.setDefault(aTokenizer.peek(pos).data);
          pos++;
        } // end of switch
      }
      pos++; // skip ','
      schema.addAttribute(attr);
    }
    aTokenizer.next(pos + 1);
    return StatusResult(noError);
  }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    if (!isValid(tableName))
      return StatusResult(ECE141::illegalIdentifier);
    bool find = false;
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          Schema aSchema;
          dbptr->storage.load(aSchema, element.first);
          if (tableName == aSchema.getName()) {
            find = true;
            break;
          }
        }
      }
    }
    if (find)
      return StatusResult(Errors::tableExists);
    dbptr->createTable(schema);
    std::cout << "create table " << tableName << " (ok)\n";
    return StatusResult(noError);
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
  std::string tableName;
  Schema schema;
}; // namespace ECE141

class DropTableStatement : public Statement {
public:
  DropTableStatement() : Statement(Keywords::drop_kw) {}

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 3) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::table_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          aTokenizer.next(3);
          tableName = secondToken.data;
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Drop Table Statement"; }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    // drop table in the database
    bool find = false;
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          Schema aSchema;
          dbptr->storage.load(aSchema, element.first);
          if (tableName == aSchema.getName()) {
            find = true;
            break;
          }
        }
      }
    }
    if (!find)
      return StatusResult(unknownTable);
    dbptr->dropTable(tableName);
    std::cout << "drop table " << tableName << " (ok)\n";
    return StatusResult(noError);
  }

private:
  std::string tableName;
};

class ShowTableStatement : public Statement {
public:
  ShowTableStatement() : Statement(Keywords::show_kw) {}

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 2) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::tables_kw) {
        aTokenizer.next(2);
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Show Tables Statement"; }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    // show tables in the database
    dbptr->showTables();
    std::cout << "show tables "
              << " (ok)\n";
    return StatusResult(noError);
  }

private:
};

class DescribeTableStatement : public Statement {
public:
  DescribeTableStatement() : Statement(Keywords::drop_kw) {}

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 2) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.type == TokenType::identifier) {
        aTokenizer.next(2);
        tableName = firstToken.data;
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Describe Table Statement"; }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    // describe table in the database
    bool find = false;
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          Schema aSchema;
          dbptr->storage.load(aSchema, element.first);
          if (tableName == aSchema.getName()) {
            find = true;
            break;
          }
        }
      }
    }
    if (!find)
      return StatusResult(unknownTable);
    dbptr->describeTable(tableName);
    std::cout << "describe table " << tableName << " (ok)\n";
    return StatusResult(noError);
  }

private:
  std::string tableName;
};

class InsertStatement : public Statement {
public:
  InsertStatement() : Statement(Keywords::insert_kw) { schema = new Schema(); }
  const char *getStatementName() const { return "Insert Statement"; }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 4) { // "insert into tableName ("
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::into_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          tableName = secondToken.data;
          aTokenizer.next(3);
          // parse field list
          StatusResult theResult = parseFields(aTokenizer);
          if (!theResult) {
            return theResult;
          }
          // parse data list
          if (aTokenizer.remaining() >= 1) {
            if (aTokenizer.current().keyword == Keywords::values_kw) {
              aTokenizer.next();
              StatusResult theResult = parseData(aTokenizer);
              return theResult;
            }
          }
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseFields(Tokenizer &aTokenizer) {
    if (aTokenizer.current().type != TokenType::punctuation ||
        aTokenizer.current().data != "(") {
      return StatusResult(syntaxError);
    }
    int pos = 1;
    while (aTokenizer.peek(pos).data != ")") {
      if (aTokenizer.peek(pos).data == ",") {
        pos++;
      }
      Token currToken = aTokenizer.peek(pos++);
      if (currToken.type != TokenType::identifier) {
        return StatusResult(syntaxError);
      }
      fieldList.push_back(currToken.data);
    }
    aTokenizer.next(pos + 1);
    return StatusResult(noError);
  }

  StatusResult parseData(Tokenizer &aTokenizer) {
    while (aTokenizer.remaining() > 0) {
      if (aTokenizer.current().type == TokenType::keyword) {
        return StatusResult();
      }
      if (aTokenizer.current().data == ",") {
        aTokenizer.next();
      }
      if (aTokenizer.current().type != TokenType::punctuation ||
          aTokenizer.current().data != "(") {
        return StatusResult(syntaxError);
      }
      int pos = 1;
      std::vector<std::string> dataVec;

      // Key-ValueType map;
      while (aTokenizer.peek(pos).data != ")") {
        if (aTokenizer.peek(pos).data == ",") {
          pos++;
        } else if (aTokenizer.peek(pos - 1).data != "(") {
          return StatusResult(syntaxError);
        }
        Token currToken = aTokenizer.peek(pos++);
        dataVec.push_back(currToken.data);
        if (pos >= aTokenizer.remaining()) {
          return StatusResult(syntaxError);
        }
      }
      valueList.push_back(dataVec);
      aTokenizer.next(pos + 1);
    }
    return StatusResult(noError);
  }

  // find ValueType data using fieldName from string data
  StatusResult findValidData(std::string fieldName, std::string data,
                             ValueType &value) const {
    DataType dtype;
    int size = 0;
    for (Attribute attr : schema->getAttributes()) {
      if (attr.getName() == fieldName) {
        dtype = attr.getType();
        if (dtype == DataType::varchar_type) {
          size = attr.getSize();
        }
      }
    }
    switch (dtype) {
    case DataType::int_type:
      try {
        value.value = std::stoi(data);
      } catch (std::invalid_argument) {
        return StatusResult(Errors::invalidArguments);
      }
      break;
    case DataType::float_type:
      try {
        value.value = (double)std::atof(data.c_str());
      } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
        return StatusResult(Errors::invalidArguments);
      }
      break;
    case DataType::bool_type:
      transform(data.begin(), data.end(), data.begin(), ::tolower);
      if (data == "true") {
        value.value = true;
      } else if (data == "false") {
        value.value = false;
      } else {
        return StatusResult(Errors::invalidArguments);
      }
      break;
    case DataType::varchar_type:
      if (data.length() > size) {
        return StatusResult(Errors::invalidArguments);
      }
      value.value = data;
      break;
    // case DataType::datetime_type:
    //   /* code */
    //   break;
    default:
      return StatusResult(unknownType);
    }
    return StatusResult();
  }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    if (!dbptr) {
      return StatusResult(Errors::noDatabaseSpecified);
    }
    bool existTable = false;
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          dbptr->storage.load(*schema, element.first);
          if (schema->getName() == tableName) {
            existTable = true;
            break;
          }
        }
      }
    }
    if (!existTable)
      return StatusResult(unknownTable);
    // Get valid attributes from schema
    AttributeList attributes = schema->getAttributes();
    std::unordered_set<std::string> attrSet;
    for (Attribute attr : attributes) {
      attrSet.insert(attr.getName());
    }
    for (std::string field : fieldList) {
      if (attrSet.count(field) == 0) {
        return StatusResult(unknownAttribute);
      }
    }
    // make pairs
    std::vector<std::map<std::string, ValueType>> mapVec;
    for (int i = 0; i < valueList.size(); i++) { // number of rows
      std::map<std::string, ValueType> map;
      for (int j = 0; j < fieldList.size(); j++) { // index of data
        std::string fieldName = fieldList[j];
        ValueType fieldValue;
        StatusResult valid =
            findValidData(fieldName, valueList[i][j], fieldValue);
        if (!valid)
          return valid;
        map[fieldName] = fieldValue;
      }
      mapVec.push_back(map);
    }
    dbptr->insertRow(schema, mapVec);
    std::cout << "insert row " << tableName << " (ok)\n";
    return StatusResult(noError);
  }

  bool isValid(std::string aName) const { return true; }

private:
  // parse
  std::string dbName;
  std::string tableName;
  std::vector<std::string> fieldList;
  std::vector<std::vector<string>> valueList;
  // run
  // Database *tempDB;
  Schema *schema;
};

class DeleteStatement : public Statement {
public:
  DeleteStatement() : Statement(Keywords::delete_kw) { schema = new Schema(); }
  const char *getStatementName() const { return "Delete Statement"; }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 3) { // delete from tableName
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::from_kw) {
        Token secondToken = aTokenizer.peek(2);
        if (secondToken.type == TokenType::identifier) {
          tableName = secondToken.data;
          aTokenizer.next(3);
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    if (!dbptr) {
      return StatusResult(Errors::noDatabaseSpecified);
    }
    bool existTable = false;
    dbptr->storage.load(dbptr->MetaBlock, 0);
    // Check if there is a table named tableName
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          dbptr->storage.load(*schema, element.first);
          if (schema->getName() == tableName) {
            existTable = true;
            break;
          }
        }
      }
    }
    if (!existTable)
      return StatusResult(unknownTable);
    // only delete records in that table
    // don't drop table
    Timer timer;
    timer.start();
    dbptr->deleteFrom(tableName);
    timer.stop();
    std::cout << "(" << timer.elapsed() << " ms.)" << std::endl;
    std::cout << "delete from " << tableName << " (ok)\n";
    return StatusResult(noError);
  }

private:
  // parse
  std::string tableName;
  Schema *schema;
};

class SelectStatement : public Statement {
public:
  SelectStatement() : Statement(Keywords::select_kw) {
    schema = new Schema();
    limitNum = -1;
    orderBy = "NULL";
  }
  const char *getStatementName() const { return "Select Statement"; }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 4) {
      // select field1, field2, ... from tableName
      int currPos = 1;
      if (parseFileds(aTokenizer, currPos)) {
        Token kwToken = aTokenizer.peek(currPos++);
        if (kwToken.keyword == Keywords::from_kw) {
          Token nameToken = aTokenizer.peek(currPos++);
          if (nameToken.type == TokenType::identifier) {
            tableName = nameToken.data;
            aTokenizer.next(currPos);
            StatusResult parseResult(noError);
            currPos = 0;
            while (aTokenizer.remaining() > currPos) {
              StatusResult hasOrder = parseOrder(aTokenizer, currPos);
              StatusResult hasLimit = parseLimit(aTokenizer, currPos);
              StatusResult hasWhere = parseWhere(aTokenizer, currPos);
              StatusResult hasJoin = parseJoin(aTokenizer, currPos);
              if (!hasOrder && !hasLimit && !hasWhere && !hasJoin)
                break;
            }
            aTokenizer.next(currPos);
            return StatusResult(noError);
          }
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseFileds(Tokenizer &aTokenizer, int &pos) {
    if (aTokenizer.peek(pos).data != "*") {
      while (true) {
        if (aTokenizer.peek(pos).data == ",") {
          pos++;
        }
        Token currToken = aTokenizer.peek(pos++);
        if (currToken.type != TokenType::identifier) {
          return StatusResult(syntaxError);
        }
        fieldList.push_back(currToken.data);
        if (aTokenizer.peek(pos).type != TokenType::punctuation) {
          break;
        }
      }
    } else {
      fieldList.push_back("*");
      pos++;
    }
    return StatusResult(noError);
  }

  StatusResult parseOrder(Tokenizer &aTokenizer, int &pos) {
    if (aTokenizer.remaining() >= pos + 3) {
      if (aTokenizer.peek(pos).keyword == Keywords::order_kw) {
        pos++;
        if (aTokenizer.peek(pos++).keyword == Keywords::by_kw) {
          orderBy = aTokenizer.peek(pos++).data;
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseLimit(Tokenizer &aTokenizer, int &pos) {
    if (aTokenizer.remaining() >= pos + 2) {
      if (aTokenizer.peek(pos).keyword == Keywords::limit_kw) {
        pos++;
        if (aTokenizer.peek(pos).type == TokenType::number) {
          try {
            limitNum = std::stoi(aTokenizer.peek(pos++).data);
          } catch (std::invalid_argument) {
            return StatusResult(syntaxError);
          }
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseWhere(Tokenizer &aTokenizer, int &pos) {
    if (aTokenizer.remaining() >= pos + 4) {
      if (aTokenizer.peek(pos).keyword == Keywords::where_kw) {
        pos++;
        if (aTokenizer.peek(pos).type == TokenType::identifier) {
          // lhs: (string) where "what" op what
          // lhs: focus on type and name
          Token currToken = aTokenizer.peek(pos++);
          ValueType aValue1;
          std::string str = "";
          aValue1.value = str;
          Operand lhs(currToken.data, currToken.type, aValue1);

          // operator
          currToken = aTokenizer.peek(pos++);
          if (currToken.type == TokenType::operators) {
            Operators aOp = Helpers::toOperator(currToken.data);

            // rhs: (identifier) where what op "what"
            // rhs: focus on name and value
            currToken = aTokenizer.peek(pos++);
            ValueType aValue2;
            aValue2.value = currToken.data;
            std::string aName = "";
            Operand rhs(aName, currToken.type, aValue2);

            // form an Expression {limit="someNumber"}
            Expression *expr = new Expression(lhs, aOp, rhs);
            expressionList.push_back(expr);
            return StatusResult(noError);
          }
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseJoin(Tokenizer &aTokenizer, int &pos) {
    if (aTokenizer.remaining() >= pos + 11) {
      Token currToken = aTokenizer.peek(pos);
      joinType = currToken.keyword;
      if (joinType == Keywords::left_kw || joinType == Keywords::right_kw) {
        pos++;
        if (aTokenizer.peek(pos++).keyword == Keywords::join_kw) {
          joinTableName = aTokenizer.peek(pos++).data;
          if (aTokenizer.peek(pos++).keyword == Keywords::on_kw) {
            std::string lhs = aTokenizer.peek(pos++).data;
            lhs += aTokenizer.peek(pos++).data;
            lhs += aTokenizer.peek(pos++).data;
            if (aTokenizer.peek(pos++).data != "=") {
              return StatusResult(syntaxError);
            }
            std::string rhs = aTokenizer.peek(pos++).data;
            rhs += aTokenizer.peek(pos++).data;
            rhs += aTokenizer.peek(pos++).data;
            Join theJoin(joinTableName, joinType, lhs, rhs);
            joins.push_back(theJoin);
          }
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    // Select from
    // Check database exist
    if (!dbptr) {
      return StatusResult(Errors::noDatabaseSpecified);
    }
    // Check table exist
    bool existTable = false;
    dbptr->storage.load(dbptr->MetaBlock, 0);
    // Check if there is a table named tableName
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          dbptr->storage.load(*schema, element.first);
          if (schema->getName() == tableName) {
            existTable = true;
            break;
          }
        }
      }
    }
    if (!existTable) {
      return StatusResult(unknownTable);
    }

    AttributeList attributes = schema->getAttributes();
    // Check FieldList and orderBy
    if (joins.size() == 0) {
      bool allFieldsExist = true;
      bool orderByExist = false;
      for (auto field : fieldList) {
        bool existField = false;
        for (auto attr : attributes) {
          if (attr.getName() == field || field == "*") {
            existField = true;
          }
          if (attr.getName() == orderBy || orderBy == "*") {
            orderByExist = true;
          }
        }
        allFieldsExist = allFieldsExist && existField;
      }
      if (!allFieldsExist || (orderBy != "NULL" && !orderByExist)) {
        return StatusResult(unknownAttribute);
      }
    }
    // Check expressionlist validation
    std::map<DataType, int> DataTypeToInt = {
        {DataType::bool_type, 0},
        {DataType::int_type, 1},
        {DataType::float_type, 2},
        {DataType::varchar_type, 3},
    };
    bool allExprValid = true;
    for (Expression *expr : expressionList) {
      bool exprValid = false;
      std::string field = expr->lhs.name;
      Value value = expr->rhs.value.value;
      for (auto attr : attributes) {
        if (attr.getName() == field) {
          DataType dtype = attr.getType();
          std::string valuestr = std::get<std::string>(value);
          switch (dtype) {
          case DataType::bool_type:
            std::transform(valuestr.begin(), valuestr.end(), valuestr.begin(),
                           ::tolower);
            if (valuestr == "true") {
              expr->rhs.value.value = true;
            } else {
              expr->rhs.value.value = false;
            }
            break;
          case DataType::int_type:
            try {
              expr->rhs.value.value = std::stoi(valuestr);
            } catch (std::invalid_argument) {
              return StatusResult(unknownType);
            }
            break;
          case DataType::float_type:
            expr->rhs.value.value = std::atof(valuestr.c_str());
            break;
          default:
            break;
          }
          exprValid = true;
          break;
        }
      }
      allExprValid = allExprValid && exprValid;
    }
    if (!allExprValid) {
      return StatusResult(unknownAttribute);
    }

    // check join
    // TODO

    Timer timer;
    timer.start();
    dbptr->selectFrom(schema, fieldList, limitNum, orderBy, expressionList, joins);
    timer.stop();
    std::cout << "(" << timer.elapsed() << " ms.)" << std::endl;
    std::cout << "select from " << tableName << " (ok)\n";
    return StatusResult(noError);
  }

private:
  // parse
  std::string tableName;
  Schema *schema;
  std::vector<std::string> fieldList;
  int limitNum;
  std::string orderBy;
  Expressions expressionList;

  // Join part
  Keywords joinType; // left or right
  std::string joinTableName;
  std::vector<Join> joins;
};

class UpdateStatement : public Statement {
public:
  UpdateStatement() : Statement(Keywords::update_kw) {
    schema = new Schema();
    op = Operators::unknown_op;
  }
  const char *getStatementName() const { return "Update Statement"; }

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 6) {
      // select field1, field2, ... from tableName
      int currPos = 1;
      Token nameToken = aTokenizer.peek(currPos++);
      if (nameToken.type == TokenType::identifier) {
        tableName = nameToken.data;
        Token kwToken = aTokenizer.peek(currPos++);
        if (kwToken.keyword == Keywords::set_kw) {
          fieldToken = aTokenizer.peek(currPos++);
          if (fieldToken.type == TokenType::identifier) {
            Token operatorToken = aTokenizer.peek(currPos++);
            if (operatorToken.type == TokenType::operators) {
              valueToken = aTokenizer.peek(currPos++);
              if (valueToken.type == TokenType::identifier ||
                  valueToken.type == TokenType::number) {
                if (aTokenizer.remaining() > currPos &&
                    aTokenizer.peek(currPos++).keyword != Keywords::where_kw) {
                  return StatusResult(syntaxError);
                }
                while (aTokenizer.remaining() > currPos) {
                  StatusResult theResult = parseExpr(aTokenizer, currPos);
                  if (!theResult)
                    break;
                  if (aTokenizer.remaining() > currPos) {
                    operatorToken = aTokenizer.peek(currPos++);
                    if (operatorToken.type == TokenType::keyword) {
                      if (gOperators.count(operatorToken.data) > 0) {
                        op = gOperators[operatorToken.data];
                      } else {
                        return StatusResult(syntaxError);
                      }
                    }
                  } else {
                    break;
                  }
                }
                aTokenizer.next(currPos);
                return StatusResult(noError);
              }
            }
          }
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult parseExpr(Tokenizer &aTokenizer, int &pos) {
    if (aTokenizer.remaining() >= pos + 3) {
      if (aTokenizer.peek(pos).type == TokenType::identifier) {
        // lhs: (string) where "what" op what
        // lhs: focus on type and name
        Token currToken = aTokenizer.peek(pos++);
        ValueType aValue1;
        std::string str = "";
        aValue1.value = str;
        Operand lhs(currToken.data, currToken.type, aValue1);
        // operator
        currToken = aTokenizer.peek(pos++);
        if (currToken.type == TokenType::operators) {
          Operators aOp = Helpers::toOperator(currToken.data);
          // rhs: (identifier) where what op "what"
          // rhs: focus on name and value
          currToken = aTokenizer.peek(pos++);
          ValueType aValue2;
          aValue2.value = currToken.data;
          std::string aName = "";
          Operand rhs(aName, currToken.type, aValue2);
          // form an Expression {limit="someNumber"}
          Expression *expr = new Expression(lhs, aOp, rhs);
          expressionList.push_back(expr);
          return StatusResult(noError);
        }
      }
    }
    return StatusResult(syntaxError);
  }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    // Update
    // Check database exist
    if (!dbptr) {
      return StatusResult(Errors::noDatabaseSpecified);
    }
    // Check table exist
    bool existTable = false;
    dbptr->storage.load(dbptr->MetaBlock, 0);
    // Check if there is a table named tableName
    for (auto element : dbptr->MetaBlock.map) {
      if (element.second == 'S') {
        StorageBlock aBlock;
        dbptr->storage.readBlock(aBlock, element.first);
        if (!aBlock.header.pos) {
          dbptr->storage.load(*schema, element.first);
          if (schema->getName() == tableName) {
            existTable = true;
            break;
          }
        }
      }
    }
    if (!existTable) {
      return StatusResult(unknownTable);
    }
    // Check Field and Set Value
    AttributeList attributes = schema->getAttributes();
    std::string field = fieldToken.data;
    ValueType setValue;
    std::string tokenData = valueToken.data;
    std::transform(tokenData.begin(), tokenData.end(), tokenData.begin(),
                   ::tolower);
    bool existField = false;
    for (auto attr : attributes) {
      if (attr.getName() == field) {
        existField = true;
        DataType dtype = attr.getType();
        switch (dtype) {
        case DataType::bool_type:
          if (valueToken.type != TokenType::identifier) {
            return StatusResult(unknownType);
          }
          if (tokenData == "true") {
            setValue.value = true;
          } else {
            setValue.value = false;
          }
          break;
        case DataType::int_type:
          if (valueToken.type != TokenType::number) {
            return StatusResult(unknownType);
          }
          setValue.value = std::stoi(tokenData);
          break;
        case DataType::float_type:
          if (valueToken.type != TokenType::number) {
            return StatusResult(unknownType);
          }
          setValue.value = std::atof(tokenData.c_str());
          break;
        default:
          if (valueToken.type != TokenType::identifier) {
            return StatusResult(unknownType);
          }
          setValue.value = tokenData;
          break;
        }
        break;
      }
    }
    if (!existField) {
      return StatusResult(unknownAttribute);
    }

    // Set Value Expression
    ValueType nullValueType;
    Operand lhs(field, TokenType::identifier, nullValueType);
    std::string nullName = "";
    Operand rhs(nullName, TokenType::number, setValue);
    Expression *setExpr = new Expression(lhs, Operators::equal_op, rhs);

    // Check expressionlist validation
    std::map<DataType, int> DataTypeToInt = {
        {DataType::bool_type, 0},
        {DataType::int_type, 1},
        {DataType::float_type, 2},
        {DataType::varchar_type, 3},
    };
    bool allExprValid = true;
    for (Expression *expr : expressionList) {
      bool exprValid = false;
      std::string field = expr->lhs.name;
      Value value = expr->rhs.value.value;
      for (auto attr : attributes) {
        if (attr.getName() == field) {
          DataType dtype = attr.getType();
          std::string valuestr = std::get<std::string>(value);
          switch (dtype) {
          case DataType::bool_type:
            std::transform(valuestr.begin(), valuestr.end(), valuestr.begin(),
                           ::tolower);
            if (valuestr == "true") {
              expr->rhs.value.value = true;
            } else {
              expr->rhs.value.value = false;
            }
            break;
          case DataType::int_type:
            try {
              expr->rhs.value.value = std::stoi(valuestr);
            } catch (std::invalid_argument) {
              return StatusResult(unknownType);
            }
            break;
          case DataType::float_type:
            expr->rhs.value.value = std::atof(valuestr.c_str());
            break;
          default:
            break;
          }
          exprValid = true;
          break;
        }
      }
      allExprValid = allExprValid && exprValid;
    }
    if (!allExprValid) {
      return StatusResult(unknownAttribute);
    }
    Timer timer;
    timer.start();
    dbptr->updateRecords(schema, setExpr, expressionList, op);
    timer.stop();
    std::cout << "(" << timer.elapsed() << " ms.)" << std::endl;
    std::cout << "update " << tableName << " set (ok)\n";
    return StatusResult(noError);
  }

private:
  // parse
  std::string tableName;
  Schema *schema;
  Token fieldToken;
  Token valueToken;
  Expressions expressionList;
  Operators op;
};

class ShowIndexStatement : public Statement {
public:
  ShowIndexStatement() : Statement(Keywords::show_kw) {}

  StatusResult parse(Tokenizer &aTokenizer) {
    if (aTokenizer.remaining() >= 2) {
      Token firstToken = aTokenizer.peek(1);
      if (firstToken.keyword == Keywords::indexes_kw) {
        aTokenizer.next(2);
        return StatusResult(noError);
      }
    }
    return StatusResult(syntaxError);
  }

  const char *getStatementName() const { return "Show Statement"; }

  StatusResult run(std::ostream &aStream, Database *&dbptr) const {
    StatusResult theResult = dbptr->showIndexes();
    std::cout << "show indexes (ok)" << std::endl;
    return theResult;
  }

private:
};

} // namespace ECE141