#ifndef RBS__CONSTANTS_H
#define RBS__CONSTANTS_H

extern VALUE RBS_TypeName;

extern VALUE RBS_Types_Alias;
extern VALUE RBS_Types_Bases;
extern VALUE RBS_Types_Bases_Any;
extern VALUE RBS_Types_Bases_Bool;
extern VALUE RBS_Types_Bases_Bottom;
extern VALUE RBS_Types_Bases_Class;
extern VALUE RBS_Types_Bases_Instance;
extern VALUE RBS_Types_Bases_Nil;
extern VALUE RBS_Types_Bases_Self;
extern VALUE RBS_Types_Bases_Top;
extern VALUE RBS_Types_Bases_Void;
extern VALUE RBS_Types_Block;
extern VALUE RBS_Types_ClassInstance;
extern VALUE RBS_Types_ClassSingleton;
extern VALUE RBS_Types_Function_Param;
extern VALUE RBS_Types_Function;
extern VALUE RBS_Types_UntypedFunction;
extern VALUE RBS_Types_Interface;
extern VALUE RBS_Types_Intersection;
extern VALUE RBS_Types_Literal;
extern VALUE RBS_Types_Optional;
extern VALUE RBS_Types_Proc;
extern VALUE RBS_Types_Record;
extern VALUE RBS_Types_Tuple;
extern VALUE RBS_Types_Union;
extern VALUE RBS_Types_Variable;

void rbs__init_constants();

#endif
