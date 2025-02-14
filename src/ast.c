/*----------------------------------------------------------------------------*/
/* This file is generated by the templates/template.rb script and should not  */
/* be modified manually.                                                      */
/* To change the template see                                                 */
/* templates/src/ast.c.erb                                                    */
/*----------------------------------------------------------------------------*/

#include "rbs/ast.h"
#include <stdlib.h>

rbs_ast_annotation_t *rbs_ast_annotation_new(VALUE ruby_value, VALUE string, VALUE location) {
    rbs_ast_annotation_t *instance = malloc(sizeof(rbs_ast_annotation_t));

    *instance = (rbs_ast_annotation_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .string = string,
        .location = location,
    };

    return instance;
}

rbs_ast_comment_t *rbs_ast_comment_new(VALUE ruby_value, VALUE string, VALUE location) {
    rbs_ast_comment_t *instance = malloc(sizeof(rbs_ast_comment_t));

    *instance = (rbs_ast_comment_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .string = string,
        .location = location,
    };

    return instance;
}

rbs_ast_declarations_class_t *rbs_ast_declarations_class_new(VALUE ruby_value, VALUE name, VALUE type_params, VALUE super_class, VALUE members, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_declarations_class_t *instance = malloc(sizeof(rbs_ast_declarations_class_t));

    *instance = (rbs_ast_declarations_class_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_ast_declarations_class_super_t *rbs_ast_declarations_class_super_new(VALUE ruby_value, VALUE name, VALUE args, VALUE location) {
    rbs_ast_declarations_class_super_t *instance = malloc(sizeof(rbs_ast_declarations_class_super_t));

    *instance = (rbs_ast_declarations_class_super_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_ast_declarations_classalias_t *rbs_ast_declarations_classalias_new(VALUE ruby_value, VALUE new_name, VALUE old_name, VALUE location, VALUE comment) {
    rbs_ast_declarations_classalias_t *instance = malloc(sizeof(rbs_ast_declarations_classalias_t));

    *instance = (rbs_ast_declarations_classalias_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .new_name = new_name,
        .old_name = old_name,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_constant_t *rbs_ast_declarations_constant_new(VALUE ruby_value, VALUE name, VALUE type, VALUE location, VALUE comment) {
    rbs_ast_declarations_constant_t *instance = malloc(sizeof(rbs_ast_declarations_constant_t));

    *instance = (rbs_ast_declarations_constant_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_global_t *rbs_ast_declarations_global_new(VALUE ruby_value, VALUE name, VALUE type, VALUE location, VALUE comment) {
    rbs_ast_declarations_global_t *instance = malloc(sizeof(rbs_ast_declarations_global_t));

    *instance = (rbs_ast_declarations_global_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_interface_t *rbs_ast_declarations_interface_new(VALUE ruby_value, VALUE name, VALUE type_params, VALUE members, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_declarations_interface_t *instance = malloc(sizeof(rbs_ast_declarations_interface_t));

    *instance = (rbs_ast_declarations_interface_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type_params = type_params,
        .members = members,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_module_t *rbs_ast_declarations_module_new(VALUE ruby_value, VALUE name, VALUE type_params, VALUE self_types, VALUE members, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_declarations_module_t *instance = malloc(sizeof(rbs_ast_declarations_module_t));

    *instance = (rbs_ast_declarations_module_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_ast_declarations_module_self_t *rbs_ast_declarations_module_self_new(VALUE ruby_value, VALUE name, VALUE args, VALUE location) {
    rbs_ast_declarations_module_self_t *instance = malloc(sizeof(rbs_ast_declarations_module_self_t));

    *instance = (rbs_ast_declarations_module_self_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_ast_declarations_modulealias_t *rbs_ast_declarations_modulealias_new(VALUE ruby_value, VALUE new_name, VALUE old_name, VALUE location, VALUE comment) {
    rbs_ast_declarations_modulealias_t *instance = malloc(sizeof(rbs_ast_declarations_modulealias_t));

    *instance = (rbs_ast_declarations_modulealias_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .new_name = new_name,
        .old_name = old_name,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_declarations_typealias_t *rbs_ast_declarations_typealias_new(VALUE ruby_value, VALUE name, VALUE type_params, VALUE type, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_declarations_typealias_t *instance = malloc(sizeof(rbs_ast_declarations_typealias_t));

    *instance = (rbs_ast_declarations_typealias_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type_params = type_params,
        .type = type,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_directives_use_t *rbs_ast_directives_use_new(VALUE ruby_value, VALUE clauses, VALUE location) {
    rbs_ast_directives_use_t *instance = malloc(sizeof(rbs_ast_directives_use_t));

    *instance = (rbs_ast_directives_use_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .clauses = clauses,
        .location = location,
    };

    return instance;
}

rbs_ast_directives_use_singleclause_t *rbs_ast_directives_use_singleclause_new(VALUE ruby_value, VALUE type_name, VALUE new_name, VALUE location) {
    rbs_ast_directives_use_singleclause_t *instance = malloc(sizeof(rbs_ast_directives_use_singleclause_t));

    *instance = (rbs_ast_directives_use_singleclause_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .type_name = type_name,
        .new_name = new_name,
        .location = location,
    };

    return instance;
}

rbs_ast_directives_use_wildcardclause_t *rbs_ast_directives_use_wildcardclause_new(VALUE ruby_value, VALUE namespace, VALUE location) {
    rbs_ast_directives_use_wildcardclause_t *instance = malloc(sizeof(rbs_ast_directives_use_wildcardclause_t));

    *instance = (rbs_ast_directives_use_wildcardclause_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .namespace = namespace,
        .location = location,
    };

    return instance;
}

rbs_ast_members_alias_t *rbs_ast_members_alias_new(VALUE ruby_value, VALUE new_name, VALUE old_name, VALUE kind, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_members_alias_t *instance = malloc(sizeof(rbs_ast_members_alias_t));

    *instance = (rbs_ast_members_alias_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .new_name = new_name,
        .old_name = old_name,
        .kind = kind,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_attraccessor_t *rbs_ast_members_attraccessor_new(VALUE ruby_value, VALUE name, VALUE type, VALUE ivar_name, VALUE kind, VALUE annotations, VALUE location, VALUE comment, VALUE visibility) {
    rbs_ast_members_attraccessor_t *instance = malloc(sizeof(rbs_ast_members_attraccessor_t));

    *instance = (rbs_ast_members_attraccessor_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_ast_members_attrreader_t *rbs_ast_members_attrreader_new(VALUE ruby_value, VALUE name, VALUE type, VALUE ivar_name, VALUE kind, VALUE annotations, VALUE location, VALUE comment, VALUE visibility) {
    rbs_ast_members_attrreader_t *instance = malloc(sizeof(rbs_ast_members_attrreader_t));

    *instance = (rbs_ast_members_attrreader_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_ast_members_attrwriter_t *rbs_ast_members_attrwriter_new(VALUE ruby_value, VALUE name, VALUE type, VALUE ivar_name, VALUE kind, VALUE annotations, VALUE location, VALUE comment, VALUE visibility) {
    rbs_ast_members_attrwriter_t *instance = malloc(sizeof(rbs_ast_members_attrwriter_t));

    *instance = (rbs_ast_members_attrwriter_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_ast_members_classinstancevariable_t *rbs_ast_members_classinstancevariable_new(VALUE ruby_value, VALUE name, VALUE type, VALUE location, VALUE comment) {
    rbs_ast_members_classinstancevariable_t *instance = malloc(sizeof(rbs_ast_members_classinstancevariable_t));

    *instance = (rbs_ast_members_classinstancevariable_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_classvariable_t *rbs_ast_members_classvariable_new(VALUE ruby_value, VALUE name, VALUE type, VALUE location, VALUE comment) {
    rbs_ast_members_classvariable_t *instance = malloc(sizeof(rbs_ast_members_classvariable_t));

    *instance = (rbs_ast_members_classvariable_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_extend_t *rbs_ast_members_extend_new(VALUE ruby_value, VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_members_extend_t *instance = malloc(sizeof(rbs_ast_members_extend_t));

    *instance = (rbs_ast_members_extend_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_include_t *rbs_ast_members_include_new(VALUE ruby_value, VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_members_include_t *instance = malloc(sizeof(rbs_ast_members_include_t));

    *instance = (rbs_ast_members_include_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_instancevariable_t *rbs_ast_members_instancevariable_new(VALUE ruby_value, VALUE name, VALUE type, VALUE location, VALUE comment) {
    rbs_ast_members_instancevariable_t *instance = malloc(sizeof(rbs_ast_members_instancevariable_t));

    *instance = (rbs_ast_members_instancevariable_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .type = type,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_methoddefinition_t *rbs_ast_members_methoddefinition_new(VALUE ruby_value, VALUE name, VALUE kind, VALUE overloads, VALUE annotations, VALUE location, VALUE comment, VALUE overloading, VALUE visibility) {
    rbs_ast_members_methoddefinition_t *instance = malloc(sizeof(rbs_ast_members_methoddefinition_t));

    *instance = (rbs_ast_members_methoddefinition_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_ast_members_methoddefinition_overload_t *rbs_ast_members_methoddefinition_overload_new(VALUE ruby_value, VALUE annotations, VALUE method_type) {
    rbs_ast_members_methoddefinition_overload_t *instance = malloc(sizeof(rbs_ast_members_methoddefinition_overload_t));

    *instance = (rbs_ast_members_methoddefinition_overload_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .annotations = annotations,
        .method_type = method_type,
    };

    return instance;
}

rbs_ast_members_prepend_t *rbs_ast_members_prepend_new(VALUE ruby_value, VALUE name, VALUE args, VALUE annotations, VALUE location, VALUE comment) {
    rbs_ast_members_prepend_t *instance = malloc(sizeof(rbs_ast_members_prepend_t));

    *instance = (rbs_ast_members_prepend_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .annotations = annotations,
        .location = location,
        .comment = comment,
    };

    return instance;
}

rbs_ast_members_private_t *rbs_ast_members_private_new(VALUE ruby_value, VALUE location) {
    rbs_ast_members_private_t *instance = malloc(sizeof(rbs_ast_members_private_t));

    *instance = (rbs_ast_members_private_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_ast_members_public_t *rbs_ast_members_public_new(VALUE ruby_value, VALUE location) {
    rbs_ast_members_public_t *instance = malloc(sizeof(rbs_ast_members_public_t));

    *instance = (rbs_ast_members_public_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_ast_typeparam_t *rbs_ast_typeparam_new(VALUE ruby_value, VALUE name, VALUE variance, VALUE upper_bound, VALUE default_type, VALUE unchecked, VALUE location) {
    rbs_ast_typeparam_t *instance = malloc(sizeof(rbs_ast_typeparam_t));

    *instance = (rbs_ast_typeparam_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .variance = variance,
        .upper_bound = upper_bound,
        .default_type = default_type,
        .unchecked = unchecked,
        .location = location,
    };

    return instance;
}

rbs_methodtype_t *rbs_methodtype_new(VALUE ruby_value, VALUE type_params, VALUE type, VALUE block, VALUE location) {
    rbs_methodtype_t *instance = malloc(sizeof(rbs_methodtype_t));

    *instance = (rbs_methodtype_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .type_params = type_params,
        .type = type,
        .block = block,
        .location = location,
    };

    return instance;
}

rbs_namespace_t *rbs_namespace_new(VALUE ruby_value, VALUE path, VALUE absolute) {
    rbs_namespace_t *instance = malloc(sizeof(rbs_namespace_t));

    *instance = (rbs_namespace_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .path = path,
        .absolute = absolute,
    };

    return instance;
}

rbs_typename_t *rbs_typename_new(VALUE ruby_value, VALUE namespace, VALUE name) {
    rbs_typename_t *instance = malloc(sizeof(rbs_typename_t));

    *instance = (rbs_typename_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .namespace = namespace,
        .name = name,
    };

    return instance;
}

rbs_types_alias_t *rbs_types_alias_new(VALUE ruby_value, VALUE name, VALUE args, VALUE location) {
    rbs_types_alias_t *instance = malloc(sizeof(rbs_types_alias_t));

    *instance = (rbs_types_alias_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_types_bases_any_t *rbs_types_bases_any_new(VALUE ruby_value, VALUE todo, VALUE location) {
    rbs_types_bases_any_t *instance = malloc(sizeof(rbs_types_bases_any_t));

    *instance = (rbs_types_bases_any_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .todo = todo,
        .location = location,
    };

    return instance;
}

rbs_types_bases_bool_t *rbs_types_bases_bool_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_bool_t *instance = malloc(sizeof(rbs_types_bases_bool_t));

    *instance = (rbs_types_bases_bool_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_bottom_t *rbs_types_bases_bottom_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_bottom_t *instance = malloc(sizeof(rbs_types_bases_bottom_t));

    *instance = (rbs_types_bases_bottom_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_class_t *rbs_types_bases_class_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_class_t *instance = malloc(sizeof(rbs_types_bases_class_t));

    *instance = (rbs_types_bases_class_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_instance_t *rbs_types_bases_instance_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_instance_t *instance = malloc(sizeof(rbs_types_bases_instance_t));

    *instance = (rbs_types_bases_instance_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_nil_t *rbs_types_bases_nil_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_nil_t *instance = malloc(sizeof(rbs_types_bases_nil_t));

    *instance = (rbs_types_bases_nil_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_self_t *rbs_types_bases_self_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_self_t *instance = malloc(sizeof(rbs_types_bases_self_t));

    *instance = (rbs_types_bases_self_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_top_t *rbs_types_bases_top_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_top_t *instance = malloc(sizeof(rbs_types_bases_top_t));

    *instance = (rbs_types_bases_top_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_bases_void_t *rbs_types_bases_void_new(VALUE ruby_value, VALUE location) {
    rbs_types_bases_void_t *instance = malloc(sizeof(rbs_types_bases_void_t));

    *instance = (rbs_types_bases_void_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .location = location,
    };

    return instance;
}

rbs_types_block_t *rbs_types_block_new(VALUE ruby_value, VALUE type, VALUE required, VALUE self_type) {
    rbs_types_block_t *instance = malloc(sizeof(rbs_types_block_t));

    *instance = (rbs_types_block_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .type = type,
        .required = required,
        .self_type = self_type,
    };

    return instance;
}

rbs_types_classinstance_t *rbs_types_classinstance_new(VALUE ruby_value, VALUE name, VALUE args, VALUE location) {
    rbs_types_classinstance_t *instance = malloc(sizeof(rbs_types_classinstance_t));

    *instance = (rbs_types_classinstance_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_types_classsingleton_t *rbs_types_classsingleton_new(VALUE ruby_value, VALUE name, VALUE location) {
    rbs_types_classsingleton_t *instance = malloc(sizeof(rbs_types_classsingleton_t));

    *instance = (rbs_types_classsingleton_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .location = location,
    };

    return instance;
}

rbs_types_function_t *rbs_types_function_new(VALUE ruby_value, VALUE required_positionals, VALUE optional_positionals, VALUE rest_positionals, VALUE trailing_positionals, VALUE required_keywords, VALUE optional_keywords, VALUE rest_keywords, VALUE return_type) {
    rbs_types_function_t *instance = malloc(sizeof(rbs_types_function_t));

    *instance = (rbs_types_function_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
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

rbs_types_function_param_t *rbs_types_function_param_new(VALUE ruby_value, VALUE type, VALUE name, VALUE location) {
    rbs_types_function_param_t *instance = malloc(sizeof(rbs_types_function_param_t));

    *instance = (rbs_types_function_param_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .type = type,
        .name = name,
        .location = location,
    };

    return instance;
}

rbs_types_interface_t *rbs_types_interface_new(VALUE ruby_value, VALUE name, VALUE args, VALUE location) {
    rbs_types_interface_t *instance = malloc(sizeof(rbs_types_interface_t));

    *instance = (rbs_types_interface_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .args = args,
        .location = location,
    };

    return instance;
}

rbs_types_intersection_t *rbs_types_intersection_new(VALUE ruby_value, VALUE types, VALUE location) {
    rbs_types_intersection_t *instance = malloc(sizeof(rbs_types_intersection_t));

    *instance = (rbs_types_intersection_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .types = types,
        .location = location,
    };

    return instance;
}

rbs_types_literal_t *rbs_types_literal_new(VALUE ruby_value, VALUE literal, VALUE location) {
    rbs_types_literal_t *instance = malloc(sizeof(rbs_types_literal_t));

    *instance = (rbs_types_literal_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .literal = literal,
        .location = location,
    };

    return instance;
}

rbs_types_optional_t *rbs_types_optional_new(VALUE ruby_value, VALUE type, VALUE location) {
    rbs_types_optional_t *instance = malloc(sizeof(rbs_types_optional_t));

    *instance = (rbs_types_optional_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .type = type,
        .location = location,
    };

    return instance;
}

rbs_types_proc_t *rbs_types_proc_new(VALUE ruby_value, VALUE type, VALUE block, VALUE location, VALUE self_type) {
    rbs_types_proc_t *instance = malloc(sizeof(rbs_types_proc_t));

    *instance = (rbs_types_proc_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .type = type,
        .block = block,
        .location = location,
        .self_type = self_type,
    };

    return instance;
}

rbs_types_record_t *rbs_types_record_new(VALUE ruby_value, VALUE all_fields, VALUE location) {
    rbs_types_record_t *instance = malloc(sizeof(rbs_types_record_t));

    *instance = (rbs_types_record_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .all_fields = all_fields,
        .location = location,
    };

    return instance;
}

rbs_types_tuple_t *rbs_types_tuple_new(VALUE ruby_value, VALUE types, VALUE location) {
    rbs_types_tuple_t *instance = malloc(sizeof(rbs_types_tuple_t));

    *instance = (rbs_types_tuple_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .types = types,
        .location = location,
    };

    return instance;
}

rbs_types_union_t *rbs_types_union_new(VALUE ruby_value, VALUE types, VALUE location) {
    rbs_types_union_t *instance = malloc(sizeof(rbs_types_union_t));

    *instance = (rbs_types_union_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .types = types,
        .location = location,
    };

    return instance;
}

rbs_types_untypedfunction_t *rbs_types_untypedfunction_new(VALUE ruby_value, VALUE return_type) {
    rbs_types_untypedfunction_t *instance = malloc(sizeof(rbs_types_untypedfunction_t));

    *instance = (rbs_types_untypedfunction_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .return_type = return_type,
    };

    return instance;
}

rbs_types_variable_t *rbs_types_variable_new(VALUE ruby_value, VALUE name, VALUE location) {
    rbs_types_variable_t *instance = malloc(sizeof(rbs_types_variable_t));

    *instance = (rbs_types_variable_t) {
        .base = (rbs_node_t) { .cached_ruby_value = ruby_value },
        .name = name,
        .location = location,
    };

    return instance;
}

