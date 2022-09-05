/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "DAASTFrontedAction.h"

extern std::string OutputFilePath;

DAASTFrontendAction::~DAASTFrontendAction() { m_fm->finilize(); }
std::unique_ptr<clang::ASTConsumer>
DAASTFrontendAction::CreateASTConsumer(clang::CompilerInstance &Compiler,
                                       llvm::StringRef InFile) {
  return std::make_unique<DAASTConsumer>(getContext(Compiler));
}

/**
 * 获取Context，只不过是把Context变成了自己定义的Context了
 * DAContext可以getFileSymbolTable
 * 也就是通过Contect获取代码中的所有的符号表
 * 这里感觉并不是很有必要，会导致代码的可读性变差
 * @param ci
 * @return
 */

DAContext *DAASTFrontendAction::getContext(clang::CompilerInstance &ci) {
  std::string mainfilepath;
  if (m_context == nullptr) {
    if(!OutputFilePath.empty()){
      mainfilepath = OutputFilePath;
    }else{
      mainfilepath =
          ci.getSourceManager()
              .getFileEntryForID(ci.getSourceManager().getMainFileID())
              ->getName()
              .str();
      mainfilepath += ".DA.json";
    }
    m_fm = new DAFileManager(mainfilepath);

    assert(m_fm != nullptr);
    m_context =
        new DAContext(ci.getSourceManager(), ci.getPreprocessor(), m_fm);
  }
  return m_context;
}

bool DAASTFrontendAction::BeginSourceFileAction(clang::CompilerInstance &ci) {
  ci.getDiagnostics().setClient(new clang::IgnoringDiagConsumer);
  /**
   * 下边一条语句添加，用于解析代码中的宏定义，
   * 解析宏定义的语句很复杂
   *目前还没有研究透彻
   */
  ci.getPreprocessor().addPPCallbacks(std::unique_ptr<clang::PPCallbacks>(new DAPPCallbacks(getContext(ci))));
//ci.getPreprocessor().addCommentHandler(preprocessor_consumer);//TODO
  return true;
}
