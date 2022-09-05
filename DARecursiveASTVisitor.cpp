/*************************************************
 * Traverse AST logic.                           *
 *                                               *
 * HISTORY:                                      *
 *	06/30/2020 pyx : Created.                    *
 *	06/01/2022 pyx : Edited.                     *
=================================================*/
#include "DARecursiveASTVisitor.h"
using namespace clang;

DARecursiveASTVisitor::DARecursiveASTVisitor(DAContext *daContext)
    : m_context(daContext) {
  assert(this->m_context != nullptr);
}

bool DARecursiveASTVisitor::VisitMemberExpr(clang::MemberExpr *e) {
  RecordDeclRefExpr(e->getMemberDecl(), e->getMemberLoc());
  return true;
}

bool DARecursiveASTVisitor::VisitTypeLoc(clang::TypeLoc tl) {
  if (!tl.getAs<clang::TagTypeLoc>().isNull()) {
    const clang::TagTypeLoc &ttl = tl.castAs<clang::TagTypeLoc>();
    RecordDeclRef(ttl.getDecl(), tl.getBeginLoc(), RT_Reference);
  } else if (!tl.getAs<clang::TypedefTypeLoc>().isNull()) {
    const clang::TypedefTypeLoc &ttl = tl.castAs<clang::TypedefTypeLoc>();
    RecordDeclRef(ttl.getTypedefNameDecl(), tl.getBeginLoc(), RT_Reference);
  } else if (!tl.getAs<clang::TemplateTypeParmTypeLoc>().isNull()) {
    const clang::TemplateTypeParmTypeLoc &ttptl =
        tl.castAs<clang::TemplateTypeParmTypeLoc>();
    RecordDeclRef(ttptl.getDecl(), tl.getBeginLoc(), RT_Reference);
  } else if (!tl.getAs<clang::TemplateSpecializationTypeLoc>().isNull()) {
    const clang::TemplateSpecializationTypeLoc &tstl =
        tl.castAs<clang::TemplateSpecializationTypeLoc>();
    const clang::TemplateSpecializationType &tst =
        *tstl.getTypePtr()->getAs<clang::TemplateSpecializationType>();
    if (tst.getAsCXXRecordDecl()) {
      RecordDeclRef(tst.getAsCXXRecordDecl(), tl.getBeginLoc(), RT_Reference);
    }
  }

  return true;
}

TypeDecl* GetTypeDecl(QualType QT){
  TypeDecl *td = nullptr;
  while (QT->isPointerType()) {
    const PointerType *pointerType = QT->getAs<PointerType>();
    assert(pointerType != nullptr);
    QT = pointerType->getPointeeType();
  }
  const clang::Type *type = QT.getTypePtrOrNull();
  if (type != nullptr && !type->isBuiltinType()) {}

  if (const TypedefType *typedefType1 = type->getAs<TypedefType>()) {
    td = typedefType1->getDecl();
  }
  else if (type->getAsTagDecl() != nullptr) { // enum、record类型
    td = type->getAsTagDecl();
  }
  else {
      td = nullptr;
  }
  return td;
}

