/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#include "DAFileManager.h"
#include "DAContext.h"
#include "Util.h"

DAFileManager::DAFileManager(std::string fpath) {
  mainfilepath = getAbsolutePath(fpath);
  std::cout << "mainfilepath:" + mainfilepath + "\n";
  assert(mainfilepath[0] == '/');
}

DAFileManager::~DAFileManager() {
  for (auto &itr : filePathMap)
    delete itr.second;
}

SymbolTable *DAFileManager::getFileSymbolTable(clang::FileID fid) {
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash>::iterator iter =
      fileIDMap.find(fid);
  if (iter != fileIDMap.end())
    return iter->second;
  else
    return nullptr;
}

SymbolTable *DAFileManager::getFileSymbolTable(std::string filepath, clang::FileID fid) {
  std::unordered_map<std::string, SymbolTable *>::iterator iter =
      filePathMap.find(filepath);
  if (iter != filePathMap.end()) {
    std::unordered_map<clang::FileID, SymbolTable *, FileIDHash>::iterator
        iter_inner = fileIDMap.find(fid);
    if (iter_inner != fileIDMap.end()) {
      std::string fidfilepath = iter_inner->second->getSymbolTableFilePath();
      assert(fidfilepath == filepath);

    } else {
      fileIDMap.emplace(fid, iter->second);
    }

    return iter->second;
  } else
    return nullptr;
}

SymbolTable *DAFileManager::createFile(clang::FileID fid,
                                       std::string filepath) {

  assert(!filepath.empty());
  SymbolTable *st = new SymbolTable(filepath);
  assert(st != nullptr);
  assert(fileIDMap.count(fid) == 0);
  assert(filePathMap.count(filepath) == 0);
  fileIDMap.emplace(fid, st);
  filePathMap.emplace(filepath, st);

  return st;
}

std::string DAFileManager::getFilePath(clang::FileID fid) {
  SymbolTable *st = getFileSymbolTable(fid);

  if (!st)
    return "";

  return st->getSymbolTableFilePath();
}

std::unordered_map<std::string, SymbolTable *> DAFileManager::GetFilepathMap(){
  return filePathMap;
}


void DAFileManager::finilize() {
  std::ofstream jsonfile;
  std::string jsonfilepath = mainfilepath;
  jsonfile.open(jsonfilepath, std::ios::out | std::ios::trunc);
  jsonfile << "{" << std::endl;
  bool begin = true;
  for (const auto &iter : filePathMap) {
    if (begin)
      begin = false;
    else
      jsonfile << ",";

    jsonfile << "\"" << iter.first << "\" : " << std::endl;
    jsonfile << "{" << std::endl;
    SymbolTable *st = iter.second;

    jsonfile.flush();
    jsonfile.close();

    st->finilize(jsonfilepath, this);

    jsonfile.open(jsonfilepath, std::ios::out | std::ios::app);
    if (!jsonfile.good()) {
      llvm::outs() << "json file write fail" << jsonfilepath << "\n";
      exit(1);
    }

    jsonfile << "}" << std::endl;
  }

  jsonfile << "}" << std::endl;
  jsonfile.flush();
  jsonfile.close();
  // llvm::errs() << "fm final ok";
}
