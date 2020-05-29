//
//  Filters.hpp
//  Assignement6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include "Row.hpp"
#include "Schema.hpp"
#include <functional>
#include <stdio.h>
namespace ECE141 {
//----------------------------Operand
//methods----------------------------------------------
Operand::Operand(const Operand &aCopyOperand) {
  type = aCopyOperand.type;
  name = aCopyOperand.name;
  value = aCopyOperand.value;
  schemaId = aCopyOperand.schemaId;
}
Operand &Operand::operator=(const Operand &aCopyOperand) {
  type = aCopyOperand.type;
  name = aCopyOperand.name;
  value = aCopyOperand.value;
  schemaId = aCopyOperand.schemaId;
  return *this;
}

//-------------------------------------Expression
//Methods---------------------------------------
Expression &Expression::operator=(const Expression &aCopyExpression) {
  lhs = aCopyExpression.lhs;
  rhs = aCopyExpression.rhs;
  op = aCopyExpression.op;
  return *this;
}
Expression::Expression(const Expression &aCopyExpression) {
  lhs = aCopyExpression.lhs;
  rhs = aCopyExpression.rhs;
  op = aCopyExpression.op;
}
//-------------------------------------------------------------------------------------------
bool Expression::less_than(ValueType &aValue) {
  switch (aValue.value.index()) {
  case 0:
    return (std::get<bool>(aValue.value) < std::get<bool>(rhs.value.value));
  case 1:
    return (std::get<int>(aValue.value) < std::get<int>(rhs.value.value));
  case 2:
    return (std::get<double>(aValue.value) < std::get<double>(rhs.value.value));
  case 3:
    return (std::get<std::string>(aValue.value) <
            std::get<std::string>(rhs.value.value));
  default:
    return StatusResult(unknownType);
  }
}

bool Expression::larger_equal(ValueType &aValue) {
  return (!less_than(aValue));
}

//----------------------------------------------------------------------------------------------
bool Expression::equal(ValueType &aValue) {
  switch (aValue.value.index()) {
  case 0:
    return (std::get<bool>(aValue.value) == std::get<bool>(rhs.value.value));
  case 1:
    return (std::get<int>(aValue.value) == std::get<int>(rhs.value.value));
  case 2:
    return (std::get<double>(aValue.value) ==
            std::get<double>(rhs.value.value));
  case 3:
    return (std::get<std::string>(aValue.value) ==
            std::get<std::string>(rhs.value.value));
  default:
    return StatusResult(unknownType);
  }
}
bool Expression::not_equal(ValueType &aValue) { return (!equal(aValue)); }

//-------------------------------------------------------------------------------------------

bool Expression::larger_than(ValueType &aValue) {
  switch (aValue.value.index()) {
  case 0:
    return (std::get<bool>(aValue.value) > std::get<bool>(rhs.value.value));
  case 1:
    return (std::get<int>(aValue.value) > std::get<int>(rhs.value.value));
  case 2:
    return (std::get<double>(aValue.value) > std::get<double>(rhs.value.value));
  case 3:
    return (std::get<std::string>(aValue.value) >
            std::get<std::string>(rhs.value.value));
  default:
    return StatusResult(unknownType);
  }
}
bool Expression::less_equal(ValueType &aValue) {
  return (!larger_than(aValue));
}

//--------------------------------------------------------------------------------------------
bool Expression::operator()(std::map<std::string, ValueType> &aList) {
  //        static std::map<ECE141::Operators, std::function<bool(ValueType)>>
  //        factories = {
  //            {ECE141::Operators::equal_op,
  //            equal(aList[std::get<std::string>(lhs.value.value)])},
  //            {ECE141::Operators::notequal_op,
  //            not_equal(aList[std::get<std::string>(lhs.value.value)])},
  //            {ECE141::Operators::lt_op,
  //            less_than(aList[std::get<std::string>(lhs.value.value)])},
  //    };

  if (op == Operators::equal_op)
    return equal(aList[lhs.name]);
  else if (op == Operators::notequal_op)
    return not_equal(aList[lhs.name]);
  else if (op == Operators::lt_op)
    return less_than(aList[lhs.name]);
  else if (op == Operators::gte_op)
    return larger_equal(aList[lhs.name]);
  else if (op == Operators::gt_op)
    return larger_than(aList[lhs.name]);
  else
    return less_equal(aList[lhs.name]);
}

//-----------------------------Filters
//Method-------------------------------------------------
Filters::Filters(const Filters &aFilters) {
  for (auto element : aFilters.expressions) {
    expressions.push_back(element);
  }
}
Filters &Filters::add(Expression *anExpression) {
  expressions.push_back(anExpression);
  return *this;
}

bool Filters::matches(std::map<std::string, ValueType> &aList) {
  for (auto element : expressions) {
    if (!(*element)(aList))
      return false;
  }
  return true;
}

} // namespace ECE141
