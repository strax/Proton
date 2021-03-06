#include <Nernel.h>
#include <CLR/GC.h>
#include <CLR/AppDomain.h>
#include <CLR/ILReader.h>
#include <CLR/Log.h>
#include <CLR/JIT/JIT.h>
#include <CLR/SyntheticStack.h>

static AppDomain* global_baseMernelDomain;
void Mernel_FinishedRunning();
void Mernel_Jitted();
void Test();

void Main(uint32_t pMultiBootMagic,
            void* pMultiBootData)
{
    if (!Nernel_Startup(pMultiBootMagic, pMultiBootData))
    {
        Nernel_Shutdown();
        return;
    }
    /*
  	MultiBoot_LoadedModule* loadedModule = MultiBoot_GetLoadedModuleByFileName("corlib.dll");
    PEFile* peFile = PEFile_Create((uint8_t*)loadedModule->Address, loadedModule->Length);
    if (peFile)
    {
        CLIFile* cliFile = CLIFile_Create(peFile, "corlib.dll");
        if (cliFile)
        {
            printf("MethodDefinition[4]: %s SignatureLength = %u\n", cliFile->MethodDefinitions[4].Name, (unsigned int)cliFile->MethodDefinitions[4].SignatureLength);
            MethodSignature* methodSignature = MethodSignature_Expand(cliFile->MethodDefinitions[4].Signature, cliFile->MethodDefinitions[4].SignatureLength);
            printf("MethodDefinition[4]: MethodSignature\n");
            printf("    HasThis = %s, ExplicitThis = %s\n", methodSignature->HasThis ? "TRUE" : "FALSE", methodSignature->ExplicitThis ? "TRUE" : "FALSE");
            printf("    ParameterCount = %u\n", (unsigned int)methodSignature->ParameterCount);
            printf("    Default = %s, VarArgs = %s, Generic = %s (Count: %u)\n", methodSignature->Default ? "TRUE" : "FALSE", methodSignature->VarArgs ? "TRUE" : "FALSE", methodSignature->Generic ? "TRUE" : "FALSE", (unsigned int)methodSignature->GenericParameterCount);
            printf("    ReturnType\n");
            printf("        Void = %s\n", methodSignature->ReturnType->Void ? "TRUE" : "FALSE");
            printf("        TypedByReference = %s\n", methodSignature->ReturnType->TypedByReference ? "TRUE" : "FALSE");
            if (methodSignature->ReturnType->Type) printf("        Type = 0x%x\n", methodSignature->ReturnType->Type->ElementType);
        }
    }
    */
    /*
    ReferenceTypeObject* root = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    GC* gc = GC_Create(root);
    ReferenceTypeObject* objA = GC_Allocate(gc, root, 2048);
    ReferenceTypeObject* objB = GC_Allocate(gc, objA, 1024);
    ReferenceTypeObject* objC = GC_Allocate(gc, objB, 1024);
    ReferenceTypeObject* objD = GC_Allocate(gc, objC, 1024);
    ReferenceTypeObject* objE = GC_Allocate(gc, objD, 4096);
    ReferenceTypeObject_RemoveReference(objA, objB);
    GC_Collect(gc);
    if (objA || objB || objC || objD || objE) { }
    */

    global_baseMernelDomain = AppDomain_CreateDomain();

	MultiBoot_LoadedModule* mernelModule = MultiBoot_GetLoadedModuleByFileName("/boot/mernel.exe");
	CLIFile* mernelFile = CLIFile_Create(PEFile_Create((uint8_t*)mernelModule->Address, mernelModule->Length), "mernel.exe");
	IRAssembly* mernelAssembly = ILReader_CreateAssembly(mernelFile, global_baseMernelDomain);


	Console_Clear(Console_CreateAttributes(Console_DarkBlack, Console_LightGreen));
	printf("Startup Successful!\n");
    StackObjectPool_Initialize();
	
	//printf("Location: %x\n", (unsigned int)global_baseMernelDomain->IRAssemblies[0]);
	//printf("Location: %x\n", (unsigned int)global_baseMernelDomain->IRAssemblies[0]->EntryPoint->MethodDefinition);
	
	//Nernel_FinishedRunning();
	/*char* blah = malloc(0xFFFFFFF);
	printf("AddressOf Blah: 0x%x\n", (unsigned int)blah);*/
	//JIT_CompileMethod(global_baseMernelDomain->IRAssemblies[0]->EntryPoint);
	JIT_CompileMethod(mernelAssembly->EntryPoint);
	
	printf("Now how do you like that.\n");

	Console_Clear(Console_CreateAttributes(Console_DarkBlack, Console_LightCyan));
	printf("Mernel JIT'd, starting up now!\n");
	Mernel_Jitted();

	//((void(*)(uint32_t))global_baseMernelDomain->IRAssemblies[0]->EntryPoint->AssembledMethod)(0);
	((void(*)(uint32_t))mernelAssembly->EntryPoint->AssembledMethod)(0);
	
	//Console_Clear(Console_CreateAttributes(Console_DarkBlack, Console_LightYellow));
	Mernel_FinishedRunning();
    StackObjectPool_Destroy();
    while (TRUE) ;
}

void Mernel_Jitted()
{
	printf("Go Mernel!\n");
}
void Mernel_FinishedRunning()
{
	printf("Mernel, Return!\n");
}

void CPU_Interrupt(InterruptRegisters pRegisters)
{
	char buf[128];
	sprintf(buf, "CPU Exception: %d", (int)pRegisters.int_no);
	Panic(buf);
}

bool_t Nernel_Startup(uint32_t pMultiBootMagic,
                      void* pMultiBootData)
{
    if (!MultiBoot_Startup(pMultiBootMagic, pMultiBootData)) return FALSE;

    SerialWriter_Startup();
    Console_Startup();
    Console_WriteLine("Nernel: Starting Proton (" BRANCH ")...");

    GDT_Startup();
    IDT_Startup();

    RTC_Startup();
    PIC_Startup();
    PIT_Startup(100);
    SystemClock_Startup();
	for (uint8_t interrupt = 0; interrupt < IDT_RemappedIRQBase; ++interrupt) IDT_RegisterHandler(interrupt, &CPU_Interrupt);
	time_t startupTime = time(NULL);
    printf("Nernel: Startup @ %s", ctime(&startupTime));

    return TRUE;
}

void Nernel_Shutdown()
{
    SystemClock_Shutdown();
    PIT_Shutdown();
    PIC_Shutdown();
    RTC_Shutdown();

    IDT_Shutdown();
    GDT_Shutdown();

    Console_Shutdown();
    SerialWriter_Shutdown();

    MultiBoot_Shutdown();
}
