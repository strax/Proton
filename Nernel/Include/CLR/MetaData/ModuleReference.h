#pragma once

#include <CLR/MetaData/MetaData.h>

struct _ModuleReference
{
	uint32_t TableIndex;
    const char* Name;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
};

const uint8_t* ModuleReference_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void ModuleReference_Cleanup(CLIFile* pFile);
const uint8_t* ModuleReference_Load(CLIFile* pFile, const uint8_t* pTableData);
void ModuleReference_Link(CLIFile* pFile);