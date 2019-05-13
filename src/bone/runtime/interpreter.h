#ifndef BONE_RUNTIME_INTERPRETER_H
#define BONE_RUNTIME_INTERPRETER_H
#include <setjmp.h>
#include <stdbool.h>
#include "../glib.h"
#include "../util/jump_stack.h"
#include "../util/stack.h"
#include "../util/string_pool.h"

#define BN_JMP_CODE_EXCEPTION (1)

struct bnObject;
struct bnFrame;
struct bnHeap;

typedef struct bnInterpreter {
        const char* filenameRef;
        GList* nativeAlloc;
        GHashTable* externTable;
        struct bnStringPool* pool;
        struct bnFrame* frame;
        struct bnHeap* heap;
        bnStack* callStack;
        bnJStack* __jstack;
} bnInterpreter;

bnInterpreter* bnNewInterpreter(const char* filenameRef);

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
void bnThrow(bnInterpreter* self, struct bnObject* exception, int code);

/**
 * do a panic in current frame.
 * in almost, this function use by wrapper function.
 * @param self
 * @param exception
 */
void bnPanic(bnInterpreter* self, struct bnObject* exception);

/**
 * @param pool
 * @param frame
 * @param cond
 * @return
 */
struct bnObject* bnGetBool(struct bnStringPool* pool, struct bnFrame* frame,
                           bool cond);

/**
 * @param pool
 * @param frame
 * @return
 */
struct bnObject* bnGetTrue(struct bnStringPool* pool, struct bnFrame* frame);

/**
 * @param pool
 * @param frame
 * @return
 */
struct bnObject* bnGetFalse(struct bnStringPool* pool, struct bnFrame* frame);

void bnDeleteInterpreter(bnInterpreter* self);
#endif