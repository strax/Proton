#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ManifestResource_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ManifestResource)) != 0)
    {
        pFile->ManifestResourceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ManifestResources = (ManifestResource*)calloc(pFile->ManifestResourceCount + 1, sizeof(ManifestResource));
    }
    return pTableData;
}

void ManifestResource_Cleanup(CLIFile* pFile)
{
    if (pFile->ManifestResources)
    {
        for (uint32_t index = 1; index <= pFile->ManifestResourceCount; ++index)
        {
            if (pFile->ManifestResources[index].CustomAttributes) free(pFile->ManifestResources[index].CustomAttributes);
        }
        free(pFile->ManifestResources);
        pFile->ManifestResources = NULL;
    }
}

const uint8_t* ManifestResource_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t implementationIndex = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ManifestResourceCount; ++index)
    {
        pFile->ManifestResources[index].TableIndex = index;
        pFile->ManifestResources[index].Offset = *(uint32_t* )pTableData; pTableData += 4;
        pFile->ManifestResources[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ManifestResources[index].Name = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->FileCount > Implementation_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > Implementation_Type_MaxRows16Bit ||
            pFile->ExportedTypeCount > Implementation_Type_MaxRows16Bit) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ManifestResources[index].TypeOfImplementation = implementationIndex & Implementation_Type_Mask;
        implementationRow = implementationIndex >> Implementation_Type_Bits;
        switch (pFile->ManifestResources[index].TypeOfImplementation)
        {
        case Implementation_Type_File:
            if (implementationRow == 0 || implementationRow > pFile->FileCount) Panic("ManifestResource_Load File");
            pFile->ManifestResources[index].Implementation.File = &pFile->Files[implementationRow];
            break;
        case Implementation_Type_AssemblyReference:
            if (implementationRow == 0 || implementationRow > pFile->AssemblyReferenceCount) Panic("ManifestResource_Load AssemblyReference");
            pFile->ManifestResources[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow];
            break;
        case Implementation_Type_ExportedType:
            if (implementationRow == 0 || implementationRow > pFile->ExportedTypeCount) Panic("ManifestResource_Load ExportedType");
            pFile->ManifestResources[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow];
            break;
        default: break;
        }
    }
    return pTableData;
}

void ManifestResource_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_ManifestResource) ++pFile->CustomAttributes[index].Parent.ManifestResource->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ManifestResourceCount; ++index)
    {
        if (pFile->ManifestResources[index].CustomAttributeCount > 0)
        {
            pFile->ManifestResources[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ManifestResources[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_ManifestResource &&
                    pFile->CustomAttributes[searchIndex].Parent.ManifestResource == &pFile->ManifestResources[index])
                {
                    pFile->ManifestResources[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
