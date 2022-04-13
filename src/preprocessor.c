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

    preproc_state_free(state);
    return true;

preproc_fail:
	printf("total %ld preprocessor errors\n", state->error_counter);

    preproc_state_free(state);
    return false;
}