bool DARecursiveASTVisitor::VisitDecl(clang::Decl *d) {
  if (clang::NamedDecl *nd = llvm::dyn_cast<clang::NamedDecl>(d)) {
    clang::SourceLocation loc = nd->getLocation();
    if (clang::FunctionDecl *fd = llvm::dyn_cast<clang::FunctionDecl>(d)) {
      if (fd->getTemplateInstantiationPattern() != NULL) {
        // 这里是为了获取fd_ctx，为了获取函数下引用的全局变量符号。
      } else {
        fd_ctx = fd;
        SourceManager &sm = m_context->sourceManager();
        clang::SourceLocation spellingloc = sm.getSpellingLoc(loc);
        fd_loc = spellingloc;
        RefType refType = fd->isThisDeclarationADefinition() ? RT_Definition : RT_Declaration;
        SymbolType symbolType = ST_Function;
        RecordDeclRef(nd, loc, refType, symbolType);
        // 获取返回值的类型
        QualType QT = fd->getReturnType();
        TypeDecl* td = GetTypeDecl(QT);

        if (td != nullptr) {
          RecordSymbolDep1(
              SDT_RetType_Function,
              fd_ctx,
              td,
              td->getBeginLoc()
          );
        }
      }
    }
    else {
      const bool IsParam = llvm::isa<clang::ParmVarDecl>(d);
      if (!d->getParentFunctionOrMethod() && !IsParam) {
        fd_ctx = nullptr;
      }
      if (clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(d)) {
        // 记录参数类型和函数的依赖关系
        if (IsParam) {
          bool omitParamVar = false;
          if (d->getDeclContext()->getDeclKind() == Decl::Function) {
            QualType QT = vd->getType();
            while (QT->isPointerType()) {
              const PointerType *pointerType = QT->getAs<PointerType>();
              assert(pointerType != nullptr);
              QT = pointerType->getPointeeType();
            }
            const clang::Type *type = QT.getTypePtrOrNull();
            if (type != nullptr && !type->isBuiltinType()) {
              TypeDecl *td = nullptr;
              if (const TypedefType *typedefType = type->getAs<TypedefType>()) {
                td = typedefType->getDecl();
              } else if (type->getAsTagDecl() != nullptr) { // enum、record类型
                td = type->getAsTagDecl();
              } else {
                const TypedefType *typedefType = type->getAs<TypedefType>();
                if (typedefType != nullptr) {
                  td = typedefType->getDecl();
                }
              }
              FunctionDecl *fd = llvm::dyn_cast_or_null<clang::FunctionDecl>(
                  d->getDeclContext());
              if (fd && !fd->isThisDeclarationADefinition()) {
                omitParamVar = true;
              }
              if (!omitParamVar) {
                RefType refType;
                if (vd->isThisDeclarationADefinition() ==
                    clang::VarDecl::DeclarationOnly) {
                  refType = RT_Declaration;
                } else {
                  refType = RT_Definition;
                }
                RecordDeclRef(nd, loc, refType, ST_Parameter);
                if (td != nullptr) {
                  RecordSymbolDep1(SDT_ParamType_Function,fd_ctx,td,td->getBeginLoc());
                }
              }
            }
          }
        }
        // 如果不是参数的符号，那么就记录该符号为全局符号，或者是局部变量。
        else if (vd->getParentFunctionOrMethod() == NULL){
          SymbolType symbolType = ST_GlobalVariable;
          RefType refType;
          if (vd->isThisDeclarationADefinition() == clang::VarDecl::DeclarationOnly) {
            refType = RT_Declaration;
          }
          else {
            refType = RT_Definition;
          }
          RecordDeclRef(nd, loc, refType, symbolType);
          QualType QT = vd->getType();
          TypeDecl* td = GetTypeDecl(QT);
          if(td!=nullptr){
            RecordSymbolDep1(SDT_Typedef_GlobalVar,vd,td,vd->getBeginLoc());
          }
        }
      }
      else if (clang::TagDecl *td = llvm::dyn_cast<clang::TagDecl>(d)) {
        RefType refType = td->isThisDeclarationADefinition() ? RT_Definition : RT_Declaration;
        SymbolType symbolType = ST_Max;
        switch (td->getTagKind()) {
        case clang::TTK_Struct:
          symbolType = ST_Tag;
          break;
        case clang::TTK_Union:
          symbolType = ST_Tag;
          break;
        case clang::TTK_Enum:
          symbolType = ST_Enum;
          break;
        default:
          return true;
        }
        std::string tmp1 = nd->getQualifiedNameAsString();
        llvm::outs() << "typedefDecl"<<" "<< tmp1<<'\n';
        RecordDeclRef(nd, loc, refType, symbolType);
      }
      else if (clang::FieldDecl *fd = llvm::dyn_cast<clang::FieldDecl>(d)) {
        RecordDeclRef(nd, loc, RT_Declaration, ST_Field);
        QualType QT = fd->getType();
        TypeDecl *td = nullptr;
        td = GetTypeDecl(QT);
        if(td!= nullptr){
          clang::RecordDecl *rd = fd->getParent();
          if (rd) {
            RecordSymbolDep1(SDT_FieldType_Typedef,rd,td,td->getBeginLoc());
          }
        }
      }
      else if (clang::TypedefNameDecl *tdd = llvm::dyn_cast<clang::TypedefNameDecl>(d)) {
        RecordDeclRef(nd, loc, RT_Declaration, ST_Typedef);
        QualType QT = tdd->getUnderlyingType();
        TypeDecl *td = nullptr;
        td = GetTypeDecl(QT);
        if (td != nullptr) {

          RecordSymbolDep1(SDT_Typedef_Typedef,tdd,td,tdd->getBeginLoc());
        }
      }
      else if (llvm::isa<clang::EnumConstantDecl>(d)) {
        RecordDeclRef(nd, loc, RT_Declaration, ST_Enumerator);
      }
      else {
        RecordDeclRef(nd, loc, RT_Declaration);
      }
    }
  }
  return true;
}


