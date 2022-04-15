#include "preprocessor.h"
#include "ast_nodes/ast_nodes.h"
#include "data_type.h"

preproc_state* preproc_state_new()
{
    preproc_state* state = malloc(sizeof(preproc_state));
    
    state->error_counter = 0;
    state->index_offset = 0;

    return state;
}
void preproc_state_free(preproc_state* state)
{
    free(state);
}

// replaces all identifiers with shared symbols
void build_symbols(preproc_state* state, ast_node* node)
{
    for (int i = 0; i < node->children->size; i++)
    {
        state->index_offset = 0;
        build_symbols(state, ast_get_child(node, i));
        i += state->index_offset;
    }

    if (node->type == AstIdentifier)
    {
        preprocess_identifier(state, node);
    }
    else if (node->type == AstInclude)
    {
        preprocess_include(state, node);
    }
}

// process all nodes (after building symbols)
void process_nodes(preproc_state* state, ast_node* node)
{
    for (int i = 0; i < node->children->size; i++)
    {
        state->index_offset = 0;
        process_nodes(state, ast_get_child(node, i));
        i += state->index_offset;
    }

    switch(node->type)
    {
        case AstRoot:
            preprocess_root(state, node);
            break;
        
        case AstBoolLit:
            preprocess_bool_lit(state, node);
            break;
        case AstFloatLit:
            preprocess_float_lit(state, node);
            break;
        case AstIntegerLit:
            preprocess_integer_lit(state, node);
            break;
        case AstStringLit:
            preprocess_string_lit(state, node);
            break;

        case AstAlloc:
            preprocess_alloc(state, node);
            break;
        case AstAssignment:
            preprocess_assignment(state, node);
            break;
        case AstDefinition:
            preprocess_definition(state, node);
            break;
        case AstFor:
            preprocess_for(state, node);
            break;
        case AstFree:
            preprocess_free(state, node);
            break;
        case AstFunction:
            preprocess_func(state, node);
            break;
        case AstFunctionCall:
            preprocess_func_call(state, node);
            break;
        case AstIf:
            preprocess_if(state, node);
            break;
        case AstInput:
            preprocess_input(state, node);
            break;
        case AstReturn:
            preprocess_return(state, node);
            break;
        case AstSymbol:
            preprocess_symbol(state, node);
            break;
        case AstWhile:
            preprocess_while(state, node);
            break;

        case AstAdd:
        case AstDiv:
        case AstMod:
        case AstMul:
        case AstPow:
        case AstSub:
        case AstBitwiseAnd:
        case AstBitwiseOr:
        case AstBitwiseXor:
        case AstShiftLeft:
        case AstShiftRight:
            preprocess_operation(state, node);
            break;

        case AstGreaterThan:
        case AstGreaterThanOrEqual:
        case AstLessThan:
        case AstLessThanOrEqual:
        case AstEqual:
        case AstNotEqual:
            preprocess_relational_expression(state, node);
            break;
        case AstAnd:
        case AstOr:
            preprocess_logical_expression(state, node);
            break;
        case AstNot:
            preprocess_not(state, node);
            break;
        default:
            break;
    }
}

void combine_modules(preproc_state* state)
{
    if (state->mod->parent != 0 || state->mod->module_store == 0)
    {
        return;
    }

    // iterate over module store in reverse to insert the deepest dependencies into the ast first
    vector* modules_vec = hashtable_to_vector(state->mod->module_store);
    for (int i = modules_vec->size - 1; i >= 0; i--)
    {
        // copy child nodes
        module* dependency = vector_get_item(modules_vec, i);
        for (int j = 0; j < dependency->ast_root->children->size; j++)
        {
            ast_node* node_copy = ast_copy(ast_get_child(dependency->ast_root, j));
            ast_insert_child(state->mod->ast_root, j, node_copy); // insert at the start of the main ast_root
        }

        // copy dependency's root symbol table
        for (int j = 0; j < dependency->ast_root->value.symbol_table->capacity; j++)
        {
            if (dependency->ast_root->value.symbol_table->keys[j] != 0)
            {
                char* key = dependency->ast_root->value.symbol_table->keys[j];
                symbol* sym = dependency->ast_root->value.symbol_table->items[j];

                hashtable_set_item(state->mod->ast_root->value.symbol_table, key, sym);
            }
        }

        // delete the dependency module
        hashtable_delete_item(state->mod->dependencies, dependency->name);
        hashtable_delete_item(state->mod->module_store, dependency->name);
        module_free(dependency, true);
    }
    vector_free(modules_vec);
}

bool preprocessor_process(module* mod)
{
    preproc_state* state = preproc_state_new();
    state->mod = mod;

    build_symbols(state, mod->ast_root);
    if (state->error_counter > 0)
	{
        goto preproc_fail;
	}

    process_nodes(state, mod->ast_root);
	if (state->error_counter > 0)
	{
        goto preproc_fail;
	}

    // combines the main and dependency ast's into one
    if (mod->parent == 0)
    {
        combine_modules(state);
    }

    preproc_state_free(state);
    return true;

preproc_fail:
	printf("total %ld preprocessor errors\n", state->error_counter);

    preproc_state_free(state);
    return false;
}