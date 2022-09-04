/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#ifndef DA_SYMBOLTABLE_H
#define DA_SYMBOLTABLE_H

#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "DAFileManager.h"
#include "DASignatureGenerator.h"
#include "DAType.h"
#include "clang/Tooling/Tooling.h"

class DAFileManager;

class SymbolTable {
public:
  struct Symbol {
    std::string signature;
    SymbolType st;
    std::string name;
  };

  struct Ref {
    unsigned sid;
    // unsigned fid;
    RefType rt;
    std::string loc;
  };

  // start symbol must in current FileManager's file
  struct SDependency {
    SDependencyType sdt;

    unsigned ssid;
    unsigned esid;

    clang::FileID sfid;
    clang::FileID efid;


    std::string sloc;
    std::string eloc;

    clang::FileID Occurrencefid;
    std::string oloc;
  };

  SymbolTable(std::string filepath) : this_filepath(filepath) {}

  ~SymbolTable();

  int size();
  // AST symbol use
  void addRef(clang::NamedDecl *nd, SymbolType st, RefType rt,
              std::string location);
  // macro symbol use
  void addRef(std::string signature, SymbolType st, RefType rt,
              std::string location);

  void addSymbolDependency(SDependencyType Sdt,
                           unsigned Ssid,
                           unsigned Esid,
                           clang::FileID Sfid,
                           clang::FileID Efid,
                           std::string Slocstring,
                           std::string Elocstring,
                           clang::FileID Ofid,
                           std::string Olocstring);

  void finilize(std::string jsonfilepath, DAFileManager *fm);

  // AST symbol use, when adding symbol dep
  unsigned getSymbolID(clang::NamedDecl *nd);

  std::string getSymbolTableFilePath() { return this_filepath; }

private:
  Symbol *getSymbol(unsigned sid) {
    int index = sid - 1;
    int n = size();
    if (index >= n) {
      llvm::errs() << "INTERNAL ERROR! invalid sid@" << sid << "\n";
      exit(1);
    }

    return symbolTable[index];
  }

  // AST symbol use
  unsigned getSymbolID(clang::NamedDecl *nd, SymbolType st);
  unsigned addSymbol(clang::NamedDecl *nd, std::string signature,
                     SymbolType st);

  // macro symbol use
  unsigned getSymbolID(std::string signature, SymbolType st);
  unsigned addSymbol(std::string signature, SymbolType st);

  std::string this_filepath; // used in finilize()

  std::vector<Symbol *> symbolTable; // sid from 1,equals index+1
  std::unordered_map<std::string, unsigned>
      symbolIDTable; // sid from 1,indexed by symbol sig
  std::unordered_map<clang::NamedDecl *, unsigned>
      symbolIDCache; // only use in one TU process!can have muitiple nd points
                     // to one sid

  std::vector<Ref *> globalDefTable;
  std::vector<Ref *> globalDecTable;
  std::vector<Ref *> refTable;
  std::vector<SDependency *> symbolDependencyTable;
  std::unordered_map<std::string, std::string> decDefTable; // TODO
};

#endif
