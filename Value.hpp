#ifndef Value_h
#define Value_h

#include "Errors.hpp"
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

namespace ECE141 {
enum class BlockType {
  meta_block = 'M',
  data_block = 'D',
  entity_block = 'E',
  schema_block = 'S',
  free_block = 'F',
  index_block = 'I',
  unknown_block = 'U',
};
enum class DataType {
  no_type = 'N',
  bool_type = 'B',
  datetime_type = 'D',
  float_type = 'F',
  int_type = 'I',
  varchar_type = 'V',
};

struct Storable {
  virtual StatusResult encode(std::ostream &aWriter) { return StatusResult(); };
  virtual StatusResult decode(std::istream &aReader) { return StatusResult(); };
  virtual ECE141::BlockType getType() const {
    return BlockType::unknown_block;
  }; // what kind of block is this?
  virtual ~Storable() {}
};

using Value = std::variant<bool, int, double, std::string>;
using KeyValues = std::map<const std::string, Value>;

//---------------------ValueType------------------------
struct ValueType {
  Value value;

  ValueType() {
    std::string nullstr = "NULL";
    value = nullstr;
  }

  ValueType &operator=(const ValueType &aValueType) {
    value = aValueType.value;
    return *this;
  }

  bool operator==(const ValueType &aValueType) {
    return value == aValueType.value ? true : false;
  }

  explicit ValueType(const bool &aValue) : value(aValue) {}
  explicit ValueType(const int &aValue) : value(aValue) {}
  explicit ValueType(const double &aValue) : value(aValue) {}
  explicit ValueType(const std::string &aValue) : value(aValue) {}
  explicit ValueType(const char *aValue) : value(std::string(aValue)) {}

  ValueType &setValue(const Value &aValue) {
    switch (aValue.index()) {
    case 0:
      value = std::get<bool>(aValue);
      break;
    case 1:
      value = std::get<int>(aValue);
      break;
    case 2:
      value = std::get<double>(aValue);
      break;
    case 3:
      value = std::get<std::string>(aValue);
      break;
    default:
      break;
    }
    return *this;
  }

  DataType get_Type() const {
    static std::map<std::size_t, DataType> gTypes{
        {0, DataType::bool_type},
        {1, DataType::int_type},
        {2, DataType::float_type},
        {3, DataType::bool_type},
    };
    return gTypes[value.index()];
  }

  std::optional<bool> getBool() const {
    if (auto pval = std::get_if<bool>(&value))
      return *pval;
    return std::nullopt;
  }

  std::optional<int> getInt() const {
    if (auto pval = std::get_if<int>(&value))
      return *pval;
    return std::nullopt;
  }

  std::optional<double> getFloat() const {
    if (auto pval = std::get_if<double>(&value))
      return *pval;
    return std::nullopt;
  }

  std::optional<std::string> getString() const {
    if (auto pval = std::get_if<std::string>(&value))
      return *pval;
    return std::nullopt;
  }

  bool operator<(const ValueType &aValue){
      switch(value.index()){
          case 0: return std::get<bool>(value) < std::get<bool>(aValue.value);
          case 1: return std::get<int>(value) < std::get<int>(aValue.value);
          case 2: return std::get<double>(value) < std::get<double>(aValue.value);
          case 3: return std::get<std::string>(value) < std::get<std::string>(aValue.value);
          default:return StatusResult(unknownType);
      }
  }
  friend std::ostream &operator<<(std::ostream &output,
                                  const ValueType &aValue) {
    switch (aValue.value.index()) {
    case 0:
      if (std::get<bool>(aValue.value)) {
        output << "TRUE";
      } else {
        output << "FALSE";
      }
      break;
    case 1:
      output << std::get<int>(aValue.value);
      break;
    case 2:
      output << std::get<double>(aValue.value);
      break;
    case 3:
      output << std::get<std::string>(aValue.value);
      break;
    default:
      break;
    }
    return output;
  }

  bool isNum(const std::string str) {
    std::stringstream ss(str);
    double d;
    char c;
    if (!(ss >> d)) {
      return false;
    }
    if (ss >> c) {
      return false;
    }
    return true;
  }

  //-------------------------storable API------------------------
  virtual StatusResult encode(std::ostream &aWriter) const {
    switch (value.index()) {
    case 0:
      aWriter << "B ";
      if (std::get<bool>(value)) {
          aWriter << "TRUE ";
      } else {
          aWriter << "FALSE ";
      }
      break;
    case 1:
      aWriter << "I " << std::get<int>(value) << " ";
      break;
    case 2:
      aWriter << "F " << std::get<double>(value) << " ";
      break;
    case 3:
      aWriter << "V " << std::get<std::string>(value).length() << " "
              << std::get<std::string>(value) << " ";
      break;
    default:
      break;
    }
    return StatusResult();
  }

  virtual StatusResult decode(std::istream &aReader) {
    std::string theType;
    aReader >> theType;
    while (theType == " ") {
      aReader >> theType;
    }
    std::string readin;
    aReader >> readin;
    if (theType == "B") {
      std::transform(readin.begin(), readin.end(), readin.begin(), ::tolower);
      if (readin == "true") value = true;
      else value = false;
    } else if (theType == "I") {
      int itemp;
      std::istringstream(readin) >> itemp;
      value = itemp;
    } else if (theType == "F") {
      double ftemp;
      std::istringstream(readin) >> ftemp;
      value = ftemp;
    } else if (theType == "V") {
      int length = std::stoi(readin);
      std::string stemp;
      aReader >> stemp;
      while (stemp.length() < length) {
        std::string str;
        aReader >> str;
        stemp += " " + str;
      }
      value = stemp;
    } else {
      return StatusResult(Errors::unknownType);
    }
    return StatusResult();
  }
};

} // namespace ECE141

#endif /* Value_h */
