//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Database.hpp"
#include "Row.hpp"
#include "Storage.hpp"
#include "View.hpp"
#include "Value.hpp"
#include <algorithm>
#include <sstream>
#include <vector>
#include <map>

// this class represents the database object.
// This class should do actual database related work,
// we called upon by your db processor or commands

namespace ECE141 {
 //using string_hash = std::hash<std::string>;


Database::Database(const std::string aName, CreateNewStorage)
    : dbName(aName), storage(aName, CreateNewStorage()) {
  rootPath = getenv("DB_PATH");
}

Database::Database(const std::string aName, OpenExistingStorage)
    : dbName(aName), storage(aName, OpenExistingStorage()) {
  rootPath = getenv("DB_PATH");
  this->storage.load(MetaBlock, 0);
}

Database::~Database() {}

StatusResult Database::createDatabase(const std::string &aName) {
  std::hash<std::string> string_hash;
  MetaBlock.map[0] = static_cast<char>(MetaBlock.getType());
  storage.save(MetaBlock, 0);

  Index schemas_Index(std::string("SchemasIndex"),string_hash(std::string("SchemasIndex")),DataType::int_type);
  Index indexes_Index(std::string("IndexesIndex"),string_hash(std::string("IndexesIndex")),DataType::int_type);
  StatusResult aResult = storage.findFreeBlockNum(schemas_Index);
  schemas_Index.setBlockNum(aResult.value);
  storage.save(schemas_Index,aResult.value);
  StatusResult anotherResult = storage.findFreeBlockNum(indexes_Index);
  indexes_Index.setBlockNum(anotherResult.value);
  storage.save(indexes_Index,anotherResult.value);

  return StatusResult(noError);
}

StatusResult Database::describeDatabase(const std::string &aName) {
  StatusResult theResult = storage.load(MetaBlock, 0);
  // output title of Database
  std::cout.setf(std::ios::left);
  std::cout << "+-----------+--------------+-----------------------------+"
            << std::endl;
  std::cout << "| " << std::setw(10) << "Block#"
            << "| " << std::setw(13) << "Type"
            << "| " << std::setw(28) << "Other"
            << "|\n";
  std::cout << "+-----------+--------------+-----------------------------+"
            << std::endl;

  // Iterate MetaBlock to find Schema and Data(Row)
  // Adding other information and compute row number
  int rowNum = 1;
  for (auto iter = MetaBlock.map.begin(); iter != MetaBlock.map.end(); iter++) {
    std::string otherInfo = "";
    if (iter->second == 'S') { // Schema
      // add other infomation about Schema_block
      rowNum++;
      Schema aSchema;
      StorageBlock aBlock;
      storage.readBlock(aBlock, iter->first);
      if (!aBlock.header.pos) {
        StatusResult theResult = storage.load(aSchema, iter->first);
        if (theResult)
          otherInfo = "\"" + aSchema.getName() + "\"";
      }
    } else if (iter->second == 'D') { // Data(Row)
      // add other infomation about Data_block
      rowNum++;
      Row aRow;
      StorageBlock aBlock;
      storage.readBlock(aBlock, iter->first);
      if (!aBlock.header.pos) {
        StatusResult theResult = storage.load(aRow, iter->first);
        if (theResult) {
          std::map<std::string, ValueType> data_map = aRow.getDataMap();
          std::string primaryKey = aRow.getPrimaryKey();
          std::stringstream ss(otherInfo);
          ss << primaryKey << "=" << data_map[primaryKey] << ",  "
             << "\"" + aRow.getName() + "\"";
          otherInfo = ss.str();
        }
      }
    }
    std::cout << "| " << std::setw(10) << iter->first << "| " << std::setw(13)
              << BlockTypeStr[static_cast<BlockType>(iter->second)].c_str()
              << "| " << std::setw(28) << otherInfo << "|\n";
  }
  std::cout << "+-----------+--------------+-----------------------------+"
            << std::endl;
  std::cout << rowNum << " rows in set" << std::endl;
  storage.save(MetaBlock, 0);
  return theResult;
}

StatusResult Database::dropDatabase(const std::string &aName) {
  std::string path = rootPath + "/" + aName + ".db";
  std::filesystem::path filePath(path);
  std::filesystem::remove(path);
  return StatusResult(noError);
}

StatusResult Database::createTable(const Schema &aSchema) {
  std::hash<std::string> string_hash;

  Schema theSchema(aSchema);
  uint32_t aFreeBlockNumber = storage.findFreeBlockNum(theSchema).value;
  storage.save(theSchema, aFreeBlockNumber);

  // update index of schemas
  Index schemasIndex;
  storage.load(MetaBlock,0);
  for(auto element:MetaBlock.map){
      Index schemasIndex_copy;
      if(element.second=='I'){
          //startblock = element.first;
          StorageBlock aBlock;
          storage.readBlock(aBlock,element.first);
          if(aBlock.header.pos==0){
              storage.load(schemasIndex_copy,element.first);
              if(schemasIndex_copy.getFieldName()==std::string("SchemasIndex")){
                  MetaBlock.map[element.first] = 'F';
                  while (aBlock.header.next) {
                      uint32_t number = aBlock.header.next;
                      storage.readBlock(aBlock, number);
                      MetaBlock.map[number] = 'F';
                  }
                  schemasIndex = schemasIndex_copy;
                  break;

              }
          }
      }
  }
  storage.save(MetaBlock,0);
  ValueType aKey;
  aKey.value = theSchema.getName();
  if(!schemasIndex.contains(aKey))
      schemasIndex.addKeyValue(aKey,aFreeBlockNumber);
  else{
      schemasIndex.removeKeyValue(aKey);
      schemasIndex.addKeyValue(aKey,aFreeBlockNumber);
  }
  uint32_t anotherFreeBlockNumber = storage.findFreeBlockNum(schemasIndex).value;
  schemasIndex.setBlockNum(anotherFreeBlockNumber);
  storage.save(schemasIndex,anotherFreeBlockNumber);

  // create primary key indexes and store
  Index primarykeyIndexes(theSchema.getPrimaryKeyName(),string_hash(theSchema.getPrimaryKeyName()),DataType::int_type);
  uint32_t freeBlockNumber = storage.findFreeBlockNum(primarykeyIndexes).value;
  primarykeyIndexes.setBlockNum(freeBlockNumber);
  storage.save(primarykeyIndexes,freeBlockNumber);

  // update indexes of index
    Index indexesIndex;
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            //startblock = element.first;
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    MetaBlock.map[element.first] = 'F';
                    while (aBlock.header.next) {
                        uint32_t number = aBlock.header.next;
                        storage.readBlock(aBlock, number);
                        MetaBlock.map[number] = 'F';
                    }
                    indexesIndex = indexesIndex_copy;
                    break;
                }
            }
        }
    }
    storage.save(MetaBlock,0);
    ValueType anotherKey;
    anotherKey.value = theSchema.getName();
    if(!indexesIndex.contains(anotherKey))
        indexesIndex.addKeyValue(anotherKey,freeBlockNumber);
    else{
        indexesIndex.removeKeyValue(anotherKey);
        indexesIndex.addKeyValue(anotherKey,freeBlockNumber);
    }
    uint32_t freeBlockNumber_1 = storage.findFreeBlockNum(indexesIndex).value;
    indexesIndex.setBlockNum(freeBlockNumber_1);
    storage.save(indexesIndex,freeBlockNumber_1);
  return StatusResult(noError);
}

