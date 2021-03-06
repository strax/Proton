#pragma once

#include <CLR/MetaData/MetaData.h>

struct _ModuleDefinition
{
	uint32_t TableIndex;
    uint16_t Generation;
    const char* Name;
    const uint8_t* ModuleVersionID;
    const uint8_t* EncID;
    const uint8_t* EncBaseID;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* ModuleDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void ModuleDefinition_Cleanup(CLIFile* pFile);
const uint8_t* ModuleDefinition_Load(CLIFile* pFile, const uint8_t* pTableData);
void ModuleDefinition_Link(CLIFile* pFile);