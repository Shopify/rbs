/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/src/ast.c.erb                                                    */
/*----------------------------------------------------------------------------*/

#include "rbs/ast.h"

#include <stdlib.h>
#include "class_constants.h"
#include "rbs_string_bridging.h"
#include "ruby/encoding.h"

/* rbs_node_list */

rbs_node_list_t* rbs_node_list_new(void) {
    rbs_node_list_t *list = (rbs_node_list_t *)malloc(sizeof(rbs_node_list_t));
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    return list;
}

void rbs_node_list_free(rbs_node_list_t *list) {
    rbs_node_list_node_t *current = list->head;
    while (current != NULL) {
        rbs_node_list_node_t *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void rbs_node_list_append(rbs_node_list_t *list, rbs_node_t *node) {
    rbs_node_list_node_t *new_node = (rbs_node_list_node_t *)malloc(sizeof(rbs_node_list_node_t));
    new_node->node = node;
    new_node->next = NULL;

    if (list->tail == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->length++;
}

/* rbs_hash */

rbs_hash_t* rbs_hash_new(void) {
    rbs_hash_t *hash = (rbs_hash_t *)malloc(sizeof(rbs_hash_t));
    hash->head = NULL;
    hash->tail = NULL;
    hash->length = 0;

    return hash;
}

void rbs_hash_free(rbs_hash_t *hash) {
    rbs_hash_node_t *current = hash->head;
    while (current != NULL) {
        rbs_hash_node_t *next = current->next;
        free(current);
        current = next;
    }
    free(hash);
}

bool rbs_node_equal(rbs_node_t *lhs, rbs_node_t *rhs) {
    if (lhs == rhs) return true;
    if (lhs->type != rhs->type) return false;

    switch (lhs->type) {
        case RBS_AST_BOOL:
            return ((rbs_ast_bool_t *)lhs)->value == ((rbs_ast_bool_t *) rhs)->value;
        case RBS_AST_SYMBOL: {
            return rbs_constant_id_equal(
                fake_constant_pool,
                ((rbs_ast_symbol_t *) lhs)->constant_id,
                ((rbs_ast_symbol_t *) rhs)->constant_id
            );
        }
        case RBS_AST_STRING: {
            return rbs_string_equal(((rbs_ast_string_t *) lhs)->string, ((rbs_ast_string_t *) rhs)->string);
        }
        case RBS_OTHER_RUBY_VALUE:
            return rb_equal(((rbs_other_ruby_value_t *) lhs)->ruby_value, ((rbs_other_ruby_value_t *) rhs)->ruby_value);
        default:
            printf("Unhandled node type: %d\n", lhs->type);
            return false;
    }
}

rbs_hash_node_t* rbs_hash_find(rbs_hash_t *hash, rbs_node_t *key) {
    rbs_hash_node_t *current = hash->head;

    while (current != NULL) {
        if (rbs_node_equal(key, current->key)) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void rbs_hash_set(rbs_hash_t *hash, rbs_node_t *key, rbs_node_t *value) {
    rbs_hash_node_t *existing_node = rbs_hash_find(hash, key);
    if (existing_node != NULL) {
        existing_node->value = value;
        return;
    }

    rbs_hash_node_t *new_node = (rbs_hash_node_t *)malloc(sizeof(rbs_hash_node_t));
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;

    if (hash->tail == NULL) {
        hash->head = new_node;
        hash->tail = new_node;
    } else {
        hash->tail->next = new_node;
        hash->tail = new_node;
    }
}

rbs_node_t* rbs_hash_get(rbs_hash_t *hash, rbs_node_t *key) {
    rbs_hash_node_t *node = rbs_hash_find(hash, key);
    return node ? node->value : NULL;
}

rbs_ast_symbol_t *rbs_ast_symbol_new(rbs_constant_id_t constant_id) {
    rbs_ast_symbol_t *instance = (rbs_ast_symbol_t *)calloc(1, sizeof(rbs_ast_symbol_t));

    *instance = (rbs_ast_symbol_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_SYMBOL
        },
        .constant_id = constant_id,
    };

    return instance;
}

rbs_other_ruby_value_t *rbs_other_ruby_value_new(VALUE ruby_value) {
    rb_gc_register_mark_object(ruby_value);

    rbs_other_ruby_value_t *instance = (rbs_other_ruby_value_t *)calloc(1, sizeof(rbs_other_ruby_value_t));

    *instance = (rbs_other_ruby_value_t) {
        .base = (rbs_node_t) {
            .type = RBS_OTHER_RUBY_VALUE
        },
        .ruby_value = ruby_value,
    };

    return instance;
}

rbs_ast_annotation_t *rbs_ast_annotation_new(rbs_string_t string, rbs_location_t *location) {
    rbs_ast_annotation_t *instance = (rbs_ast_annotation_t *)calloc(1, sizeof(rbs_ast_annotation_t));


    *instance = (rbs_ast_annotation_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_ANNOTATION
        },
        .string = string,
        .location = location,
    };

    return instance;
}

rbs_ast_bool_t *rbs_ast_bool_new(bool value) {
    rbs_ast_bool_t *instance = (rbs_ast_bool_t *)calloc(1, sizeof(rbs_ast_bool_t));


    *instance = (rbs_ast_bool_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_BOOL
        },
        .value = value,
    };

    return instance;
}

rbs_ast_comment_t *rbs_ast_comment_new(rbs_string_t string, rbs_location_t *location) {
    rbs_ast_comment_t *instance = (rbs_ast_comment_t *)calloc(1, sizeof(rbs_ast_comment_t));


    *instance = (rbs_ast_comment_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_COMMENT
        },
        .string = string,
        .location = location,
    };

    return instance;
}

rbs_ast_declarations_class_t *rbs_ast_declarations_class_new(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_ast_declarations_class_super_t *super_class, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_class_t *instance = (rbs_ast_declarations_class_t *)calloc(1, sizeof(rbs_ast_declarations_class_t));


    *instance = (rbs_ast_declarations_class_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_CLASS
        },
        .name = name,
        .type_params = type_params,
        .super_class = super_class,
        .members = members,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_class_super_t *rbs_ast_declarations_class_super_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
    rbs_ast_declarations_class_super_t *instance = (rbs_ast_declarations_class_super_t *)calloc(1, sizeof(rbs_ast_declarations_class_super_t));


    *instance = (rbs_ast_declarations_class_super_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_CLASS_SUPER
        },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_ast_declarations_classalias_t *rbs_ast_declarations_classalias_new(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_classalias_t *instance = (rbs_ast_declarations_classalias_t *)calloc(1, sizeof(rbs_ast_declarations_classalias_t));


    *instance = (rbs_ast_declarations_classalias_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_CLASSALIAS
        },
        .new_name = new_name,
        .old_name = old_name,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_constant_t *rbs_ast_declarations_constant_new(rbs_typename_t *name, rbs_node_t *type, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_constant_t *instance = (rbs_ast_declarations_constant_t *)calloc(1, sizeof(rbs_ast_declarations_constant_t));


    *instance = (rbs_ast_declarations_constant_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_CONSTANT
        },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_global_t *rbs_ast_declarations_global_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_global_t *instance = (rbs_ast_declarations_global_t *)calloc(1, sizeof(rbs_ast_declarations_global_t));


    *instance = (rbs_ast_declarations_global_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_GLOBAL
        },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_interface_t *rbs_ast_declarations_interface_new(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_interface_t *instance = (rbs_ast_declarations_interface_t *)calloc(1, sizeof(rbs_ast_declarations_interface_t));


    *instance = (rbs_ast_declarations_interface_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_INTERFACE
        },
        .name = name,
        .type_params = type_params,
        .members = members,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_module_t *rbs_ast_declarations_module_new(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_list_t *self_types, rbs_node_list_t *members, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_module_t *instance = (rbs_ast_declarations_module_t *)calloc(1, sizeof(rbs_ast_declarations_module_t));


    *instance = (rbs_ast_declarations_module_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_MODULE
        },
        .name = name,
        .type_params = type_params,
        .self_types = self_types,
        .members = members,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_module_self_t *rbs_ast_declarations_module_self_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
    rbs_ast_declarations_module_self_t *instance = (rbs_ast_declarations_module_self_t *)calloc(1, sizeof(rbs_ast_declarations_module_self_t));


    *instance = (rbs_ast_declarations_module_self_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_MODULE_SELF
        },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_ast_declarations_modulealias_t *rbs_ast_declarations_modulealias_new(rbs_typename_t *new_name, rbs_typename_t *old_name, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_modulealias_t *instance = (rbs_ast_declarations_modulealias_t *)calloc(1, sizeof(rbs_ast_declarations_modulealias_t));


    *instance = (rbs_ast_declarations_modulealias_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_MODULEALIAS
        },
        .new_name = new_name,
        .old_name = old_name,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_typealias_t *rbs_ast_declarations_typealias_new(rbs_typename_t *name, rbs_node_list_t *type_params, rbs_node_t *type, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_declarations_typealias_t *instance = (rbs_ast_declarations_typealias_t *)calloc(1, sizeof(rbs_ast_declarations_typealias_t));


    *instance = (rbs_ast_declarations_typealias_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DECLARATIONS_TYPEALIAS
        },
        .name = name,
        .type_params = type_params,
        .type = type,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_directives_use_t *rbs_ast_directives_use_new(rbs_node_list_t *clauses, rbs_location_t *location) {
    rbs_ast_directives_use_t *instance = (rbs_ast_directives_use_t *)calloc(1, sizeof(rbs_ast_directives_use_t));


    *instance = (rbs_ast_directives_use_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DIRECTIVES_USE
        },
        .clauses = clauses,
        .location = location,
    };

    return instance;
}

rbs_ast_directives_use_singleclause_t *rbs_ast_directives_use_singleclause_new(rbs_typename_t *type_name, rbs_ast_symbol_t *new_name, rbs_location_t *location) {
    rbs_ast_directives_use_singleclause_t *instance = (rbs_ast_directives_use_singleclause_t *)calloc(1, sizeof(rbs_ast_directives_use_singleclause_t));


    *instance = (rbs_ast_directives_use_singleclause_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DIRECTIVES_USE_SINGLECLAUSE
        },
        .type_name = type_name,
        .new_name = new_name,
        .location = location,
    };

    return instance;
}

rbs_ast_directives_use_wildcardclause_t *rbs_ast_directives_use_wildcardclause_new(rbs_namespace_t *namespace, rbs_location_t *location) {
    rbs_ast_directives_use_wildcardclause_t *instance = (rbs_ast_directives_use_wildcardclause_t *)calloc(1, sizeof(rbs_ast_directives_use_wildcardclause_t));


    *instance = (rbs_ast_directives_use_wildcardclause_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_DIRECTIVES_USE_WILDCARDCLAUSE
        },
        .namespace = namespace,
        .location = location,
    };

    return instance;
}

rbs_ast_integer_t *rbs_ast_integer_new(rbs_string_t string_representation) {
    rbs_ast_integer_t *instance = (rbs_ast_integer_t *)calloc(1, sizeof(rbs_ast_integer_t));


    *instance = (rbs_ast_integer_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_INTEGER
        },
        .string_representation = string_representation,
    };

    return instance;
}

rbs_ast_members_alias_t *rbs_ast_members_alias_new(rbs_ast_symbol_t *new_name, rbs_ast_symbol_t *old_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_alias_t *instance = (rbs_ast_members_alias_t *)calloc(1, sizeof(rbs_ast_members_alias_t));


    *instance = (rbs_ast_members_alias_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_ALIAS
        },
        .new_name = new_name,
        .old_name = old_name,
        .kind = kind,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_attraccessor_t *rbs_ast_members_attraccessor_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_node_t *ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment, rbs_ast_symbol_t *visibility) {
    rbs_ast_members_attraccessor_t *instance = (rbs_ast_members_attraccessor_t *)calloc(1, sizeof(rbs_ast_members_attraccessor_t));


    *instance = (rbs_ast_members_attraccessor_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_ATTRACCESSOR
        },
        .name = name,
        .type = type,
        .ivar_name = ivar_name,
        .kind = kind,
        .annotations = annotations,
        .location = location,
        .comment = comment,
        .visibility = visibility,
    };

    return instance;
}

rbs_ast_members_attrreader_t *rbs_ast_members_attrreader_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_node_t *ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment, rbs_ast_symbol_t *visibility) {
    rbs_ast_members_attrreader_t *instance = (rbs_ast_members_attrreader_t *)calloc(1, sizeof(rbs_ast_members_attrreader_t));


    *instance = (rbs_ast_members_attrreader_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_ATTRREADER
        },
        .name = name,
        .type = type,
        .ivar_name = ivar_name,
        .kind = kind,
        .annotations = annotations,
        .location = location,
        .comment = comment,
        .visibility = visibility,
    };

    return instance;
}

rbs_ast_members_attrwriter_t *rbs_ast_members_attrwriter_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_node_t *ivar_name, rbs_ast_symbol_t *kind, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment, rbs_ast_symbol_t *visibility) {
    rbs_ast_members_attrwriter_t *instance = (rbs_ast_members_attrwriter_t *)calloc(1, sizeof(rbs_ast_members_attrwriter_t));


    *instance = (rbs_ast_members_attrwriter_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_ATTRWRITER
        },
        .name = name,
        .type = type,
        .ivar_name = ivar_name,
        .kind = kind,
        .annotations = annotations,
        .location = location,
        .comment = comment,
        .visibility = visibility,
    };

    return instance;
}

