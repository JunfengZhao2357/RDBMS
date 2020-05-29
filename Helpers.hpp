//
//  Helpers.hpp
//  Database3
//
//  Created by rick gessner on 4/14/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Helpers_h
#define Helpers_h

#include "keywords.hpp"
#include <algorithm>

namespace ECE141 {

  template<typename T, size_t aSize>
  bool in_array(T (&anArray)[aSize], const T &aValue) {
    return std::find(std::begin(anArray), std::end(anArray), aValue);
  };
  

  static std::map<std::string, Operators> gOperators = {
    std::make_pair(".", Operators::dot_op),
    std::make_pair("+", Operators::add_op),
    std::make_pair("-", Operators::subtract_op),
    std::make_pair("*", Operators::multiply_op),
    std::make_pair("/", Operators::divide_op),
    std::make_pair("^", Operators::power_op),
    std::make_pair("%", Operators::mod_op),
    std::make_pair("=", Operators::equal_op),
    std::make_pair("!=", Operators::notequal_op),
    std::make_pair("not", Operators::notequal_op),
    std::make_pair("<", Operators::lt_op),
    std::make_pair("<=", Operators::lte_op),
    std::make_pair(">", Operators::gt_op),
    std::make_pair(">=", Operators::gte_op),
    std::make_pair("and", Operators::and_op),
    std::make_pair("or", Operators::or_op),
    std::make_pair("nor", Operators::nor_op),
    std::make_pair("between", Operators::between_op),
  };
  
  static ECE141::Keywords gJoinTypes[]={
    ECE141::Keywords::cross_kw,ECE141::Keywords::full_kw, ECE141::Keywords::inner_kw,
    ECE141::Keywords::left_kw, ECE141::Keywords::right_kw
  };
  
  //a list of known functions...
  static std::map<std::string,int> gFunctions = {
    std::make_pair("avg", 10),
    std::make_pair("count", 20),
    std::make_pair("max", 30),
    std::make_pair("min", 40)
  };
  
  
  //This map binds a keyword string with a Keyword (token)...
  static std::map<std::string,  Keywords> gDictionary = {
    std::make_pair("add",       Keywords::add_kw),
    std::make_pair("all",       Keywords::all_kw),
    std::make_pair("alter",     Keywords::alter_kw),
    std::make_pair("and",       Keywords::and_kw),
    std::make_pair("as",        Keywords::as_kw),
    std::make_pair("avg",       ECE141::Keywords::avg_kw),
    std::make_pair("auto_increment", Keywords::auto_increment_kw),
    std::make_pair("between",   ECE141::Keywords::between_kw),
    std::make_pair("boolean",   ECE141::Keywords::boolean_kw),
    std::make_pair("by",        ECE141::Keywords::by_kw),
    std::make_pair("char",      ECE141::Keywords::char_kw),
    std::make_pair("column",    ECE141::Keywords::column_kw),
    std::make_pair("count",     ECE141::Keywords::count_kw),
    std::make_pair("create",    ECE141::Keywords::create_kw),
    std::make_pair("cross",     ECE141::Keywords::cross_kw),
    std::make_pair("database",  ECE141::Keywords::database_kw),
    std::make_pair("databases", ECE141::Keywords::databases_kw),
    std::make_pair("datetime",  ECE141::Keywords::datetime_kw),
    std::make_pair("decimal",   ECE141::Keywords::decimal_kw),
    std::make_pair("delete",    ECE141::Keywords::delete_kw),
    std::make_pair("describe",  ECE141::Keywords::describe_kw),
    std::make_pair("distinct",  ECE141::Keywords::distinct_kw),
    std::make_pair("double",    ECE141::Keywords::double_kw),
    std::make_pair("drop",      ECE141::Keywords::drop_kw),
    std::make_pair("explain",   ECE141::Keywords::explain_kw),
    std::make_pair("float",     ECE141::Keywords::float_kw),
    std::make_pair("foreign",   ECE141::Keywords::foreign_kw),
    std::make_pair("from",      ECE141::Keywords::from_kw),
    std::make_pair("full",      ECE141::Keywords::full_kw),
    std::make_pair("group",     ECE141::Keywords::group_kw),
    std::make_pair("help",      ECE141::Keywords::help_kw),
    std::make_pair("indexes",   ECE141::Keywords::indexes_kw),
    std::make_pair("in",        ECE141::Keywords::in_kw),
    std::make_pair("inner",     ECE141::Keywords::inner_kw),
    std::make_pair("insert",    ECE141::Keywords::insert_kw),
    std::make_pair("int",       ECE141::Keywords::integer_kw),
    std::make_pair("integer",   ECE141::Keywords::integer_kw),
    std::make_pair("into",      ECE141::Keywords::into_kw),
    std::make_pair("join",      ECE141::Keywords::join_kw),
    std::make_pair("key",       ECE141::Keywords::key_kw),
    std::make_pair("left",      ECE141::Keywords::left_kw),
    std::make_pair("limit",     ECE141::Keywords::limit_kw),
    std::make_pair("max",       ECE141::Keywords::max_kw),
    std::make_pair("min",       ECE141::Keywords::min_kw),
    std::make_pair("modify",    ECE141::Keywords::modify_kw),
    std::make_pair("not",       ECE141::Keywords::not_kw),
    std::make_pair("null",      ECE141::Keywords::null_kw),
    std::make_pair("on",        ECE141::Keywords::on_kw),
    std::make_pair("or",        ECE141::Keywords::or_kw),
    std::make_pair("order",     ECE141::Keywords::order_kw),
    std::make_pair("outer",     ECE141::Keywords::outer_kw),
    std::make_pair("primary",   ECE141::Keywords::primary_kw),
    std::make_pair("quit",      ECE141::Keywords::quit_kw),
    std::make_pair("references", ECE141::Keywords::references_kw),
    std::make_pair("right",     ECE141::Keywords::right_kw),
    std::make_pair("select",    ECE141::Keywords::select_kw),
    std::make_pair("self",      ECE141::Keywords::self_kw),
    std::make_pair("set",       ECE141::Keywords::set_kw),
    std::make_pair("show",      ECE141::Keywords::show_kw),
    std::make_pair("sum",       ECE141::Keywords::sum_kw),
    std::make_pair("table",     ECE141::Keywords::table_kw),
    std::make_pair("tables",    ECE141::Keywords::tables_kw),
    std::make_pair("unique",    ECE141::Keywords::unique_kw),
    std::make_pair("update",    ECE141::Keywords::update_kw),
    std::make_pair("use",       ECE141::Keywords::use_kw),
    std::make_pair("values",    ECE141::Keywords::values_kw),
    std::make_pair("varchar",   ECE141::Keywords::varchar_kw),
    std::make_pair("version",   ECE141::Keywords::version_kw),
    std::make_pair("where",     ECE141::Keywords::where_kw)
  };
  
