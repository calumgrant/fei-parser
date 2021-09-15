/*
    These "type tags" are used internally by the template profiler
*/

namespace cellar
{
    struct add_to_closure_tag;
    struct build_closure_tag;
    struct closure_tag;
    struct token_tag;
    struct rule_tag;
    struct rule_position_tag;
    struct no_tag;
    struct typeset_tag;
    struct typeset_insert_tag;
    struct typeset_union_tag;
    struct typeset_contains_tag;
    struct typeset_size_tag;
    struct typeset_sort_tag;
    struct typeset_sorted_insert_tag;
    struct typeset_sorted_union_tag;
    struct follow_tag;
    struct first_tag;
    struct potentially_empty_symbol_tag;
    struct getnext_tag;
    struct expand_symbol_tag;
    struct build_next_token_list_tag;
    struct get_next_token_tag;
}