#pragma once

#include <CLR/CLIFile.h>

typedef struct _InternalCall InternalCall;

#define MAX_INTERNAL_ARGS 6

struct _InternalCall
{
	const char* Namespace;
	const char* TypeName;
	const char* Name;
	const uint32_t ArgCount;
	const uint8_t Args[MAX_INTERNAL_ARGS];
	InternalCallPointer TargetMethod;
};

InternalCallPointer ResolveInternalCall(MethodDefinition* methodDef, CLIFile* fil);