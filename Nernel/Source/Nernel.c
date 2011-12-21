#include <Nernel.h>
#include <CLR/ILReader.h>
#include <CLR/Log.h>

void Main(uint32_t pMultiBootMagic,
            void* pMultiBootData)
{
    if (!Nernel_Startup(pMultiBootMagic, pMultiBootData))
    {
        Nernel_Shutdown();
        return;
    }
    Log_Initialize((LogFlags)(
        LogFlags_ILReading
        | LogFlags_IREmitting
        //| LogFlags_MetaDataLoading
        ));
    printf("Finished Initializing");
    MultiBoot_LoadedModule* loadedModule = MultiBoot_GetLoadedModuleByFileName("corlib.dll");
    PEFile* peFile = PEFile_Create((uint8_t*)loadedModule->Address, loadedModule->Length);
    printf("PE Created");
    if (peFile)
    {
        CLIFile* cliFile = CLIFile_Create(peFile);
        printf("CLI Created");
        if (cliFile)
        {
            ILAssembly* asmb = ILReader_CreateAssembly(cliFile);
            printf("Method Count: %u\r\n", (unsigned int)asmb->IRAssembly->MethodCount);
        }
    }

    /*
	tCLIFile* cliFile;
	const char* args = "";
	cliFile = CLIFile_Load("mernel.exe");
	CLIFile_Execute(cliFile, 0, &args);
    */
    while (TRUE) ;
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

	time_t startupTime = time(NULL);
    printf("Nernel: Startup @ %s", ctime(&startupTime));

    /*
	JIT_Execute_Init();
	MetaData_Init();
	Type_Init();
	Heap_Init();
	Finalizer_Init();
    */
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
