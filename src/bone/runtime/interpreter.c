#include "interpreter.h"
#include "../il/il_toplevel.h"
#include "../parse/ast.h"
#include "../parse/ast2il.h"
#include "../parse/parser.h"
#include "bool.h"
#include "enviroment.h"
#include "frame.h"
#include "integer.h"
#include "lambda.h"
#include "object.h"
#include "std.h"
#include "vm.h"

bnInterpreter* bnNewInterpreter(const char* filenameRef) {
        bnInterpreter* ret = BN_MALLOC(sizeof(bnInterpreter));
        ret->filenameRef = filenameRef;
        ret->pool = bnNewStringPool();
        ret->__return = NULL;
        ret->__exception = NULL;
        return ret;
}

int bnEval(bnInterpreter* self) {
        bnAST* ret = bnParseFile(self->pool, self->filenameRef);
        if (ret == NULL) {
                // fail parse
                return 1;
        }
        bnILToplevel* iltop = bnAST2IL(ret);
        if (iltop == NULL) {
                // fail compile
                return 1;
        }
        // generate instructions
        bnEnviroment* env = bnNewEnviroment();
        bnFrame* frame = bnNewFrame();
        // declare true, false
        g_hash_table_insert(frame->variableTable, bnIntern(self->pool, "true"),
                            bnNewBool(self, true));
        g_hash_table_insert(frame->variableTable, bnIntern(self->pool, "false"),
                            bnNewBool(self, false));
#if DEBUG
        g_hash_table_insert(
            frame->variableTable, bnIntern(self->pool, "assert"),
            bnNewLambdaFromCFunc(bnStdDebugAssert, self->pool, BN_C_ADD_PARAM,
                                 "cond", BN_C_ADD_EXIT));
        g_hash_table_insert(
            frame->variableTable, bnIntern(self->pool, "die"),
            bnNewLambdaFromCFunc(bnStdDebugDie, self->pool, BN_C_ADD_EXIT));
#endif
        g_hash_table_insert(
            frame->variableTable, bnIntern(self->pool, "print"),
            bnNewLambdaFromCFunc(bnStdSystemPrint, self->pool, BN_C_ADD_PARAM,
                                 "str", BN_C_ADD_EXIT));
        g_hash_table_insert(
            frame->variableTable, bnIntern(self->pool, "println"),
            bnNewLambdaFromCFunc(bnStdSystemPrintln, self->pool, BN_C_ADD_PARAM,
                                 "str", BN_C_ADD_EXIT));
        g_hash_table_insert(
            frame->variableTable, bnIntern(self->pool, "object"),
            bnNewLambdaFromCFunc(bnStdSystemObject, self->pool, BN_C_ADD_RETURN,
                                 "ret", BN_C_ADD_EXIT));
        bnGenerateILTopLevel(self, iltop, env);
        bnDeleteAST(ret);
        bnExecute(self, env, frame);
        bnDeleteILTopLevel(iltop);
        self->__return = bnReturnValue(frame);
        return 0;
}

void bnPanic(bnInterpreter* self, bnObject* exception, int code) {
        self->__exception = exception;
        longjmp(self->__jmp, code);
}

bnObject* bnGetBool(struct bnStringPool* pool, bnFrame* frame, bool cond) {
        bnObject* ret = cond ? bnGetTrue(pool, frame) : bnGetFalse(pool, frame);
        assert(ret != NULL);
        return ret;
}

bnObject* bnGetTrue(struct bnStringPool* pool, bnFrame* frame) {
        return g_hash_table_lookup(frame->variableTable,
                                   bnIntern(pool, "true"));
}

bnObject* bnGetFalse(struct bnStringPool* pool, bnFrame* frame) {
        return g_hash_table_lookup(frame->variableTable,
                                   bnIntern(pool, "false"));
}

void bnDeleteInterpreter(bnInterpreter* self) {
        bnDeleteStringPool(self->pool);
        BN_FREE(self);
}