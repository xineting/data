/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "DAASTConsumer.h"

using namespace clang;
DAASTConsumer::DAASTConsumer(DAContext *Context) : m_context(Context), Visitor(Context) {
  assert(this->m_context != nullptr);
}

void DAASTConsumer::HandleTranslationUnit(clang::ASTContext &Context) {
  Visitor.TraverseDecl(Context.getTranslationUnitDecl()); // entry
}
