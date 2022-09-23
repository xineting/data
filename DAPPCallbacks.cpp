/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#include "DAPPCallbacks.h"

DAPPCallbacks::DAPPCallbacks(DAContext *Context) : Context(Context) {
  assert(this->Context != nullptr);
}

/*
void DAPPCallbacks::InclusionDirective(
    clang::SourceLocation hashLoc, const clang::Token &includeTok,
    llvm::StringRef fileName, bool isAngled,
    clang::CharSourceRange filenameRange, const clang::FileEntry *file,
    llvm::StringRef searchPath, llvm::StringRef relativePath,
    const clang::Module *imported, SrcMgr::CharacteristicKind FileType)
    */

void DAPPCallbacks::InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                        StringRef FileName, bool IsAngled,
                        CharSourceRange FilenameRange,
                        Optional<FileEntryRef> File, StringRef SearchPath,
                        StringRef RelativePath, const Module *Imported,
                         SrcMgr::CharacteristicKind FileType)

{
  if (!File.hasValue()) {
    return;
  }
  clang::SourceManager &sm = Context->sourceManager();
  clang::SourceLocation spellingLoc =
      sm.getSpellingLoc(FilenameRange.getBegin());
  if (!spellingLoc.isValid()) {
    llvm::errs() << "include spelling loc invalid\n";
    return;
  }
  clang::FileID RefFileId = sm.getFileID(spellingLoc);
  std::string Location = "";
  unsigned int Offset = sm.getFileOffset(spellingLoc);
  Location += std::to_string(sm.getLineNumber(RefFileId, Offset));
  Location += ",";
  Location += std::to_string(sm.getColumnNumber(RefFileId, Offset));
  std::string Symbol = "@";
  llvm::StringRef Fname = File->getName();
  char *Path;
  Path = realpath(Fname.data(), NULL); // FIXME: only work in linux os
  if (Path != NULL) {
    Symbol += Path;
    free(Path);
  }
  if (Symbol.size() == 1)
    Symbol += "<blank>"; // FIXME
  recordMacro(Symbol, RefFileId, ST_Path, RT_Included, Location);
}

// inner function,USE EXCEPT INCLUDE!
void DAPPCallbacks::recordMacro(std::string Symbolname, clang::FileID FileID,
                                SymbolType St, RefType Rt,
                                std::string Location) {
  assert(!Symbolname.empty());
  assert(!Location.empty());
  SymbolTable *FSt = Context->getFileSymbolTable(FileID);
  FSt->addRef(Symbolname, St, Rt, Location);
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