  static std::map<Keywords, std::string> HelpInfo = {
    std::make_pair(Keywords::add_kw, "-- Add your database use \"add database <dbName>\""),
    std::make_pair(Keywords::all_kw, "-- Some help information about all"),
    std::make_pair(Keywords::alter_kw, "-- Some help information about alter"),
    std::make_pair(Keywords::and_kw, "-- Some help information about and"),
    std::make_pair(Keywords::as_kw,  "-- Some help information about as"),
    std::make_pair(Keywords::avg_kw, "-- Some help information about avg"),
    std::make_pair(Keywords::auto_increment_kw,  "-- Some help information about auto_increament"),
    std::make_pair(Keywords::between_kw, "-- Some help information about between"),
    std::make_pair(Keywords::boolean_kw, "-- Some help information about boolean"),
    std::make_pair(Keywords::by_kw,  "-- Some help information about by"),
    std::make_pair(Keywords::char_kw,  "-- Some help information about char"),
    std::make_pair(Keywords::column_kw,  "-- Some help information about column"),
    std::make_pair(Keywords::count_kw, "-- Some help information about count"),
    std::make_pair(Keywords::create_kw,  "-- Create your database use \"create database <dbName>\""),
    std::make_pair(Keywords::cross_kw, "-- Some help information about cross"),
    std::make_pair(Keywords::database_kw,  "-- Some help information about database"),
    std::make_pair(Keywords::databases_kw, "-- Some help information about databases"),
    std::make_pair(Keywords::datetime_kw,  "-- Some help information about datetime"),
    std::make_pair(Keywords::decimal_kw, "-- Some help information about decimal"),
    std::make_pair(Keywords::delete_kw,  "-- Some help information about delete"),
    std::make_pair(Keywords::describe_kw,  "-- Describe your database use \"describe database <dbName>\""),
    std::make_pair(Keywords::distinct_kw,  "-- Some help information about distinct"),
    std::make_pair(Keywords::double_kw,  "-- S o i meelp information about double"),
    std::make_pair(Keywords::drop_kw,  "-- Drop your database use \"drop database <dbName>\""),
    std::make_pair(Keywords::explain_kw, "-- Some help information about explain"),
    std::make_pair(Keywords::float_kw, "-- Some help information about float"),
    std::make_pair(Keywords::foreign_kw, "-- Some help information about foreign"),
    std::make_pair(Keywords::from_kw,  "-- Some help information about from"),
    std::make_pair(Keywords::full_kw,  "-- Some help information about full"),
    std::make_pair(Keywords::group_kw, "-- Some help information about group"),
    std::make_pair(Keywords::help_kw,  "-- Get help information about commands"),
    std::make_pair(Keywords::in_kw,  "-- Some help information about in"),
    std::make_pair(Keywords::inner_kw, "-- Some help information about inner"),
    std::make_pair(Keywords::insert_kw,  "-- Some help information about insert"),
    std::make_pair(Keywords::integer_kw, "-- Some help information about integer"),
    std::make_pair(Keywords::into_kw,  "-- Some help information about into"),
    std::make_pair(Keywords::join_kw,  "-- Some help information about join"),
    std::make_pair(Keywords::key_kw, "-- Some help information about key"),
    std::make_pair(Keywords::left_kw,  "-- Some help information about left"),
    std::make_pair(Keywords::limit_kw,  "-- Some help information about limit"),
    std::make_pair(Keywords::max_kw, "-- Some help information about max"),
    std::make_pair(Keywords::min_kw, "-- Some help information about min"),
    std::make_pair(Keywords::modify_kw,  "-- Some help information about modify"),
    std::make_pair(Keywords::not_kw, "-- Some help information about not"),
    std::make_pair(Keywords::null_kw,  "-- Some help information about null"),
    std::make_pair(Keywords::on_kw,  "-- Some help information about on"),
    std::make_pair(Keywords::or_kw,  "-- Some help information about or"),
    std::make_pair(Keywords::order_kw, "-- Some help information about order"),
    std::make_pair(Keywords::outer_kw, "-- Some help information about outer"),
    std::make_pair(Keywords::primary_kw, "-- Some help information about primary"),
    std::make_pair(Keywords::quit_kw,  "-- Quit"),
    std::make_pair(Keywords::references_kw,  "-- Some help information about references"),
    std::make_pair(Keywords::right_kw, "-- Some help information about right"),
    std::make_pair(Keywords::select_kw,  "-- Some help information about select"),
    std::make_pair(Keywords::self_kw,  "-- Some help information about self"),
    std::make_pair(Keywords::set_kw, "-- Some help information about set"),
    std::make_pair(Keywords::show_kw,  "-- Show your database use \"show databases\""),
    std::make_pair(Keywords::sum_kw, "-- Some help information about sum"),
    std::make_pair(Keywords::table_kw, "-- Some help information about table"),
    std::make_pair(Keywords::tables_kw,  "-- Some help information about tables"),
    std::make_pair(Keywords::unique_kw,  "-- Some help information about unique"),
    std::make_pair(Keywords::update_kw,  "-- Some help information about update"),
    std::make_pair(Keywords::use_kw, "-- Use your database use \"use database <dbName>\""),
    std::make_pair(Keywords::values_kw,  "-- Some help information about values"),
    std::make_pair(Keywords::varchar_kw, "-- Some help information about varchar"),
    std::make_pair(Keywords::version_kw, "-- Get the current version of this program"),
    std::make_pair(Keywords::where_kw,  "-- Some help information abou where")
  };
  
