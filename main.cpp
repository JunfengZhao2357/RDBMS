//
//  main.cpp
//  Database2
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "AppProcessor.hpp"
#include "DBProcessor.hpp"
#include "Errors.hpp"
#include "SQLProcessor.hpp"
#include "Storage.hpp"
#include "Tester.hpp"
#include "Tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

// USE: ---------------------------------------------

static std::map<int, std::string> theErrorMessages = {
    {ECE141::illegalIdentifier, "Illegal identifier"},
    {ECE141::unknownIdentifier, "Unknown identifier"},
    {ECE141::databaseExists, "Database exists"},
    {ECE141::tableExists, "Table Exists"},
    {ECE141::syntaxError, "Syntax Error"},
    {ECE141::noDatabaseSpecified, "No Database Specified"},
    {ECE141::unknownCommand, "Unknown command"},
    {ECE141::unknownDatabase, "Unknown database"},
    {ECE141::unknownTable, "Unknown table"},
    {ECE141::unknownError, "Unknown error"},
    {ECE141::userTerminated, "User Terminated"}};

void showError(ECE141::StatusResult &aResult) {
  std::string theMessage = "Unknown Error";
  if (theErrorMessages.count(aResult.code)) {
    theMessage = theErrorMessages[aResult.code];
  }
  if (aResult.code == ECE141::Errors::userTerminated) {
    std::cout << theMessage << "\n";
  } else {
    std::cout << "Error (" << aResult.code << ") " << theMessage << "\n";
  }
}

// build a tokenizer, tokenize input, ask processors to handle...
ECE141::StatusResult handleInput(std::istream &aStream,
                                 ECE141::CommandProcessor &aProcessor) {
  ECE141::Tokenizer theTokenizer(aStream);
  // tokenize the input from aStream...
  ECE141::StatusResult theResult = theTokenizer.tokenize();
  while (theResult && theTokenizer.more()) {
    if (";" == theTokenizer.current().data) {
      theTokenizer.next(); // skip the ";"...
    } else
      theResult = aProcessor.processInput(theTokenizer);
  }
  return theResult;
}

//----------------------------------------------

int main(int argc, const char *argv[]) {
  // Tester test;
  // test.indexTest();
  setenv("DB_PATH", "./tmp", 0);
  // std::cout << "Current DB_PATH = " << getenv("DB_PATH") << std::endl;
  ECE141::DBCmdProcessor theDBProcessor;
  ECE141::SQLCmdProcessor theSQLProcessor(&theDBProcessor);
  ECE141::AppCmdProcessor theAppProcessor(&theSQLProcessor);

  ECE141::StatusResult theResult;
  argc = 2; // for debug
  argv[1] = "command.txt"; // for debug
  if (argc > 1) { // read from txt file
    std::ifstream infile(argv[1]);
    bool running = true;
    std::string theUserInput;
    do {
      if (std::getline(infile, theUserInput, ';')) {
        std::stringstream theStream(theUserInput);
        std::cout << "\n[Command] " << theStream.str() << std::endl;
        theResult = handleInput(theStream, theAppProcessor);
        if (!theResult) {
          showError(theResult);
        }
        if (ECE141::userTerminated == theResult.code) {
          running = false;
        }
      } else {
        running = false;
      }
    } while (running);
  } else { // read from command line
    std::string theUserInput;
    bool running = true;
    do {
      std::cout << "\n> ";
      if (std::getline(std::cin, theUserInput)) {
        if (theUserInput.length()) {
          std::stringstream theStream(theUserInput);
          std::cout << "[Command] " << theStream.str() << std::endl;
          theResult = handleInput(theStream, theAppProcessor);
          if (!theResult) {
            showError(theResult);
          }
        }
        if (ECE141::userTerminated == theResult.code) {
          running = false;
        }
      }
    } while (running);
  }

  return 0;
}
