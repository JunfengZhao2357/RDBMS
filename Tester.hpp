//
//  Tester.hpp
//
//  Created by Yuchen Xing on 5/21/2020.
//  Copyright © 2020 Yuchen Xing. All rights reserved.
//

#ifndef Tester_h
#define Tester_h

#include "Index.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

class Tester {
public:
  void indexTest() {
    const std::string &aField = "TestIndex";
    uint32_t aHashId = 12345;
    ECE141::DataType aType = ECE141::DataType::varchar_type;
    ECE141::Index index(aField, aHashId, aType);
    ECE141::ValueType vt1;
    std::string str1 = "testvalue1";
    vt1.value = str1;
    index.addKeyValue(vt1, 0);
    ECE141::ValueType vt2;
    std::string str2 = "testvalue2";
    vt2.value = str2;
    ECE141::ValueType vt3;
    std::string str3 = "deletevalue";
    vt3.value = str3;
    ECE141::ValueType vt4;
    std::string str4 = "deletevalue";
    vt4.value = str4;
    index.addKeyValue(vt2, 1);
    index.addKeyValue(vt3, 2);
    index.setChanged();
    index.setBlockNum(2);

    int valueGet;
    valueGet = index.getValue(vt4);
    index.removeKeyValue(vt4);
    std::stringstream ss;
    index.encode(ss);
    std::cout << ss.str() << std::endl;
    ECE141::Index newIndex("someIndex", 54321, ECE141::DataType::no_type);
    newIndex.decode(ss);
    return;
  }
};
#endif /* Tester_h */