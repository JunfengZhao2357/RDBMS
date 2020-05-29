//
//  Storage.hpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include "Errors.hpp"
#include "Filters.hpp"
#include "Row.hpp"
#include "StorageBlock.hpp"
#include "Value.hpp"
#include <fcntl.h>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>
#include <variant>

namespace ECE141 {

// first, some utility classes...

class StorageInfo {
public:
  static const char *getDefaultStoragePath();
};

struct CreateNewStorage {};
struct OpenExistingStorage {};

class BlockVisitor;

struct BlockIterator {
  virtual bool each(BlockVisitor &aVisitor) = 0;
  virtual bool canIndexBy(const std::string &aField) {
    return false;
  } // override this
};

class MetaData : public Storable {
public:
  virtual StatusResult encode(std::ostream &aWriter);
  virtual StatusResult decode(std::istream &aReader);
  BlockType getType() const { return BlockType::meta_block; };

  std::map<uint32_t, char> map;
};

// USE: Our storage manager class...
class Storage : public BlockIterator {
public:
  Storage(const std::string aName, CreateNewStorage);
  Storage(const std::string aName, OpenExistingStorage);
  ~Storage();
  uint32_t getTotalBlockCount();

  // high-level IO (you're not required to use this, but it may help)...
  StatusResult save(Storable &aStorable,
                    int aStartBlockNum = -1); // using a stream api
  StatusResult load(Storable &aStorable,
                    int aStartBlockNum = -1); // using a stream api

  // low-level IO...
  StatusResult readBlock(StorageBlock &aBlock, uint32_t aBlockNumber);
  StatusResult writeBlock(StorageBlock &aBlock, uint32_t aBlockNumber);

  StatusResult findFreeBlockNum(Storable &aStorable);
  StatusResult findFreeBlockNum(Storable &aStorable, MetaData &metaStorable);
  bool isReady() const;
  bool each(BlockVisitor &aVisitor);

  std::vector<Row> rowCollection;

protected:
  std::string name;
  std::fstream stream;
};

class BlockVisitor {
public:
  BlockVisitor(std::string aName, Filters &aFilters)
      : tableName(aName), filters(aFilters) {}
  StatusResult operator()(Storage &aStorage, Row &aRow, uint32_t aBlockNum);
  std::string tableName;
  Filters &filters;
};

} // namespace ECE141

#endif /* Storage_hpp */
