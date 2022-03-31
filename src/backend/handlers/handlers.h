#include "backend/handlers/keywords/print.h"
#include "backend/handlers/keywords/input.h"
#include "backend/handlers/keywords/asm.h"
#include "backend/handlers/keywords/alloc.h"

#include "backend/handlers/literals/str.h"
#include "backend/handlers/literals/i32.h"

#include "backend/handlers/scoping/func.h"
#include "backend/handlers/scoping/func_call.h"
#include "backend/handlers/scoping/return.h"
#include "backend/handlers/scoping/declaration.h"
#include "backend/handlers/scoping/symbol.h"