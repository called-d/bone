#ifndef BONE_RUNTIME_INTERPRETER_H
#define BONE_RUNTIME_INTERPRETER_H
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include "../glib.h"
#include "../util/jump_stack.h"
#include "../util/stack.h"
#include "../util/string_pool.h"
#include "storage.h"

#define BN_JMP_CODE_EXCEPTION (1)

struct bnObject;
struct bnFrame;
struct bnHeap;

typedef struct bnInterpreter {
        const char* filenameRef;
        GList* nativeAlloc;
        GList* plugins;
        int argc;
        GPtrArray* argv;
        GHashTable* externTable;
        struct bnStringPool* pool;
        struct bnFrame* frame;
        struct bnHeap* heap;
        bnStack* callStack;
        bnJStack* __jstack;
#if DEBUG
        FILE* out;
        FILE* err;
        FILE* in;
#endif
} bnInterpreter;

typedef void (*bnPluginInit)(bnInterpreter*);

typedef void (*bnPluginDestroy)(bnInterpreter*);

typedef const char* (*bnPluginGetTargetVersion)();

bnInterpreter* bnNewInterpreter(const char* filenameRef, int argc,
                                char* argv[]);

/**
 * link dynamic libraries.
 * it will be freed on bnEval.
 * @param bone
 * @param path
 */
void bnLink(bnInterpreter* bone, const char* path);

int bnEval(bnInterpreter* self);

/**
 * @param self
 * @param frame
 * @param pool
 */
void bnWriteDefaults(bnInterpreter* self, struct bnFrame* frame,
                     struct bnStringPool* pool);

/**
 * @param self
 * @param fmt
 * @param ...
 */
void bnFormatThrow(bnInterpreter* self, const char* fmt, ...);

/**
 * @param self
 * @param fmt
 * @param ...
 */
void bnVFormatThrow(bnInterpreter* self, const char* fmt, va_list ap);

/**
 * bnThrow is call longjmp().
 * shoud be invoke only in can native function.
 * call bnPanic in internal.
 * @param self
 * @param exception
 * @param code
 */
void bnThrow(bnInterpreter* self, bnReference exception, int code);

/**
 * do a panic in current frame.
 * in almost, this function use by wrapper function.
 * @param self
 * @param exception
 */
void bnPanic(bnInterpreter* self, bnReference exception);

/**
 * @param pool
 * @param frame
 * @param cond
 * @return
 */
bnReference bnGetBool(struct bnStringPool* pool, struct bnFrame* frame,
                      bool cond);

/**
 * @param pool
 * @param frame
 * @return
 */
bnReference bnGetTrue(struct bnStringPool* pool, struct bnFrame* frame);

/**
 * @param pool
 * @param frame
 * @return
 */
bnReference bnGetFalse(struct bnStringPool* pool, struct bnFrame* frame);

/**
 * @param self
 * @param name
 * @param ref
 */
void bnWriteExtern(bnInterpreter* self, bnStringView name, bnReference ref);

/**
 * @param self
 * @param str
 * @param ref
 */
void bnWriteExtern2(bnInterpreter* self, const char* str, bnReference ref);

/**
 * @param self
 * @param name
 * @return
 */
bnReference bnReadExtern(bnInterpreter* self, bnStringView name);

/**
 * @param self
 * @param str
 * @return
 */
bnReference bnReadExtern2(bnInterpreter* self, const char* str);

void bnDeleteInterpreter(bnInterpreter* self);
#endif