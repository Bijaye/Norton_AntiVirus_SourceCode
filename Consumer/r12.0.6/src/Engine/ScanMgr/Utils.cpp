
#include "Stdafx.h"
#include "Utils.h"

SMRESULT NavScanToScanManager(SCANSTATUS Status)
{
	switch(Status)
	{
	case SCAN_OK:
		return SMRESULT_OK;
	case SCAN_ABORT:
		return SMRESULT_ABORT;
	case SCAN_ERROR_MEMORY:
		return SMRESULT_ERROR_MEMORY;
	case SCAN_ERROR_UNKNOWN:
		return SMRESULT_ERROR_UNKNOWN;
	case SCAN_ERROR_DEFINITIONS:
		return SMRESULT_ERROR_VIRUS_DEFS;
    case SCAN_ERROR_DEFINITIONS_NOT_AUTHENTIC:
        return SMRESULT_ERROR_DEF_AUTHENTICATION_FAILURE;
	default:
		return SMRESULT_ERROR_SCANNER;
	}
}

#define HT_BIT		0x10000000	// EDX[28] - Bit 28 set indicates
								// Hyper-Threading Technology is supported
								// in hardware.
#define FAMILY_ID	0x0f00		// EAX[11:8] - Bit 11 thru 8 contains family
								// processor id
#define EXT_FAMILY_ID 0x0f00000	// EAX[23:20] - Bit 23 thru 20 contains
								// extended family  processor id
#define PENTIUM4_ID	0x0f00		// Pentium 4 family processor id

