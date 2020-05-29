#ifndef Row_hpp
#define Row_hpp

#include "Value.hpp"
#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ECE141 {

class Row : public Storable {
public:
  Row(std::string aName = "NULL") : tableName(aName) {}
  Row(const Row &aRow);
  ~Row() {}

  Row &operator=(const Row &aRow);
  bool operator==(Row &aCopy);
  Row &setTableName(std::string aName);
  Row &setPrimaryKey(std::string aPrimaryKey);

  // STUDENT: What other methods do you require?
  std::string getName() const { return tableName; }
  std::string getPrimaryKey() const { return primaryKey; }
  std::map<std::string, ValueType> &getDataMap() { return data_map; }

  Row &addColumn(const std::string &aString, ValueType &aValue);

  virtual StatusResult encode(std::ostream &aWriter);
  virtual StatusResult decode(std::istream &aReader);
  virtual BlockType getType() const { return BlockType::data_block; }

public:
  // KeyValues data;  //you're free to change this if you like...
  std::map<std::string, ValueType> data_map;
  std::string tableName;
  std::string primaryKey;
};

struct RowSorters {
  RowSorters(std::string aField) : field(aField) {}
  bool operator()(Row &row1, Row &row2) {
    ValueType &theLHS = row1.data_map[field];
    ValueType &theRHS = row2.data_map[field];
    return theLHS.value < theRHS.value;
  }
  const std::string field;
};

} // namespace ECE141

#endif /* Row_hpp */
