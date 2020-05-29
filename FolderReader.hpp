//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <filesystem>
#include <string>
using namespace std;
namespace ECE141 {

class FolderListener {
public:
  virtual bool operator()(const std::string &aName) = 0;
};

class FolderReader {
public:
  FolderReader(const char *aPath) : path(aPath) {}
  virtual ~FolderReader() {}

  virtual bool exists(const std::string &aPath) {
    // STUDENT: add logic to see if FOLDER at given path exists.
    std::filesystem::path fp(aPath);
    return filesystem::exists(fp);
  }

  virtual void each(FolderListener &aListener,
                    const std::string &anExtension) const {
    // STUDENT: iterate db's, pass the name of each to listener
    // choose files end with anExtension
    // ".*" means all kind of extension
    filesystem::path fp(path);
    if (!filesystem::exists(fp)) {
      return;
    }
    filesystem::directory_entry entry(fp);
    if (entry.status().type() != filesystem::file_type::directory) {
      return;
    }
    filesystem::directory_iterator list(fp);
    for (auto &it : list) {
      if (anExtension.compare(".*") == 0 ||
          it.path().filename().extension() == anExtension) {
        aListener(it.path().filename().stem());
      }
    }
  }

  std::string path;
};

} // namespace ECE141

#endif /* FolderReader_h */