StatusResult Database::dropTable(const std::string &aName) {
    bool find = false;
  storage.load(MetaBlock, 0);
  // Iterate all Blocks and take out tableName
  Schema aSchema;
  for (auto element : MetaBlock.map) {
    if (element.second == 'S') { // Find Schema according to aName
      storage.load(aSchema, element.first);
      if (aSchema.getName() == aName) {
        StorageBlock aBlock;
        storage.readBlock(aBlock, element.first);
        MetaBlock.map[element.first] = 'F';
        while (aBlock.header.next > 0) {
          uint32_t number = aBlock.header.next;
          storage.readBlock(aBlock, number);
          MetaBlock.map[number] = 'F';
        }
        storage.save(MetaBlock, 0);
        find = true;
      }
    } else if (element.second == 'D') { // Find DataBlock according to aName
      Row aRow;
      storage.load(aRow, element.first);
      if (aRow.getName() == aName) {
        StorageBlock aBlock;
        storage.readBlock(aBlock, element.first);
        MetaBlock.map[element.first] = 'F';
        while (aBlock.header.next > 0) {
          uint32_t number = aBlock.header.next;
          storage.readBlock(aBlock, number);
          MetaBlock.map[number] = 'F';
        }
        storage.save(MetaBlock, 0);
        find = true;
      }
    }
  }

  // find primaryKey Index Block
    ValueType target;
    target.value = aName;
    uint32_t Keynum;
    Index indexesIndex;
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            //startblock = element.first;
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    Keynum = indexesIndex_copy.getValue(target);
                    break;
                }
            }
        }
    }
    // delete primarykey index
    StorageBlock primaryBlock;
    storage.readBlock(primaryBlock,Keynum);
    MetaBlock.map[Keynum]='F';
    while (primaryBlock.header.next ) {
        uint32_t number = primaryBlock.header.next;
        storage.readBlock(primaryBlock, number);
        MetaBlock.map[number] = 'F';
    }
    storage.save(MetaBlock,0);


    // update indexs of Index
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    MetaBlock.map[element.first] = 'F';
                    while (aBlock.header.next) {
                        uint32_t number = aBlock.header.next;
                        storage.readBlock(aBlock, number);
                        MetaBlock.map[number] = 'F';
                    }
                    indexesIndex = indexesIndex_copy;
                    break;
                }
            }
        }
    }
    storage.save(MetaBlock,0);
    ValueType anotherKey;
    anotherKey.value = aName;
    if(indexesIndex.contains(anotherKey)){
        indexesIndex.removeKeyValue(anotherKey);
    }
    uint32_t freeBlockNumber_1 = storage.findFreeBlockNum(indexesIndex).value;
    indexesIndex.setBlockNum(freeBlockNumber_1);
    storage.save(indexesIndex,freeBlockNumber_1);

    // update schemas Index
    Index schemasIndex;
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index schemasIndex_copy;
        if(element.second=='I'){
            //startblock = element.first;
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(schemasIndex_copy,element.first);
                if(schemasIndex_copy.getFieldName()==std::string("SchemasIndex")){
                    MetaBlock.map[element.first] = 'F';
                    while (aBlock.header.next) {
                        uint32_t number = aBlock.header.next;
                        storage.readBlock(aBlock, number);
                        MetaBlock.map[number] = 'F';
                    }
                    schemasIndex = schemasIndex_copy;
                    break;
                }
            }
        }
    }
    storage.save(MetaBlock,0);
    ValueType aKey;
    aKey.value = aName;
    if(schemasIndex.contains(aKey)){
        schemasIndex.removeKeyValue(aKey);
    }
    uint32_t anotherFreeBlockNumber = storage.findFreeBlockNum(schemasIndex).value;
    schemasIndex.setBlockNum(anotherFreeBlockNumber);
    storage.save(schemasIndex,anotherFreeBlockNumber);

  if (!find)
    return StatusResult(unknownTable);
  else
    return StatusResult(noError);
}

