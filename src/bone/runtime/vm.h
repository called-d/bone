#ifndef BONE_RUNTIME_VM_H
#define BONE_RUNTIME_VM_H
#include "enviroment.h"
#include "frame.h"
#include "interpreter.h"

/**
 * bnExecute be run a bone.
 * return other than zero if breaked bone vm.
 * @param bone
 * @param env
 * @param frame
 * @return
 */
int bnExecute(bnInterpreter* bone, bnEnviroment* env, bnFrame* frame);
#endif