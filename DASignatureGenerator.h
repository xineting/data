/*************************************************
 * HISTORY:                                      *
 *	01/30/2021 pyx : Created.                    *
  *	03/04/2021 pyx : Edited.                     *
=================================================*/




#ifndef DASignatureGenerator_H
#define DASignatureGenerator_H

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclVisitor.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

#include "Util.h"

class DASignatureGenerator : public clang::DeclVisitor<DASignatureGenerator> {
public:
  DASignatureGenerator(clang::NamedDecl *originalDecl, std::string &output);
  bool error() { return m_error; }
  void VisitDeclContext(clang::DeclContext *context);
  void VisitNamespaceDecl(clang::NamespaceDecl *decl);
  void VisitClassTemplateSpecializationDecl(
      clang::ClassTemplateSpecializationDecl *decl);
  void VisitTagDecl(clang::TagDecl *decl);
  void VisitVarDecl(clang::VarDecl *decl);
  void VisitFunctionDecl(clang::FunctionDecl *decl);
  void VisitNamedDecl(clang::NamedDecl *decl);

private:
  void printSeparator();

private:
  clang::NamedDecl *m_originalDecl;
  bool m_error;
  bool m_needSeparator;
  bool m_needFilePrefix;
  bool m_needOffsetPrefix;
  bool m_inDeclContext;
  llvm::raw_string_ostream m_out;
};
#endif