StatusResult Database::describeTable(const std::string &aName) {
  storage.load(MetaBlock, 0);
  Schema aSchema;
  // Check if there is a Table named aName
  for (auto element : MetaBlock.map) {
    if (element.second == 'S') {
      StorageBlock aBlock;
      storage.readBlock(aBlock, element.first);
      if (!aBlock.header.pos) {
        storage.load(aSchema, element.first);
        if (aSchema.getName() == aName) {
          break;
        }
      }
    }
  }
  // output title of the table
  std::cout << "+-----------+--------------+------+-----+---------+------------"
               "-----------------+"
            << std::endl;
  std::cout << "| Field     | Type         | Null | Key | Default | Extra      "
               "                 |"
            << std::endl;
  std::cout << "+-----------+--------------+------+-----+---------+------------"
               "-----------------+"
            << std::endl;
  // output Attributes and their properties
  for (auto attr : aSchema.getAttributes()) {
    //----------- write field name like id,title-----------------
    std::cout << "| " << std::left << std::setw(10) << attr.getName();

    //------------write feild type like interger varchar
    if (attr.getType() == DataType::varchar_type) {
      std::string type = "varchar(";
      type += std::to_string(attr.getSize());
      type += ")";
      std::cout << "| " << std::left << std::setw(13) << type;
    } else if (attr.getType() == DataType::int_type) {
      std::cout << "| " << std::left << std::setw(13) << "integer";
    } else if (attr.getType() == DataType::float_type) {
      std::cout << "| " << std::left << std::setw(13) << "float";
    } else if (attr.getType() == DataType::datetime_type) {
      std::cout << "| " << std::left << std::setw(13) << "date";
    } else if (attr.getType() == DataType::bool_type) {
      std::cout << "| " << std::left << std::setw(13) << "boolean";
    } else {
      std::cout << "| " << std::left << std::setw(13) << "no type";
    }

    //-----------------write isNUllable()-------------------------
    if (attr.getNullable()) {
      std::cout << "| " << std::left << std::setw(5) << "YES";
    } else {
      std::cout << "| " << std::left << std::setw(5) << "NO";
    }

    //-------------------write isKey()---------------------------
    if (attr.getPrimary()) {
      std::cout << "| " << std::left << std::setw(4) << "YES";
    } else {
      std::cout << "| " << std::left << std::setw(4) << "";
    }

    //-------------------write Default---------------------------
    std::cout << "| " << std::left << std::setw(8) << attr.getDefault();

    //--------------------write Extra-------------------------
    if (attr.getAI() && attr.getPrimary()) {
      std::cout << "| " << std::left << std::setw(28)
                << "auto_increment primary key"
                << "|" << std::endl;
    } else if (attr.getAI() && !attr.getPrimary()) {
      std::cout << "| " << std::left << std::setw(28) << "auto_increment"
                << "|" << std::endl;
    } else if (!attr.getAI() && attr.getPrimary()) {
      std::cout << "| " << std::left << std::setw(28) << "primary key"
                << "|" << std::endl;
    } else {
      std::cout << "| " << std::left << std::setw(28) << ""
                << "|" << std::endl;
    }
  }
  std::cout << "+-----------+--------------+------+-----+---------+------------"
               "-----------------+"
            << std::endl;
  std::cout << aSchema.getAttributes().size() << " rows in set" << std::endl;
  storage.save(MetaBlock, 0);
  return StatusResult(noError);
}

