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
  DAContext *mc = nullptr;
  mc = getContext(Compiler);
  return std::make_unique<DAASTConsumer>(mc);
}

bool DAASTFrontendAction::BeginSourceFileAction(clang::CompilerInstance &ci) {
  ci.getDiagnostics().setClient(new clang::IgnoringDiagConsumer);
  /**
   * 下边一条语句添加，用于解析代码中的宏定义，
   * 解析宏定义的语句很复杂
   *目前还没有研究透彻
   */
  DAContext *mc = nullptr;
  mc = getContext(ci);
  ci.getPreprocessor().addPPCallbacks(
      std::unique_ptr<clang::PPCallbacks>
          (new DAPPCallbacks(mc)
      ));
//ci.getPreprocessor().addCommentHandler(preprocessor_consumer);//TODO
  return true;
}
