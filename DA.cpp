/*************************************************
 * The entry of DA program, only extract one     *
 * source file's information.                    *
 *                                               *
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/

#include "DAASTFrontedAction.h"
#include "Util.h"
#include <vector>
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Driver/Job.h"
#include "DAFileManager.h"

DAFileManager *m_fm;
static llvm::cl::OptionCategory MyToolCategory("my-tool options");
llvm::cl::opt<std::string> OutputFileName("o",
                                          llvm::cl::desc("Specify output filename"),
                                          llvm::cl::value_desc("filename"),
                                          llvm::cl::cat(MyToolCategory));

int main(int argc, const char **argv) {
  auto ExpectedParser = tooling::CommonOptionsParser::create(
      argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  m_fm = new DAFileManager();
  clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();
  std::vector<clang::tooling::CompileCommand> CompileCommandsForProject = OptionsParser.getCompilations().getAllCompileCommands();
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),OptionsParser.getSourcePathList());
  Tool.appendArgumentsAdjuster(OptionsParser.getArgumentsAdjuster());//考虑extra-arg参数
  Tool.run(clang::tooling::newFrontendActionFactory<DAASTFrontendAction>().get());
  m_fm->Out2File(OutputFileName);
  llvm::llvm_shutdown();
  return 0;
}