StatusResult Database::showTables() {
  storage.load(MetaBlock, 0);
  std::vector<std::string> table_vec;
  // Iterate MetaBlock to find Schema
  for (auto element : MetaBlock.map) {
    if (element.second == 'S') {
      StorageBlock aBlock;
      storage.readBlock(aBlock, element.first);
      if (!aBlock.header.pos) {
        Schema aSchema;
        storage.load(aSchema, element.first);
        table_vec.push_back(aSchema.getName());
      }
    }
  }
  std::cout << "+----------------------+" << std::endl;
  std::string tableName = "Tables_in_" + dbName;
  std::cout << "| " << std::left << std::setw(21) << tableName << "|"
            << std::endl;
  std::cout << "+----------------------+" << std::endl;
  for (auto name : table_vec) {
    std::cout << "| " << std::left << std::setw(21) << name << "|" << std::endl;
  }
  std::cout << "+----------------------+" << std::endl;
  std::cout << table_vec.size() << " rows in set" << std::endl;
  storage.save(MetaBlock, 0);
  return StatusResult(noError);
}

StatusResult
Database::insertRow(Schema *schemaPtr,
                    std::vector<std::map<std::string, ValueType>> vec) {
   //extract data from vec, and put them into Rowcollection
  std::vector<Row> rowCollection;
  std::string tableName = schemaPtr->getName();
  AttributeList attributes = schemaPtr->getAttributes();
  for (auto aMap : vec) {
    Row aRow(tableName);
    std::string pkname = schemaPtr->getPrimaryKeyName();
    if (aMap.count(pkname) == 0 && schemaPtr->getAttribute(pkname)->getAI()) {
      int aiValue = schemaPtr->getNextAutoIncrementValue();
      ValueType vt;
      vt.value = aiValue;
      aMap[pkname] = vt;
    }
    aRow.setPrimaryKey(schemaPtr->getPrimaryKeyName());
    for (auto element : aMap) {
      aRow.addColumn(element.first, element.second);
    }

    for (auto attr : attributes) {
      if (attr.isDefault()) {
        if (!aRow.data_map.count(attr.getName()))
          aRow.addColumn(attr.getName(),
                         const_cast<ValueType &>(attr.getDefault()));
      }
    }
    rowCollection.push_back(aRow);
  }

  // call function findFreeBlockNum() to find freeblock and save each row in
  // database
  storage.load(MetaBlock,0);
  std::map<uint32_t,ValueType> keyBlockMap;
  for (auto aRow : rowCollection) {
    uint32_t aFreeBlockNumber = storage.findFreeBlockNum(aRow, MetaBlock).value;
    keyBlockMap[aFreeBlockNumber]=aRow.data_map[schemaPtr->getPrimaryKeyName()];
    storage.save(aRow, aFreeBlockNumber);
  }
  storage.save(MetaBlock, 0);
  
 //find primaryKeyIndex block
  ValueType target;
  target.value = schemaPtr->getName();
  uint32_t Keynum;
  Index indexesIndex;
  storage.load(MetaBlock,0);
  for(auto element:MetaBlock.map){
      Index indexesIndex_copy;
      if(element.second=='I'){
          StorageBlock aBlock;
          storage.readBlock(aBlock,element.first);
          if(aBlock.header.pos==0){
              storage.load(indexesIndex_copy,element.first);
              if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    Keynum = indexesIndex_copy.getValue(target);
                    break;
              }
          }
      }
  }
  //update primaryKey index;
  Index primaryKeyIndex;
  storage.load(primaryKeyIndex,Keynum);
  MetaBlock.map[Keynum]='F';
  StorageBlock aBlock_1;
  storage.readBlock(aBlock_1,Keynum);
  while(aBlock_1.header.next){
      uint32_t number = aBlock_1.header.next;
      storage.readBlock(aBlock_1, number);
      MetaBlock.map[number] = 'F';
  }
  storage.save(MetaBlock,0);
  for(auto element:keyBlockMap){
      if(!primaryKeyIndex.contains(element.second))
        primaryKeyIndex.addKeyValue(element.second,element.first);
      else{
           primaryKeyIndex.removeKeyValue(element.second);
           primaryKeyIndex.addKeyValue(element.second,element.first);
      }
  }
  uint32_t freeBlockNumber_1 = storage.findFreeBlockNum(primaryKeyIndex).value;
  indexesIndex.setBlockNum(freeBlockNumber_1);
  storage.save(primaryKeyIndex,freeBlockNumber_1);
  // update indexesIndex Block
  storage.load(MetaBlock,0);
  for(auto element:MetaBlock.map){
      Index indexesIndex_copy;
      if(element.second=='I'){
          StorageBlock aBlock;
          storage.readBlock(aBlock,element.first);
          if(aBlock.header.pos==0){
              storage.load(indexesIndex_copy,element.first);
              if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                  MetaBlock.map[element.first] = 'F';
                  while (aBlock.header.next) {
                      uint32_t number = aBlock.header.next;
                      storage.readBlock(aBlock, number);
                      MetaBlock.map[number] = 'F';
                  }
                  indexesIndex = indexesIndex_copy;
                  break;
              }
          }
      }
  }
  storage.save(MetaBlock,0);
  ValueType anotherKey;
  anotherKey.value = schemaPtr->getName();
  if(!indexesIndex.contains(anotherKey))
      indexesIndex.addKeyValue(anotherKey,freeBlockNumber_1);
  else{
      indexesIndex.removeKeyValue(anotherKey);
      indexesIndex.addKeyValue(anotherKey,freeBlockNumber_1);
  }
  uint32_t freeBlockNumber_2 = storage.findFreeBlockNum(indexesIndex).value;
  indexesIndex.setBlockNum(freeBlockNumber_2);
  storage.save(indexesIndex,freeBlockNumber_2);
  return StatusResult(noError);
}