rbs_ast_members_classinstancevariable_t *rbs_ast_members_classinstancevariable_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_classinstancevariable_t *instance = (rbs_ast_members_classinstancevariable_t *)calloc(1, sizeof(rbs_ast_members_classinstancevariable_t));


    *instance = (rbs_ast_members_classinstancevariable_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_CLASSINSTANCEVARIABLE
        },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_classvariable_t *rbs_ast_members_classvariable_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_classvariable_t *instance = (rbs_ast_members_classvariable_t *)calloc(1, sizeof(rbs_ast_members_classvariable_t));


    *instance = (rbs_ast_members_classvariable_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_CLASSVARIABLE
        },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_extend_t *rbs_ast_members_extend_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_extend_t *instance = (rbs_ast_members_extend_t *)calloc(1, sizeof(rbs_ast_members_extend_t));


    *instance = (rbs_ast_members_extend_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_EXTEND
        },
        .name = name,
        .args = args,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_include_t *rbs_ast_members_include_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_include_t *instance = (rbs_ast_members_include_t *)calloc(1, sizeof(rbs_ast_members_include_t));


    *instance = (rbs_ast_members_include_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_INCLUDE
        },
        .name = name,
        .args = args,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_instancevariable_t *rbs_ast_members_instancevariable_new(rbs_ast_symbol_t *name, rbs_node_t *type, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_instancevariable_t *instance = (rbs_ast_members_instancevariable_t *)calloc(1, sizeof(rbs_ast_members_instancevariable_t));


    *instance = (rbs_ast_members_instancevariable_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_INSTANCEVARIABLE
        },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_methoddefinition_t *rbs_ast_members_methoddefinition_new(rbs_ast_symbol_t *name, rbs_ast_symbol_t *kind, rbs_node_list_t *overloads, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment, bool overloading, rbs_ast_symbol_t *visibility) {
    rbs_ast_members_methoddefinition_t *instance = (rbs_ast_members_methoddefinition_t *)calloc(1, sizeof(rbs_ast_members_methoddefinition_t));


    *instance = (rbs_ast_members_methoddefinition_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_METHODDEFINITION
        },
        .name = name,
        .kind = kind,
        .overloads = overloads,
        .annotations = annotations,
        .location = location,
        .comment = comment,
        .overloading = overloading,
        .visibility = visibility,
    };

    return instance;
}

