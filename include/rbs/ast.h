/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/include/rbs/ast.h.erb                                            */
/*----------------------------------------------------------------------------*/

#ifndef RBS__AST_H
#define RBS__AST_H

#include "ruby.h"
#include "rbs/util/rbs_allocator.h"

enum rbs_node_type {
    RBS_AST_ANNOTATION = 1,
    RBS_AST_COMMENT = 2,
    RBS_AST_DECLARATIONS_CLASS = 3,
    RBS_AST_DECLARATIONS_CLASS_SUPER = 4,
    RBS_AST_DECLARATIONS_CLASSALIAS = 5,
    RBS_AST_DECLARATIONS_CONSTANT = 6,
    RBS_AST_DECLARATIONS_GLOBAL = 7,
    RBS_AST_DECLARATIONS_INTERFACE = 8,
    RBS_AST_DECLARATIONS_MODULE = 9,
    RBS_AST_DECLARATIONS_MODULE_SELF = 10,
    RBS_AST_DECLARATIONS_MODULEALIAS = 11,
    RBS_AST_DECLARATIONS_TYPEALIAS = 12,
    RBS_AST_DIRECTIVES_USE = 13,
    RBS_AST_DIRECTIVES_USE_SINGLECLAUSE = 14,
    RBS_AST_DIRECTIVES_USE_WILDCARDCLAUSE = 15,
    RBS_AST_MEMBERS_ALIAS = 16,
    RBS_AST_MEMBERS_ATTRACCESSOR = 17,
    RBS_AST_MEMBERS_ATTRREADER = 18,
    RBS_AST_MEMBERS_ATTRWRITER = 19,
    RBS_AST_MEMBERS_CLASSINSTANCEVARIABLE = 20,
    RBS_AST_MEMBERS_CLASSVARIABLE = 21,
    RBS_AST_MEMBERS_EXTEND = 22,
    RBS_AST_MEMBERS_INCLUDE = 23,
    RBS_AST_MEMBERS_INSTANCEVARIABLE = 24,
    RBS_AST_MEMBERS_METHODDEFINITION = 25,
    RBS_AST_MEMBERS_METHODDEFINITION_OVERLOAD = 26,
    RBS_AST_MEMBERS_PREPEND = 27,
    RBS_AST_MEMBERS_PRIVATE = 28,
    RBS_AST_MEMBERS_PUBLIC = 29,
    RBS_AST_SYMBOL = 30,
    RBS_AST_TYPEPARAM = 31,
    RBS_METHODTYPE = 32,
    RBS_NAMESPACE = 33,
    RBS_TYPENAME = 34,
    RBS_TYPES_ALIAS = 35,
    RBS_TYPES_BASES_ANY = 36,
    RBS_TYPES_BASES_BOOL = 37,
    RBS_TYPES_BASES_BOTTOM = 38,
    RBS_TYPES_BASES_CLASS = 39,
    RBS_TYPES_BASES_INSTANCE = 40,
    RBS_TYPES_BASES_NIL = 41,
    RBS_TYPES_BASES_SELF = 42,
    RBS_TYPES_BASES_TOP = 43,
    RBS_TYPES_BASES_VOID = 44,
    RBS_TYPES_BLOCK = 45,
    RBS_TYPES_CLASSINSTANCE = 46,
    RBS_TYPES_CLASSSINGLETON = 47,
    RBS_TYPES_FUNCTION = 48,
    RBS_TYPES_FUNCTION_PARAM = 49,
    RBS_TYPES_INTERFACE = 50,
    RBS_TYPES_INTERSECTION = 51,
    RBS_TYPES_LITERAL = 52,
    RBS_TYPES_OPTIONAL = 53,
    RBS_TYPES_PROC = 54,
    RBS_TYPES_RECORD = 55,
    RBS_TYPES_TUPLE = 56,
    RBS_TYPES_UNION = 57,
    RBS_TYPES_UNTYPEDFUNCTION = 58,
    RBS_TYPES_VARIABLE = 59,
};

typedef struct rbs_node {
    VALUE cached_ruby_value;
    enum rbs_node_type type;
} rbs_node_t;

typedef struct rbs_node_list_node {
    rbs_node_t *node;
    struct rbs_node_list_node *next;
} rbs_node_list_node_t;

typedef struct rbs_node_list {
    rbs_allocator_t *allocator;
    rbs_node_list_node_t *head;
    rbs_node_list_node_t *tail;
    size_t length;
    VALUE cached_ruby_value;
} rbs_node_list_t;

rbs_node_list_t* rbs_node_list_new(rbs_allocator_t *);

void rbs_node_list_append(rbs_node_list_t *list, rbs_node_t *node);

