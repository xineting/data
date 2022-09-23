//
// Created by 王志星 on 2022/9/20.
//

#include "../DAASTFrontedAction.h"
#include "../Util.h"
#include "CheckDependence.h"
#include "CheckReference.h"
#include "CheckSymbol.h"
#include "iostream"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "gtest/gtest.h"
#include <vector>

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
llvm::cl::opt<std::string>
    OutputFileName("o", llvm::cl::desc("Specify output filename"),
                   llvm::cl::value_desc("filename"),
                   llvm::cl::cat(MyToolCategory));
DAFileManager *m_fm = nullptr;
extern std::string workdirectory;

bool RUN_DA(int argc, const char **argv) {
  auto ExpectedParser =
      tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    exit(0);
  }
  m_fm = new DAFileManager();
  clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
  std::vector<clang::tooling::CompileCommand> CompileCommandsForProject =
      OptionsParser.getCompilations().getAllCompileCommands();
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());
  Tool.appendArgumentsAdjuster(
      OptionsParser.getArgumentsAdjuster()); // 考虑extra-arg参数
  Tool.run(
      clang::tooling::newFrontendActionFactory<DAASTFrontendAction>().get());
  //  llvm::llvm_shutdown();
  return true;
}

// FieldTypeTag测试
TEST(FieldTypeTagTest, FieldTypeTag) {
  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/FieldTypeTagTest";

  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct1",
                      "targettypedef1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct1",
                      "targettypedef2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct1",
                      "targetstruct1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct1",
                      "targetunion2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef1",
                      "targettypedef1", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult5);

  bool TrueResult6 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef1",
                      "targettypedef2", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult6);

  bool TrueResult7 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef1",
                      "targetstruct1", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult7);

  bool TrueResult8 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef1",
                      "targetunion2", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult8);

  bool TrueResult9 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion1",
                      "targettypedef1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult9);

  bool TrueResult10 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion1",
                      "targettypedef2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult10);

  bool TrueResult11 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion1",
                      "targetstruct1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult11);

  bool TrueResult12 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion1",
                      "targetunion2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult12);

  bool TrueResult13 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef2",
                      "targettypedef1", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult13);

  bool TrueResult14 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef2",
                      "targettypedef2", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult14);

  bool TrueResult15 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef2",
                      "targetstruct1", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult15);

  bool TrueResult16 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcetypedef2",
                      "targetunion2", SDT_FieldType_Tag);
  EXPECT_FALSE(TrueResult16);

  bool TrueResult17 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct2",
                      "targettypedef1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult17);

  bool TrueResult18 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct2",
                      "targettypedef2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult18);

  bool TrueResult19 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct2",
                      "targetstruct1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult19);

  bool TrueResult20 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourcestruct2",
                      "targetunion2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult20);

  bool TrueResult21 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion2",
                      "targettypedef1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult21);

  bool TrueResult22 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion2",
                      "targettypedef2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult22);

  bool TrueResult23 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion2",
                      "targetstruct1", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult23);

  bool TrueResult24 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceunion2",
                      "targetunion2", SDT_FieldType_Tag);
  EXPECT_TRUE(TrueResult24);
}

// FunctionFunction测试
TEST(FunctionFunctionTest, FunctionFunction) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/FunctionFunctionTest";

  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "main",
                      "targetfunctioninhello1", SDT_Function_Function);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "main",
                      "targetfunctioninhello2h2", SDT_Function_Function);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "targetfunctioninhello1",
                      "targetfunctioninhello2h2", SDT_Function_Function);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "targetfunctioninhello1",
                      "targetfunctioninhello2h", SDT_Function_Function);
  EXPECT_TRUE(TrueResult4);
}

// GlobVarFunctionTest
TEST(GlobVarFunctionTest, GlobVarFunction) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/GlobVarFunctionTest";
  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "main", "globinhello_h",
                      SDT_GlobalVar_Function);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/main.c", "main", "globinmain",
                      SDT_GlobalVar_Function);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "test", "globinhello2",
                      SDT_GlobalVar_Function);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "main",
                      "structtypedefdecl", SDT_GlobalVar_Function);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/main.c", "main", "externglob1",
                      SDT_GlobalVar_Function);
  EXPECT_TRUE(TrueResult5);

  // 局部变量不在依赖关系中
  bool FalseResult1 = CheckDependence(
      Filemap, FileIDMap, WorkingDirectory + "/main.c",
      WorkingDirectory + "/main.c", "main", "noglob1", SDT_GlobalVar_Function);
  EXPECT_FALSE(FalseResult1);
}