bool DARecursiveASTVisitor::VisitDeclRefExpr(clang::DeclRefExpr * e) {
    clang::ValueDecl *vd = e->getDecl();
    RecordDeclRefExpr(vd, e->getLocation());
    if (fd_ctx && vd) {
      if (clang::VarDecl *var = llvm::dyn_cast<clang::VarDecl>(vd)) {

        if (var->hasGlobalStorage() && var->getStorageClass() != SC_Static) {
          SourceManager &sm = m_context->sourceManager();
          if (fd_loc.isValid()) {
            llvm::outs() << "fd_loc:" << sm.getFilename(fd_loc).str() << ":"
                         << sm.getSpellingLineNumber(fd_loc) << ":"
                         << sm.getSpellingColumnNumber(fd_loc) << "\n";
          }
          RecordSymbolDep1(
              SDT_GlobalVar_Function,
              fd_ctx,
              var,
              e->getBeginLoc()
          );
        }
      }
    }
    if (vd_ctx && vd) {
      if (clang::VarDecl *var = llvm::dyn_cast<clang::VarDecl>(vd)) {
        if (var->hasGlobalStorage() && var->getStorageClass() != SC_Static &&
            vd_ctx != var) {
          RecordSymbolDep1(
              SDT_GlobalVar_GlobalVar,
              vd_ctx,
              var,
              e->getBeginLoc()
          );
        }
      }
    }
    return true;
  }

void DARecursiveASTVisitor::RecordDeclRefExpr(clang::NamedDecl * d, clang::SourceLocation loc) {
    RecordDeclRef(d, loc, RT_Reference);
  }

bool DARecursiveASTVisitor::VisitStmt(clang::Stmt * st) {
  if (clang::BinaryOperator *binaryOp = llvm::dyn_cast<clang::BinaryOperator>(st)){
    if (binaryOp->isAssignmentOp()) {
    clang::Expr *lhs = binaryOp->getLHS();
    clang::Expr *rhs = binaryOp->getRHS();

    if (clang::DeclRefExpr *left = llvm::dyn_cast<clang::DeclRefExpr>(lhs)) {
      clang::ValueDecl *vd = left->getDecl();
      if (vd) {
        if (clang::VarDecl *var = llvm::dyn_cast<clang::VarDecl>(vd)) {
          if (var->hasGlobalStorage() && var->getStorageClass() != SC_Static) {
            vd_ctx = var;
            vd_loc = vd_ctx->getBeginLoc();
          }
        }
      }
    }
  }
  }
  else{
    vd_ctx = nullptr;
  }
  if (clang::CallExpr *call = llvm::dyn_cast<clang::CallExpr>(st)) {
      clang::FunctionDecl *func_decl = call->getDirectCallee();
      if (func_decl == nullptr) {
        return true;
      }
      if (fd_ctx == nullptr) {
        return true;
      }
      if (!func_decl->isStatic() || !fd_ctx->isStatic())
        RecordSymbolDep1(
            SDT_Function_Function,
            fd_ctx,
            func_decl,
            call->getBeginLoc()
        );
    }
  return true;
  }