rbs_ast_members_methoddefinition_overload_t *rbs_ast_members_methoddefinition_overload_new(rbs_node_list_t *annotations, rbs_node_t *method_type) {
    rbs_ast_members_methoddefinition_overload_t *instance = (rbs_ast_members_methoddefinition_overload_t *)calloc(1, sizeof(rbs_ast_members_methoddefinition_overload_t));


    *instance = (rbs_ast_members_methoddefinition_overload_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_METHODDEFINITION_OVERLOAD
        },
        .annotations = annotations,
        .method_type = method_type,
    };

    return instance;
}

rbs_ast_members_prepend_t *rbs_ast_members_prepend_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_node_list_t *annotations, rbs_location_t *location, rbs_ast_comment_t *comment) {
    rbs_ast_members_prepend_t *instance = (rbs_ast_members_prepend_t *)calloc(1, sizeof(rbs_ast_members_prepend_t));


    *instance = (rbs_ast_members_prepend_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_PREPEND
        },
        .name = name,
        .args = args,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_private_t *rbs_ast_members_private_new(rbs_location_t *location) {
    rbs_ast_members_private_t *instance = (rbs_ast_members_private_t *)calloc(1, sizeof(rbs_ast_members_private_t));


    *instance = (rbs_ast_members_private_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_PRIVATE
        },
        .location = location,
    };

    return instance;
}