  class Helpers {
  public:
    
    static Keywords getKeywordId(const std::string aKeyword) {
      auto theIter = gDictionary.find(aKeyword);
      if (theIter != gDictionary.end()) {
        return theIter->second;
      }
      return Keywords::unknown_kw;
    }
    
    //convert from char to keyword...
    static Keywords charToKeyword(char aChar) {
      switch(toupper(aChar)) {
        case 'I': return Keywords::integer_kw;
        case 'T': return Keywords::datetime_kw;
        case 'B': return Keywords::boolean_kw;
        case 'F': return Keywords::float_kw;
        case 'V': return Keywords::varchar_kw;
        default:  return Keywords::unknown_kw;
      }
    }
        
    static const char* keywordToString(Keywords aType) {
      switch(aType) {
        case Keywords::integer_kw:  return "integer";
        case Keywords::float_kw:    return "float";
        case Keywords::boolean_kw:  return "bool";
        case Keywords::datetime_kw: return "datetime";
        case Keywords::varchar_kw:  return "varchar";
        default:                    return "unknown";
      }
    }
  
    // USE: ---validate that given keyword is a datatype...
    static bool isDatatype(Keywords aKeyword) {
      switch(aKeyword) {
        case Keywords::char_kw:
        case Keywords::datetime_kw:
        case Keywords::float_kw:
        case Keywords::integer_kw:
        case Keywords::varchar_kw:
          return true;
        default: return false;
      }
    }
    
    static Operators toOperator(std::string aString) {
      auto theIter = gOperators.find(aString);
      if (theIter != gOperators.end()) {
        return theIter->second;
      }
      return Operators::unknown_op;
    }
    
    static int getFunctionId(const std::string anIdentifier) {
      auto theIter = gFunctions.find(anIdentifier);
      if (theIter != gFunctions.end()) {
        return theIter->second;
      }
      return 0;
    }
    
    static bool isNumericKeyword(Keywords aKeyword) {
      static Keywords theTypes[]={Keywords::decimal_kw, Keywords::double_kw, Keywords::float_kw, Keywords::integer_kw};
      for(auto k : theTypes) {
        if(aKeyword==k) return true;
      }
      return false;
    }

  };
  
  
}
#endif /* Helpers_h */
