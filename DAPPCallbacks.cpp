/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#include "DAPPCallbacks.h"

DAPPCallbacks::DAPPCallbacks(DAContext *Context) : Context(Context) {
  assert(this->Context != nullptr);
}

void DAPPCallbacks::InclusionDirective(
    clang::SourceLocation hashLoc, const clang::Token &includeTok,
    llvm::StringRef fileName, bool isAngled,
    clang::CharSourceRange filenameRange, const clang::FileEntry *file,
    llvm::StringRef searchPath, llvm::StringRef relativePath,
    const clang::Module *imported, SrcMgr::CharacteristicKind FileType) {

  if (file == NULL) {
    return;
  }

  clang::SourceManager &sm = Context->sourceManager();

  clang::SourceLocation spellingLoc =
      sm.getSpellingLoc(filenameRange.getBegin());

  if (!spellingLoc.isValid()) {
    llvm::errs() << "include spelling loc invalid\n";
    // exit(1);
    return;
  }

  clang::FileID refFileID = sm.getFileID(spellingLoc);

  std::string location = "";

  unsigned int offset = sm.getFileOffset(spellingLoc);
  location += std::to_string(sm.getLineNumber(refFileID, offset));
  location += ",";
  location += std::to_string(sm.getColumnNumber(refFileID, offset));

  std::string symbol = "@";
  llvm::StringRef fname = file->getName();
  char *path = realpath(fname.data(), NULL); // FIXME: only work in linux os
  if (path != NULL) {
    symbol += path;
    free(path);
  }
  if (symbol.size() == 1)
    symbol += "<blank>"; // FIXME

  recordMacro(symbol, refFileID, ST_Path, RT_Included, location);
}

// inner function,USE EXCEPT INCLUDE!
void DAPPCallbacks::recordMacro(std::string symbolname, clang::FileID FileID,
                                SymbolType st, RefType rt,
                                std::string location) {
  assert(!symbolname.empty());
  assert(!location.empty());

  SymbolTable *f_st = Context->getFileSymbolTable(FileID);

  f_st->addRef(symbolname, st, rt, location);

  return;
}

void DAPPCallbacks::recordMacro(const clang::Token &MacroNameTok,
                                RefType refType) {
  clang::FileID fileID;

  clang::SourceLocation sloc =
      Context->sourceManager().getSpellingLoc(MacroNameTok.getLocation());
  std::string symbolname = "#";

  std::string location = "";
  if (sloc.isValid()) {

    fileID = Context->sourceManager().getFileID(sloc);
    symbolname =
        symbolname + MacroNameTok.getIdentifierInfo()->getName().str(); // FIXME

    clang::SourceManager &sourceManager = Context->sourceManager();
    unsigned int offset = sourceManager.getFileOffset(sloc);
    location += std::to_string(sourceManager.getLineNumber(fileID, offset));
    location += ",";
    location += std::to_string(sourceManager.getColumnNumber(fileID, offset));
  }
  recordMacro(symbolname, fileID, ST_Macro, refType, location);
}

void DAPPCallbacks::MacroExpands(
    const Token &MacroNameTok, const MacroDefinition &MD, SourceRange Range,
    const MacroArgs
        *Args) { // ignore multiple use loc!(same path but different offset)

  recordMacro(MacroNameTok, RT_Expansion);
}

void DAPPCallbacks::MacroDefined(const Token &MacroNameTok,
                                 const MacroDirective *MD) {
  recordMacro(MacroNameTok, RT_Definition);
}

void DAPPCallbacks::MacroUndefined(const Token &MacroNameTok,
                                   const MacroDefinition &MD,
                                   const MacroDirective *Undef) {
  recordMacro(MacroNameTok, RT_Undefinition);
}

void DAPPCallbacks::Defined(const Token &MacroNameTok,
                            const MacroDefinition &MD, SourceRange Range) {
  recordMacro(MacroNameTok, RT_DefinedTest);
}

void DAPPCallbacks::Ifdef(SourceLocation Loc, const Token &MacroNameTok,
                          const MacroDefinition &MD) {

  recordMacro(MacroNameTok, RT_DefinedTest);
}

void DAPPCallbacks::Ifndef(SourceLocation Loc, const Token &MacroNameTok,
                           const MacroDefinition &MD) {

  recordMacro(MacroNameTok, RT_DefinedTest);
}