rbs_ast_members_public_t *rbs_ast_members_public_new(rbs_location_t *location) {
    rbs_ast_members_public_t *instance = (rbs_ast_members_public_t *)calloc(1, sizeof(rbs_ast_members_public_t));


    *instance = (rbs_ast_members_public_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_MEMBERS_PUBLIC
        },
        .location = location,
    };

    return instance;
}

rbs_ast_string_t *rbs_ast_string_new(rbs_string_t string) {
    rbs_ast_string_t *instance = (rbs_ast_string_t *)calloc(1, sizeof(rbs_ast_string_t));


    *instance = (rbs_ast_string_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_STRING
        },
        .string = string,
    };

    return instance;
}

rbs_ast_typeparam_t *rbs_ast_typeparam_new(rbs_ast_symbol_t *name, rbs_ast_symbol_t *variance, rbs_node_t *upper_bound, bool unchecked, rbs_node_t *default_type, rbs_location_t *location) {
    rbs_ast_typeparam_t *instance = (rbs_ast_typeparam_t *)calloc(1, sizeof(rbs_ast_typeparam_t));


    *instance = (rbs_ast_typeparam_t) {
        .base = (rbs_node_t) {
            .type = RBS_AST_TYPEPARAM
        },
        .name = name,
        .variance = variance,
        .upper_bound = upper_bound,
        .unchecked = unchecked,
        .default_type = default_type,
        .location = location,
    };

    return instance;
}

