/*************************************************
 * Traverse AST logic.                           *
 *                                               *
 * HISTORY:                                      *
 *	06/30/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/
#ifndef DA_RECURSIVEASTVISITOR_H
#define DA_RECURSIVEASTVISITOR_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "DAContext.h"
#include "DAType.h"

class DARecursiveASTVisitor
    : public clang::RecursiveASTVisitor<DARecursiveASTVisitor> {
public:
  explicit DARecursiveASTVisitor(DAContext *daContext);

private:
  friend class clang::RecursiveASTVisitor<DARecursiveASTVisitor>;
  friend class DAASTConsumer;

  // member variables
  DAContext *m_context;

  // Context to save
  clang::FunctionDecl *fd_ctx = nullptr;
  clang::VarDecl *vd_ctx = nullptr;

  clang::SourceLocation fd_loc;

  clang::SourceLocation vd_loc;
  //  bool TraverseNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc
  //  qualifier); bool TraverseBinAssign(clang::BinaryOperator *binaryOp);
  bool VisitMemberExpr(clang::MemberExpr *e);

  bool VisitDeclRefExpr(clang::DeclRefExpr *e);

  bool VisitStmt(clang::Stmt *st);
  //  bool VisitDecl(clang::Decl *d);

  void RecordDeclRefExpr(clang::NamedDecl *d, clang::SourceLocation loc);

  void RecordDeclRef(clang::NamedDecl *d, clang::SourceLocation beginLoc,
                     RefType refType, SymbolType symbolType = ST_Max);
  bool VisitTypeLoc(clang::TypeLoc tl);

  bool VisitDecl(clang::Decl *d);

  bool VisitBinaryOperator(clang::BinaryOperator *binaryOp);

  void RecordSymbolDep1(
      SDependencyType Sdt,
      clang::NamedDecl * Sd,
      clang::NamedDecl * Ed,
      clang::SourceLocation OcurrenceLoc
      );
};

#endif // DA_RECURSIVEASTVISITOR_H
