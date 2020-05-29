//
//  Schema.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Schema.hpp"

namespace ECE141 {

// STUDENT: Implement the Schema class here...
Schema::Schema(const std::string aName) : name(aName), aiValue(1) {}
Schema::Schema(const Schema &aCopy)
    : name(aCopy.getName()), blockNum(aCopy.getBlockNum()),
      changed(aCopy.isChanged()), attributes(aCopy.getAttributes()),
      aiValue(1) {}

Schema &Schema::addAttribute(const Attribute &anAttribute) {
  if (anAttribute.isValid()) {
    attributes.push_back(anAttribute);
  }
  return *this;
}

const AttributeOpt &Schema::getAttribute(const std::string &aName) const {
  AttributeOpt *return_attr = new std::optional<Attribute>;
  for (auto attr : attributes) {
    if (attr.getName() == aName)
      *return_attr = attr;
  }
  return *return_attr;
}

std::string Schema::getPrimaryKeyName() const {
  std::string res = attributes[0].getName();
  for (auto attr : attributes) {
    if (attr.getPrimary())
      res = attr.getName();
  }
  return res;
}

uint32_t Schema::getNextAutoIncrementValue() {
  uint32_t ret = aiValue++;
  return ret;
}

// Storable interface
StatusResult Schema::encode(std::ostream &aWriter) {
  aWriter << name << " " << blockNum << " " << changed
          << " " << aiValue; //"tablename, blockNum changed"
  aWriter << " " << attributes.size();
  for (auto attr : attributes) {
    aWriter << " " << attr.getName() << " " << (char)attr.getType() << " "
            << attr.getSize() << " " << attr.getAI() << " " << attr.getPrimary()
            << " " << attr.getNullable() << " "; // << attr.getDefault();
    attr.getDefault().encode(aWriter);
  }
  return StatusResult(noError);
}

StatusResult Schema::decode(std::istream &aReader) {
  std::string aName; // table name
  uint32_t aBlockNum;
  bool achanged;
  uint32_t aValue;
  aReader >> aName >> aBlockNum >> achanged >> aValue;
  Schema::setName(aName);
  Schema::setBlockNum(aBlockNum);
  Schema::setChanged(achanged);
  Schema::setAIValue(aValue);
  uint32_t theCount;
  aReader >> theCount;
  AttributeList aList;
  for (int i = 0; i < theCount; i++) {
    Attribute aAttribute;
    std::string theName;
    char theChar2; // datatype
    uint32_t theSize;
    uint32_t AI;
    uint32_t thePrimary;
    uint32_t theNullable;
    ValueType theDefault;
    std::string temp_str;
    aReader >> theName >> theChar2 >> theSize >> AI >> thePrimary >>
        theNullable; // >> theDefault;
    theDefault.decode(aReader);
    aAttribute.setName(theName);
    aAttribute.setType((DataType)theChar2);
    aAttribute.setSize(theSize);
    aAttribute.setAutoIncrement(AI);
    aAttribute.setPrimary(thePrimary);
    aAttribute.setNullable(theNullable);
    aAttribute.setDefault(theDefault);
    // aAttribute.setDefault(temp_str);
    aList.push_back(aAttribute);
  }
  Schema::attributes = aList;
  return StatusResult(noError);
}

} // namespace ECE141
