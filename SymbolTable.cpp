/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "SymbolTable.h"
#include "Util.h"

int SymbolTable::size() {
  assert(symbolIDTable.size() == symbolTable.size());
  return symbolTable.size();
}

// for AST symbol
void SymbolTable::addRef(clang::NamedDecl *nd, SymbolType st, RefType rt,
                         std::string location) {

  // if(st == ST_Parameter || st == ST_LocalVariable) return;//TODO
  assert(st >= 0 && st <= ST_Max); // FIXME
  assert(rt >= 0 && rt <= RT_Max); // FIXME
  // assert(fid > 0);
  assert(nd != nullptr);

  Ref *r = new Ref;
  r->sid = getSymbolID(nd, st);
  // r->fid = fid;
  r->rt = rt;
  r->loc = location;

  assert(r->sid > 0);

  // m_st.push_back(r);

  if (st == ST_Macro) {
    llvm::errs() << "INTERNAL ERROR! AST addRef() used when st type invalid!@" +
                        nd->getNameAsString() + "\n";
    // exit(1);
    return;
  }

  // if(st == ST_Max) refTable.push_back(s);

  if (rt == RT_Definition) {
    for (const auto &iter : globalDefTable) {
      if (iter->sid == r->sid && iter->loc == r->loc) {
        /*llvm::errs() << "SAME REF DEF!@" + nd->getNameAsString() << " , "
                     << r->loc << " , " << this_filepath + "\n";
        llvm::errs() << "old rt" << r->rt << " ,new rt, " << iter->rt << "\n";
        */
        return;
      }
    }
    globalDefTable.push_back(r);
  } else if (rt == RT_Declaration) {
    for (const auto &iter : globalDecTable) {
      if (iter->sid == r->sid && iter->loc == r->loc) {
        return;
      }
    }
    globalDecTable.push_back(r);
  } else {
    for (const auto &iter : refTable) {
      if (iter->sid == r->sid && iter->loc == r->loc) {
        return;
      }
    }
    refTable.push_back(r);
  }
}

// for Macro symbol
void SymbolTable::addRef(std::string signature, SymbolType st, RefType rt,
                         std::string location) {
  assert(st >= 0 && st <= ST_Max); // FIXME
  assert(rt >= 0 && rt <= RT_Max); // FIXME
  // assert(fid > 0);
  assert(!signature.empty());
  assert(signature[0] == '#' || signature[0] == '@');

  Ref *r = new Ref;
  r->sid = getSymbolID(signature, st);
  // r->fid = fid;
  r->rt = rt;
  r->loc = location;

  assert(r->sid > 0);

  // m_st.push_back(r);

  if (st == ST_Macro) {
    if (rt == RT_Definition) {
      for (const auto &iter : globalDefTable) {
        if (iter->sid == r->sid && iter->loc == r->loc) {
          // llvm::errs() << "SAME MACRO REF DEF!@" + signature << " , " <<
          // r->loc
          //              << " , " << this_filepath + "\n";
          // llvm::errs() << "old rt" << r->rt << " ,new rt, " << iter->rt <<
          // "\n";
          return;
        }
      }
      globalDefTable.push_back(r);

    } else {
      for (const auto &iter : refTable) {
        if (iter->sid == r->sid && iter->loc == r->loc) {
          // llvm::errs() << "SAME MACRO REF REF!@" + signature << " , " <<
          // r->loc
          //              << " , " << this_filepath + "\n";
          // llvm::errs() << "old rt" << r->rt << " ,new rt, " << iter->rt <<
          // "\n";
          return;
        }
      }
      refTable.push_back(r);
    }
  } else if (st == ST_Path) {
    if (rt == RT_Included) {
      for (const auto &iter : refTable) {
        if (iter->sid == r->sid && iter->loc == r->loc) {
          // llvm::errs() << "SAME MACRO REF REF!@" + signature << " , " <<
          // r->loc
          //              << " , " << this_filepath + "\n";
          // llvm::errs() << "old rt" << r->rt << " ,new rt, " << iter->rt <<
          // "\n";
          return;
        }
      }
      refTable.push_back(r);
    }
  }

  else {
    llvm::errs()
        << "INTERNAL ERROR! marco addRef() used when st type invalid!@" +
               signature + "\n";
    // exit(1);
    return;
  }

  // std::cout << "insert macro REF ok @" << r->sid << "|" << r->loc << "|"
  //           << r->rt << std::endl;
}