StatusResult Database::deleteFrom(std::string aName) {
  // find table datablock
  bool find = false;
  int affectedNumber = 0;
  ValueType target;
  storage.load(MetaBlock, 0);
  for (auto element : MetaBlock.map) {
    if (element.second == 'D') {
      Row aRow;
      storage.load(aRow, element.first);
      if (aRow.getName() == aName) {
        affectedNumber++;
        StorageBlock aBlock;
        storage.readBlock(aBlock, element.first);
        MetaBlock.map[element.first] = 'F';
        while (aBlock.header.next) {
          uint32_t number = aBlock.header.next;
          storage.readBlock(aBlock, number);
          MetaBlock.map[number] = 'F';
        }
        storage.save(MetaBlock, 0);
        find = true;
      }
    }
  }

  // find primaryKey Index
    ValueType target_1;
    target_1.value = aName;
    uint32_t Keynum;
    Index indexesIndex;
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    Keynum = indexesIndex_copy.getValue(target_1);
                    break;
                }
            }
        }
    }

  //update (clear) primaryKey Index
    Index primaryKeyIndex;
    storage.load(primaryKeyIndex,Keynum);
    MetaBlock.map[Keynum]='F';
    StorageBlock aBlock_1;
    storage.readBlock(aBlock_1,Keynum);
    while(aBlock_1.header.next){
        uint32_t number = aBlock_1.header.next;
        storage.readBlock(aBlock_1, number);
        MetaBlock.map[number] = 'F';
    }
    storage.save(MetaBlock,0);
    primaryKeyIndex.clearList();
    uint32_t freeBlockNumber_1 = storage.findFreeBlockNum(primaryKeyIndex).value;
    indexesIndex.setBlockNum(freeBlockNumber_1);
    storage.save(primaryKeyIndex,freeBlockNumber_1);

    //update indexes of Index
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    MetaBlock.map[element.first] = 'F';
                    while (aBlock.header.next) {
                        uint32_t number = aBlock.header.next;
                        storage.readBlock(aBlock, number);
                        MetaBlock.map[number] = 'F';
                    }
                    indexesIndex = indexesIndex_copy;
                    break;
                }
            }
        }
    }
    storage.save(MetaBlock,0);
    ValueType anotherKey;
    anotherKey.value = aName;
    if(!indexesIndex.contains(anotherKey))
        indexesIndex.addKeyValue(anotherKey,freeBlockNumber_1);
    else{
        indexesIndex.removeKeyValue(anotherKey);
        indexesIndex.addKeyValue(anotherKey,freeBlockNumber_1);
    }
    uint32_t freeBlockNumber_2 = storage.findFreeBlockNum(indexesIndex).value;
    indexesIndex.setBlockNum(freeBlockNumber_2);
    storage.save(indexesIndex,freeBlockNumber_2);

    if (!find) {
    return StatusResult(unknownTable);
  } else {
    std::cout << affectedNumber << " rows affected ";
    return StatusResult(noError);
  }
}

