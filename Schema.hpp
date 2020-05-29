//
//  Schema.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include "Attribute.hpp"
#include "Errors.hpp"
#include "Storage.hpp"
#include "Value.hpp"
#include <optional>
#include <stdio.h>
#include <vector>
//#include "Row.hpp"

namespace ECE141 {

struct Block;
struct Expression;
class Database;
class Tokenizer;

// using StringList = std::vector<std::string>;
// using attribute_callback = std::function<bool(const Attribute &anAttribute)>;
using StringOpt = std::optional<std::string>;
using AttributeOpt = std::optional<Attribute>;
using AttributeList = std::vector<Attribute>;

// STUDENT: If you're using the Storable interface, add that to Schema class?

class Schema : public Storable {
public:
  Schema(const std::string aName = "");
  Schema(const Schema &aCopy);

  ~Schema() {}

  const std::string &getName() const { return name; }
  const AttributeList &getAttributes() const { return attributes; }
  uint32_t getBlockNum() const { return blockNum; }
  bool isChanged() const { return changed; }

  void setName(std::string aName) { name = aName; }
  void setBlockNum(int aBlockNum) { blockNum = aBlockNum; }
  void setChanged(bool aChanged) { changed = aChanged; }
  void setAIValue(int aValue) { aiValue = aValue; }

  Schema &addAttribute(const Attribute &anAttribute);
  const std::optional<Attribute> &getAttribute(const std::string &aName) const;

  // STUDENT: These methods will be implemented in the next assignment...

  // Value                 getDefaultValue(const Attribute &anAttribute) const;
  // StatusResult          validate(KeyValues &aList);

  std::string getPrimaryKeyName() const;
  uint32_t getNextAutoIncrementValue();

  // Storable interface
  virtual StatusResult encode(std::ostream &aWriter);
  virtual StatusResult decode(std::istream &aReader);
  virtual BlockType getType() const { return BlockType::schema_block; }
  // STUDENT: Do you want to provide an each() method for observers?

  // friend class Database; //is this helpful?

protected:
  std::string name;  // table name
  uint32_t blockNum; // storage location.
  bool changed;
  AttributeList attributes;
  uint32_t aiValue;
};

} // namespace ECE141
#endif /* Schema_hpp */
