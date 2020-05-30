//
//  Storage.cpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <math.h>
#include <sstream>

namespace ECE141 {
using namespace std;
// USE: Our main class for managing storage...
const char *StorageInfo::getDefaultStoragePath() {
  // STUDENT -- MAKE SURE TO SET AN ENVIRONMENT VAR for DB_PATH!
  //           This lets us change the storage location during autograder
  //           testing

  // WINDOWS USERS:  Use forward slash (/) not backslash (\) to separate paths.
  //                (Windows convert forward slashes for you)

  const char *thePath = std::getenv("DB_PATH");
  return thePath;
}

//----------------------------------------------------------

// path to the folder where you want to store your DB's...
std::string getDatabasePath(const std::string &aDBName) {
  std::string thePath = std::string(StorageInfo::getDefaultStoragePath()) +
                        "/" + aDBName + ".db";
  // build a full path (in default storage location) to a given db file..

  return thePath;
}

// USE: ctor ---------------------------------------
Storage::Storage(const std::string aName, CreateNewStorage) : name(aName) {
  std::string thePath = getDatabasePath(name);
  // try to create a new db file in known storage location.
  try {
    stream.open(thePath, std::fstream::out | std::fstream::binary);
    if (!stream.is_open()) {
      throw std::string("[Error] Failed to CREATE new database " + aName);
    }
    // throw error if it fails...
  } catch (std::string err) {
    std::cout << err << std::endl;
  }
}

// USE: ctor ---------------------------------------
Storage::Storage(const std::string aName, OpenExistingStorage) : name(aName) {
  std::string thePath = getDatabasePath(aName);
  // try to OPEN a db file a given storage location
  try {
    stream.open(thePath,
                std::fstream::in | std::fstream::out | std::fstream::binary);
    if (!stream.is_open()) {
      //        stream.close();
      //        stream.open(thePath,
      //                    std::fstream::in | std::fstream::out |
      //                    std::fstream::binary);
      throw std::string("[Error] Failed to OPEN database " + aName);
    }
    // throw error if it fails...
  } catch (std::string err) {
    std::cout << err << std::endl;
  }
}

// USE: dtor ---------------------------------------
Storage::~Storage() { stream.close(); }

// USE: validate we're open and ready ---------------------------------------
bool Storage::isReady() const { return stream.is_open(); }

// USE: count blocks in file ---------------------------------------
uint32_t Storage::getTotalBlockCount() {
  // how can we compute the total number of blocks in storage?
  uint32_t theCount = 0;
  return theCount;
}

// Call this to locate a free block in this storage file.
// If you can't find a free block, then append a new block and return its
// blocknumber
StatusResult Storage::findFreeBlockNum(Storable &aStorable) {
  // find free block
  MetaData metaStorable;
  StatusResult result = load(metaStorable, 0);
  if (result) {
    for (auto element : metaStorable.map) {
      if (element.second == 'F') {
        element.second = static_cast<char>(aStorable.getType());
        metaStorable.map[element.first] = element.second;
        save(metaStorable, 0);
        return StatusResult(noError, element.first);
      }
    }
    metaStorable.map.insert(std::make_pair(
        metaStorable.map.size(), static_cast<char>(aStorable.getType())));
    save(metaStorable, 0);
    return StatusResult(noError, metaStorable.map.size() - 1);
  } else {
    return StatusResult(readError);
  }
}

StatusResult Storage::findFreeBlockNum(Storable &aStorable, MetaData &metaStorable) {
  // find free block
  // load(metaStorable, 0);
  for (auto element : metaStorable.map) {
    if (element.second == 'F') {
      element.second = static_cast<char>(aStorable.getType());
      metaStorable.map[element.first] = element.second;
      // save(metaStorable, 0);
      return StatusResult(noError, element.first);
    }
  }
  metaStorable.map.insert(std::make_pair(
      metaStorable.map.size(), static_cast<char>(aStorable.getType())));
  // save(metaStorable, 0);
  return StatusResult(noError, metaStorable.map.size() - 1);
}

// USE: for use with "storable API" [NOT REQUIRED -- but very useful]

StatusResult Storage::save(Storable &aStorable, int aStartBlockNum) {
  // High-level IO: save a storable object (like a table row)...
  uint32_t theStartNum = aStartBlockNum;

  // find the start block num
  if (aStorable.getType() == BlockType::meta_block) {
    theStartNum = 0;
  } else if (aStartBlockNum == -1) {
    StatusResult theResult = findFreeBlockNum(aStorable);
    theStartNum = theResult.value;
  }

  // make a string(or char*) to store encode the Storable Instance
  stringstream theStream;
  StatusResult theResult = aStorable.encode(theStream);
  // const char *theBuf = theStream.str().data();
  std::string buf = theStream.str();

  if (theResult) {
    uint32_t theLength = theStream.tellp();
    uint32_t theCount = ceil((theLength / kPayloadSize) + 0.5);
    uint32_t thePart = 0;

    StorageBlock theBlock;
    theBlock.header.type = static_cast<char>(aStorable.getType());

    // write the theBuf to file using writeBlock()
    while (theResult && thePart < theCount) {
      // std::memcpy(reinterpret_cast<void *>(&theBlock.data),
      //             &theBuf[thePart * kPayloadSize], kPayloadSize);
      std::string substr;
      if (kPayloadSize*(thePart+1)<buf.length()) {
        substr = buf.substr(thePart * kPayloadSize, kPayloadSize);
      } else {
        substr = buf.substr(thePart * kPayloadSize);
      }
      for (int i=0; i<substr.length(); i++) {
        theBlock.data[i] = substr[i];
      }
      theBlock.header.count = theCount;
      theBlock.header.next = 0; // next=0 means end
      theBlock.header.pos = thePart++;
      if (thePart < theCount) {
        theResult = findFreeBlockNum(aStorable);
        theBlock.header.next = theResult.value;
      }
      theResult = writeBlock(theBlock, theStartNum);
      theStartNum = theBlock.header.next;
    }
  }
  theResult.value = theStartNum;
  return theResult;
}

// USE: for use with "storable API" [NOT REQUIRED -- but very useful]

StatusResult Storage::load(Storable &aStorable, int aStartBlockNum) {
  // high-level IO: load a storable object (like a table row)
  uint32_t theStartNum = aStartBlockNum;
  if (aStorable.getType() == BlockType::meta_block) {
    theStartNum = 0;
  }
  stringstream theStream;
  StorageBlock theBlock;
  StatusResult theResult;
  while (theResult) {
    theResult = readBlock(theBlock, theStartNum);
    theStream << theBlock.data;
    if (theBlock.header.next == 0 ||
        theBlock.header.pos >= theBlock.header.count ||
        theBlock.header.type != static_cast<char>(aStorable.getType())) {
      break;
    }
    theStartNum = theBlock.header.next;
  }
  theResult = aStorable.decode(theStream);
  return theResult;
}

// USE: write data a given block (after seek)
StatusResult Storage::writeBlock(StorageBlock &aBlock, uint32_t aBlockNumber) {
  // STUDENT: Implement this; this is your low-level block IO...
  stream.clear();
  stream.seekp(sizeof(aBlock) * (aBlockNumber), std::fstream::beg);
  stream.write(reinterpret_cast<char *>(&aBlock), sizeof(aBlock));
  stream.seekp(std::fstream::beg);
  return StatusResult();
}

// USE: read data from a given block (after seek)
StatusResult Storage::readBlock(StorageBlock &aBlock, uint32_t aBlockNumber) {
  // STUDENT: Implement this; this is your low-level block IO...
  stream.clear();
  stream.seekg(sizeof(aBlock) * (aBlockNumber), std::fstream::beg);
  stream.read(reinterpret_cast<char *>(&aBlock), sizeof(aBlock));
  stream.seekg(std::fstream::beg);
  return StatusResult();
}

// Storable MetaData

StatusResult MetaData::encode(std::ostream &aWriter) {
  aWriter << map.size() << ", ";
  for (auto thePair : map) {
    aWriter << thePair.first << ":" << thePair.second << ", ";
  }
  return StatusResult(noError);
}

StatusResult MetaData::decode(std::istream &aReader) {
  uint32_t theBlockNum = 0;
  char blockType;
  char theChar1, theChar2;
  uint32_t theCount = 0;

  aReader >> theCount >> theChar1; // How many blocks
  for (size_t i = 0; i < theCount; i++) {
    aReader >> theBlockNum >> theChar1 >> blockType >> theChar2;
    map[theBlockNum] = blockType;
  }
  return StatusResult(noError);
}

bool Storage::each(BlockVisitor &aVisitor) {
  MetaData MetaBlock;
  this->load(MetaBlock, 0);
  this->rowCollection.clear();
  for (auto element : MetaBlock.map) {
    Row aRow;
    if (aVisitor(*this, aRow, element.first)) {
      this->rowCollection.push_back(aRow);
    }
  }
  return true;
}

StatusResult BlockVisitor::operator()(Storage &aStorage, Row &aRow,
                                      uint32_t aBlockNum) {
  StorageBlock aBlock;
  aStorage.readBlock(aBlock, aBlockNum);
  if (aBlock.header.pos == 0 && aBlock.header.type == 'D' && aBlock.header.pos == 0) {
    if (filters.getCount() == 0) {
      aStorage.load(aRow, aBlockNum);
      if (aRow.tableName == tableName) {
        return StatusResult();
      } else {
        return StatusResult(readError);
      }
    }
    aStorage.load(aRow, aBlockNum);
    if (filters.matches(aRow.data_map) && aRow.tableName == tableName) {
      return StatusResult();
    }
  }
  return StatusResult(Errors::readError);
}
} // namespace ECE141