StatusResult Database::selectFrom(Schema *schemaPtr,
                                  std::vector<std::string> fieldList,
                                  int limitNum, std::string orderBy,
                                  Expressions expressionList) {
  
  std::string tableName = schemaPtr->getName();
  storage.load(MetaBlock, 0);
  Filters aFilter;
  AttributeList attributes = schemaPtr->getAttributes();
  //deal with "select *"
  if (fieldList[0] == "*") {
    for (int i = 0; i < attributes.size(); i++) {
      if (i < fieldList.size())
        fieldList[i] = attributes[i].getName();
      else
        fieldList.push_back(attributes[i].getName());
    }
  }
  // build filter
  // bool match = false;
  // if (expressionList.size()) {
  bool isPrimaryKey_filter = true;
  for (auto expression : expressionList) {
      if(expression->lhs.name!=schemaPtr->getPrimaryKeyName()){
          isPrimaryKey_filter = false;
      }
      aFilter.add(expression);
  }
  // } else {
  //   match = true;
  // }
  //-------------where------------------------
  if(isPrimaryKey_filter){
      // find primary Key Index
      ValueType target_1;
      target_1.value = schemaPtr->getName();
      uint32_t Keynum;
      Index indexesIndex;
      storage.load(MetaBlock,0);
      for(auto element:MetaBlock.map){
          Index indexesIndex_copy;
          if(element.second=='I'){
              StorageBlock aBlock;
              storage.readBlock(aBlock,element.first);
              if(aBlock.header.pos==0){
                  storage.load(indexesIndex_copy,element.first);
                  if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                      Keynum = indexesIndex_copy.getValue(target_1);
                      break;
                  }
              }
          }
      }
      Index primaryKeyIndex;
      storage.load(primaryKeyIndex,Keynum);
      std::vector<uint32_t> rowsNum;
      for(auto element:primaryKeyIndex.list){
          std::map<std::string, ValueType> aList;
          aList[schemaPtr->getPrimaryKeyName()]=element.first;
          if(aFilter.matches(aList)){
              rowsNum.push_back(element.second);
          }
      }
      // push back rows in rowCollection
      storage.rowCollection.clear();
      for(auto element : rowsNum){
          Row aRow;
          storage.load(aRow,element);
          storage.rowCollection.push_back(aRow);
      }
  }
  else{
      storage.rowCollection.clear();
      BlockVisitor aVisitor(tableName, aFilter);
      storage.each(aVisitor);
  }



  //-----------------order by-------------------------------

  if (orderBy != "NULL") {
    RowSorters theSorter(orderBy);
    std::sort(storage.rowCollection.begin(), storage.rowCollection.end(), theSorter);
  }

  //-------------------Limit-------------------------------
  if (limitNum != -1 && limitNum < storage.rowCollection.size()) {
    storage.rowCollection.erase(storage.rowCollection.begin() + limitNum, storage.rowCollection.end());
  }

  int length = fieldList.size();
  std::string splitLine;
  std::string unit = "+-----------------------------";
  for (int i = 0; i < length; i++)
    splitLine += unit;
  splitLine += "+";
  std::stringstream header;
  for (int j = 0; j < length; j++) {
    header << "| " << std::left << std::setw(28) << fieldList[j];
  }
  header << "|";
  std::cout << splitLine << std::endl;
  std::cout << header.str() << std::endl;
  std::cout << splitLine << std::endl;
  if (storage.rowCollection.size()) {
    for (auto aRow : storage.rowCollection) {
      std::stringstream content;
      content.clear();
      for (auto key : fieldList) {
        if (aRow.data_map[key].getString() == std::nullopt ||
            aRow.data_map[key].getString() != "NULL")
          content << "| " << std::left << std::setw(28) << aRow.data_map[key];
        else
          content << "|                             ";
      }
      content << "|";
      std::cout << content.str() << std::endl;
    }
  }
  std::cout << splitLine << std::endl;
  std::cout << storage.rowCollection.size() << " rows in set" << " ";
  return StatusResult();
}