static inline bool isNamedDeclUnnamed(clang::NamedDecl * d) {
    return d->getDeclName().isIdentifier() && d->getIdentifier() == NULL;
  }

void DARecursiveASTVisitor::RecordDeclRef(clang::NamedDecl * d, clang::SourceLocation beginLoc, RefType refType,SymbolType symbolType) {
    assert(d != NULL);
    if (isNamedDeclUnnamed(d))
      return;

    SourceManager &sm = m_context->sourceManager();

    clang::SourceLocation spellingLoc = sm.getSpellingLoc(beginLoc);
    std::string location = "";
    clang::FileID FileID;

    if (spellingLoc.isValid()) {
      FileID = sm.getFileID(spellingLoc);
      unsigned int offset = sm.getFileOffset(spellingLoc);

      location =
          location + std::to_string(sm.getLineNumber(FileID, offset)) + ",";
      location = location + std::to_string(sm.getColumnNumber(FileID, offset));

    }
    SymbolTable *f_st = m_context->getFileSymbolTable(FileID);
    assert(f_st != nullptr);
    f_st->addRef(d, symbolType, refType, location);
  }


void DARecursiveASTVisitor::RecordSymbolDep1(
      SDependencyType Sdt,
      clang::NamedDecl * Sd,
      clang::NamedDecl * Ed,
      clang::SourceLocation OcurrenceLoc
      )
  {
    assert(Sd != NULL);
    assert(Ed != NULL);
    if (isNamedDeclUnnamed(Sd) || isNamedDeclUnnamed(Ed)) return;

    SourceManager &Sm = m_context->sourceManager();

    clang::SourceLocation SSpellingLoc,ESpellingLoc,OSpellingLoc;

    SSpellingLoc = Sm.getSpellingLoc(Sd->getBeginLoc());
    ESpellingLoc = Sm.getSpellingLoc(Ed->getBeginLoc());
    OSpellingLoc = Sm.getSpellingLoc(OcurrenceLoc);

    clang::FileID Sfid;
    clang::FileID Efid;
    clang::FileID Ofid;

    if (SSpellingLoc.isValid()&&ESpellingLoc.isValid()&&OSpellingLoc.isValid()) {
      Sfid = Sm.getFileID(SSpellingLoc);
      Efid = Sm.getFileID(ESpellingLoc);
      Ofid = Sm.getFileID(OSpellingLoc);
    }else{
      return;
    }

    unsigned int Soffset = Sm.getFileOffset(SSpellingLoc);
    unsigned int Eoffset = Sm.getFileOffset(ESpellingLoc);
    unsigned int Ooffset = Sm.getFileOffset(OSpellingLoc);

    std::string Slocation = "";
    Slocation = Slocation + std::to_string(Sm.getLineNumber(Sfid, Soffset)) + ",";
    Slocation = Slocation + std::to_string(Sm.getColumnNumber(Sfid, Soffset));

    std::string Elocation = "";
    Elocation = Elocation + std::to_string(Sm.getLineNumber(Efid, Eoffset)) + ",";
    Elocation = Elocation + std::to_string(Sm.getColumnNumber(Efid, Eoffset));

    std::string Olocation = "";
    Olocation = Olocation + std::to_string(Sm.getLineNumber(Ofid, Ooffset)) + ",";
    Olocation = Olocation + std::to_string(Sm.getColumnNumber(Ofid, Ooffset));

    SymbolTable *ESt = m_context->getFileSymbolTable(Efid);
    assert(ESt != nullptr);
    unsigned Esid = ESt->getSymbolID(Ed);
    SymbolTable *FSt = m_context->getFileSymbolTable(Sfid);
    assert(FSt != nullptr);

    unsigned  Ssid = FSt->getSymbolID(Sd);

    FSt->addSymbolDependency(Sdt,
                             Ssid,
                             Esid,
                             Sfid,
                             Efid,
                             Slocation,
                             Elocation,
                             Ofid,
                             Olocation);
  }
