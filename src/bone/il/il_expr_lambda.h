#ifndef $INCLUDE
#define $INCLUDE
#include "../bone.h"
struct bnEnviroment;
struct bnInterpreter;

/**
 * bnILExprLambda is lambda.
 */
typedef struct bnILExprLambda {
        GList* parameters;
        GList* returns;
        GList* statements;
} bnILExprLambda;

/**
 * return new instance of bnILExprLambda.
 * @return
 */
bnILExprLambda* bnNewILExprLambda();

/**
 * print a information of bnILExprLambda.
 * @param fp
 * @param pool
 * @param self
 * @param depth
 */
void bnDumpILExprLambda(FILE* fp, struct bnStringPool* pool,
                        bnILExprLambda* self, int depth);

void bnGenerateILExprLambda(struct bnInterpreter* bone, bnILExprLambda* self,
                            struct bnEnviroment* env);

/**
 * free a bnILExprLambda.
 * @param self
 */
void bnDeleteILExprLambda(bnILExprLambda* self);
#endif