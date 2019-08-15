#include "reflection.h"
#include "../../bone.h"
#include "../array.h"
#include "../char.h"
#include "../frame.h"
#include "../integer.h"
#include "../interpreter.h"
#include "../lambda.h"
#include "../string.h"

void bnExternReflection(bnInterpreter* bone) {
        bnWriteExtern2(
            bone, "define",
            bnNewLambdaFromCFunc(bone, bnExtReflectionDefine, bone->pool,
                                 BN_C_ADD_PARAM, "self", BN_C_ADD_PARAM, "name",
                                 BN_C_ADD_PARAM, "value", BN_C_ADD_EXIT));
        bnWriteExtern2(
            bone, "undef",
            bnNewLambdaFromCFunc(bone, bnExtReflectionUndef, bone->pool,
                                 BN_C_ADD_PARAM, "self", BN_C_ADD_PARAM, "name",
                                 BN_C_ADD_RETURN, "ret", BN_C_ADD_EXIT));
        bnWriteExtern2(
            bone, "defined",
            bnNewLambdaFromCFunc(bone, bnExtReflectionDefined, bone->pool,
                                 BN_C_ADD_PARAM, "self", BN_C_ADD_PARAM, "name",
                                 BN_C_ADD_RETURN, "ret", BN_C_ADD_EXIT));
        bnWriteExtern2(
            bone, "expand",
            bnNewLambdaFromCFunc(bone, bnExtReflectionExpand, bone->pool,
                                 BN_C_ADD_PARAM, "self", BN_C_ADD_PARAM, "name",
                                 BN_C_ADD_RETURN, "ret", BN_C_ADD_RETURN,
                                 "error", BN_C_ADD_EXIT));
        bnWriteExtern2(
            bone, "entries",
            bnNewLambdaFromCFunc(bone, bnExtReflectionEntries, bone->pool,
                                 BN_C_ADD_PARAM, "self", BN_C_ADD_RETURN, "ret",
                                 BN_C_ADD_EXIT));
        bnWriteExtern2(
            bone, "export",
            bnNewLambdaFromCFunc(bone, bnExtReflectionExport, bone->pool,
                                 BN_C_ADD_PARAM, "self", BN_C_ADD_RETURN, "...",
                                 BN_C_ADD_EXIT));
}

void bnExtReflectionDefine(bnInterpreter* bone, bnFrame* frame) {
        bnObject* a = bnPopStack(frame->vStack);
        bnObject* b = bnPopStack(frame->vStack);
        bnObject* c = bnPopStack(frame->vStack);
        if (b->type != BN_OBJECT_STRING) {
                bnFormatThrow(bone, "shoud be `name` is string");
        }
        bnDefine(a, bnGetStringValue(b), c);
}

void bnExtReflectionUndef(bnInterpreter* bone, bnFrame* frame) {
        bnObject* a = bnPopStack(frame->vStack);
        bnObject* b = bnPopStack(frame->vStack);
        if (b->type != BN_OBJECT_STRING) {
                bnFormatThrow(bone, "shoud be `name` is string");
        }
        bool removed = bnUndef(a, bnGetStringValue(b));
        bnWriteVariable2(frame, bone->pool, "ret",
                         bnGetBool(bone->pool, frame, removed));
}

void bnExtReflectionDefined(bnInterpreter* bone, bnFrame* frame) {
        bnObject* a = bnPopStack(frame->vStack);
        bnObject* b = bnPopStack(frame->vStack);
        if (b->type != BN_OBJECT_STRING) {
                bnFormatThrow(bone, "shoud be `name` is string");
        }
        bool contains = bnDefined(a, bnGetStringValue(b));
        bnWriteVariable2(frame, bone->pool, "ret",
                         bnGetBool(bone->pool, frame, contains));
}

void bnExtReflectionExpand(bnInterpreter* bone, bnFrame* frame) {
        bnObject* a = bnPopStack(frame->vStack);
        bnObject* b = bnPopStack(frame->vStack);
        if (b->type != BN_OBJECT_STRING) {
                bnFormatThrow(bone, "shoud be `name` is string");
        }
        bool contains = bnDefined(a, bnGetStringValue(b));
        bnWriteVariable2(frame, bone->pool, "error",
                         bnGetBool(bone->pool, frame, !contains));
        if (contains) {
                bnWriteVariable2(frame, bone->pool, "ret",
                                 bnLookup(a, bnGetStringValue(b)));
        }
}

void bnExtReflectionEntries(bnInterpreter* bone, bnFrame* frame) {
        bnObject* a = bnPopStack(frame->vStack);
        GList* entries = NULL;
        GHashTableIter hashIter;
        gpointer k, v;
        g_hash_table_iter_init(&hashIter, a->table);
        while (g_hash_table_iter_next(&hashIter, &k, &v)) {
                const char* kstr = bnView2Str(bone->pool, (bnStringView)k);
                if (*kstr == '$') {
                        continue;
                }
                entries = g_list_append(entries, k);
        }
        bnObject* arrayFunc = bnReadVariable2(frame, bone->pool, "array");
        bnPushStack(frame->vStack, bnNewInteger(bone, g_list_length(entries)));
        bnFrame* sub = bnFuncCall(arrayFunc, bone, frame, 1);
        bnDeleteFrame(sub);
        bnObject* arrayInst = bnPopStack(frame->vStack);
        GList* iter = entries;
        for (int i = 0; i < bnGetArrayLength(arrayInst); i++) {
                bnSetArrayElementAt(arrayInst, i,
                                    bnNewString(bone, iter->data));
                iter = iter->next;
        }
        bnWriteVariable2(frame, bone->pool, "ret", arrayInst);
        g_list_free(entries);
}

void bnExtReflectionExport(bnInterpreter* bone, bnFrame* frame) {
        bnObject* a = bnPopStack(frame->vStack);
        GHashTableIter hashIter;
        g_hash_table_iter_init(&hashIter, a->table);
        gpointer k, v;
        while (g_hash_table_iter_next(&hashIter, &k, &v)) {
                bnStringView view = k;
                const char* str = bnView2Str(bone->pool, view);
                if (*str == '$') {
                        continue;
                }
                // view = bnGetExportVariableName(bone->pool, view);
                bnWriteVariable(frame, view, v);
        }
}
