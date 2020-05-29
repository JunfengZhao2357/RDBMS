//
//  Index.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/17/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Index_h
#define Index_h

#include "Storage.hpp"
#include "keywords.hpp"

namespace ECE141 {

using IntOpt = std::optional<uint32_t>;

struct LessKey {
  bool operator()(const ValueType &anLHS, const ValueType &aRHS) const {
    return anLHS.value < aRHS.value;
  }
};


class Index : public BlockIterator, public Storable {
public:

    using ValueMap = std::map<ValueType, uint32_t, LessKey>;
  Index(const std::string &aField="NULL", uint32_t aHashId=0, DataType aType=DataType::no_type,uint32_t aBlockNum=0)
      : field(aField), type(aType), schemaId(aHashId), blockNum(aBlockNum) {
    type = aType;
    changed = false;
  }

  Index(const Index &aCopy):field(aCopy.getFieldName()),schemaId(aCopy.getSchemaId()),blockNum(aCopy.getBlockNum()){
      type= static_cast<DataType>(aCopy.getType());
      changed = false;
      for(auto iter=aCopy.list.begin();iter!=aCopy.list.end();iter++)
          list[iter->first]=iter->second;
  }

  Index& operator=(const Index &assignment){
      field = assignment.getFieldName();
      schemaId = assignment.getSchemaId();
      blockNum = assignment.getBlockNum();
      type = static_cast<DataType>(assignment.getType());
      changed = false;
      for(auto iter=assignment.list.begin();iter!=assignment.list.end();iter++)
          list[iter->first]=iter->second;
      return *this;
  }
  virtual ~Index() {}

  ValueMap &getList() { return list; }
  void setChanged(bool aValue = true) { changed = aValue; }
  bool isChanged() { return changed; }
  void setFieldName(std::string aName) { field = aName; }
  const std::string &getFieldName() const { return field; }
  void setBlockNum(uint32_t aNum) { blockNum = aNum; }
  uint32_t getBlockNum() const { return blockNum; }
  uint32_t getSchemaId() const {return schemaId;}
  // manage keys/values in the index...
  Index &addKeyValue(const ValueType &aKey, uint32_t aValue);
  Index &removeKeyValue(const ValueType &aKey);
  bool contains(const ValueType &aKey);
  uint32_t getValue(const ValueType &aKey);
  Index &clearList();

  // don't forget to support the storable interface IF you want it...
  virtual StatusResult encode(std::ostream &aWriter);
  virtual StatusResult decode(std::istream &aReader);
  virtual ECE141::BlockType getType() const{return BlockType::index_block;}
  // void initBlock(StorageBlock &aBlock);

  // and the blockIterator interface...
  bool each(BlockVisitor &aVisitor);
  bool canIndexBy(const std::string &aField);

  ValueMap list;
protected:
  std::string field; // what field are we indexing?
  DataType type;
  uint32_t schemaId;
  bool changed;
  uint32_t blockNum; // storage block# of index...


};

} // namespace ECE141
#endif /* Index_h */

