#include "Row.hpp"

namespace ECE141 {

// STUDENT: You need to fully implement these methods...

Row::Row(const Row &aRow) {
  tableName = aRow.getName();
  primaryKey = aRow.getPrimaryKey();
  for (auto element : aRow.data_map) {
    data_map[element.first] = element.second;
  }
}

Row &Row::operator=(const Row &aRow) {
  tableName = aRow.tableName;
  primaryKey = aRow.primaryKey;
  for (auto element : aRow.data_map) {
    // data_map.insert(std::make_pair(element.first, element.second));
    data_map[element.first] = element.second;
  }
  return *this;
}

bool Row::operator==(Row &aCopy) {
  for (auto element : aCopy.data_map) {
    if (!(data_map[element.first] == element.second))
      return false;
  }
  return true;
}

Row &Row::setTableName(std::string aName) {
  tableName = aName;
  return *this;
}

Row &Row::setPrimaryKey(std::string aPrimaryKey) {
  primaryKey = aPrimaryKey;
  return *this;
}

Row &Row::addColumn(const std::string &aString, ValueType &aValue) {
  data_map[aString] = aValue;
  return *this;
}

StatusResult Row::encode(std::ostream &aWriter) {
  aWriter << tableName << " "; //"tablename" ' '
  aWriter << primaryKey << " ";
  aWriter << data_map.size() << " "; //"size" ' '
  for (auto thePair : data_map) {
    aWriter << thePair.first << " ";
    thePair.second.encode(aWriter);
  }
  return StatusResult();
}

StatusResult Row::decode(std::istream &aReader) {
  int aSize;
  aReader >> tableName >> primaryKey >> aSize;
  for (int i = 0; i < aSize; i++) {
    std::string aKey;
    ValueType aValue;
    aReader >> aKey;
    aValue.decode(aReader);
    // aReader >> aValue;
    addColumn(aKey, aValue);
  }
  return StatusResult();
}

} // namespace ECE141
