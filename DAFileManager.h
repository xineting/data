/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#ifndef DA_FILEMANAGER_H
#define DA_FILEMANAGER_H

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_map>

#include "SymbolTable.h"
#include "clang/Tooling/Tooling.h"

class SymbolTable;

class DAFileManager {
public:
  explicit DAFileManager(std::string fpath);
  ~DAFileManager();
  SymbolTable *
  getFileSymbolTable(clang::FileID fid); // internal,only in DAContext
  SymbolTable *
  getFileSymbolTable(std::string filepath,
                     clang::FileID fid); // internal,only in DAContext
  SymbolTable *createFile(clang::FileID fid,
                          std::string filepath); // internal,only in DAContext
  SymbolTable *createFile(std::string filepath); // internal,only in DAContext

  std::string getFilePath(clang::FileID fid);

  std::string getMainFilePath() { return mainfilepath; }

  void finilize();

private:
  std::string mainfilepath;
  std::string outFilePath;

  struct FileIDHash {
    size_t operator()(clang::FileID fid) const { return fid.getHashValue(); }
  };

  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash>
      fileIDMap; // clang fid
  std::unordered_map<std::string, SymbolTable *>
      filePathMap; // file absolute path
};

#endif // DA_FILEMANAGER_H
