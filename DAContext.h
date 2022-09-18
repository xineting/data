/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
  *	06/01/2022 pyx : Edited.                     *
=================================================*/

#ifndef DA_CONTEXT_H
#define DA_CONTEXT_H

#include "DAFileManager.h"
#include "SymbolTable.h"

#include "clang/Tooling/Tooling.h"

class DAContext {
public:
  DAContext(clang::SourceManager &sourceManager,
            clang::Preprocessor &preprocessor)
      : m_sourceManager(sourceManager),
        m_preprocessor(preprocessor){}

  SymbolTable *getFileSymbolTable(clang::FileID fid);

  clang::SourceManager &sourceManager() { return m_sourceManager; }
  clang::Preprocessor &preprocessor() { return m_preprocessor; }
  char *GetFileName(clang::FileID fid);

private:
  clang::SourceManager &m_sourceManager;
  clang::Preprocessor &m_preprocessor;
};

#endif // DA_CONTEXT_H
