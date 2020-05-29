//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

namespace ECE141 {

// STUDENT: Implement the attribute class here...
Attribute::Attribute(DataType aType)
    : name("NULL"), type(aType), size(10), autoIncrement(0),
      primary(0), nullable(1) {
        std::string init = "NULL";
        defaultValue.value = init;
}

Attribute::Attribute(std::string aName, ValueType aDefault, DataType aType,
                     uint32_t aSize, uint32_t aAI, uint32_t aPrimary,
                     uint32_t aNullable)
    : name(aName), defaultValue(aDefault), type(aType), size(aSize),
      autoIncrement(aAI), primary(aPrimary), nullable(aNullable) {}

Attribute::Attribute(const Attribute &aCopy)
    : name(aCopy.getName()), defaultValue(aCopy.getDefault()),
      type(aCopy.getType()), size(aCopy.getSize()),
      autoIncrement(aCopy.getAI()), primary(aCopy.getPrimary()),
      nullable(aCopy.getNullable()) {}

bool Attribute::isValid() const { return true; }

} // namespace ECE141
