/*************************************************
 * HISTORY:                                      *
 *	01/30/2021 pyx : Created.                    *
  *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "DASignatureGenerator.h"

DASignatureGenerator::DASignatureGenerator(clang::NamedDecl *originalDecl,
                                           std::string &output)
    : m_originalDecl(originalDecl), m_error(false), m_needSeparator(false),
      m_needFilePrefix(false), m_needOffsetPrefix(false),
      m_inDeclContext(false), m_out(output) {
  m_out.str();
}

void DASignatureGenerator::printSeparator() {
  if (!m_needSeparator)
    return;
  m_out << "::";
  m_needSeparator = false;
  m_out.str();
}

void DASignatureGenerator::VisitDeclContext(clang::DeclContext *context) {

  if (clang::NamedDecl *decl =
          llvm::dyn_cast_or_null<clang::NamedDecl>(context)) {
    if (clang::FunctionDecl *funcDecl =
            llvm::dyn_cast<clang::FunctionDecl>(decl)) {
      if (funcDecl->isThisDeclarationADefinition()) {
        m_needFilePrefix = true;
        m_needOffsetPrefix = true;
      }
    }
    m_inDeclContext = true;
    Visit(decl);
    return;
  }

  if (!m_needFilePrefix)
    return;

  clang::Decl *decl = m_originalDecl;
  if (clang::Decl *canonicalDecl = decl->getCanonicalDecl())
    decl = canonicalDecl;
  clang::SourceManager &sourceManager =
      decl->getASTContext().getSourceManager();
  clang::SourceLocation sloc =
      sourceManager.getSpellingLoc(decl->getLocation());
  if (!sloc.isInvalid()) {
    clang::FileID fileID = sourceManager.getFileID(sloc);
    if (!fileID.isInvalid()) {
      const clang::FileEntry *fileEntry =
          sourceManager.getFileEntryForID(fileID);
      if (fileEntry != NULL) {
        llvm::StringRef fname = fileEntry->getName();
        m_out << const_basename(fname.data());
        if (m_needOffsetPrefix)
          m_out << '@' << sourceManager.getFileOffset(sloc);
        m_out << '/';
      }
    }
  }
  m_out.str();
}

void DASignatureGenerator::VisitNamespaceDecl(clang::NamespaceDecl *decl) {
  if (decl->isAnonymousNamespace()) {
    m_needFilePrefix = true;
    VisitDeclContext(decl->getDeclContext());
    printSeparator();
    m_out << "<anon>";
    m_needSeparator = true;
  } else {
    VisitDeclContext(decl->getDeclContext());
    printSeparator();
    decl->printName(m_out);
    m_needSeparator = true;
  }
  m_out.str();
  // llvm::errs() << m_out.str() + "\n";
}

void DASignatureGenerator::VisitClassTemplateSpecializationDecl(
    clang::ClassTemplateSpecializationDecl *decl) {
  VisitTagDecl(decl);
  while (decl->getSpecializationKind() != clang::TSK_ExplicitSpecialization) {
    llvm::PointerUnion<clang::ClassTemplateDecl *,
                       clang::ClassTemplatePartialSpecializationDecl *>
        instantiatedFrom = decl->getInstantiatedFrom();
    clang::ClassTemplatePartialSpecializationDecl *partialSpec =
        instantiatedFrom
            .dyn_cast<clang::ClassTemplatePartialSpecializationDecl *>();
    if (partialSpec != NULL) {
      decl = partialSpec;
      continue;
    }
    return;
  }

  m_out << '<';
  const clang::TemplateArgumentList &args = decl->getTemplateArgs();
  for (unsigned i = 0, iEnd = args.size(); i != iEnd; ++i) {
    if (i != 0)
      m_out << ", ";
    args[i].print(decl->getASTContext().getPrintingPolicy(), m_out, true);
  }
  m_out << '>';
  m_out.str();
  // llvm::errs() << m_out.str() + "\n";
}

void DASignatureGenerator::VisitTagDecl(clang::TagDecl *decl) {
  VisitDeclContext(decl->getDeclContext());

  if (m_inDeclContext && decl->isEnum())
    return;

  clang::NamedDecl *namedDecl = decl;
  clang::TypedefNameDecl *anonDecl = decl->getTypedefNameForAnonDecl();
  if (anonDecl != NULL)
    namedDecl = anonDecl;

  printSeparator();

  if (decl->getTagKind() == clang::TTK_Struct)
    m_out << "struct::";
  else if (decl->getTagKind() == clang::TTK_Union)
    m_out << "union::";
  else if (decl->getTagKind() == clang::TTK_Enum)
    m_out << "enum::";

  clang::IdentifierInfo *identifier = namedDecl->getIdentifier();
  if (identifier == NULL) {
    m_out << "<unnamed"
          << decl->getASTContext().getSourceManager().getFileOffset(
                 decl->getLocation())
          << '>';
  } else {
    m_out << identifier->getName();
  }
  m_needSeparator = true;
  m_out.str();
  // llvm::errs() << m_out.str() + "\n";
}

void DASignatureGenerator::VisitVarDecl(clang::VarDecl *decl) {
  if (!decl->isExternC() && !decl->hasExternalStorage())
    m_needFilePrefix = true;
  VisitDeclContext(decl->getDeclContext());

  printSeparator();
  decl->printName(m_out);
  m_needSeparator = true;
  m_out.str();

  // llvm::errs() << m_out.str() + "\n";
}

void DASignatureGenerator::VisitFunctionDecl(clang::FunctionDecl *decl) {
  clang::PrintingPolicy policy = decl->getASTContext().getPrintingPolicy();
  policy.SuppressTagKeyword = true;
  bool isExternC = false;

  if (clang::FunctionDecl *canonical = decl->getCanonicalDecl())
    isExternC = canonical->isExternC();

  if (decl->getTemplateInstantiationPattern() != NULL)
    decl = decl->getTemplateInstantiationPattern();

  if (!isExternC && !decl->isInExternCContext() &&
      !decl->isInExternCXXContext())
    m_needFilePrefix = true;

  VisitDeclContext(decl->getDeclContext());
  printSeparator();

  m_out << decl->getDeclName().getAsString();

  if (const clang::TemplateArgumentList *templateArgs =
          decl->getTemplateSpecializationArgs()) {
    m_out << '<';
    for (unsigned int i = 0; i < templateArgs->size(); ++i) {
      if (i != 0)
        m_out << ", ";
      templateArgs->get(i).print(policy, m_out,true);
    }
    m_out << '>';
  }
  // llvm::errs() << m_out.str() + "\n";
  m_out.str();
  m_needSeparator = true;
}

void DASignatureGenerator::VisitNamedDecl(clang::NamedDecl *decl) {
  VisitDeclContext(decl->getDeclContext());
  printSeparator();

  decl->printName(m_out);
  m_needSeparator = true;
  m_out.str();
  // llvm::errs() << m_out.str() + "\n";
}