StatusResult Database::updateRecords(Schema *schemaPtr, Expression *setExpr,
                                     Expressions whereExprs, Operators op) {
      std::vector<Row> rowCollection;
    std::string tableName = schemaPtr->getName();
    storage.load(MetaBlock, 0);
    if(op==Operators::unknown_op){
        Filters aFilter;
        for (auto expression : whereExprs)
            aFilter.add(expression);
        for (auto element : MetaBlock.map) {
            if (element.second == 'D') {
                StorageBlock aBlock;
                storage.readBlock(aBlock, element.first);
                if (aBlock.header.pos == 0) {
                    Row aRow;
                    storage.load(aRow, element.first);
                    if (aRow.getName() == tableName) {
                        if (aFilter.matches(aRow.data_map)){
                            rowCollection.push_back(aRow);
                            MetaBlock.map[element.first] = 'F';
                            while (aBlock.header.next) {
                                uint32_t number = aBlock.header.next;
                                storage.readBlock(aBlock, number);
                                MetaBlock.map[number] = 'F';
                            }
                        }
                    }
                }
            }
        }
    }
    else if(op==Operators::and_op){
        Filters and_Filter;
        for (auto expression : whereExprs)
            and_Filter.add(expression);
        for (auto element : MetaBlock.map) {
            if (element.second == 'D') {
                StorageBlock aBlock;
                storage.readBlock(aBlock, element.first);
                if (aBlock.header.pos == 0) {
                    Row aRow;
                    storage.load(aRow, element.first);
                    if (aRow.getName() == tableName) {
                        if (and_Filter.matches(aRow.data_map)){
                            rowCollection.push_back(aRow);
                            MetaBlock.map[element.first] = 'F';
                            while (aBlock.header.next) {
                                uint32_t number = aBlock.header.next;
                                storage.readBlock(aBlock, number);
                                MetaBlock.map[number] = 'F';
                            }
                        }
                    }
                }
            }
        }
    }
    else  {    // op==Operators::or_op
        Filters or_Filter_1,or_Filter_2;
        or_Filter_1.add(whereExprs[0]);
        or_Filter_2.add(whereExprs[1]);
        for (auto element : MetaBlock.map) {
            if (element.second == 'D') {
                StorageBlock aBlock;
                storage.readBlock(aBlock, element.first);
                if (aBlock.header.pos == 0) {
                    Row aRow;
                    storage.load(aRow, element.first);
                    if (aRow.getName() == tableName) {
                        if (or_Filter_1.matches(aRow.data_map)||or_Filter_2.matches(aRow.data_map)){
                            rowCollection.push_back(aRow);
                            MetaBlock.map[element.first] = 'F';
                            while (aBlock.header.next) {
                                uint32_t number = aBlock.header.next;
                                storage.readBlock(aBlock, number);
                                MetaBlock.map[number] = 'F';
                            }
                        }
                    }
                }
            }
        }
    }
    storage.save(MetaBlock,0);

    // change data in selected rows
    for(int i=0;i<rowCollection.size();++i)
        rowCollection[i].data_map[setExpr->lhs.name]=setExpr->rhs.value;

    // write rows in database file
    std::map<uint32_t,ValueType> newRowMessage;
    for (auto aRow : rowCollection) {
        uint32_t aFreeBlockNumber = storage.findFreeBlockNum(aRow).value;
        storage.save(aRow, aFreeBlockNumber);
        newRowMessage[aFreeBlockNumber]=aRow.data_map[schemaPtr->getPrimaryKeyName()];
    }

    //find primaryKey Indexes
    ValueType target_1;
    target_1.value = schemaPtr->getName();
    uint32_t Keynum;
    Index indexesIndex;
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    Keynum = indexesIndex_copy.getValue(target_1);
                    break;
                }
            }
        }
    }

    //update primary Key Index
    Index primaryKeyIndex;
    storage.load(primaryKeyIndex,Keynum);
    MetaBlock.map[Keynum]='F';
    StorageBlock aBlock_1;
    storage.readBlock(aBlock_1,Keynum);
    while(aBlock_1.header.next){
        uint32_t number = aBlock_1.header.next;
        storage.readBlock(aBlock_1, number);
        MetaBlock.map[number] = 'F';
    }
    storage.save(MetaBlock,0);
    for(auto element:newRowMessage){
        if(!primaryKeyIndex.contains(element.second))
            primaryKeyIndex.addKeyValue(element.second,element.first);
        else{
            primaryKeyIndex.removeKeyValue(element.second);
            primaryKeyIndex.addKeyValue(element.second,element.first);
        }
    }
    uint32_t freeBlockNumber_1 = storage.findFreeBlockNum(primaryKeyIndex).value;
    indexesIndex.setBlockNum(freeBlockNumber_1);
    storage.save(primaryKeyIndex,freeBlockNumber_1);

    //update indexes Index
    storage.load(MetaBlock,0);
    for(auto element:MetaBlock.map){
        Index indexesIndex_copy;
        if(element.second=='I'){
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(indexesIndex_copy,element.first);
                if(indexesIndex_copy.getFieldName()==std::string("IndexesIndex")){
                    MetaBlock.map[element.first] = 'F';
                    while (aBlock.header.next) {
                        uint32_t number = aBlock.header.next;
                        storage.readBlock(aBlock, number);
                        MetaBlock.map[number] = 'F';
                    }
                    indexesIndex = indexesIndex_copy;
                    break;
                }
            }
        }
    }
    storage.save(MetaBlock,0);
    ValueType anotherKey;
    anotherKey.value = schemaPtr->getName();
    if(!indexesIndex.contains(anotherKey))
        indexesIndex.addKeyValue(anotherKey,freeBlockNumber_1);
    else{
        indexesIndex.removeKeyValue(anotherKey);
        indexesIndex.addKeyValue(anotherKey,freeBlockNumber_1);
    }
    uint32_t freeBlockNumber_2 = storage.findFreeBlockNum(indexesIndex).value;
    indexesIndex.setBlockNum(freeBlockNumber_2);
    storage.save(indexesIndex,freeBlockNumber_2);


    std::cout << rowCollection.size() << " rows affected ";
    return StatusResult(noError);
}