// GlobVarGlobVar测试
TEST(GlobVarGlobVarTest, GlobVarGlobVar) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/GlobVarGlobVarTest";
  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);
  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  // 可以测到的测试用例
  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceinmain_c",
                      "targetinhello_h", SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "sourceinmain_c",
                      "targetinhello2_h", SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "sourceinhello_h",
                      "targetinhello2_h", SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/main.c", "EnumSource", "EnumTarget",
                      SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "typedefTarget",
                      "typedefSource", SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult5);

  bool TrueResult6 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceinmain_c2",
                      "targetinhello_h", SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult6);

  bool TrueResult7 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/main.c", "int_operator", "int_add",
                      SDT_GlobalVar_GlobalVar);
  EXPECT_TRUE(TrueResult7);

  bool CantFindDepence =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/main.c", "sourceinmain_c2",
                      "paraminmain", SDT_GlobalVar_GlobalVar);
  EXPECT_FALSE(CantFindDepence);

  // 第一个不认为是全局变量依赖于全局变量的测试用例
  // glob-field
  bool FalseResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "typedefTarget",
                      "Enumuator_1", SDT_GlobalVar_GlobalVar);
  EXPECT_FALSE(FalseResult1);

  // glob->一个不存在的变量
  bool FalseResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello.h", "sourceinmain_c",
                      "noexist", SDT_GlobalVar_GlobalVar);
  EXPECT_FALSE(FalseResult2);

  // a=b+c,
  // b->c
  // 并且b=c这条语句不存在
  bool FalseResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "targetinhello_h",
                      "targetinhello2_h", SDT_GlobalVar_GlobalVar);
  EXPECT_FALSE(FalseResult3);

  // a=b+c,
  // b->a
  bool FalseResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/main.c", "targetinhello_h",
                      "sourceinmain_c", SDT_GlobalVar_GlobalVar);
  EXPECT_FALSE(FalseResult4);
}

// ParamTypeFunction测试
TEST(ParamTypeFunctionTest, ParamTypeFunction) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/ParamTypeFunctionTest";
  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);

  m_fm->Out2File(OutputFileName);
  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "functioninhello1",
                      "typedefname", SDT_ParamType_Function);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello1",
                      "typedefname2", SDT_ParamType_Function);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "functioninhello2",
                      "typedefname", SDT_ParamType_Function);

  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello2",
                      "typedefname2", SDT_ParamType_Function);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello3",
                      "structname2", SDT_ParamType_Function);
  EXPECT_TRUE(TrueResult5);

  bool TrueResult6 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello3",
                      "enumname", SDT_ParamType_Function);
  EXPECT_TRUE(TrueResult6);



}

// RetTypeFunctionTest测试
TEST(RetTypeFunctionTest, RetTypeFunction) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/RetTypeFunctionTest";

  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello2.h", "functioninhello1",
                      "typedefname", SDT_RetType_Function);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello2",
                      "typedefname2", SDT_RetType_Function);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello3",
                      "typedefname2", SDT_RetType_Function);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                      WorkingDirectory + "/hello.h", "functioninhello4",
                      "typedefname2", SDT_RetType_Function);
  EXPECT_TRUE(TrueResult4);
}

// TagTypedefTEST
TEST(TagTypedefTEST, TagTypedef) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/TagTypedefTEST";
  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/main.c", "typedefname2",
                      "structname1", SDT_Tag_Typedef);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/main.c", "typedefname3",
                      "structname2", SDT_Tag_Typedef);
  EXPECT_TRUE(TrueResult2);
}

