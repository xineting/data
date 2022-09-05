/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#include "DAASTConsumer.h"
#include "DAContext.h"
#include "DAFileManager.h"
#include "DAPPCallbacks.h"
#include "SymbolTable.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

class DAASTFrontendAction : public clang::ASTFrontendAction {
public:
  ~DAASTFrontendAction();
  bool BeginSourceFileAction(clang::CompilerInstance &ci);
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile);

private:
  DAContext *getContext(clang::CompilerInstance &ci);
  DAContext *m_context;
  DAFileManager *m_fm; // TODO:remove me
};
