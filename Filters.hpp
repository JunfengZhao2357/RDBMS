//
//  Filters.hpp
//  RGAssignment6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Filters_h
#define Filters_h

#include "Errors.hpp"
#include "Tokenizer.hpp"
#include "Value.hpp"
#include <stdio.h>
#include <string>
#include <vector>

namespace ECE141 {

class Row;
class Schema;

struct Operand {
  Operand() {}
  Operand(std::string &aName, TokenType aType, ValueType &aValue,
          uint32_t anId = 0)
      : type(aType), name(aName), value(aValue), schemaId(anId) {}
  Operand(const Operand &aCopyOperand);
  Operand &operator=(const Operand &aCopyOperand);
  TokenType type;   // so we know if it's a field, a const (number, string)...
  std::string name; // for named attr. in schema
  ValueType value;
  uint32_t schemaId;
};

//---------------------------------------------------

struct Expression {
  Operand lhs;
  Operand rhs;
  Operators op;
  //--------------------OCF methods----------------------
  Expression(Operand &aLHSOperand, Operators anOp, Operand &aRHSOperand)
      : lhs(aLHSOperand), rhs(aRHSOperand), op(anOp) {}
  Expression(const Expression &aCopyExpression);
  Expression &operator=(const Expression &aCopyExpression);

  //-------------------other methods------------------

  bool less_than(ValueType &aValue);
  bool larger_equal(ValueType &aValue);

  bool equal(ValueType &aValue);
  bool not_equal(ValueType &aValue);

  bool larger_than(ValueType &aValue);
  bool less_equal(ValueType &aValue);
  bool operator()(std::map<std::string, ValueType> &aList);
};

//---------------------------------------------------

using Expressions = std::vector<Expression *>;

//---------------------------------------------------

class Filters {
public:
  Filters(){};
  Filters(const Filters &aFilters);
  ~Filters(){};

  size_t getCount() const { return expressions.size(); }
  bool matches(std::map<std::string, ValueType> &aList);
  Filters &add(Expression *anExpression);

  friend class Tokenizer;

protected:
  Expressions expressions;
};

class TableField {
public:
  TableField(std::string aStr) : originString(aStr) {}

  TableField &operator=(TableField aTableField) {
    originString = aTableField.originString;
    tableName = aTableField.tableName;
    fieldName = aTableField.fieldName;
    return *this;
  }

  TableField &parse() {
    size_t dotpos = originString.find('.');
    if (dotpos == originString.npos) {
      return *this;
    }
    tableName = originString.substr(0, dotpos);
    fieldName = originString.substr(dotpos + 1);
    return *this;
  }

  std::string originString;
  std::string tableName;
  std::string fieldName;
};

class Join {
public:
  Join(const std::string &aTable, Keywords aType, const std::string &aLHS,
       const std::string &aRHS)
      : tableName(aTable), joinType(aType), onLeft(aLHS), onRight(aRHS) {}

  void swaplr() {
    if (onLeft.tableName == this->tableName) {
      TableField temp = onLeft;
      onLeft = onRight;
      onRight = temp;
    }
  }
  std::string tableName;
  Keywords joinType;
  TableField onLeft;
  TableField onRight;
};
} // namespace ECE141

#endif /* Filters_h */