// TypedefGlobVarTest
TEST(TypedefTypedefTEST, TypedefTypedef) {

  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/TypedefGlobVarTest";

  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";

  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  bool TrueResult1 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "enumglob", "typdefenum",
                      SDT_Typedef_GlobalVar);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "enumglob2", "enumdef",
                      SDT_Typedef_GlobalVar);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "typedef1glob",
                      "typedef1", SDT_Typedef_GlobalVar);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "typedef2glob",
                      "typedef2", SDT_Typedef_GlobalVar);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "typedef3glob",
                      "typedef3", SDT_Typedef_GlobalVar);
  EXPECT_TRUE(TrueResult5);

  bool TrueResult6 =
      CheckDependence(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                      WorkingDirectory + "/hello2.h", "struct2glob", "struct2",
                      SDT_Typedef_GlobalVar);
  EXPECT_TRUE(TrueResult6);
}

// IncludeSymbol测试
TEST(IncludeSymbolTest, IncludeSymbol) {
  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/IncludeSymbolTest";
  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";
  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  // 关于include主要检查一下symbol是否有相关的符号，并且是否是reftable中包含，因此有两个需要check的内容。
  bool TrueResult1 =
      CheckSymbol(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  WorkingDirectory + "/hello.h", ST_Path);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckSymbol(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  WorkingDirectory + "/hello2.h", ST_Path);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckSymbol(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                  WorkingDirectory + "/hello2.h", ST_Path);
  EXPECT_TRUE(TrueResult3);

  bool TrueResult4 =
      CheckReference(Filemap,
                     FileIDMap,
                     WorkingDirectory + "/main.c",
                     WorkingDirectory + "/hello.h",
                     RT_Included);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  WorkingDirectory + "/hello2.h", RT_Included);
  EXPECT_TRUE(TrueResult5);

  bool TrueResult6 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/hello.h",
                  WorkingDirectory + "/hello2.h", RT_Included);
  EXPECT_TRUE(TrueResult6);


}

// MacroSymbolTest
TEST(MacroSymbolTest, MacroSymbol) {
  // getenv("ProjectPath");
  std::string WorkingDirectory = workdirectory + "/MacroSymbolTest";
  std::string CompileComandsJson = WorkingDirectory + "/build";
  std::string Mainfilepath = WorkingDirectory + "/main.c";
  std::string Outfilepath = WorkingDirectory + "/main.c.json";
  const char *Argv[6] = {"",
                         "-p",
                         CompileComandsJson.c_str(),
                         "-o",
                         Outfilepath.c_str(),
                         Mainfilepath.c_str()};

  bool ResultRunDa = RUN_DA(6, Argv);
  EXPECT_TRUE(ResultRunDa);
  m_fm->Out2File(OutputFileName);

  std::unordered_map<std::string, SymbolTable *> Filemap =
      m_fm->GetFilepathMap();
  std::unordered_map<clang::FileID, SymbolTable *, FileIDHash> FileIDMap =
      m_fm->GetFileIDMap();

  // 关于include主要检查一下symbol是否有相关的符号，并且是否是reftable中包含，因此有两个需要check的内容。
  bool TrueResult1 =
      CheckSymbol(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  +"DefineName1", ST_Macro);
  EXPECT_TRUE(TrueResult1);

  bool TrueResult2 =
      CheckSymbol(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  "DefineName2", ST_Macro);
  EXPECT_TRUE(TrueResult2);

  bool TrueResult3 =
      CheckSymbol(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  "DefineNameInHello", ST_Macro);
  EXPECT_TRUE(TrueResult3);


  bool TrueResult4 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  +"DefineName1", RT_Definition);
  EXPECT_TRUE(TrueResult4);

  bool TrueResult5 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                  "DefineName2", RT_Definition);
  EXPECT_TRUE(TrueResult5);

  bool TrueResult6 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                     "DefineNameInHello", RT_DefinedTest);
  EXPECT_TRUE(TrueResult6);


  bool TrueResult7 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                     "DefineNameInHello4", RT_Definition);
  EXPECT_TRUE(TrueResult7);

  bool TrueResult8 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                     "DefineNameInHello5", RT_Definition);
  EXPECT_TRUE(TrueResult8);

  bool TrueResult9 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                     "DefineNameInHello5", RT_Undefinition);
  EXPECT_TRUE(TrueResult9);

  bool FalseResult1 =
      CheckReference(Filemap, FileIDMap, WorkingDirectory + "/main.c",
                     "DefineNameInHello3", RT_Definition);
  EXPECT_FALSE(FalseResult1);
}