#include "Index.hpp"

using namespace std;
namespace ECE141 {

Index &Index::clearList(){
    list.clear();
    return *this;
}
Index &Index::addKeyValue(const ValueType &aKey, uint32_t aValue) {
  list.insert(make_pair(aKey, aValue));
  return *this;
}

Index &Index::removeKeyValue(const ValueType &aKey) {
   auto iter = list.find(aKey);
   if(iter!=list.end())
       list.erase(iter);
  return *this;
}

bool Index::contains(const ValueType &aKey) { return list.count(aKey) > 0; }

uint32_t Index::getValue(const ValueType &aKey) {

  if (list.count(aKey) > 0) {
    return list[aKey];
  }
  return -1;
}

StatusResult Index::encode(std::ostream &aWriter) {
  aWriter << field << " " << static_cast<char>(type) << " " << schemaId << " "
          << changed << " " << blockNum << " ";
  aWriter << list.size() << " ";
  for (auto element : list) {
    element.first.encode(aWriter);
    aWriter << ": " << element.second << " ";
  }
  return StatusResult();
}

StatusResult Index::decode(std::istream &aReader) {
  std::string aField;
  char aType;
  uint32_t aSchemaId;
  bool aChanged;
  uint32_t aBlockNum;
  size_t aSize;
  aReader >> aField >> aType >> aSchemaId >> aChanged >> aBlockNum >> aSize;
  this->field = aField;
  this->type = static_cast<DataType>(aType);
  this->schemaId = aSchemaId;
  this->blockNum = aBlockNum;
  for (int i=0; i<aSize; i++) {
    ValueType aValueType;
    uint32_t aNumber;
    char aChar;
    aValueType.decode(aReader);
    aReader >> aChar >> aNumber;
    if (list.count(aValueType) > 0) list[aValueType] = aNumber;
    else this->addKeyValue(aValueType, aNumber);
  }
  return StatusResult();
}

bool Index::each(BlockVisitor &aVisitor) {
  return true;
}

bool Index::canIndexBy(const std::string &aField) {
  return true;
}

} // end namespace ECE141