void SymbolTable::addSymbolDependency(
    SDependencyType Sdt,  unsigned Ssid,unsigned Esid, clang::FileID Sfid,
    clang::FileID Efid, std::string Slocstring, std::string Elocstring,
    clang::FileID Ofid, std::string Olocstring) {
  assert(sdt >= 0 && sdt <= SDT_Max);
  assert(st >= 0 && st <= ST_Max);
  assert(nd != nullptr);
  SDependency *sd = new SDependency;
  sd->sdt = Sdt;

  sd->ssid = Ssid;
  sd->esid = Esid;

  sd->sfid = Sfid;
  sd->efid = Efid;

  sd->sloc = Slocstring;
  sd->eloc = Elocstring;

  sd->Occurrencefid = Ofid;
  sd->oloc = Olocstring;

  for (const auto &iter : symbolDependencyTable) {
    if (iter->sdt == sd->sdt && iter->ssid == sd->ssid &&
        iter->esid == sd->esid && iter->sfid == sd->sfid &&
        iter->efid == sd->efid && iter->sloc == sd->sloc &&
        iter->eloc == sd->eloc && iter->Occurrencefid == sd->Occurrencefid &&
        iter->oloc == sd->oloc) {
      return;
    }
  }
  symbolDependencyTable.push_back(sd);
}

// for macro symbol
unsigned SymbolTable::addSymbol(std::string signature, SymbolType st) {

  unsigned Newsid = symbolIDTable.size() + 1;

  Symbol *S = new Symbol;
  S->signature = signature;
  S->st = st;
  S->name = signature.substr(1); // take off '#' for name!
  symbolIDTable[signature] = Newsid;
  symbolTable.push_back(S);

  assert(getSymbol(Newsid) == s);
  return Newsid;
}
// for AST symbol
unsigned SymbolTable::addSymbol(clang::NamedDecl *nd, std::string signature,
                                SymbolType st) {

  unsigned newsid = symbolIDTable.size() + 1;

  Symbol *s = new Symbol;
  s->signature = signature;
  s->st = st;
  s->name = nd->getNameAsString();

  // if the symbol is anonymous(e.g. typedef struct{} aaa),name is empty;
  if (s->name.empty()) {
    // std::cout<<"empty name!!!"<<signature<<std::endl;
    // s->name = nd->getQualifiedNameAsString ();

    std::cout << s->name << std::endl;
  }

  symbolIDTable[signature] = newsid;
  symbolTable.push_back(s);

  symbolIDCache[nd] = newsid; // only for AST symbol

  assert(getSymbol(newsid) == s);
  /*
    Symbol *tmp = getSymbol(newsid);

    llvm::errs() << "insert ast symbol@" << tmp->signature << "|symboltype:|"
                 << tmp->st << "|sid:|" << newsid << "\n";
    llvm::errs() << "2insert ast symbol@" << s->signature << "|symboltype:|"
                 << s->st << "|sid:|" << newsid << "\n";
  */
  return newsid;
}

// for macro symbol
unsigned SymbolTable::getSymbolID(std::string signature, SymbolType st) {

  // symbolsig already exists
  const std::unordered_map<std::string, unsigned>::iterator &itr =
      symbolIDTable.find(signature);
  if (itr != symbolIDTable.end()) {
    unsigned sid = itr->second;

    assert(st == getSymbol(sid)->st);

    return sid;
  }

  return addSymbol(signature, st);
}

