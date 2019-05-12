#include "il_stmt_defer.h"
#include "../runtime/enviroment.h"

bnILStmtDefer* bnNewILStmtDefer(bnILStatement* stmt) {
        bnILStmtDefer* ret = BN_MALLOC(sizeof(bnILStmtDefer));
        ret->stmt = stmt;
        return ret;
}

void bnDumpILStmtDefer(FILE* fp, struct bnStringPool* pool, bnILStmtDefer* self,
                       int depth) {
        bnFindent(fp, depth);
        fprintf(fp, "defer\n");
        bnDumpILStatement(fp, pool, self->stmt, depth + 1);
}

void bnGenerateILStmtDefer(struct bnInterpreter* bone, bnILStmtDefer* self,
                           bnEnviroment* env) {
        bnLabel* lab = bnAutoNewLabel(env, 0);
        g_ptr_array_add(env->codeArray, BN_OP_DEFER_PUSH);
        g_ptr_array_add(env->codeArray, lab);
        g_ptr_array_add(env->codeArray, BN_OP_DEFER_BEGIN);
        lab->pos = bnGenerateNOP(env) - bnGetLambdaOffset(env);
        bnGenerateNOP(env);
        bnGenerateILStatement(bone, self->stmt, env);
        g_ptr_array_add(env->codeArray, BN_OP_DEFER_NEXT);
        g_ptr_array_add(env->codeArray, BN_OP_DEFER_END);
}

void bnDeleteILStmtDefer(bnILStmtDefer* self) {
        bnDeleteILStatement(self->stmt);
        BN_FREE(self);
}