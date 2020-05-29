//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include "Value.hpp"
#include <iostream>
#include <stdio.h>
#include <string>

namespace ECE141 {

// enum class DataType {
//   no_type = 'N',
//   bool_type = 'B',
//   datetime_type = 'D',
//   float_type = 'F',
//   int_type = 'I',
//   varchar_type = 'V',
// };

class Attribute {
protected:
  std::string name;
  ValueType defaultValue;
  DataType type;
  uint32_t size : 24;
  uint32_t autoIncrement : 1;
  uint32_t primary : 1;
  uint32_t nullable : 1;
  // STUDENT: What other data should you save in each attribute?

public:
  Attribute(DataType aType = DataType::no_type);
  Attribute(std::string aName, ValueType aDefault, DataType aType,
            uint32_t aSize, uint32_t aAI, uint32_t aPrimary,
            uint32_t aNullable);
  Attribute(const Attribute &aCopy);
  ~Attribute() {}

  Attribute &setName(std::string &aName) {
    name = aName;
    return *this;
  }
  Attribute &setDefault(ValueType &aDefault) {
    defaultValue = aDefault;
    return *this;
  }
  Attribute &setDefault(std::string &aDefault) {
    if (aDefault == "NULL") {
      // no default value
      std::string nullstr = "NULL";
      defaultValue.value = nullstr;
      return *this;
    }
    switch (type) {
    case DataType::int_type:
      try {
        defaultValue.value = std::stoi(aDefault);
      } catch (std::invalid_argument) {
        std::cout << "[ERROR] setDefault(int) type error" << std::endl;
      }
      break;
    case DataType::float_type:
      try {
        defaultValue.value = (double)std::atof(aDefault.c_str());
      } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
      }
      break;
    case DataType::bool_type:
      transform(aDefault.begin(), aDefault.end(), aDefault.begin(), ::tolower);
      if (aDefault == "true") {
        defaultValue.value = true;
      } else if (aDefault == "false") {
        defaultValue.value = false;
      } else {
        std::cout << "[ERROR] setDefault(bool) type error" << std::endl;
      }
      break;
    // case DataType::datetime_type:
    //   /* code */
    //   break;
    default:
      if (aDefault.length() > size) {
        std::cout << "[ERROR] setDefault(varchar) length error" << std::endl;
      }
      defaultValue.value = aDefault;
    }
    return *this;
  }
  
  bool isDefault(){
      if(defaultValue.value.index()==3){
          if(std::get<std::string>(defaultValue.value)=="NULL")
              return false;
          else
              return true;
      }
      return true;

  }
  Attribute &setType(DataType aType) {
    type = aType;
    return *this;
  }
  Attribute &setSize(uint32_t aSize) {
    size = aSize;
    return *this;
  }
  Attribute &setAutoIncrement(uint32_t aAI) {
    autoIncrement = aAI;
    if (autoIncrement == 1) {
      defaultValue.value = 1;
    }
    return *this;
  }
  Attribute &setPrimary(uint32_t aPrimary) {
    primary = aPrimary;
    if (aPrimary == 1) {
      nullable = 0;
    }
    return *this;
  }
  Attribute &setNullable(uint32_t aNullable) {
    nullable = aNullable;
    return *this;
  }

  bool isValid() const; // is this schema valid? Are all the attributes value?

  const std::string &getName() const { return name; }
  const ValueType &getDefault() const { return defaultValue; }
  DataType getType() const { return type; }
  uint32_t getSize() const { return size; }
  uint32_t getAI() const { return autoIncrement; }
  uint32_t getPrimary() const { return primary; }
  uint32_t getNullable() const { return nullable; }

  // STUDENT: are there other getter/setters to manage other attribute
  // properties?
  bool isPrimaryKey() { return primary; }
  bool isNullable() { return nullable; }
  bool isAutoIncrement() { return autoIncrement; }
};

} // namespace ECE141

#endif /* Attribute_hpp */
