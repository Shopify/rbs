#include "rbs_extension.h"

VALUE RBS_TypeName;

VALUE RBS_Types_Alias;
VALUE RBS_Types_Bases_Any;
VALUE RBS_Types_Bases_Bool;
VALUE RBS_Types_Bases_Bottom;
VALUE RBS_Types_Bases_Class;
VALUE RBS_Types_Bases_Instance;
VALUE RBS_Types_Bases_Nil;
VALUE RBS_Types_Bases_Self;
VALUE RBS_Types_Bases_Top;
VALUE RBS_Types_Bases_Void;
VALUE RBS_Types_Block;
VALUE RBS_Types_ClassInstance;
VALUE RBS_Types_ClassSingleton;
VALUE RBS_Types_Function_Param;
VALUE RBS_Types_Function;
VALUE RBS_Types_UntypedFunction;
VALUE RBS_Types_Interface;
VALUE RBS_Types_Intersection;
VALUE RBS_Types_Literal;
VALUE RBS_Types_Optional;
VALUE RBS_Types_Proc;
VALUE RBS_Types_Record;
VALUE RBS_Types_Tuple;
VALUE RBS_Types_Union;
VALUE RBS_Types_Variable;

#define IMPORT_CONSTANT(var, parent, name) { var = rb_const_get(parent, rb_intern(name)); rb_gc_register_mark_object(var); }

void rbs__init_constants(void) {
  IMPORT_CONSTANT(RBS_TypeName, RBS, "TypeName");
  IMPORT_CONSTANT(RBS_Types_Alias, RBS_Types, "Alias");
  IMPORT_CONSTANT(RBS_Types_Bases_Any, RBS_Types_Bases, "Any");
  IMPORT_CONSTANT(RBS_Types_Bases_Bool, RBS_Types_Bases, "Bool");
  IMPORT_CONSTANT(RBS_Types_Bases_Bottom, RBS_Types_Bases, "Bottom");
  IMPORT_CONSTANT(RBS_Types_Bases_Class, RBS_Types_Bases, "Class");
  IMPORT_CONSTANT(RBS_Types_Bases_Instance, RBS_Types_Bases, "Instance");
  IMPORT_CONSTANT(RBS_Types_Bases_Nil, RBS_Types_Bases, "Nil");
  IMPORT_CONSTANT(RBS_Types_Bases_Self, RBS_Types_Bases, "Self");
  IMPORT_CONSTANT(RBS_Types_Bases_Top, RBS_Types_Bases, "Top");
  IMPORT_CONSTANT(RBS_Types_Bases_Void, RBS_Types_Bases, "Void");
  IMPORT_CONSTANT(RBS_Types_Block, RBS_Types, "Block");
  IMPORT_CONSTANT(RBS_Types_ClassInstance, RBS_Types, "ClassInstance");
  IMPORT_CONSTANT(RBS_Types_ClassSingleton, RBS_Types, "ClassSingleton");
  IMPORT_CONSTANT(RBS_Types_Function, RBS_Types, "Function");
  IMPORT_CONSTANT(RBS_Types_UntypedFunction, RBS_Types, "UntypedFunction");
  IMPORT_CONSTANT(RBS_Types_Function_Param, RBS_Types_Function, "Param");
  IMPORT_CONSTANT(RBS_Types_Interface, RBS_Types, "Interface");
  IMPORT_CONSTANT(RBS_Types_Intersection, RBS_Types, "Intersection");
  IMPORT_CONSTANT(RBS_Types_Literal, RBS_Types, "Literal");
  IMPORT_CONSTANT(RBS_Types_Optional, RBS_Types, "Optional");
  IMPORT_CONSTANT(RBS_Types_Proc, RBS_Types, "Proc");
  IMPORT_CONSTANT(RBS_Types_Record, RBS_Types, "Record");
  IMPORT_CONSTANT(RBS_Types_Tuple, RBS_Types, "Tuple");
  IMPORT_CONSTANT(RBS_Types_Union, RBS_Types, "Union");
  IMPORT_CONSTANT(RBS_Types_Variable, RBS_Types, "Variable");
}
