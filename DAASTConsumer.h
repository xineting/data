/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
  *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "DAContext.h"

#include <assert.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "clang/AST/ASTContext.h"
#include "clang/Tooling/Tooling.h"

#include "DARecursiveASTVisitor.h"

class DAASTConsumer : public clang::ASTConsumer {
public:
  explicit DAASTConsumer(DAContext *Context);
  virtual void HandleTranslationUnit(clang::ASTContext &Context);

private:
  DAContext *m_context;
  DARecursiveASTVisitor Visitor;
};
