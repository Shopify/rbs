/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/include/rbs/ast.h.erb                                            */
/*----------------------------------------------------------------------------*/

#ifndef RBS__AST_H
#define RBS__AST_H

#include "rbs_string.h"
#include "rbs_location.h"
#include "rbs_constant_pool.h"

enum rbs_node_type {
    RBS_AST_ANNOTATION = 1,
    RBS_AST_BOOL = 2,
    RBS_AST_COMMENT = 3,
    RBS_AST_DECLARATIONS_CLASS = 4,
    RBS_AST_DECLARATIONS_CLASS_SUPER = 5,
    RBS_AST_DECLARATIONS_CLASSALIAS = 6,
    RBS_AST_DECLARATIONS_CONSTANT = 7,
    RBS_AST_DECLARATIONS_GLOBAL = 8,
    RBS_AST_DECLARATIONS_INTERFACE = 9,
    RBS_AST_DECLARATIONS_MODULE = 10,
    RBS_AST_DECLARATIONS_MODULE_SELF = 11,
    RBS_AST_DECLARATIONS_MODULEALIAS = 12,
    RBS_AST_DECLARATIONS_TYPEALIAS = 13,
    RBS_AST_DIRECTIVES_USE = 14,
    RBS_AST_DIRECTIVES_USE_SINGLECLAUSE = 15,
    RBS_AST_DIRECTIVES_USE_WILDCARDCLAUSE = 16,
    RBS_AST_INTEGER = 17,
    RBS_AST_MEMBERS_ALIAS = 18,
    RBS_AST_MEMBERS_ATTRACCESSOR = 19,
    RBS_AST_MEMBERS_ATTRREADER = 20,
    RBS_AST_MEMBERS_ATTRWRITER = 21,
    RBS_AST_MEMBERS_CLASSINSTANCEVARIABLE = 22,
    RBS_AST_MEMBERS_CLASSVARIABLE = 23,
    RBS_AST_MEMBERS_EXTEND = 24,
    RBS_AST_MEMBERS_INCLUDE = 25,
    RBS_AST_MEMBERS_INSTANCEVARIABLE = 26,
    RBS_AST_MEMBERS_METHODDEFINITION = 27,
    RBS_AST_MEMBERS_METHODDEFINITION_OVERLOAD = 28,
    RBS_AST_MEMBERS_PREPEND = 29,
    RBS_AST_MEMBERS_PRIVATE = 30,
    RBS_AST_MEMBERS_PUBLIC = 31,
    RBS_AST_STRING = 32,
    RBS_AST_TYPEPARAM = 33,
    RBS_METHODTYPE = 34,
    RBS_NAMESPACE = 35,
    RBS_SIGNATURE = 36,
    RBS_TYPENAME = 37,
    RBS_TYPES_ALIAS = 38,
    RBS_TYPES_BASES_ANY = 39,
    RBS_TYPES_BASES_BOOL = 40,
    RBS_TYPES_BASES_BOTTOM = 41,
    RBS_TYPES_BASES_CLASS = 42,
    RBS_TYPES_BASES_INSTANCE = 43,
    RBS_TYPES_BASES_NIL = 44,
    RBS_TYPES_BASES_SELF = 45,
    RBS_TYPES_BASES_TOP = 46,
    RBS_TYPES_BASES_VOID = 47,
    RBS_TYPES_BLOCK = 48,
    RBS_TYPES_CLASSINSTANCE = 49,
    RBS_TYPES_CLASSSINGLETON = 50,
    RBS_TYPES_FUNCTION = 51,
    RBS_TYPES_FUNCTION_PARAM = 52,
    RBS_TYPES_INTERFACE = 53,
    RBS_TYPES_INTERSECTION = 54,
    RBS_TYPES_LITERAL = 55,
    RBS_TYPES_OPTIONAL = 56,
    RBS_TYPES_PROC = 57,
    RBS_TYPES_RECORD = 58,
    RBS_TYPES_RECORD_FIELDTYPE = 59,
    RBS_TYPES_TUPLE = 60,
    RBS_TYPES_UNION = 61,
    RBS_TYPES_UNTYPEDFUNCTION = 62,
    RBS_TYPES_VARIABLE = 63,
    RBS_AST_SYMBOL,
};

