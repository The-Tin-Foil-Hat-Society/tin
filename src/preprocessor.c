#include "preprocessor.h"
#include "ast_nodes/ast_nodes.h"
#include "typechecking.h"

preproc_state* preproc_state_new()
{
    preproc_state* state = malloc(sizeof(preproc_state));
    
    state->error_counter = 0;

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
        build_symbols(state, ast_get_child(node, i));
    }

    if (node->type == AstIdentifier)
    {
        preprocess_identifier(state, node);
    }
}

// process all nodes (after building symbols)
void process_nodes(preproc_state* state, ast_node* node)
{
    for (int i = 0; i < node->children->size; i++)
    {
        process_nodes(state, ast_get_child(node, i));
    }

    switch(node->type)
    {
        case AstRoot:
            preprocess_root(state, node);
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
        case AstInput:
            preprocess_input(state, node);
            break;
        case AstIntegerLit:
            preprocess_integer_lit(state, node);
            break;
        case AstStringLit:
            preprocess_string_lit(state, node);
            break;
        case AstSymbol:
            preprocess_symbol(state, node);
            break;
        default:
            break;
    }
}

bool preprocessor_process(module* mod, ast_node* node)
{
    preproc_state* state = preproc_state_new();
    state->mod = mod;

    build_symbols(state, mod->ast_root);
    process_nodes(state, mod->ast_root);

	if (state->error_counter > 0)
	{
		printf("total %ld preprocessor errors\n", state->error_counter);
        preproc_state_free(state);
		return false;
	}

    preproc_state_free(state);
    return true;
}