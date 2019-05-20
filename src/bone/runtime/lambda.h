#ifndef BONE_RUNTIME_LAMBDA_H
#define BONE_RUNTIME_LAMBDA_H
#include <stdarg.h>
#include <stdbool.h>
#include "enviroment.h"
#include "object.h"

struct bnFrame;
struct bnInterpreter;
typedef void (*bnNativeFunc)(struct bnInterpreter* bone, struct bnFrame* frame);

#define BN_C_ADD_PARAM (0)
#define BN_C_ADD_RETURN (1)
#define BN_C_ADD_EXIT (2)

/**
 * bnLambdaType is type of bnLambda.
 */
typedef enum bnLambdaType {
        BN_LAMBDA_SCRIPT,
        BN_LAMBDA_NATIVE,
} bnLambdaType;

/**
 * bnLambda is function pointer in bone.
 */
typedef struct bnLambda {
        bnObject base;
        bnLambdaType type;
        GHashTable* outer;
        GList* parameters;
        GList* returns;
        bnStringView filename;
        int lineno;
        union {
                bnEnviroment* vEnv;
                bnNativeFunc vFunc;
        } u;
} bnLambda;

#define bnNewLambda(bone, type) \
        (bnNewLambdaFunc(bone, type, __FILE__, __LINE__))
/**
 * return new instance of bnLambda.
 * @param bone
 * @param type
 * @param filename
 * @param lineno
 * @return
 */
bnLambda* bnNewLambdaFunc(struct bnInterpreter* bone, bnLambdaType type,
                          const char* filename, int lineno);

#define bnNewLambdaFromCFunc(bone, func, pool, ...)                     \
        (bnNewLambdaFromCFuncFunc(bone, func, pool, __FILE__, __LINE__, \
                                  __VA_ARGS__))
/**
 * return new instance of bnLambda, from C function.
 * @param bone
 * @param func
 * @param pool
 * @param filename
 * @param lineno
 * @param ... (parameter, named returns)
 * @return
 */
bnLambda* bnNewLambdaFromCFuncFunc(struct bnInterpreter* bone,
                                   bnNativeFunc func, struct bnStringPool* pool,
                                   const char* filename, int lineno, ...);

/**
 * return true, if first parameter name is self.
 * @param pool
 * @param self
 * @return
 */
bool bnIsInstanceBaseLambda(struct bnStringPool* pool, bnLambda* self);

/**
 * return true, if variadic return value.
 * @param pool
 * @param self
 * @return
 */
bool bnIsVariadicReturn(struct bnStringPool* pool, bnLambda* self);

#endif