typedef struct {
    rbs_node_t base;

    VALUE string;
    VALUE location;
} rbs_ast_annotation_t;

typedef struct {
    rbs_node_t base;

    VALUE string;
    VALUE location;
} rbs_ast_comment_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type_params;
    VALUE super_class;
    VALUE members;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_class_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE location;
} rbs_ast_declarations_class_super_t;

typedef struct {
    rbs_node_t base;

    VALUE new_name;
    VALUE old_name;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_classalias_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_constant_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_global_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type_params;
    VALUE members;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_interface_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type_params;
    VALUE self_types;
    VALUE members;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_module_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE location;
} rbs_ast_declarations_module_self_t;

typedef struct {
    rbs_node_t base;

    VALUE new_name;
    VALUE old_name;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_modulealias_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type_params;
    VALUE type;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_declarations_typealias_t;

typedef struct {
    rbs_node_t base;

    VALUE clauses;
    VALUE location;
} rbs_ast_directives_use_t;

typedef struct {
    rbs_node_t base;

    VALUE type_name;
    VALUE new_name;
    VALUE location;
} rbs_ast_directives_use_singleclause_t;

typedef struct {
    rbs_node_t base;

    VALUE namespace;
    VALUE location;
} rbs_ast_directives_use_wildcardclause_t;

typedef struct {
    rbs_node_t base;

    VALUE new_name;
    VALUE old_name;
    VALUE kind;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_members_alias_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE ivar_name;
    VALUE kind;
    VALUE annotations;
    VALUE location;
    VALUE comment;
    VALUE visibility;
} rbs_ast_members_attraccessor_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE ivar_name;
    VALUE kind;
    VALUE annotations;
    VALUE location;
    VALUE comment;
    VALUE visibility;
} rbs_ast_members_attrreader_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE ivar_name;
    VALUE kind;
    VALUE annotations;
    VALUE location;
    VALUE comment;
    VALUE visibility;
} rbs_ast_members_attrwriter_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE location;
    VALUE comment;
} rbs_ast_members_classinstancevariable_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE location;
    VALUE comment;
} rbs_ast_members_classvariable_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_members_extend_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_members_include_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE type;
    VALUE location;
    VALUE comment;
} rbs_ast_members_instancevariable_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE kind;
    VALUE overloads;
    VALUE annotations;
    VALUE location;
    VALUE comment;
    VALUE overloading;
    VALUE visibility;
} rbs_ast_members_methoddefinition_t;

typedef struct {
    rbs_node_t base;

    VALUE annotations;
    VALUE method_type;
} rbs_ast_members_methoddefinition_overload_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE annotations;
    VALUE location;
    VALUE comment;
} rbs_ast_members_prepend_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_ast_members_private_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_ast_members_public_t;

typedef struct {
    rbs_node_t base;

} rbs_ast_symbol_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE variance;
    VALUE upper_bound;
    VALUE default_type;
    VALUE unchecked;
    VALUE location;
} rbs_ast_typeparam_t;

typedef struct {
    rbs_node_t base;

    VALUE type_params;
    VALUE type;
    VALUE block;
    VALUE location;
} rbs_methodtype_t;

typedef struct {
    rbs_node_t base;

    VALUE path;
    VALUE absolute;
} rbs_namespace_t;

typedef struct {
    rbs_node_t base;

    VALUE namespace;
    VALUE name;
} rbs_typename_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE location;
} rbs_types_alias_t;

typedef struct {
    rbs_node_t base;

    VALUE todo;
    VALUE location;
} rbs_types_bases_any_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_bool_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_bottom_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_class_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_instance_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_nil_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_self_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_top_t;

typedef struct {
    rbs_node_t base;

    VALUE location;
} rbs_types_bases_void_t;

typedef struct {
    rbs_node_t base;

    VALUE type;
    VALUE required;
    VALUE self_type;
} rbs_types_block_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE location;
} rbs_types_classinstance_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE location;
} rbs_types_classsingleton_t;

typedef struct {
    rbs_node_t base;

    VALUE required_positionals;
    VALUE optional_positionals;
    VALUE rest_positionals;
    VALUE trailing_positionals;
    VALUE required_keywords;
    VALUE optional_keywords;
    VALUE rest_keywords;
    VALUE return_type;
} rbs_types_function_t;

typedef struct {
    rbs_node_t base;

    VALUE type;
    VALUE name;
    VALUE location;
} rbs_types_function_param_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE args;
    VALUE location;
} rbs_types_interface_t;

typedef struct {
    rbs_node_t base;

    VALUE types;
    VALUE location;
} rbs_types_intersection_t;