rbs_methodtype_t *rbs_methodtype_new(rbs_node_list_t *type_params, rbs_node_t *type, rbs_types_block_t *block, rbs_location_t *location) {
    rbs_methodtype_t *instance = (rbs_methodtype_t *)calloc(1, sizeof(rbs_methodtype_t));


    *instance = (rbs_methodtype_t) {
        .base = (rbs_node_t) {
            .type = RBS_METHODTYPE
        },
        .type_params = type_params,
        .type = type,
        .block = block,
        .location = location,
    };

    return instance;
}

rbs_namespace_t *rbs_namespace_new(rbs_node_list_t *path, bool absolute) {
    rbs_namespace_t *instance = (rbs_namespace_t *)calloc(1, sizeof(rbs_namespace_t));


    *instance = (rbs_namespace_t) {
        .base = (rbs_node_t) {
            .type = RBS_NAMESPACE
        },
        .path = path,
        .absolute = absolute,
    };

    return instance;
}

rbs_signature_t *rbs_signature_new(rbs_node_list_t *directives, rbs_node_list_t *declarations) {
    rbs_signature_t *instance = (rbs_signature_t *)calloc(1, sizeof(rbs_signature_t));


    *instance = (rbs_signature_t) {
        .base = (rbs_node_t) {
            .type = RBS_SIGNATURE
        },
        .directives = directives,
        .declarations = declarations,
    };

    return instance;
}

rbs_typename_t *rbs_typename_new(rbs_namespace_t *namespace, rbs_ast_symbol_t *name) {
    rbs_typename_t *instance = (rbs_typename_t *)calloc(1, sizeof(rbs_typename_t));


    *instance = (rbs_typename_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPENAME
        },
        .namespace = namespace,
        .name = name,
    };

    return instance;
}

rbs_types_alias_t *rbs_types_alias_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
    rbs_types_alias_t *instance = (rbs_types_alias_t *)calloc(1, sizeof(rbs_types_alias_t));


    *instance = (rbs_types_alias_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_ALIAS
        },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_types_bases_any_t *rbs_types_bases_any_new(bool todo, rbs_location_t *location) {
    rbs_types_bases_any_t *instance = (rbs_types_bases_any_t *)calloc(1, sizeof(rbs_types_bases_any_t));


    *instance = (rbs_types_bases_any_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_ANY
        },
        .todo = todo,
        .location = location,
    };

    return instance;
}

