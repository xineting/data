/*************************************************
 * HISTORY:                                      *
 *	01/30/2021 pyx : Created.                    *
  *	06/01/2022 pyx : Edited.                     *
=================================================*/

#include "DAContext.h"


/*
 * 获取文件名的操作可以写成一个函数。
 */
extern DAFileManager *m_fm;

char *DAContext::GetFileName(clang::FileID fid) {
  const clang::FileEntry *fe = m_sourceManager.getFileEntryForID(fid);
  if (fe != nullptr) {
    llvm::StringRef fname = fe->getName();
    char *filename =
        realpath(fname.data(), NULL); // FIXME: only work in linux os
    return filename;
  }
  return nullptr;
}

SymbolTable *DAContext::getFileSymbolTable(clang::FileID fid) {
  SymbolTable *ret_st = nullptr;
  std::string filepath = "";
  if (fid.isValid()) {
    ret_st = m_fm->getFileSymbolTable(fid);
    if (ret_st != nullptr)
      return ret_st;
    const clang::FileEntry *fe = m_sourceManager.getFileEntryForID(fid);
    if (fe != nullptr) {
      llvm::StringRef fname = fe->getName();
      char *filename =
          realpath(fname.data(), NULL); // FIXME: only work in linux os
      assert(filename[0] == '/');
      if (filename != NULL) {
        filepath += filename;
        free(filename);
      }
    } else {
      filepath += m_sourceManager.getBufferOrNone(fid)
                      ->getBufferIdentifier(); //<built-in> and <scratch-space>
    }
  }

  trim(filepath);
  if (filepath.empty())
    filepath += "<blank>"; // FIXME
  ret_st = m_fm->getFileSymbolTable(filepath, fid);
  if (ret_st != nullptr)
    return ret_st;
  ret_st = m_fm->createFile(fid, filepath);

  if (ret_st != nullptr)
    return ret_st;
  else {
    llvm::errs() << "INTERNAL ERROR! get st failed@" << filepath << "\n";
    exit(1);
  }
}
