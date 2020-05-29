//
//  StorageBlock.hpp
//  Assignment3
//
//  Created by rick gessner on 4/11/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef StorageBlock_hpp
#define StorageBlock_hpp

#include "Value.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>
#include <variant>

namespace ECE141 {

// a "storage" file is comprised of fixed-sized blocks (defined below)

const size_t kPayloadSize =
    1008; // area reserved in storage-block for user data...

// using ValueType = std::variant<int, double, std::string>;

// using KeyValues = std::map<const std::string, ValueType>;

// enum class BlockType {
//   meta_block = 'M',
//   data_block = 'D',
//   entity_block = 'E',
//   schema_block = 'S',
//   free_block = 'F',
//   index_block = 'I',
//   unknown_block = 'U',
// };

static std::map<BlockType, std::string> BlockTypeStr = {
    {BlockType::meta_block, "meta"},       {BlockType::data_block, "data"},
    {BlockType::entity_block, "entity"},   {BlockType::schema_block, "schema"},
    {BlockType::free_block, "free"},       {BlockType::index_block, "index"},
    {BlockType::unknown_block, "unknown"},
};

using NamedBlockNums = std::map<std::string, uint32_t>;

struct BlockHeader {

  BlockHeader(BlockType aType = BlockType::data_block)
      : type(static_cast<char>(aType)), count(0), pos(0), next(0) {}

  BlockHeader(const BlockHeader &aCopy) { *this = aCopy; }

  BlockHeader &operator=(const BlockHeader &aCopy) {
    type = aCopy.type;
    pos = aCopy.pos;
    count = aCopy.count;
    return *this;
  }

  char type; // char version of block type {[D]ata, [F]ree... }
  uint32_t count;
  uint32_t pos; // use this anyway you like
  uint32_t next;
};

struct StorageBlock {

  StorageBlock(BlockType aType = BlockType::data_block); // constructor

  StorageBlock(const StorageBlock &aCopy);            // copy constructor
  StorageBlock &operator=(const StorageBlock &aCopy); // assigment

  StorageBlock &store(std::ostream &aStream);

  // we use attributes[0] as table name...
  BlockHeader header;
  char data[kPayloadSize];
};

} // namespace ECE141

#endif /* StorageBlock_hpp */