rbs_types_bases_bool_t *rbs_types_bases_bool_new(rbs_location_t *location) {
    rbs_types_bases_bool_t *instance = (rbs_types_bases_bool_t *)calloc(1, sizeof(rbs_types_bases_bool_t));


    *instance = (rbs_types_bases_bool_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_BOOL
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_bottom_t *rbs_types_bases_bottom_new(rbs_location_t *location) {
    rbs_types_bases_bottom_t *instance = (rbs_types_bases_bottom_t *)calloc(1, sizeof(rbs_types_bases_bottom_t));


    *instance = (rbs_types_bases_bottom_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_BOTTOM
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_class_t *rbs_types_bases_class_new(rbs_location_t *location) {
    rbs_types_bases_class_t *instance = (rbs_types_bases_class_t *)calloc(1, sizeof(rbs_types_bases_class_t));


    *instance = (rbs_types_bases_class_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_CLASS
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_instance_t *rbs_types_bases_instance_new(rbs_location_t *location) {
    rbs_types_bases_instance_t *instance = (rbs_types_bases_instance_t *)calloc(1, sizeof(rbs_types_bases_instance_t));


    *instance = (rbs_types_bases_instance_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_INSTANCE
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_nil_t *rbs_types_bases_nil_new(rbs_location_t *location) {
    rbs_types_bases_nil_t *instance = (rbs_types_bases_nil_t *)calloc(1, sizeof(rbs_types_bases_nil_t));


    *instance = (rbs_types_bases_nil_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_NIL
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_self_t *rbs_types_bases_self_new(rbs_location_t *location) {
    rbs_types_bases_self_t *instance = (rbs_types_bases_self_t *)calloc(1, sizeof(rbs_types_bases_self_t));


    *instance = (rbs_types_bases_self_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_SELF
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_top_t *rbs_types_bases_top_new(rbs_location_t *location) {
    rbs_types_bases_top_t *instance = (rbs_types_bases_top_t *)calloc(1, sizeof(rbs_types_bases_top_t));


    *instance = (rbs_types_bases_top_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_TOP
        },
        .location = location,
    };

    return instance;
}

rbs_types_bases_void_t *rbs_types_bases_void_new(rbs_location_t *location) {
    rbs_types_bases_void_t *instance = (rbs_types_bases_void_t *)calloc(1, sizeof(rbs_types_bases_void_t));


    *instance = (rbs_types_bases_void_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BASES_VOID
        },
        .location = location,
    };

    return instance;
}

rbs_types_block_t *rbs_types_block_new(rbs_node_t *type, bool required, rbs_node_t *self_type) {
    rbs_types_block_t *instance = (rbs_types_block_t *)calloc(1, sizeof(rbs_types_block_t));


    *instance = (rbs_types_block_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_BLOCK
        },
        .type = type,
        .required = required,
        .self_type = self_type,
    };

    return instance;
}

rbs_types_classinstance_t *rbs_types_classinstance_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
    rbs_types_classinstance_t *instance = (rbs_types_classinstance_t *)calloc(1, sizeof(rbs_types_classinstance_t));


    *instance = (rbs_types_classinstance_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_CLASSINSTANCE
        },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_types_classsingleton_t *rbs_types_classsingleton_new(rbs_typename_t *name, rbs_location_t *location) {
    rbs_types_classsingleton_t *instance = (rbs_types_classsingleton_t *)calloc(1, sizeof(rbs_types_classsingleton_t));


    *instance = (rbs_types_classsingleton_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_CLASSSINGLETON
        },
        .name = name,
        .location = location,
    };

    return instance;
}

rbs_types_function_t *rbs_types_function_new(rbs_node_list_t *required_positionals, rbs_node_list_t *optional_positionals, rbs_node_t *rest_positionals, rbs_node_list_t *trailing_positionals, rbs_hash_t *required_keywords, rbs_hash_t *optional_keywords, rbs_node_t *rest_keywords, rbs_node_t *return_type) {
    rbs_types_function_t *instance = (rbs_types_function_t *)calloc(1, sizeof(rbs_types_function_t));


    *instance = (rbs_types_function_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_FUNCTION
        },
        .required_positionals = required_positionals,
        .optional_positionals = optional_positionals,
        .rest_positionals = rest_positionals,
        .trailing_positionals = trailing_positionals,
        .required_keywords = required_keywords,
        .optional_keywords = optional_keywords,
        .rest_keywords = rest_keywords,
        .return_type = return_type,
    };

    return instance;
}

rbs_types_function_param_t *rbs_types_function_param_new(rbs_node_t *type, rbs_ast_symbol_t *name, rbs_location_t *location) {
    rbs_types_function_param_t *instance = (rbs_types_function_param_t *)calloc(1, sizeof(rbs_types_function_param_t));


    *instance = (rbs_types_function_param_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_FUNCTION_PARAM
        },
        .type = type,
        .name = name,
        .location = location,
    };

    return instance;
}

rbs_types_interface_t *rbs_types_interface_new(rbs_typename_t *name, rbs_node_list_t *args, rbs_location_t *location) {
    rbs_types_interface_t *instance = (rbs_types_interface_t *)calloc(1, sizeof(rbs_types_interface_t));


    *instance = (rbs_types_interface_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_INTERFACE
        },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_types_intersection_t *rbs_types_intersection_new(rbs_node_list_t *types, rbs_location_t *location) {
    rbs_types_intersection_t *instance = (rbs_types_intersection_t *)calloc(1, sizeof(rbs_types_intersection_t));


    *instance = (rbs_types_intersection_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_INTERSECTION
        },
        .types = types,
        .location = location,
    };

    return instance;
}

rbs_types_literal_t *rbs_types_literal_new(rbs_node_t *literal, rbs_location_t *location) {
    rbs_types_literal_t *instance = (rbs_types_literal_t *)calloc(1, sizeof(rbs_types_literal_t));


    *instance = (rbs_types_literal_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_LITERAL
        },
        .literal = literal,
        .location = location,
    };

    return instance;
}

rbs_types_optional_t *rbs_types_optional_new(rbs_node_t *type, rbs_location_t *location) {
    rbs_types_optional_t *instance = (rbs_types_optional_t *)calloc(1, sizeof(rbs_types_optional_t));


    *instance = (rbs_types_optional_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_OPTIONAL
        },
        .type = type,
        .location = location,
    };

    return instance;
}

rbs_types_proc_t *rbs_types_proc_new(rbs_node_t *type, rbs_types_block_t *block, rbs_location_t *location, rbs_node_t *self_type) {
    rbs_types_proc_t *instance = (rbs_types_proc_t *)calloc(1, sizeof(rbs_types_proc_t));


    *instance = (rbs_types_proc_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_PROC
        },
        .type = type,
        .block = block,
        .location = location,
        .self_type = self_type,
    };

    return instance;
}

rbs_types_record_t *rbs_types_record_new(rbs_hash_t *all_fields, rbs_location_t *location) {
    rbs_types_record_t *instance = (rbs_types_record_t *)calloc(1, sizeof(rbs_types_record_t));


    *instance = (rbs_types_record_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_RECORD
        },
        .all_fields = all_fields,
        .location = location,
    };

    return instance;
}

rbs_types_record_fieldtype_t *rbs_types_record_fieldtype_new(rbs_node_t *type, bool required) {
    rbs_types_record_fieldtype_t *instance = (rbs_types_record_fieldtype_t *)calloc(1, sizeof(rbs_types_record_fieldtype_t));


    *instance = (rbs_types_record_fieldtype_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_RECORD_FIELDTYPE
        },
        .type = type,
        .required = required,
    };

    return instance;
}

rbs_types_tuple_t *rbs_types_tuple_new(rbs_node_list_t *types, rbs_location_t *location) {
    rbs_types_tuple_t *instance = (rbs_types_tuple_t *)calloc(1, sizeof(rbs_types_tuple_t));


    *instance = (rbs_types_tuple_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_TUPLE
        },
        .types = types,
        .location = location,
    };

    return instance;
}

rbs_types_union_t *rbs_types_union_new(rbs_node_list_t *types, rbs_location_t *location) {
    rbs_types_union_t *instance = (rbs_types_union_t *)calloc(1, sizeof(rbs_types_union_t));


    *instance = (rbs_types_union_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_UNION
        },
        .types = types,
        .location = location,
    };

    return instance;
}

rbs_types_untypedfunction_t *rbs_types_untypedfunction_new(rbs_node_t *return_type) {
    rbs_types_untypedfunction_t *instance = (rbs_types_untypedfunction_t *)calloc(1, sizeof(rbs_types_untypedfunction_t));


    *instance = (rbs_types_untypedfunction_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_UNTYPEDFUNCTION
        },
        .return_type = return_type,
    };

    return instance;
}

rbs_types_variable_t *rbs_types_variable_new(rbs_ast_symbol_t *name, rbs_location_t *location) {
    rbs_types_variable_t *instance = (rbs_types_variable_t *)calloc(1, sizeof(rbs_types_variable_t));


    *instance = (rbs_types_variable_t) {
        .base = (rbs_node_t) {
            .type = RBS_TYPES_VARIABLE
        },
        .name = name,
        .location = location,
    };

    return instance;
}

