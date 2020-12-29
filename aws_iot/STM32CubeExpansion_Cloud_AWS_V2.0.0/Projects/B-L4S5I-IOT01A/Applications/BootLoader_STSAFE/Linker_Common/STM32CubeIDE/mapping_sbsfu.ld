/* SE Code region protected by firewall */
VECTOR_SIZE = 0x200;
__ICFEDIT_SE_Code_region_ROM_start__       = 0x08000000 + VECTOR_SIZE;
__ICFEDIT_SE_CallGate_region_ROM_start__   = __ICFEDIT_SE_Code_region_ROM_start__;  /* no need to do +4 as we have dummy bytes in SE_CoreBin .ld file */
__ICFEDIT_SE_CallGate_region_ROM_end__     = __ICFEDIT_SE_Code_region_ROM_start__ + 0x1FF;

/* SE key region protected by firewall */
__ICFEDIT_SE_Key_region_ROM_start__        = __ICFEDIT_SE_CallGate_region_ROM_end__ + 0x1;
__ICFEDIT_SE_Key_region_ROM_end__          = __ICFEDIT_SE_Key_region_ROM_start__ + 0xFF;

/* SE Startup: call before enabling firewall*/
__ICFEDIT_SE_Startup_region_ROM_start__    = __ICFEDIT_SE_Key_region_ROM_end__ + 0x1;
__ICFEDIT_SE_Code_nokey_region_ROM_start__ = __ICFEDIT_SE_Startup_region_ROM_start__ + 0x100;
__ICFEDIT_SE_Code_region_ROM_end__         = __ICFEDIT_SE_Startup_region_ROM_start__ + 0x1AFFF;

/* SE IF ROM: used to locate Secure Engine interface code out of firewall */
__ICFEDIT_SE_IF_region_ROM_start__         = __ICFEDIT_SE_Code_region_ROM_end__ + 1;
__ICFEDIT_SE_IF_region_ROM_end__           = __ICFEDIT_SE_IF_region_ROM_start__ + 0xDFF;

/* SBSFU Code region */
__ICFEDIT_SB_region_ROM_start__            = __ICFEDIT_SE_IF_region_ROM_end__ + 0x1;
__ICFEDIT_SB_region_ROM_end__              = 0x08035FFF;

/* KMS Data Storage (NVMS) region protected by firewall */
/* In Dual Bank configuration, we propose to set it up to the beginning of the second bank */
/* KMS Data Storage need for 2 images : 6 kbytes * 2 ==> 12 kbytes */
__ICFEDIT_KMS_DataStorage_start__          = 0x08101000;
__ICFEDIT_KMS_DataStorage_end__            = 0x08104FFF;

SE_Entry_Secure_ROM_Region_Length = __ICFEDIT_SE_CallGate_region_ROM_end__ - __ICFEDIT_SE_CallGate_region_ROM_start__ + 1;
SE_Key_region_ROM_Length          = __ICFEDIT_SE_Key_region_ROM_end__ - __ICFEDIT_SE_Key_region_ROM_start__ + 1;
SE_Startup_region_ROM_Length      = __ICFEDIT_SE_Code_nokey_region_ROM_start__ - __ICFEDIT_SE_Startup_region_ROM_start__ ;
SE_ROM_region_Length              = __ICFEDIT_SE_Code_region_ROM_end__ - __ICFEDIT_SE_Code_nokey_region_ROM_start__ + 1;
SE_IF_region_ROM_Length           = __ICFEDIT_SE_IF_region_ROM_end__ - __ICFEDIT_SE_IF_region_ROM_start__ + 1;
SB_ROM_region_Length              = __ICFEDIT_SB_region_ROM_end__ - __ICFEDIT_SB_region_ROM_start__ + 1;

/* RAM section */
/* SE RAM1 region protected by firewall */
/* SE stack is placed 1st in RAM, stack overflow does not write on other RAM area */
__ICFEDIT_SE_region_SRAM1_start__     = 0x20000000;
__ICFEDIT_SE_region_SRAM1_stack_top__ = 0x20002000;
__ICFEDIT_SE_region_SRAM1_end__       = 0x2001FFBF; /* 0x1FFC0 bytes of SRAM1 under FWALL */
__ICFEDIT_SE_region_SRAM1_do_not_use_start__     = 0x2001FFC0;
__ICFEDIT_SE_region_SRAM1_do_not_use_end__       = 0x2002FFFF; /* 0x10040 bytes we cannot use to avoid spurious firewall reset */
/* See errata sheet: ES0393 - Rev 6 - October 2019                                                                              */
/* Only 128 Kbytes - 1 byte and with the 6 lowest bits set to 0 of SRAM1 can be protected by the firewall instead of the full memory space (192 Kbytes of SRAM1). */
/* Use SRAM2 or SRAM3 as system RAM and avoid accessing SRAM1 before any access to an address outside the protected SRAM1 area,                                   */
/* of which the 18 LSBs correspond to an address within the protected SRAM1 area.                                                                                 */
/* So we declare all SRAM1 to avoid spurious Firewall reset but only 0x1FFC0 bytes can be actually used (under Firewall).                                         */

/* SBSFU RAM1 region */
__ICFEDIT_SB_region_SRAM1_start__     = 0x20030000; /* SRAM2 */
__ICFEDIT_SB_region_SRAM1_end__       = 0x2009FFFF; /* SRAM2 + SRAM3 */

SE_SRAM1_region_Length  = __ICFEDIT_SE_region_SRAM1_end__ - __ICFEDIT_SE_region_SRAM1_stack_top__ + 1;
SRAM1_do_not_use_Length = __ICFEDIT_SE_region_SRAM1_do_not_use_end__ - __ICFEDIT_SE_region_SRAM1_do_not_use_start__ +1;
SB_SRAM1_region_Length  = __ICFEDIT_SB_region_SRAM1_end__ - __ICFEDIT_SB_region_SRAM1_start__ + 1;

MEMORY
{
 SE_Entry_Secure_ROM_Region (rx)     : ORIGIN = __ICFEDIT_SE_CallGate_region_ROM_start__, LENGTH = SE_Entry_Secure_ROM_Region_Length
 SE_Key_region_ROM (rx)              : ORIGIN = __ICFEDIT_SE_Key_region_ROM_start__, LENGTH = SE_Key_region_ROM_Length
 SE_Startup_region_ROM (rx)          : ORIGIN = __ICFEDIT_SE_Startup_region_ROM_start__, LENGTH = SE_Startup_region_ROM_Length
 SE_ROM_region (rx)                  : ORIGIN = __ICFEDIT_SE_Code_nokey_region_ROM_start__, LENGTH = SE_ROM_region_Length
 SE_IF_region_ROM (rx)               : ORIGIN = __ICFEDIT_SE_IF_region_ROM_start__, LENGTH = SE_IF_region_ROM_Length
 SB_ROM_region (rx)                  : ORIGIN = __ICFEDIT_SB_region_ROM_start__, LENGTH = SB_ROM_region_Length
 SE_SRAM1_region (xrw)               : ORIGIN = __ICFEDIT_SE_region_SRAM1_stack_top__, LENGTH = SE_SRAM1_region_Length
 SRAM1_do_not_use_region(xrw)        : ORIGIN = __ICFEDIT_SE_region_SRAM1_do_not_use_start__, LENGTH = SRAM1_do_not_use_Length
 SB_SRAM1_region (xrw)               : ORIGIN = __ICFEDIT_SB_region_SRAM1_start__, LENGTH = SB_SRAM1_region_Length
}

