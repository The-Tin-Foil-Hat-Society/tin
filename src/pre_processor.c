#include "pre_processor.h"
#include "typechecking.h"

int build_symbols(module* mod, ast_node* node)
{
    int error_counter = 0;

    for (int i = 0; i < node->children->size; i++)
    {
        error_counter += build_symbols(mod, ast_get_child(node, i));
    }

    if (node->type == AstRoot && ast_find_symbol(node, "main") == 0)
    {
        printf("error: no main function defined\n"); 
        error_counter += 1;
    }

    int index_in_parent = 0;
    if (node->parent != 0)
    {
        index_in_parent = ast_get_child_index(node->parent, node);
    }

    if (node->type == AstIdentifier)
    {
        bool is_function_argument_identifier = node->parent->type == AstDefinition && node->parent->parent != 0 && node->parent->parent->type == AstDefinitionList && node->parent->parent->parent->type == AstFunction;
        bool is_being_assigned_to = (node->parent->type == AstAlloc || node->parent->type == AstDefinition || node->parent->type == AstInput || (node->parent->type == AstAssignment && index_in_parent == 0));

        hashtable* table;
        symbol* sym;

        if (is_function_argument_identifier)
        {
            table = ast_get_child(node->parent->parent->parent, 2)->value.symbol_table; // scope should always be the 3rd child as per the grammar if there's also a definition list
            // for definitions for function args, make sure they're in the function's scope and not outside it
            sym = hashtable_get_item(table, node->value.string);
        }
        else
        {
            sym = ast_find_symbol(node, node->value.string);
        }

        if (sym != 0 && node->parent->type == AstDefinition)
        {
            // TODO: create a specific function for errors to print all the required information and to quit the compiler correctly?
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s is already defined\n", node->value.string); 
            error_counter += 1;
        }
        else if (sym == 0 && is_being_assigned_to)
        {
            sym = symbol_new();
            sym->name = strdup(node->value.string);

            if (!is_function_argument_identifier)
            {
                table = ast_get_closest_symtable(node);
            }
            else
            {
                sym->is_initialised = true; // if its a function argument, assume it's already initialized. actually check for that during function calls not in function definitions
            }
            
            hashtable_set_item(table, node->value.string, sym);
        }
        else if (sym == 0)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s undefined\n", node->value.string); 
            error_counter += 1;
        }
        else if (sym->is_initialised == false && !is_being_assigned_to)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s is not initialized\n", node->value.string);
            error_counter += 1;
        }
        
        ast_node* symbol_node = ast_new(AstSymbol);
        symbol_node->value.symbol = sym;

        // replace the indentifier node with a symbol node
        ast_set_child(node->parent, index_in_parent, symbol_node);
        ast_free(node);
    }
    else if (node->type == AstDefinition) 
    {
        ast_node* data_type = ast_get_child(node, 0);
        ast_node* symbol_node = ast_get_child(node, 1);
        symbol* sym = symbol_node->value.symbol;

        sym->data_type = strdup(data_type->value.string);
        sym->pointer_level = data_type->pointer_level;

        if (node->parent->type == AstFunction)
        {
            sym->is_function = true;
            sym->is_initialised = true;
            sym->value = node->parent;
        }

        if (node->type == AstDefinition && node->parent->type != AstRoot && node->parent->type != AstScope)
        {
            // definition node no longer needed, move up the symbol in its place
            // interpreter and codegen should make definitions from the symbol tables at the start of the program/scope
            symbol_node = ast_copy(symbol_node);
            
            ast_set_child(node->parent, index_in_parent, symbol_node);
            ast_free(node);
        }
    }
    else if (node->type == AstAssignment || node->type == AstDeclaration || node->type == AstAlloc)
    {
        symbol* left_sym = ast_get_child(node, 0)->value.symbol; // 1st child should always be an identifier/symbol
        ast_node* right_node = ast_get_child(node, 1);

        if (node->type == AstAlloc && left_sym->pointer_level < 1)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("warning: %s must have a pointer type \n", left_sym->name);
        }

        if (right_node->type == AstSymbol)
        {
            symbol* right_sym = right_node->value.symbol;
            if (node->type == AstAlloc)
            {
                if (!is_int(right_sym->data_type) || right_sym->pointer_level > 0)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: size must be an integer type\n");
                    error_counter += 1;
                }
            }
            else 
            {
                if (strcmp(left_sym->data_type, right_sym->data_type) != 0)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: %s has type %s while %s has type %s\n", left_sym->name, left_sym->data_type, right_sym->name, right_sym->data_type);
                    error_counter += 1;
                }
                else if (left_sym->pointer_level != right_sym->pointer_level)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("warning: %s is a level %ld pointer while %s is a level %ld pointer\n", left_sym->name, left_sym->pointer_level, right_sym->name, right_sym->pointer_level);
                }
            }
        }
        else if (right_node->type == AstIntegerLit)
        {
            if (node->type == AstAlloc)
            {
                if (right_node->value.integer < 0)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: cannot allocate less than 0 bytes\n");
                    error_counter += 1;
                }
                else if (right_node->value.integer > UINT32_MAX)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: cannot allocate more than u32 max bytes\n");
                    error_counter += 1;
                }   
            }
            else if(!is_valid_int(left_sym->data_type, right_node->value.integer))
            {
                printf("%s\n", ast_find_closest_src_line(node));
                printf("error: value does not fit the data type of variable %s\n", left_sym->name);
                error_counter += 1;
            }
            
        }
        else if (right_node->type == AstStringLit)
        {
            if (node->type == AstAlloc)
            {
                printf("%s\n", ast_find_closest_src_line(node));
                printf("error: size must be an integer type\n");
                error_counter += 1;
            }
            else
            {
                if (strcmp(left_sym->data_type, "u8") != 0 || left_sym->pointer_level != 1)  // must be (ptr u8)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: %s must be of type ptr u8\n", left_sym->name);
                    error_counter += 1;
                }

                left_sym->value = strdup(right_node->value.string);
            }
        }
        else
        {
            // in any other case, just look for the closest data type node, expressions should already be validated to have the same data type for all values
            char* found_type = 0;
            size_t found_pointer_level = 0;

            ast_node* current_node = right_node;
            while (current_node != 0)
            {
                current_node = ast_get_child(current_node, 0);

                if (current_node->type == AstDataType)
                {
                    found_type = current_node->value.string;
                    found_pointer_level = current_node->pointer_level;
                    break;
                }
                else if (current_node->type == AstSymbol)
                {
                    found_type = current_node->value.symbol->data_type;
                    found_pointer_level = current_node->value.symbol->pointer_level;
                    break;
                }
            }

            if (found_type == 0)
            {
                printf("%s\n", ast_find_closest_src_line(node));
                printf("error: %s, could not determine type of the right hand value (this should not happen!! parser bug)\n", left_sym->name);
                error_counter += 1;
            }

            if (node->type == AstAlloc)
            {
                if (!is_int(found_type) || found_pointer_level > 0)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: size must be an integer type\n");
                    error_counter += 1;
                }
            }
            else
            {
                if (strcmp(left_sym->data_type, found_type) != 0)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("error: %s has type %s while the right hand value has type %s\n", left_sym->name, left_sym->data_type, found_type);
                    error_counter += 1;
                }
                if (left_sym->pointer_level != found_pointer_level)
                {
                    printf("%s\n", ast_find_closest_src_line(node));
                    printf("warning: %s is a level %ld pointer while the right hand value is a level %ld pointer\n", left_sym->name, left_sym->pointer_level, found_pointer_level);
                }
            }
        }
        
        left_sym->is_initialised = true;
    }
    else if (node->type == AstInput)
    {
        ast_get_child(node, 0)->value.symbol->is_initialised = true;
    }

    return error_counter;
}

