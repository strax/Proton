#include <PortIO.h>
#include <Core/DeviceManager.h>

Core::COMPortLogger::COMPortLogger()
{
}

bool Core::COMPortLogger::OnRegister()
{
    if (!IsIOPortAvailable(DataIOPort) ||
        !IsIOPortAvailable(InterruptIOPort) ||
        !IsIOPortAvailable(FIFOIOPort) ||
        !IsIOPortAvailable(LineDataIOPort) ||
        !IsIOPortAvailable(ModemDataIOPort) ||
        !IsIOPortAvailable(LineStatusIOPort) ||
        !IsIOPortAvailable(ModemStatusIOPort) ||
        !IsIOPortAvailable(ScratchIOPort)) return false;

    ClaimIOPort(DataIOPort);
    ClaimIOPort(InterruptIOPort);
    ClaimIOPort(FIFOIOPort);
    ClaimIOPort(LineDataIOPort);
    ClaimIOPort(ModemDataIOPort);
    ClaimIOPort(LineStatusIOPort);
    ClaimIOPort(ModemStatusIOPort);
    ClaimIOPort(ScratchIOPort);

	outb(InterruptIOPort, 0x00);
	outb(LineDataIOPort, 0x80);
	outb(DataIOPort, 0x03);
	outb(InterruptIOPort, 0x00);
	outb(LineDataIOPort, 0x03);
	outb(FIFOIOPort, 0xC7);
	outb(ModemDataIOPort, 0x0B);

    return true;
}

void Core::COMPortLogger::OnUnregister()
{
    ReleaseIOPort(DataIOPort);
    ReleaseIOPort(InterruptIOPort);
    ReleaseIOPort(FIFOIOPort);
    ReleaseIOPort(LineDataIOPort);
    ReleaseIOPort(ModemDataIOPort);
    ReleaseIOPort(LineStatusIOPort);
    ReleaseIOPort(ModemStatusIOPort);
    ReleaseIOPort(ScratchIOPort);
}

void Core::COMPortLogger::WriteByte(uint8_t pByte)
{
	uint32_t attempts = WriteAttempts;
	while (attempts && !IsTransmitEmpty()) --attempts;
	outb(DataIOPort, pByte);
}

bool Core::COMPortLogger::IsTransmitEmpty()
{
	return (inb(LineStatusIOPort) & 0x20) != 0;
}