StatusResult Database::showIndexes() {

    std::cout<<"+-----------------+-----------------+"<<std::endl;
    std::cout<<"| table           | field           |"<<std::endl;
    std::cout<<"+-----------------+-----------------+"<<std::endl;
    // schema Index
//    std::cout<<"|                 ";
//    std::cout<<"| "<<std::left<<std::setw(16)<<"SchemaIndex";
//    std::cout<<"|"<<std::endl;
//    std::cout<<"+-----------------+-----------------+"<<std::endl;
//    // indexes Index
//    std::cout<<"|                 ";
//    std::cout<<"| "<<std::left<<std::setw(16)<<"IndexesIndex";
//    std::cout<<"|"<<std::endl;
//    std::cout<<"+-----------------+-----------------+"<<std::endl;

    storage.load(MetaBlock,0);
    // find schema Index
    Index schemasIndex;
    for(auto element:MetaBlock.map){
        Index schemasIndex_copy;
        if(element.second=='I'){
            StorageBlock aBlock;
            storage.readBlock(aBlock,element.first);
            if(aBlock.header.pos==0){
                storage.load(schemasIndex_copy,element.first);
                if(schemasIndex_copy.getFieldName()==std::string("SchemasIndex")){
                    schemasIndex = schemasIndex_copy;
                    break;
                }
            }
        }
    }
    std::map<std::string,ValueType> indexMap;

    for(auto element : schemasIndex.list){
        Schema aSchema;
        storage.load(aSchema,element.second);
        indexMap[aSchema.getPrimaryKeyName()]=element.first;
    }
    for(auto element : indexMap){
        std::cout<<"| "<<std::left<<std::setw(16)<<element.second;
        std::cout<<"| "<<std::left<<std::setw(16)<<element.first;
        std::cout<<"|"<<std::endl;
        std::cout<<"+-----------------+-----------------+"<<std::endl;
    }
    uint32_t rowNumber=indexMap.size();
    std::cout<<rowNumber<<" rows in set"<<std::endl;


  return StatusResult();
}

} // namespace ECE141

