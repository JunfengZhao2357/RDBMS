cmake_minimum_required(VERSION 3.15)
project(RDBMS)

set(CMAKE_CXX_STANDARD 17)

add_executable(RDBMS main.cpp AppProcessor.cpp AppProcessor.hpp CommandProcessor.cpp CommandProcessor.hpp Statement.cpp Statement.hpp Tokenizer.cpp Tokenizer.hpp Storage.cpp Storage.hpp Errors.hpp FolderReader.hpp Helpers.hpp keywords.hpp View.hpp AppStatement.hpp DBStatement.hpp DBProcessor.cpp DBProcessor.hpp Database.cpp Database.hpp StorageBlock.cpp StorageBlock.hpp Attribute.cpp Attribute.hpp SQLProcessor.cpp SQLProcessor.hpp SQLStatement.hpp Schema.cpp Schema.hpp Row.cpp Row.hpp Value.hpp Filters.cpp Filters.hpp Index.cpp Index.hpp Timer.hpp Tester.hpp)