// for AST symbol
unsigned SymbolTable::getSymbolID(clang::NamedDecl *nd, SymbolType st) {
  const std::unordered_map<clang::NamedDecl *, unsigned>::iterator &res =
      symbolIDCache.find(nd);
  if (res != symbolIDCache.end()) {
    unsigned sid = res->second;
    Symbol *ts = getSymbol(sid);

    if (st != ts->st) {

      if (st != ST_Max && ts->st != ST_Max) {
        return 0;
      }
      if (ts->st == ST_Max) {
        ts->st = st;
      }

      // if(symbolTable[sid]->st == ST_Max) symbolTable[sid]->st = st;
    }

    return sid;
  }

  // doesn't exist,then gen sig, insert 1)snametable,2)sidtable and 3)sidcache!
  std::string symbolsignature = "";
  DASignatureGenerator generator(nd, symbolsignature);
  generator.Visit(nd);
  if (generator.error()) {
    llvm::errs() << "INTERNAL ERROR!\nSIGNATURE GENNERATOR ERROR!\n@" +
                        nd->getNameAsString() + "\n";
    exit(1);
  };

  // llvm::errs() << "sig gen ok!\n Is" + symbolsignature + "\n";

  // symbolsig already exists
  const std::unordered_map<std::string, unsigned>::iterator &itr =
      symbolIDTable.find(symbolsignature);
  if (itr != symbolIDTable.end()) {
    unsigned sid = itr->second;

    Symbol *ts = getSymbol(sid);

    if (st != ts->st) {

      if (st != ST_Max && ts->st != ST_Max) {
        // FIXME
        llvm::errs()
            << "INTERNAL ERROR!\n same symbol has different types:\nsig1:"
            << ts->signature << " sig2:" << symbolTable[sid - 1]->signature
            << "\n";
        llvm::errs() << "\n1):" << st << " \n2):" << ts->st << "\n";
        nd->dump();
        // exit(1);
      } else if (ts->st == ST_Max) {
        ts->st = st;
      }

      // if(symbolTable[sid]->st == ST_Max) symbolTable[sid]->st = st;
    }

    symbolIDCache[nd] = sid;
    return sid;
  }

  return addSymbol(nd, symbolsignature, st);
}

// for AST symbol
unsigned SymbolTable::getSymbolID(clang::NamedDecl *nd) {

  const std::unordered_map<clang::NamedDecl *, unsigned>::iterator &res =
      symbolIDCache.find(nd);
  if (res != symbolIDCache.end()) {
    unsigned sid = res->second;
    // Symbol *ts = getSymbol(sid);
    return sid;
  }

  return getSymbolID(nd, ST_Max);
  // we assume type symbol must have been created before!!!!

  // llvm::errs() << "INTERNAL ERROR!\nType symbol missing!\n@" +
  //                       td->getNameAsString() + "\n";
  // exit(1);
}

SymbolTable::~SymbolTable() {

  for (auto itr : symbolTable)
    delete itr;

  for (auto itr : globalDefTable)
    delete itr;

  for (auto itr : globalDecTable)
    delete itr;
}

