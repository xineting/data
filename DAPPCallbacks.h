/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#ifndef DA_PPCALLBACK_H
#define DA_PPCALLBACK_H

#include "DAContext.h"
#include "DAFileManager.h"
#include "SymbolTable.h"
#include "Util.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class DAPPCallbacks : public clang::PPCallbacks {
public:
  DAPPCallbacks(DAContext *Context);
  void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                          StringRef FileName, bool IsAngled,
                          CharSourceRange FilenameRange,
                          Optional<FileEntryRef> File, StringRef SearchPath,
                          StringRef RelativePath, const Module *Imported,
                          SrcMgr::CharacteristicKind FileType);


  virtual void MacroExpands(const Token &MacroNameTok,
                            const MacroDefinition &MD, SourceRange Range,
                            const MacroArgs *Args);
  virtual void MacroDefined(const Token &MacroNameTok,
                            const MacroDirective *MD);
  virtual void MacroUndefined(const Token &MacroNameTok,
                              const MacroDefinition &MD,
                              const MacroDirective *Undef);
  virtual void Defined(const Token &MacroNameTok, const MacroDefinition &MD,
                       SourceRange Range);
  virtual void Ifdef(SourceLocation Loc, const Token &MacroNameTok,
                     const MacroDefinition &MD);
  virtual void Ifndef(SourceLocation Loc, const Token &MacroNameTok,
                      const MacroDefinition &MD);
  DAContext *Context;

private:
  // inner record
  void recordMacro(std::string symbolname, clang::FileID FileID, SymbolType st,
                   RefType rt, std::string location);

  void recordMacro(const clang::Token &MacroNameTok, RefType refType);


};

#endif