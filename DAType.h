/*************************************************
 * HISTORY:                                      *
 *	11/22/2020 pyx : Created.                *
 *	06/01/2022 pyx : Edited.                 *
=================================================*/

#ifndef DA_TYPE_H
#define DA_TYPE_H

enum RefType : int {
  RT_Declaration,
  RT_DefinedTest, // macro
  RT_Definition,  // macro
  RT_Expansion,   // macro
  RT_Included,    // macro
  RT_Reference,
  RT_Undefinition, // macro
  RT_Max
};

enum SymbolType : int {
  ST_Enum,
  ST_Enumerator,
  ST_Field, // struct/union/class member
  ST_Function,
  ST_GlobalVariable,
  ST_Macro,
  ST_Parameter,
  ST_Path,
  ST_Tag,
  ST_Typedef,
  ST_Max
};

enum SDependencyType : int {
  SDT_Function_Function, // exclude static function call static function
  SDT_RetType_Function,//function -> return type
  SDT_ParamType_Function,//function -> params type
  SDT_Typedef_GlobalVar,//GlobalVar -> Typedef type
  SDT_FieldType_Tag,//Tag -> Field
  SDT_Tag_Typedef,//Typedef->Tag
  SDT_GlobalVar_GlobalVar, // not static var
  SDT_GlobalVar_Function, // function needs gv, because func call uses a gv as
  SDT_Max
};

#endif // DA_TYPE_H
