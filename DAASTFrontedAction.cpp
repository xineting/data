/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "DAASTFrontedAction.h"
#include "iostream"

DAASTFrontendAction::~DAASTFrontendAction() {
}

DAContext *DAASTFrontendAction::getContext(clang::CompilerInstance &ci) {
  if (m_context == nullptr) {
    m_context = new DAContext(ci.getSourceManager(), ci.getPreprocessor());
  }
  return m_context;
}

std::unique_ptr<clang::ASTConsumer> DAASTFrontendAction::CreateASTConsumer
    (clang::CompilerInstance &Compiler,llvm::StringRef InFile) {
  auto tmp = getContext(Compiler);
  auto tmp1 = new DAASTConsumer(tmp);
  return std::make_unique<DAASTConsumer>(tmp);
}

bool DAASTFrontendAction::BeginSourceFileAction(clang::CompilerInstance &ci) {
  ci.getDiagnostics().setClient(new clang::IgnoringDiagConsumer);
  ci.getPreprocessor().addPPCallbacks(std::unique_ptr<clang::PPCallbacks>(new DAPPCallbacks( getContext(ci))));
  return true;
}
