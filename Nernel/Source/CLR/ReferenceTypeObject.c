#include <stdio.h>
#include <stdlib.h>

#include <CLR/ReferenceTypeObject.h>
#include <CLR/GC.h>
#include <CLR/Log.h>

void ReferenceTypeObject_AddReference(ReferenceTypeObject* pParent, ReferenceTypeObject* pReferenced)
{
	//Log_WriteLine(
	//	LogFlags_GC_ReferenceManagement, 
	//	"Adding reference from parent @ 0x%x to object @ 0x%x, refcount %u", 
	//	(unsigned int)pParent,
	//	(unsigned int)pReferenced, 
	//	(unsigned int)pReferenced->ReferenceCount
	//);

    if (!pParent || !pReferenced) Panic("Attempting to add a reference to null parent or with null object");

    ++pReferenced->ReferenceCount;

    if (!pParent->DependancyPool)
    {
        pParent->DependancyPoolSize = 4;
        pParent->DependancyPoolCount = 1;
        pParent->DependancyPool = (ReferenceTypeObject**)calloc(pParent->DependancyPoolSize, sizeof(ReferenceTypeObject*));
        pParent->DependancyPool[0] = pReferenced;
    }
    else if (pParent->DependancyPoolCount < pParent->DependancyPoolSize)
    {
        for (uint32_t index = 0; index < pParent->DependancyPoolSize; ++index)
        {
            if (!pParent->DependancyPool[index])
            {
                ++pParent->DependancyPoolCount;
                pParent->DependancyPool[index] = pReferenced;
                break;
            }
        }
    }
    else
    {
        pParent->DependancyPoolSize <<= 1;
        pParent->DependancyPool = (ReferenceTypeObject**)realloc(pParent->DependancyPool, sizeof(ReferenceTypeObject*) * pParent->DependancyPoolSize);
        for (uint32_t objectIndex = pParent->DependancyPoolSize >> 1; objectIndex < pParent->DependancyPoolSize; ++objectIndex)
            pParent->DependancyPool[objectIndex] = NULL;
        ++pParent->DependancyPoolCount;
        pParent->DependancyPool[pParent->DependancyPoolSize >> 1] = pReferenced;
    }
}

void ReferenceTypeObject_RemoveReference(ReferenceTypeObject* pParent, ReferenceTypeObject* pReferenced)
{
	//Log_WriteLine(
	//	LogFlags_GC_ReferenceManagement, "Removing reference from parent @ 0x%x to object @ 0x%x, refcount %u", 
	//	(unsigned int)pParent, 
	//	(unsigned int)pReferenced, 
	//	(unsigned int)pReferenced->ReferenceCount	
	//);

    if (!pParent || !pReferenced)
		Panic("Attempting to remove a reference from null parent or with null object");

	bool_t found = FALSE;
    for (uint32_t index = 0; !found && index < pParent->DependancyPoolSize; ++index)
    {
        if (pParent->DependancyPool[index] == pReferenced)
        {
		    --pReferenced->ReferenceCount;
            --pParent->DependancyPoolCount;
            pParent->DependancyPool[index] = NULL;
			found = TRUE;
        }
    }
	if (!found)
		Panic("Attempting to remove reference to object which isn't in dependancy pool!");
}

void ReferenceTypeObject_Dispose(ReferenceTypeObject* pObject)
{
    // TODO: Call object finalizer callback here
    pObject->Flags = pObject->Flags | ReferenceTypeObject_Flags_Disposed;
    pObject->Stack->Allocated -= pObject->Size;
    pObject->Stack->Disposed += pObject->Size;
}

void ReferenceTypeObject_Reset(ReferenceTypeObject* pObject)
{
    pObject->Flags = 0;
    pObject->Size = 0;
    pObject->Object = NULL;
    pObject->Stack = NULL;
    pObject->Age = 0;
    pObject->ReferenceCount = 0;
    pObject->DependancyPoolSize = 0;
    pObject->DependancyPoolCount = 0;
    if (pObject->DependancyPool)
        free(pObject->DependancyPool);
    pObject->DependancyPool = NULL;
}