typedef struct rbs_node {
    enum rbs_node_type type;
    rbs_location_t *location;
} rbs_node_t;

const char* rbs_node_type_name(rbs_node_t *node);

/* rbs_node_list_node */

typedef struct rbs_node_list_node {
    rbs_node_t *node;
    struct rbs_node_list_node *next;
} rbs_node_list_node_t;

typedef struct rbs_node_list {
    rbs_node_list_node_t *head;
    rbs_node_list_node_t *tail;
    size_t length;
} rbs_node_list_t;

rbs_node_list_t* rbs_node_list_new(void);

void rbs_node_list_free(rbs_node_list_t *list);

void rbs_node_list_append(rbs_node_list_t *list, rbs_node_t *node);

/* rbs_hash */

typedef struct rbs_hash_node {
    rbs_node_t *key;
    rbs_node_t *value;
    struct rbs_hash_node *next;
} rbs_hash_node_t;

typedef struct rbs_hash {
    rbs_hash_node_t *head;
    rbs_hash_node_t *tail;
    size_t length;
} rbs_hash_t;

rbs_hash_t* rbs_hash_new(void);

void rbs_hash_free(rbs_hash_t *hash);

void rbs_hash_set(rbs_hash_t *hash, rbs_node_t *key, rbs_node_t *value);

rbs_hash_node_t* rbs_hash_find(rbs_hash_t *hash, rbs_node_t *key);

rbs_node_t* rbs_hash_get(rbs_hash_t *hash, rbs_node_t *key);

/* rbs_ast_node */

typedef struct rbs_ast_annotation {
    rbs_node_t base;

    rbs_string_t string;
} rbs_ast_annotation_t;

typedef struct rbs_ast_bool {
    rbs_node_t base;

    bool value;
} rbs_ast_bool_t;

typedef struct rbs_ast_comment {
    rbs_node_t base;

    rbs_string_t string;
} rbs_ast_comment_t;