typedef struct {
    rbs_node_t base;

    VALUE literal;
    VALUE location;
} rbs_types_literal_t;

typedef struct {
    rbs_node_t base;

    VALUE type;
    VALUE location;
} rbs_types_optional_t;

typedef struct {
    rbs_node_t base;

    VALUE type;
    VALUE block;
    VALUE location;
    VALUE self_type;
} rbs_types_proc_t;

typedef struct {
    rbs_node_t base;

    VALUE all_fields;
    VALUE location;
} rbs_types_record_t;

typedef struct {
    rbs_node_t base;

    VALUE types;
    VALUE location;
} rbs_types_tuple_t;

typedef struct {
    rbs_node_t base;

    VALUE types;
    VALUE location;
} rbs_types_union_t;

typedef struct {
    rbs_node_t base;

    VALUE return_type;
} rbs_types_untypedfunction_t;

typedef struct {
    rbs_node_t base;

    VALUE name;
    VALUE location;
} rbs_types_variable_t;


rbs_ast_annotation_t *rbs_ast_annotation_new(rbs_allocator_t *allocator, VALUE string, VALUE location);
rbs_ast_comment_t *rbs_ast_comment_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE string, VALUE location);
rbs_ast_declarations_class_t *rbs_ast_declarations_class_new(rbs_allocator_t *allocator, VALUE name, rbs_node_list_t *type_params, VALUE super_class, rbs_node_list_t *members, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_declarations_class_super_t *rbs_ast_declarations_class_super_new(rbs_allocator_t *allocator, VALUE name, VALUE args, VALUE location);
rbs_ast_declarations_classalias_t *rbs_ast_declarations_classalias_new(rbs_allocator_t *allocator, rbs_typename_t *new_name, rbs_typename_t *old_name, VALUE location, VALUE comment);
rbs_ast_declarations_constant_t *rbs_ast_declarations_constant_new(rbs_allocator_t *allocator, rbs_typename_t *name, rbs_node_t *type, VALUE location, VALUE comment);
rbs_ast_declarations_global_t *rbs_ast_declarations_global_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE location, VALUE comment);
rbs_ast_declarations_interface_t *rbs_ast_declarations_interface_new(rbs_allocator_t *allocator, rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *members, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_declarations_module_t *rbs_ast_declarations_module_new(rbs_allocator_t *allocator, VALUE name, rbs_node_list_t *type_params, VALUE self_types, rbs_node_list_t *members, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_declarations_module_self_t *rbs_ast_declarations_module_self_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE name, VALUE args, VALUE location);
rbs_ast_declarations_modulealias_t *rbs_ast_declarations_modulealias_new(rbs_allocator_t *allocator, rbs_typename_t *new_name, rbs_typename_t *old_name, VALUE location, VALUE comment);
rbs_ast_declarations_typealias_t *rbs_ast_declarations_typealias_new(rbs_allocator_t *allocator, rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_t *type, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_directives_use_t *rbs_ast_directives_use_new(rbs_allocator_t *allocator, VALUE clauses, VALUE location);
rbs_ast_directives_use_singleclause_t *rbs_ast_directives_use_singleclause_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE type_name, VALUE new_name, VALUE location);
rbs_ast_directives_use_wildcardclause_t *rbs_ast_directives_use_wildcardclause_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE namespace, VALUE location);
rbs_ast_members_alias_t *rbs_ast_members_alias_new(rbs_allocator_t *allocator, VALUE new_name, VALUE old_name, VALUE kind, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_members_attraccessor_t *rbs_ast_members_attraccessor_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE ivar_name, VALUE kind, rbs_node_list_t *annotations, VALUE location, VALUE comment, VALUE visibility);
rbs_ast_members_attrreader_t *rbs_ast_members_attrreader_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE ivar_name, VALUE kind, rbs_node_list_t *annotations, VALUE location, VALUE comment, VALUE visibility);
rbs_ast_members_attrwriter_t *rbs_ast_members_attrwriter_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE ivar_name, VALUE kind, rbs_node_list_t *annotations, VALUE location, VALUE comment, VALUE visibility);
rbs_ast_members_classinstancevariable_t *rbs_ast_members_classinstancevariable_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE location, VALUE comment);
rbs_ast_members_classvariable_t *rbs_ast_members_classvariable_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE location, VALUE comment);
rbs_ast_members_extend_t *rbs_ast_members_extend_new(rbs_allocator_t *allocator, VALUE name, VALUE args, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_members_include_t *rbs_ast_members_include_new(rbs_allocator_t *allocator, VALUE name, VALUE args, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_members_instancevariable_t *rbs_ast_members_instancevariable_new(rbs_allocator_t *allocator, VALUE name, rbs_node_t *type, VALUE location, VALUE comment);
rbs_ast_members_methoddefinition_t *rbs_ast_members_methoddefinition_new(rbs_allocator_t *allocator, VALUE name, VALUE kind, VALUE overloads, rbs_node_list_t *annotations, VALUE location, VALUE comment, VALUE overloading, VALUE visibility);
rbs_ast_members_methoddefinition_overload_t *rbs_ast_members_methoddefinition_overload_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE annotations, VALUE method_type);
rbs_ast_members_prepend_t *rbs_ast_members_prepend_new(rbs_allocator_t *allocator, VALUE name, VALUE args, rbs_node_list_t *annotations, VALUE location, VALUE comment);
rbs_ast_members_private_t *rbs_ast_members_private_new(rbs_allocator_t *allocator, VALUE location);
rbs_ast_members_public_t *rbs_ast_members_public_new(rbs_allocator_t *allocator, VALUE location);
rbs_ast_symbol_t *rbs_ast_symbol_new(rbs_allocator_t *allocator, VALUE ruby_value);
rbs_ast_typeparam_t *rbs_ast_typeparam_new(rbs_allocator_t *allocator, VALUE name, VALUE variance, VALUE upper_bound, VALUE default_type, VALUE unchecked, VALUE location);
rbs_methodtype_t *rbs_methodtype_new(rbs_allocator_t *allocator, rbs_node_list_t *type_params, VALUE type, VALUE block, VALUE location);
rbs_namespace_t *rbs_namespace_new(rbs_allocator_t *allocator, VALUE path, VALUE absolute);
rbs_typename_t *rbs_typename_new(rbs_allocator_t *allocator, VALUE namespace, VALUE name);
rbs_types_alias_t *rbs_types_alias_new(rbs_allocator_t *allocator, rbs_typename_t *name, VALUE args, VALUE location);
rbs_types_bases_any_t *rbs_types_bases_any_new(rbs_allocator_t *allocator, VALUE todo, VALUE location);
rbs_types_bases_bool_t *rbs_types_bases_bool_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_bottom_t *rbs_types_bases_bottom_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_class_t *rbs_types_bases_class_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_instance_t *rbs_types_bases_instance_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_nil_t *rbs_types_bases_nil_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_self_t *rbs_types_bases_self_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_top_t *rbs_types_bases_top_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_bases_void_t *rbs_types_bases_void_new(rbs_allocator_t *allocator, VALUE location);
rbs_types_block_t *rbs_types_block_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE type, VALUE required, VALUE self_type);
rbs_types_classinstance_t *rbs_types_classinstance_new(rbs_allocator_t *allocator, rbs_typename_t *name, VALUE args, VALUE location);
rbs_types_classsingleton_t *rbs_types_classsingleton_new(rbs_allocator_t *allocator, rbs_typename_t *name, VALUE location);
rbs_types_function_t *rbs_types_function_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE required_positionals, VALUE optional_positionals, VALUE rest_positionals, VALUE trailing_positionals, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type);
rbs_types_function_param_t *rbs_types_function_param_new(rbs_allocator_t *allocator, rbs_node_t *type, VALUE name, VALUE location);
rbs_types_interface_t *rbs_types_interface_new(rbs_allocator_t *allocator, rbs_typename_t *name, VALUE args, VALUE location);
rbs_types_intersection_t *rbs_types_intersection_new(rbs_allocator_t *allocator, VALUE types, VALUE location);
rbs_types_literal_t *rbs_types_literal_new(rbs_allocator_t *allocator, VALUE literal, VALUE location);
rbs_types_optional_t *rbs_types_optional_new(rbs_allocator_t *allocator, rbs_node_t *type, VALUE location);
rbs_types_proc_t *rbs_types_proc_new(rbs_allocator_t *allocator, VALUE type, VALUE block, VALUE location, VALUE self_type);
rbs_types_record_t *rbs_types_record_new(rbs_allocator_t *allocator, VALUE all_fields, VALUE location);
rbs_types_tuple_t *rbs_types_tuple_new(rbs_allocator_t *allocator, VALUE types, VALUE location);
rbs_types_union_t *rbs_types_union_new(rbs_allocator_t *allocator, VALUE types, VALUE location);
rbs_types_untypedfunction_t *rbs_types_untypedfunction_new(rbs_allocator_t *allocator, VALUE ruby_value, VALUE return_type);
rbs_types_variable_t *rbs_types_variable_new(rbs_allocator_t *allocator, VALUE name, VALUE location);

VALUE rbs_struct_to_ruby_value(rbs_node_t *instance);

#endif