// Returns non-zero if Hyper-Threading Technology is supported on
// the processors and zero if not.  This does not mean that
// Hyper-Threading Technology is necessarily enabled.
unsigned int HTSupported(void)
{
	unsigned int reg_eax = 0;
	unsigned int reg_edx = 0;
	unsigned int vendor_id[3] = {0, 0, 0};

	// verify cpuid instruction is supported
	__try
	{
		__asm
		{
			xor   eax, eax		// call cpuid with eax = 0
			cpuid				// get vendor id string
			mov   vendor_id, ebx
			mov   vendor_id + 4, edx
			mov   vendor_id + 8, ecx
			mov   eax, 1		// call cpuid with eax = 1
			cpuid
			mov   reg_eax, eax	// eax contains cpu family type info
			mov   reg_edx, edx	// edx has info whether Hyper-Threading
								// Technology is available
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// CPUID is not supported and so Hyper-Threading Technology is not supported
		return 0;
	}

	// Check to see if this is a Pentium 4 or later processor
	if(((reg_eax & FAMILY_ID) ==  PENTIUM4_ID) || (reg_eax & EXT_FAMILY_ID))
	{
		if(vendor_id[0] == 'uneG')
		{
			if(vendor_id[1] == 'Ieni')
			{
				if(vendor_id[2] == 'letn')
				{
					return (reg_edx & HT_BIT);  // Genuine Intel Processor with Hyper-Threading Technology
				}
			}
		}
	}

	// This is not a genuine Intel processor.
	return 0;
}

// logical processors per package
// Returns the number of logical processors per physical processors.
#define NUM_LOGICAL_BITS	0x00FF0000	// EBX[23:16] indicate number of
unsigned char LogicalProcessorsPerPackage(void)
{
	unsigned int reg_ebx = 0;
	if(!HTSupported())
	{
		return (unsigned char) 1;
	}
	__asm
	{
		mov	 eax, 1			// call cpuid with eax = 1
		cpuid
		mov	 reg_ebx, ebx	// Has info on number of logical processors
	}

	return (unsigned char)((reg_ebx & NUM_LOGICAL_BITS) >> 16);
}

// Returns the 8-bit unique Initial APIC ID for the processor this
// code is actually running on. The default value returned is 0xFF if
// Hyper-Threading Technology is not supported.
#define INITIAL_APIC_ID_BITS	0xFF000000	// EBX[31:24] unique APIC ID
unsigned char GetAPIC_ID(void)
{
	unsigned int reg_ebx = 0;
	if (!HTSupported())
	{
		return (unsigned char)-1;
	}
	__asm
	{
		mov	 eax, 1				// call cpuid with eax = 1
		cpuid
		mov	 reg_ebx, ebx		// Has APIC ID info
	}

	return (unsigned char)((reg_ebx & INITIAL_APIC_ID_BITS) >> 24);
}

// Be sure to include the routines previously documented in this
// application note here.
DWORD GetNumberOfPhysicalProcessors(void)
{
	// Check to see if Hyper-Threading Technology is available
    bool bUseSystemInfo = false;
    std::set<unsigned char> setOfPhysicalIDs;

	if(HTSupported())
	{
		// Bit 28 set indicated Hyper-Threading Technology
		unsigned char Logical_Per_Package;
		CCTRACEI("GetNumberOfPhysicalProcessors() - Hyper-Threading Technology is available.");
		Logical_Per_Package = LogicalProcessorsPerPackage();
		CCTRACEI("GetNumberOfPhysicalProcessors() - Logical Processors Per Package: %d", Logical_Per_Package);

		// Just because logical processors is > 1
		// does not mean that Hyper-Threading Technology is enabled.
		if(Logical_Per_Package > 1)
		{
			HANDLE hCurrentProcessHandle;
			DWORD dwProcessAffinity;
			DWORD dwSystemAffinity;
			DWORD dwAffinityMask;

			// Physical processor ID and Logical processor IDs are derived
			// from the APIC ID.  We'll calculate the appropriate shift and
			// mask values knowing the number of logical processors per
			// physical processor package.
			unsigned char i = 1;
			unsigned char PHY_ID_MASK = 0xFF;
			unsigned char PHY_ID_SHIFT = 0;

			while(i < Logical_Per_Package)
			{
				i *= 2;
				PHY_ID_MASK <<= 1;
				PHY_ID_SHIFT++;
			}

			// The OS may limit the processors that this process may run on.
			hCurrentProcessHandle = GetCurrentProcess();
			GetProcessAffinityMask(hCurrentProcessHandle, &dwProcessAffinity, &dwSystemAffinity);

			// If our available process affinity mask does not equal the
			// available system affinity mask, then we may not be able to
			// determine if Hyper-Threading Technology is enabled.
			if(dwProcessAffinity != dwSystemAffinity)
			{
				CCTRACEI("GetNumberOfPhysicalProcessors() - This process can not utilize all processors.");
			}

			dwAffinityMask = 1;
			while(dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity)
			{
				// Check to make sure we can utilize this processor first.
				if(dwAffinityMask & dwProcessAffinity)
				{
					if(SetProcessAffinityMask(hCurrentProcessHandle, dwAffinityMask))
					{
						unsigned char APIC_ID;
						//unsigned char LOG_ID;
						unsigned char PHY_ID;

						// We may not be running on the cpu that we
						// just set the affinity to.  Sleep gives the OS
						// a chance to switch us to the desired cpu.
						Sleep(0);

						APIC_ID = GetAPIC_ID();
						//LOG_ID = APIC_ID & ~PHY_ID_MASK;
						PHY_ID = APIC_ID >> PHY_ID_SHIFT;

                        // Add this physical process ID to our set
                        setOfPhysicalIDs.insert(PHY_ID);
						CCTRACEI("GetNumberOfPhysicalProcessors() - Added processor Physical ID: %d to our set. There are now %d physical processors detected", PHY_ID, setOfPhysicalIDs.size());
					}
					else
					{
						// This shouldn't happen since we check to make sure we
						// can utilize this processor before trying to set
						// affinity mask.
						CCTRACEI("GetNumberOfPhysicalProcessors() - Set Affinity Mask Error Code: %d", GetLastError());
					}
				}

				dwAffinityMask = dwAffinityMask << 1;
			}

			// Don't forget to reset the processor affinity if you use this code in an application.
			SetProcessAffinityMask(hCurrentProcessHandle, dwProcessAffinity);
		}
		else
        {
            CCTRACEI("GetNumberOfPhysicalProcessors() - Processors with Hyper-Threading Technology is not enabled.");
            bUseSystemInfo = true;
        }
	}
    else
        bUseSystemInfo = true;

    // If hyper threading is not available or enabled use system info to get processor count
	if( bUseSystemInfo )
	{
        CCTRACEI("GetNumberOfPhysicalProcessors() - Hyper-Threading Technology Processors are not detected. Using System Info to get # of processors.");
        SYSTEM_INFO si;
		GetSystemInfo(&si);

        return si.dwNumberOfProcessors;
	}
    else
        return setOfPhysicalIDs.size();
}