void SymbolTable::finilize(std::string jsonfilepath, DAFileManager *fm) {

  std::ofstream log;
  log.open(jsonfilepath, std::ios::out | std::ios::app);

  if (!log.good()) {
    llvm::outs() << "symboltable json file write fail" << jsonfilepath << "\n";
    exit(1);
  }

  // symboltable
  log << "\"symboltable\" : [" << std::endl;

  int n = symbolTable.size();

  for (int i = 0; i < n; i++) {

    Symbol *s = symbolTable[i];

    log << "{" << std::endl;
    log << "  \"sid\" : " << i + 1 << "," << std::endl;
    log << "  \"signature\" : \"" << s->signature << "\"," << std::endl;
    log << "  \"symboltype\" : " << s->st << "," << std::endl;
    log << "  \"name\" : \"" << s->name << "\"" << std::endl;

    if (i == n - 1)
      log << "}" << std::endl;
    else
      log << "}," << std::endl;
  }

  log << "]," << std::endl;
  // llvm::errs()<<"dump symboltable ok!\n";

  // symbolDependencyTable

  n = symbolDependencyTable.size();
  log << "\"symbolDependencyTable\" : [" << std::endl;
  for (int i = 0; i < n; i++) {
    SDependency *sd = symbolDependencyTable[i];
    assert(sd != nullptr);
    std::string sfilepath = fm->getFilePath(sd->sfid);
    std::string efilepath = fm->getFilePath(sd->efid);
    if (sfilepath.empty()||efilepath.empty()) {
      llvm::errs() << "INTERNAL ERROR!can't get filepath by efid\n";
      continue;
    }
    log << "{" << std::endl;
    log << "  \"SDependencyType\" : " << sd->sdt << "," << std::endl;

    log << "  \"ssid\" : " << sd->ssid << "," << std::endl;
    log << "  \"esid\" : " << sd->esid << "," << std::endl;

    log << "  \"sfilepath\" : \"" << sfilepath << "\"," << std::endl;
    log << "  \"efilepath\" : \"" << efilepath << "\"," << std::endl;

    log << "  \"sloc\" : \"" << sd->sloc << "\"," << std::endl;
    log << "  \"eloc\" : \"" << sd->eloc << "\"," << std::endl;

    log << "  \"OccurrenceFilePath\" : \"" << fm->getFilePath(sd->Occurrencefid)
        << "\"," << std::endl;

    log << "  \"OccurrenceLoc\" : \"" << sd->oloc<< "\"" << std::endl;

    if (i == n - 1)
      log << "}" << std::endl;
    else
      log << "}," << std::endl;
  }

  log << "]," << std::endl;
  // llvm::errs()<<"dump symbolDependencyTable ok!\n";

  // globaldeftable

  n = globalDefTable.size();
  log << "\"globaldeftable\" : [" << std::endl;

  for (int i = 0; i < n; i++) {

    Ref *r = globalDefTable[i];
    assert(r != nullptr);

#if 0
    std::string locpath = fm->getFilePath(r->fid);  
		if(locpath != mainfilepath) {
			llvm::outs()<<"loc:"+ locpath + "\nmainfile:" + mainfilepath + "\n";
			//continue;//FIXME
		}
#endif

    log << "{" << std::endl;
    log << "  \"sid\" : " << r->sid << "," << std::endl;
    // log << "  \"fid\" : " << r->fid << "," << std::endl;
    log << "  \"reftype\" : " << r->rt << "," << std::endl;
    log << "  \"location\" : \"" << r->loc << "\"" << std::endl;

    if (i == n - 1)
      log << "}" << std::endl;
    else
      log << "}," << std::endl;
  }

  log << "]," << std::endl;
  // llvm::errs()<<"dump globaldeftable ok!\n";

  // globaldectable
  n = globalDecTable.size();
  log << "\"globaldectable\" : [" << std::endl;

  for (int i = 0; i < n; i++) {

    Ref *r = globalDecTable[i];

#if 0
    std::string locpath = fm->getFilePath(r->fid);
		if(locpath != mainfilepath) {
			llvm::outs()<<"loc:"+ locpath + "\nmainfile:" + mainfilepath + "\n";
			//continue;//FIXME
		}
#endif

    log << "{" << std::endl;
    log << "  \"sid\" : " << r->sid << "," << std::endl;
    // log << "  \"fid\" : " << r->fid << "," << std::endl;
    log << "  \"reftype\" : " << r->rt << "," << std::endl;
    log << "  \"location\" : \"" << r->loc << "\"" << std::endl;

    if (i == n - 1)
      log << "}" << std::endl;
    else
      log << "}," << std::endl;
  }

  log << "]," << std::endl;
  // llvm::errs()<<"dump globaldectable ok!\n";

  // reftable
  n = refTable.size();
  log << "\"reftable\" : [" << std::endl;

  for (int i = 0; i < n; i++) {

    Ref *r = refTable[i];

#if 0
    std::string locpath = fm->getFilePath(r->fid);
		if(locpath != mainfilepath) {
			llvm::outs()<<"loc:"+ locpath + "\nmainfile:" + mainfilepath + "\n";
			//continue;//FIXME
		}
#endif

    log << "{" << std::endl;
    log << "  \"sid\" : " << r->sid << "," << std::endl;
    // log << "  \"fid\" : " << r->fid << "," << std::endl;
    log << "  \"reftype\" : " << r->rt << "," << std::endl;
    log << "  \"location\" : \"" << r->loc << "\"" << std::endl;

    if (i == n - 1)
      log << "}" << std::endl;
    else
      log << "}," << std::endl;
    log.flush();
  }

  log << "]" << std::endl;
  // llvm::errs()<<"dump reftable ok!\n";

  log.flush();
  log.close();
  // llvm::errs() << "symboltable finalize ok\n";
}