typedef struct rbs_ast_declarations_class {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *type_params;
    struct rbs_ast_declarations_class_super *super_class;
    struct rbs_node_list *members;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_class_t;

typedef struct rbs_ast_declarations_class_super {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
} rbs_ast_declarations_class_super_t;

typedef struct rbs_ast_declarations_classalias {
    rbs_node_t base;

    struct rbs_typename *new_name;
    struct rbs_typename *old_name;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_classalias_t;

typedef struct rbs_ast_declarations_constant {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node *type;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_constant_t;

typedef struct rbs_ast_declarations_global {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_global_t;

typedef struct rbs_ast_declarations_interface {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *type_params;
    struct rbs_node_list *members;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_interface_t;

typedef struct rbs_ast_declarations_module {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *type_params;
    struct rbs_node_list *self_types;
    struct rbs_node_list *members;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_module_t;

typedef struct rbs_ast_declarations_module_self {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
} rbs_ast_declarations_module_self_t;

typedef struct rbs_ast_declarations_modulealias {
    rbs_node_t base;

    struct rbs_typename *new_name;
    struct rbs_typename *old_name;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_modulealias_t;

typedef struct rbs_ast_declarations_typealias {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *type_params;
    struct rbs_node *type;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_declarations_typealias_t;

typedef struct rbs_ast_directives_use {
    rbs_node_t base;

    struct rbs_node_list *clauses;
} rbs_ast_directives_use_t;

typedef struct rbs_ast_directives_use_singleclause {
    rbs_node_t base;

    struct rbs_typename *type_name;
    struct rbs_ast_symbol *new_name;
} rbs_ast_directives_use_singleclause_t;

typedef struct rbs_ast_directives_use_wildcardclause {
    rbs_node_t base;

    struct rbs_namespace *rbs_namespace;
} rbs_ast_directives_use_wildcardclause_t;

typedef struct rbs_ast_integer {
    rbs_node_t base;

    rbs_string_t string_representation;
} rbs_ast_integer_t;

typedef struct rbs_ast_members_alias {
    rbs_node_t base;

    struct rbs_ast_symbol *new_name;
    struct rbs_ast_symbol *old_name;
    struct rbs_ast_symbol *kind;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_members_alias_t;

typedef struct rbs_ast_members_attraccessor {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_node *ivar_name;
    struct rbs_ast_symbol *kind;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
    struct rbs_ast_symbol *visibility;
} rbs_ast_members_attraccessor_t;

typedef struct rbs_ast_members_attrreader {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_node *ivar_name;
    struct rbs_ast_symbol *kind;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
    struct rbs_ast_symbol *visibility;
} rbs_ast_members_attrreader_t;

typedef struct rbs_ast_members_attrwriter {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_node *ivar_name;
    struct rbs_ast_symbol *kind;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
    struct rbs_ast_symbol *visibility;
} rbs_ast_members_attrwriter_t;

typedef struct rbs_ast_members_classinstancevariable {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_ast_comment *comment;
} rbs_ast_members_classinstancevariable_t;

typedef struct rbs_ast_members_classvariable {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_ast_comment *comment;
} rbs_ast_members_classvariable_t;

typedef struct rbs_ast_members_extend {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_members_extend_t;

typedef struct rbs_ast_members_include {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_members_include_t;

typedef struct rbs_ast_members_instancevariable {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_node *type;
    struct rbs_ast_comment *comment;
} rbs_ast_members_instancevariable_t;

typedef struct rbs_ast_members_methoddefinition {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_ast_symbol *kind;
    struct rbs_node_list *overloads;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
    bool overloading;
    struct rbs_ast_symbol *visibility;
} rbs_ast_members_methoddefinition_t;

typedef struct rbs_ast_members_methoddefinition_overload {
    rbs_node_t base;

    struct rbs_node_list *annotations;
    struct rbs_node *method_type;
} rbs_ast_members_methoddefinition_overload_t;

typedef struct rbs_ast_members_prepend {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
    struct rbs_node_list *annotations;
    struct rbs_ast_comment *comment;
} rbs_ast_members_prepend_t;

typedef struct rbs_ast_members_private {
    rbs_node_t base;

} rbs_ast_members_private_t;

typedef struct rbs_ast_members_public {
    rbs_node_t base;

} rbs_ast_members_public_t;

typedef struct rbs_ast_string {
    rbs_node_t base;

    rbs_string_t string;
} rbs_ast_string_t;

typedef struct rbs_ast_typeparam {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
    struct rbs_ast_symbol *variance;
    struct rbs_node *upper_bound;
    bool unchecked;
    struct rbs_node *default_type;
} rbs_ast_typeparam_t;

typedef struct rbs_methodtype {
    rbs_node_t base;

    struct rbs_node_list *type_params;
    struct rbs_node *type;
    struct rbs_types_block *block;
} rbs_methodtype_t;

typedef struct rbs_namespace {
    rbs_node_t base;

    struct rbs_node_list *path;
    bool absolute;
} rbs_namespace_t;

typedef struct rbs_signature {
    rbs_node_t base;

    struct rbs_node_list *directives;
    struct rbs_node_list *declarations;
} rbs_signature_t;

typedef struct rbs_typename {
    rbs_node_t base;

    struct rbs_namespace *rbs_namespace;
    struct rbs_ast_symbol *name;
} rbs_typename_t;

typedef struct rbs_types_alias {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
} rbs_types_alias_t;

typedef struct rbs_types_bases_any {
    rbs_node_t base;

    bool todo;
} rbs_types_bases_any_t;

typedef struct rbs_types_bases_bool {
    rbs_node_t base;

} rbs_types_bases_bool_t;

typedef struct rbs_types_bases_bottom {
    rbs_node_t base;

} rbs_types_bases_bottom_t;

typedef struct rbs_types_bases_class {
    rbs_node_t base;

} rbs_types_bases_class_t;

typedef struct rbs_types_bases_instance {
    rbs_node_t base;

} rbs_types_bases_instance_t;

typedef struct rbs_types_bases_nil {
    rbs_node_t base;

} rbs_types_bases_nil_t;

typedef struct rbs_types_bases_self {
    rbs_node_t base;

} rbs_types_bases_self_t;

typedef struct rbs_types_bases_top {
    rbs_node_t base;

} rbs_types_bases_top_t;

typedef struct rbs_types_bases_void {
    rbs_node_t base;

} rbs_types_bases_void_t;

typedef struct rbs_types_block {
    rbs_node_t base;

    struct rbs_node *type;
    bool required;
    struct rbs_node *self_type;
} rbs_types_block_t;

typedef struct rbs_types_classinstance {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
} rbs_types_classinstance_t;

typedef struct rbs_types_classsingleton {
    rbs_node_t base;

    struct rbs_typename *name;
} rbs_types_classsingleton_t;

typedef struct rbs_types_function {
    rbs_node_t base;

    struct rbs_node_list *required_positionals;
    struct rbs_node_list *optional_positionals;
    struct rbs_node *rest_positionals;
    struct rbs_node_list *trailing_positionals;
    struct rbs_hash *required_keywords;
    struct rbs_hash *optional_keywords;
    struct rbs_node *rest_keywords;
    struct rbs_node *return_type;
} rbs_types_function_t;

typedef struct rbs_types_function_param {
    rbs_node_t base;

    struct rbs_node *type;
    struct rbs_ast_symbol *name;
} rbs_types_function_param_t;

typedef struct rbs_types_interface {
    rbs_node_t base;

    struct rbs_typename *name;
    struct rbs_node_list *args;
} rbs_types_interface_t;

typedef struct rbs_types_intersection {
    rbs_node_t base;

    struct rbs_node_list *types;
} rbs_types_intersection_t;

typedef struct rbs_types_literal {
    rbs_node_t base;

    struct rbs_node *literal;
} rbs_types_literal_t;

typedef struct rbs_types_optional {
    rbs_node_t base;

    struct rbs_node *type;
} rbs_types_optional_t;

typedef struct rbs_types_proc {
    rbs_node_t base;

    struct rbs_node *type;
    struct rbs_types_block *block;
    struct rbs_node *self_type;
} rbs_types_proc_t;

typedef struct rbs_types_record {
    rbs_node_t base;

    struct rbs_hash *all_fields;
} rbs_types_record_t;

typedef struct rbs_types_record_fieldtype {
    rbs_node_t base;

    struct rbs_node *type;
    bool required;
} rbs_types_record_fieldtype_t;

typedef struct rbs_types_tuple {
    rbs_node_t base;

    struct rbs_node_list *types;
} rbs_types_tuple_t;

typedef struct rbs_types_union {
    rbs_node_t base;

    struct rbs_node_list *types;
} rbs_types_union_t;

typedef struct rbs_types_untypedfunction {
    rbs_node_t base;

    struct rbs_node *return_type;
} rbs_types_untypedfunction_t;

typedef struct rbs_types_variable {
    rbs_node_t base;

    struct rbs_ast_symbol *name;
} rbs_types_variable_t;


typedef struct rbs_ast_symbol {
    rbs_node_t base;
    rbs_constant_id_t constant_id;
} rbs_ast_symbol_t;

rbs_ast_symbol_t *rbs_ast_symbol_new(rbs_location_t *location, rbs_constant_id_t constant_id);

rbs_ast_annotation_t *rbs_ast_annotation_new(rbs_location_t *location, rbs_string_t string);
rbs_ast_bool_t *rbs_ast_bool_new(rbs_location_t *location, bool value);
rbs_ast_comment_t *rbs_ast_comment_new(rbs_location_t *location, rbs_string_t string);
rbs_ast_declarations_class_t *rbs_ast_declarations_class_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *type_params, rbs_ast_declarations_class_super_t *super_class, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_declarations_class_super_t *rbs_ast_declarations_class_super_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args);
rbs_ast_declarations_classalias_t *rbs_ast_declarations_classalias_new(rbs_location_t *location, rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_ast_comment_t *comment);
rbs_ast_declarations_constant_t *rbs_ast_declarations_constant_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_t *type, rbs_ast_comment_t *comment);
rbs_ast_declarations_global_t *rbs_ast_declarations_global_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_ast_comment_t *comment);
rbs_ast_declarations_interface_t *rbs_ast_declarations_interface_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_declarations_module_t *rbs_ast_declarations_module_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *self_types, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_declarations_module_self_t *rbs_ast_declarations_module_self_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args);
rbs_ast_declarations_modulealias_t *rbs_ast_declarations_modulealias_new(rbs_location_t *location, rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_ast_comment_t *comment);
rbs_ast_declarations_typealias_t *rbs_ast_declarations_typealias_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_t *type, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_directives_use_t *rbs_ast_directives_use_new(rbs_location_t *location, rbs_node_list_t *clauses);
rbs_ast_directives_use_singleclause_t *rbs_ast_directives_use_singleclause_new(rbs_location_t *location, rbs_typename_t *type_name, rbs_ast_symbol_t *new_name);
rbs_ast_directives_use_wildcardclause_t *rbs_ast_directives_use_wildcardclause_new(rbs_location_t *location, rbs_namespace_t *rbs_namespace);
rbs_ast_integer_t *rbs_ast_integer_new(rbs_location_t *location, rbs_string_t string_representation);
rbs_ast_members_alias_t *rbs_ast_members_alias_new(rbs_location_t *location, rbs_ast_symbol_t *new_name, rbs_ast_symbol_t *old_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_members_attraccessor_t *rbs_ast_members_attraccessor_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_node_t *ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_ast_comment_t *comment, rbs_ast_symbol_t *visibility);
rbs_ast_members_attrreader_t *rbs_ast_members_attrreader_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_node_t *ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_ast_comment_t *comment, rbs_ast_symbol_t *visibility);
rbs_ast_members_attrwriter_t *rbs_ast_members_attrwriter_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_node_t *ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_ast_comment_t *comment, rbs_ast_symbol_t *visibility);
rbs_ast_members_classinstancevariable_t *rbs_ast_members_classinstancevariable_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_ast_comment_t *comment);
rbs_ast_members_classvariable_t *rbs_ast_members_classvariable_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_ast_comment_t *comment);
rbs_ast_members_extend_t *rbs_ast_members_extend_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_members_include_t *rbs_ast_members_include_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_members_instancevariable_t *rbs_ast_members_instancevariable_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_node_t *type, rbs_ast_comment_t *comment);
rbs_ast_members_methoddefinition_t *rbs_ast_members_methoddefinition_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_ast_symbol_t *kind, rbs_node_list_t *overloads, rbs_node_list_t *annotations, rbs_ast_comment_t *comment, bool overloading, rbs_ast_symbol_t *visibility);
rbs_ast_members_methoddefinition_overload_t *rbs_ast_members_methoddefinition_overload_new(rbs_location_t *location, rbs_node_list_t *annotations, rbs_node_t *method_type);
rbs_ast_members_prepend_t *rbs_ast_members_prepend_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_ast_comment_t *comment);
rbs_ast_members_private_t *rbs_ast_members_private_new(rbs_location_t *location);
rbs_ast_members_public_t *rbs_ast_members_public_new(rbs_location_t *location);
rbs_ast_string_t *rbs_ast_string_new(rbs_location_t *location, rbs_string_t string);
rbs_ast_typeparam_t *rbs_ast_typeparam_new(rbs_location_t *location, rbs_ast_symbol_t *name, rbs_ast_symbol_t *variance, rbs_node_t *upper_bound, bool unchecked, rbs_node_t *default_type);
rbs_methodtype_t *rbs_methodtype_new(rbs_location_t *location, rbs_node_list_t *type_params, rbs_node_t *type, rbs_types_block_t *block);
rbs_namespace_t *rbs_namespace_new(rbs_location_t *location, rbs_node_list_t *path, bool absolute);
rbs_signature_t *rbs_signature_new(rbs_location_t *location, rbs_node_list_t *directives, rbs_node_list_t *declarations);
rbs_typename_t *rbs_typename_new(rbs_location_t *location, rbs_namespace_t *rbs_namespace, rbs_ast_symbol_t *name);
rbs_types_alias_t *rbs_types_alias_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args);
rbs_types_bases_any_t *rbs_types_bases_any_new(rbs_location_t *location, bool todo);
rbs_types_bases_bool_t *rbs_types_bases_bool_new(rbs_location_t *location);
rbs_types_bases_bottom_t *rbs_types_bases_bottom_new(rbs_location_t *location);
rbs_types_bases_class_t *rbs_types_bases_class_new(rbs_location_t *location);
rbs_types_bases_instance_t *rbs_types_bases_instance_new(rbs_location_t *location);
rbs_types_bases_nil_t *rbs_types_bases_nil_new(rbs_location_t *location);
rbs_types_bases_self_t *rbs_types_bases_self_new(rbs_location_t *location);
rbs_types_bases_top_t *rbs_types_bases_top_new(rbs_location_t *location);
rbs_types_bases_void_t *rbs_types_bases_void_new(rbs_location_t *location);
rbs_types_block_t *rbs_types_block_new(rbs_location_t *location, rbs_node_t *type, bool required, rbs_node_t *self_type);
rbs_types_classinstance_t *rbs_types_classinstance_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args);
rbs_types_classsingleton_t *rbs_types_classsingleton_new(rbs_location_t *location, rbs_typename_t *name);
rbs_types_function_t *rbs_types_function_new(rbs_location_t *location, rbs_node_list_t *required_positionals, rbs_node_list_t *optional_positionals, rbs_node_t *rest_positionals, rbs_node_list_t *trailing_positionals, rbs_hash_t *required_keywords, rbs_hash_t *optional_keywords, rbs_node_t *rest_keywords, rbs_node_t *return_type);
rbs_types_function_param_t *rbs_types_function_param_new(rbs_location_t *location, rbs_node_t *type, rbs_ast_symbol_t *name);
rbs_types_interface_t *rbs_types_interface_new(rbs_location_t *location, rbs_typename_t *name, rbs_node_list_t *args);
rbs_types_intersection_t *rbs_types_intersection_new(rbs_location_t *location, rbs_node_list_t *types);
rbs_types_literal_t *rbs_types_literal_new(rbs_location_t *location, rbs_node_t *literal);
rbs_types_optional_t *rbs_types_optional_new(rbs_location_t *location, rbs_node_t *type);
rbs_types_proc_t *rbs_types_proc_new(rbs_location_t *location, rbs_node_t *type, rbs_types_block_t *block, rbs_node_t *self_type);
rbs_types_record_t *rbs_types_record_new(rbs_location_t *location, rbs_hash_t *all_fields);
rbs_types_record_fieldtype_t *rbs_types_record_fieldtype_new(rbs_location_t *location, rbs_node_t *type, bool required);
rbs_types_tuple_t *rbs_types_tuple_new(rbs_location_t *location, rbs_node_list_t *types);
rbs_types_union_t *rbs_types_union_new(rbs_location_t *location, rbs_node_list_t *types);
rbs_types_untypedfunction_t *rbs_types_untypedfunction_new(rbs_location_t *location, rbs_node_t *return_type);
rbs_types_variable_t *rbs_types_variable_new(rbs_location_t *location, rbs_ast_symbol_t *name);

#endif
