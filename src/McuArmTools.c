/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : McuArmTools.h
**     Project     : FRDM-K64F_Generator
**     Processor   : MK64FN1M0VLL12
**     Component   : KinetisTools
**     Version     : Component 01.055, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2024-07-09, 10:10, # CodeGen: 829
**     Abstract    :
**
**     Settings    :
**          Component name                                 : McuArmTools
**          Utility                                        : McuUtility
**          SDK                                            : McuLib
**          Shell                                          : Enabled
**            Shell                                        : McuShell
**     Contents    :
**         SoftwareReset           - void McuArmTools_SoftwareReset(void);
**         UIDGet                  - uint8_t McuArmTools_UIDGet(McuArmTools_UID *uid);
**         UIDSame                 - bool McuArmTools_UIDSame(const McuArmTools_UID *src, const McuArmTools_UID...
**         UIDtoString             - uint8_t McuArmTools_UIDtoString(const McuArmTools_UID *uid, uint8_t *buf,...
**         GetKinetisFamilyString  - McuArmTools_ConstCharPtr McuArmTools_GetKinetisFamilyString(void);
**         GetPC                   - void* McuArmTools_GetPC(void);
**         GetSP                   - void* McuArmTools_GetSP(void);
**         SetPSP                  - void McuArmTools_SetPSP(void *setval);
**         SetLR                   - void McuArmTools_SetLR(uint32_t setval);
**         InitCycleCounter        - void McuArmTools_InitCycleCounter(void);
**         ResetCycleCounter       - void McuArmTools_ResetCycleCounter(void);
**         EnableCycleCounter      - void McuArmTools_EnableCycleCounter(void);
**         DisableCycleCounter     - void McuArmTools_DisableCycleCounter(void);
**         GetCycleCounter         - uint32_t McuArmTools_GetCycleCounter(void);
**         GetUsedMainStackSpace   - uint32_t McuArmTools_GetUsedMainStackSpace(void);
**         GetUnusedMainStackSpace - uint32_t McuArmTools_GetUnusedMainStackSpace(void);
**         FillMainStackSpace      - void McuArmTools_FillMainStackSpace(void);
**         GetLinkerMainStackSize  - uint32_t McuArmTools_GetLinkerMainStackSize(void);
**         GetLinkerMainStackTop   - McuArmTools_uint32_t_Ptr McuArmTools_GetLinkerMainStackTop(void);
**         GetLinkerMainStackBase  - McuArmTools_uint32_t_Ptr McuArmTools_GetLinkerMainStackBase(void);
**         ParseCommand            - uint8_t McuArmTools_ParseCommand(const unsigned char* cmd, bool *handled,...
**         Deinit                  - void McuArmTools_Deinit(void);
**         Init                    - void McuArmTools_Init(void);
**
** * Copyright (c) 2014-2024, Erich Styger
**  * Web:         https://mcuoneclipse.com
**  * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**  * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**  * All rights reserved.
**  *
**  * Redistribution and use in source and binary forms, with or without modification,
**  * are permitted provided that the following conditions are met:
**  *
**  * - Redistributions of source code must retain the above copyright notice, this list
**  *   of conditions and the following disclaimer.
**  *
**  * - Redistributions in binary form must reproduce the above copyright notice, this
**  *   list of conditions and the following disclaimer in the documentation and/or
**  *   other materials provided with the distribution.
**  *
**  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**  * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file McuArmTools.h
** @version 01.00
** @brief
**
*/         
/*!
**  @addtogroup McuArmTools_module McuArmTools module documentation
**  @{
*/         

/* MODULE McuArmTools. */

#include "McuArmTools.h"

#if McuLib_CONFIG_CPU_IS_ARM_CORTEX_M

#include "McuUtility.h" /* various utility functions */
#if McuLib_CONFIG_NXP_SDK_2_0_USED
  #include "fsl_common.h"
  #if McuLib_CONFIG_CPU_IS_KINETIS && !McuLib_CONFIG_IS_KINETIS_KE
    #include "fsl_sim.h" /* system integration module, used for CPU ID */
  #elif McuLib_CONFIG_CPU_IS_LPC && McuLib_CONFIG_CPU_IS_LPC55xx /* LPC55x */
    #include "fsl_iap.h" /* if missing, add this module from the MCUXpresso SDK */
    #include "fsl_iap_ffr.h"
  #elif McuLib_CONFIG_CPU_IS_LPC  /* LPC845, LPC804 */
    #include "fsl_iap.h" /* if missing, add this module from the MCUXpresso SDK */
  #endif
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_KINETIS_1_3
  #include "Cpu.h" /* include CPU related interfaces and defines */
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_S32K
  #include "Cpu.h" /* include CPU related interfaces and defines */
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_RPI_PICO
  #include "pico/unique_id.h" /* for UID */
#elif McuLib_CONFIG_CPU_IS_ARM_CORTEX_M
  /* include device specific header file for CMSIS inside "McuArmToolsconfig.h" */
#endif


/* On ARM Cortex, the stack grows from 'top' (higher address) to the 'bottom' (lower address).
 * We are declaring the symbols as functions, as they are filled by the linker with an immediate value.
 * If we would use an integer, this might conflict how such a symbol might be used in NXP startup code, e.g. LPC55S16.
 */
extern void McuArmTools_CONFIG_LINKER_SYMBOL_STACK_BASE(void); /*!< base address of stack, this is a numerically lower address than the top */
extern void McuArmTools_CONFIG_LINKER_SYMBOL_STACK_TOP(void);  /*!< top or end of stack, at the top. Highest address. Stack is growing from base to top */

#if McuLib_CONFIG_CPU_IS_KINETIS
#if McuLib_CONFIG_CORTEX_M==4
static const unsigned char *const KinetisM4FamilyStrings[] =
{ /* FAMID (3 bits) are used as index */
  (const unsigned char *)"K10 or K12 Family",          /* 000 */
  (const unsigned char *)"K20 or K22 Family",          /* 001 */
  (const unsigned char *)"K30, K11 or K61 Family",     /* 010 */
  (const unsigned char *)"K40 or K21 Family",          /* 011 */
  (const unsigned char *)"K60 or K62 Family",          /* 100 */
  (const unsigned char *)"K70 Family",                 /* 101 */
  (const unsigned char *)"Reserved",                   /* 110 */
  (const unsigned char *)"Reserved"                    /* 111 */
};
#endif

#if McuLib_CONFIG_CORTEX_M==0 && !McuLib_CONFIG_IS_KINETIS_KE
static const unsigned char *const KinetisM0FamilyStrings[] =
{ /* FAMID (3 bits) are used as index */
  (const unsigned char *)"KL0x",          /* 0000 */
  (const unsigned char *)"KL1x",          /* 0001 */
  (const unsigned char *)"KL2x",          /* 0010 */
  (const unsigned char *)"KL3x",          /* 0011 */
  (const unsigned char *)"KL4x",          /* 0100 */
  (const unsigned char *)"Reserved",      /* 0101 */
  (const unsigned char *)"Reserved",      /* 0110 */
  (const unsigned char *)"Reserved"       /* 0111 */
};
#endif
#endif

#if McuArmTools_CONFIG_PARSE_COMMAND_ENABLED
static uint8_t PrintStatus(const McuShell_StdIOType *io)
{
#if McuLib_CONFIG_CPU_IS_KINETIS || McuLib_CONFIG_CPU_IS_LPC || McuLib_CONFIG_CPU_IS_RPxxxx
  uint8_t buf[1+(16*5)+1+1]; /* "{0xAA,...0xBB}" */
  uint8_t res;
  McuArmTools_UID uid;
#endif

  McuShell_SendStatusStr((unsigned char*)"McuArmTools", (unsigned char*)"Hardware status\r\n", io->stdOut);
#if McuLib_CONFIG_CPU_IS_KINETIS || McuLib_CONFIG_CPU_IS_LPC || McuLib_CONFIG_CPU_IS_RPxxxx
  res = McuArmTools_UIDGet(&uid);
  if (res==ERR_OK) {
    res = McuArmTools_UIDtoString(&uid, buf, sizeof(buf));
  }
  if (res!=ERR_OK) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR");
  }
  McuShell_SendStatusStr((unsigned char*)"  UID", buf, io->stdOut);
  McuShell_SendStr((unsigned char*)"\r\n", io->stdOut);
#endif

#if McuLib_CONFIG_CPU_IS_LPC && (McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC845 || McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC804)
  uint32_t val;

  res = IAP_ReadPartID(&val);
  if (res == kStatus_IAP_Success) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"0x");
    McuUtility_strcatNum32Hex(buf, sizeof(buf), val);
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
  }
  McuShell_SendStatusStr((unsigned char*)"  PartID", buf, io->stdOut);

  res = IAP_ReadBootCodeVersion(&val);
  if (res == kStatus_IAP_Success) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"Version 0x");
    McuUtility_strcatNum32Hex(buf, sizeof(buf), val);
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
  }
  McuShell_SendStatusStr((unsigned char*)"  BootCode", buf, io->stdOut);
#endif

  McuShell_SendStatusStr((unsigned char*)"  Family", (uint8_t*)McuArmTools_GetKinetisFamilyString(), io->stdOut);
  McuShell_SendStr((unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}
#endif

#if McuArmTools_CONFIG_PARSE_COMMAND_ENABLED
static uint8_t PrintHelp(const McuShell_StdIOType *io)
{
  McuShell_SendHelpStr((unsigned char*)"McuArmTools", (unsigned char*)"Group of McuArmTools commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  reset", (unsigned char*)"Performs a software reset\r\n", io->stdOut);
#if McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_RPI_PICO
  McuShell_SendHelpStr((unsigned char*)"  bootloader", (unsigned char*)"Enter bootloader mode\r\n", io->stdOut);
#endif
  return ERR_OK;
}
#endif

/*
** ===================================================================
**     Method      :  SoftwareReset (component KinetisTools)
**
**     Description :
**         Performs a reset of the device
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuArmTools_SoftwareReset(void)
{
  /* Generic way to request a reset from software for ARM Cortex */
  /* See https://community.freescale.com/thread/99740
     To write to this register, you must write 0x5FA to the VECTKEY field, otherwise the processor ignores the write.
     SYSRESETREQ will cause a system reset asynchronously, so need to wait afterwards.
   */
#if McuLib_CONFIG_CPU_IS_ARM_CORTEX_M
#if McuLib_CONFIG_PEX_SDK_USED
  SCB_AIRCR = SCB_AIRCR_VECTKEY(0x5FA) | SCB_AIRCR_SYSRESETREQ_MASK;
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_S32K
  S32_SCB->AIRCR = S32_SCB_AIRCR_VECTKEY(0x5FA) | S32_SCB_AIRCR_SYSRESETREQ_MASK;
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_RPI_PICO
  #define SCB_AIRCR                          (*((uint32_t*)(0xe0000000+0xed0c)))
  #define SCB_AIRCR_VECTKEY_Pos              16U                                            /*!< SCB AIRCR: VECTKEY Position */
  #define SCB_AIRCR_SYSRESETREQ_Pos           2U                                            /*!< SCB AIRCR: SYSRESETREQ Position */
  #define SCB_AIRCR_SYSRESETREQ_Msk          (1UL << SCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

  SCB_AIRCR = (0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;
#else
  SCB->AIRCR = (0x5FA<<SCB_AIRCR_VECTKEY_Pos)|SCB_AIRCR_SYSRESETREQ_Msk;
#endif
#endif
  for(;;) {
    /* wait until reset */
  }
}

/*
** ===================================================================
**     Method      :  UIDGet (component KinetisTools)
**
**     Description :
**         Return the 128bit UID of the device
**     Parameters  :
**         NAME            - DESCRIPTION
**       * uid             - Pointer to 
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Return the 80bit UID of the device
 * \param uid Pointer to the buffer where to store the UID
 * \return Error code, ERR_OK if everything is ok.
 */
uint8_t McuArmTools_UIDGet(McuArmTools_UID *uid)
{
#if McuLib_CONFIG_CPU_IS_KINETIS
  #if McuLib_CONFIG_NXP_SDK_2_0_USED && !McuLib_CONFIG_IS_KINETIS_KE
  sim_uid_t tmp;
  unsigned int i, j;

  SIM_GetUniqueId(&tmp);
  /* init */
  for(i=0;i<sizeof(McuArmTools_UID);i++) {
    uid->id[i] = 0;
  }
  if (sizeof(sim_uid_t)>sizeof(McuArmTools_UID)) {
    return ERR_OVERFLOW;
  }
  /* copy into our own structure, data is right justified */
  for(i=0,j=sizeof(McuArmTools_UID)-sizeof(sim_uid_t);i<sizeof(sim_uid_t)&&i<sizeof(McuArmTools_UID);i++,j++) {
    uid->id[j] = ((uint8_t*)&tmp)[i];
  }
  #elif McuLib_CONFIG_IS_KINETIS_KE
  /* some devices like the KE02Z only have 64bit UUID: only SIM_UUIDH and SIM_UUIDL */
  uid->id[0] = 0;
  uid->id[1] = 0;
  uid->id[2] = 0;
  uid->id[3] = 0;
  uid->id[4] = 0;
  uid->id[5] = 0;
  uid->id[6] = 0;
  uid->id[7] = 0;
  uid->id[8] = (SIM->UUIDH>>24)&0xff;
  uid->id[9] = (SIM->UUIDH>>16)&0xff;
  uid->id[10] = (SIM->UUIDH>>8)&0xff;
  uid->id[11] = SIM->UUIDH&0xff;

  uid->id[12] = (SIM->UUIDL>>24)&0xff;
  uid->id[13] = (SIM->UUIDL>>16)&0xff;
  uid->id[14] = (SIM->UUIDL>>8)&0xff;
  uid->id[15] = SIM->UUIDL&0xff;
  #else /* not McuLib_CONFIG_NXP_SDK_2_0_USED */
    #ifdef SIM_UIDMH /* 80 or 128 bit UUID: SIM_UIDMH, SIM_UIDML and SIM_UIDL */
      #ifdef SIM_UIDH
  uid->id[0] = (SIM_UIDH>>24)&0xff;
  uid->id[1] = (SIM_UIDH>>16)&0xff;
  uid->id[2] = (SIM_UIDH>>8)&0xff;
  uid->id[3] = SIM_UIDH&0xff;
      #else
  uid->id[0] = 0;
  uid->id[1] = 0;
  uid->id[2] = 0;
  uid->id[3] = 0;
      #endif
  uid->id[4] = (SIM_UIDMH>>24)&0xff;
  uid->id[5] = (SIM_UIDMH>>16)&0xff;
  uid->id[6] = (SIM_UIDMH>>8)&0xff;
  uid->id[7] = SIM_UIDMH&0xff;

  uid->id[8] = (SIM_UIDML>>24)&0xff;
  uid->id[9] = (SIM_UIDML>>16)&0xff;
  uid->id[10] = (SIM_UIDML>>8)&0xff;
  uid->id[11] = SIM_UIDML&0xff;

  uid->id[12] = (SIM_UIDL>>24)&0xff;
  uid->id[13] = (SIM_UIDL>>16)&0xff;
  uid->id[14] = (SIM_UIDL>>8)&0xff;
  uid->id[15] = SIM_UIDL&0xff;
    #elif defined(SIM_UUIDMH) /* KE06Z: SIM_UUIDMH, SIM_UUIDML and SIM_UUIDL */
  uid->id[0] = 0;
  uid->id[1] = 0;
  uid->id[2] = 0;
  uid->id[3] = 0;
  uid->id[4] = 0;
  uid->id[5] = 0;
  uid->id[6] = (SIM_UUIDMH>>8)&0xff;
  uid->id[7] = SIM_UUIDMH&0xff;

  uid->id[8] = (SIM_UUIDML>>24)&0xff;
  uid->id[9] = (SIM_UUIDML>>16)&0xff;
  uid->id[10] = (SIM_UUIDML>>8)&0xff;
  uid->id[11] = SIM_UUIDML&0xff;

  uid->id[12] = (SIM_UUIDL>>24)&0xff;
  uid->id[13] = (SIM_UUIDL>>16)&0xff;
  uid->id[14] = (SIM_UUIDL>>8)&0xff;
  uid->id[15] = SIM_UUIDL&0xff;
    #else /* some devices like the KE02Z only have 64bit UUID: only SIM_UUIDH and SIM_UUIDL */
  uid->id[0] = 0;
  uid->id[1] = 0;
  uid->id[2] = 0;
  uid->id[3] = 0;
  uid->id[4] = 0;
  uid->id[5] = 0;
  uid->id[6] = 0;
  uid->id[7] = 0;
  uid->id[8] = (SIM_UUIDH>>24)&0xff;
  uid->id[9] = (SIM_UUIDH>>16)&0xff;
  uid->id[10] = (SIM_UUIDH>>8)&0xff;
  uid->id[11] = SIM_UUIDH&0xff;

  uid->id[12] = (SIM_UUIDL>>24)&0xff;
  uid->id[13] = (SIM_UUIDL>>16)&0xff;
  uid->id[14] = (SIM_UUIDL>>8)&0xff;
  uid->id[15] = SIM_UUIDL&0xff;
    #endif
  #endif /* McuLib_CONFIG_NXP_SDK_2_0_USED */
  return ERR_OK;
#elif McuLib_CONFIG_CPU_IS_LPC && (McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC845 || McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC804)
  status_t res;

  res = IAP_ReadUniqueID((uint32_t*)&uid->id[0]);
  if (res != kStatus_IAP_Success) {
    return ERR_FAILED;
  }
  return ERR_OK;
#elif McuLib_CONFIG_CPU_IS_LPC && McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC55S16
  int i;
  uint8_t *p;

  /* init */
  for(i=0;i<sizeof(McuArmTools_UID);i++) {
    uid->id[i] = 0;
  }
  p = (uint8_t*)&FLASH_NMPA->UUID_ARRAY[0];
  for(i=0;i<sizeof(McuArmTools_UID) && i<sizeof(FLASH_NMPA->UUID_ARRAY);i++) {
    uid->id[i] = *p;
    p++;
  }
  return ERR_OK;
#elif McuLib_CONFIG_CPU_IS_LPC && McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC55S69
  int i;

  /* init */
  for(i=0;i<sizeof(McuArmTools_UID);i++) {
    uid->id[i] = 0;
  }

  status_t status;
  flash_config_t flashConfig;

  status = FFR_Init(&flashConfig);
  if (status!=kStatus_Success) {
    return ERR_FAILED;
  }

  assert(sizeof(uid->id)*8>=128); /* must be at least 128 bits */
  status = FFR_GetUUID(&flashConfig, &uid->id[0]);
  if (status!=kStatus_Success) {
    return ERR_FAILED;
  }
  return ERR_OK; /* not implemented yet */
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_RPI_PICO
  memset(uid, 0, sizeof(McuArmTools_UID));
  /* We do *not* use flash_get_unique_id() of the flash API, because this requires interrupts disabled for *both* cores.
   * Instead, we use the one stored by the RP2040 boot code with pico_get_unique_board_id() */
  pico_unique_board_id_t id;

  pico_get_unique_board_id(&id);
  for(int i=0; i<sizeof(uid->id) && i<sizeof(id.id); i++) {
    uid->id[i] = id.id[i];
  }
  return ERR_OK;
#else
  (void)uid; /* not used */
  return ERR_FAILED;
#endif
}

/*
** ===================================================================
**     Method      :  UIDSame (component KinetisTools)
**
**     Description :
**         Compares two UID
**     Parameters  :
**         NAME            - DESCRIPTION
**       * src             - Pointer to 
**         Variable_1      - 
**     Returns     :
**         ---             - TRUE if the same, FALSE otherwise
** ===================================================================
*/
/*!
 * \brief Compares two UID
 * \param src One UID
 * \param dst The other UID
 * \return TRUE if the two UID's are the same
 */
bool McuArmTools_UIDSame(const McuArmTools_UID *src, const McuArmTools_UID *dst)
{
  unsigned int i;

  for(i=0; i<sizeof(McuArmTools_UID); i++) {
    if (src->id[i]!=dst->id[i]) {
      return FALSE; /* no match */
    }
  }
  return TRUE;
}

/*
** ===================================================================
**     Method      :  UIDtoString (component KinetisTools)
**
**     Description :
**         Returns the value of the UID as string
**     Parameters  :
**         NAME            - DESCRIPTION
**         uid             - 
**       * buf             - Pointer to 
**         bufSize         - 
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Transforms the 80bit UID into a string
 * \param id Pointer to the buffer where to store the string
 * \param bufSize Size of buffer in bytes
 * \return Error code, ERR_OK if everything is ok.
 */
uint8_t McuArmTools_UIDtoString(const McuArmTools_UID *uid, uint8_t *buf, size_t bufSize)
{
  unsigned int i;

  McuUtility_strcpy(buf, bufSize, (unsigned char*)"{");
  for(i=0;i<sizeof(McuArmTools_UID);i++) {
    McuUtility_strcat(buf, bufSize, (unsigned char*)"0x");
    McuUtility_strcatNum8Hex(buf, bufSize, uid->id[i]);
    if (i<sizeof(McuArmTools_UID)-1) {
      McuUtility_strcat(buf, bufSize, (unsigned char*)",");
    }
  }
  McuUtility_strcat(buf, bufSize, (unsigned char*)"}");
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  ParseCommand (component KinetisTools)
**
**     Description :
**         Shell Command Line parser. Method is only available if Shell
**         is enabled in the component properties.
**     Parameters  :
**         NAME            - DESCRIPTION
**         cmd             - Pointer to command string
**       * handled         - Pointer to variable which tells if
**                           the command has been handled or not
**       * io              - Pointer to I/O structure
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
#if McuArmTools_CONFIG_PARSE_COMMAND_ENABLED
uint8_t McuArmTools_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io)
{
  uint8_t res = ERR_OK;

  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP) == 0
    || McuUtility_strcmp((char*)cmd, "McuArmTools help") == 0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (   (McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0)
             || (McuUtility_strcmp((char*)cmd, "McuArmTools status") == 0)
            )
  {
    *handled = TRUE;
    res = PrintStatus(io);
  } else if (McuUtility_strcmp((char*)cmd, "McuArmTools reset") == 0) {
    *handled = TRUE;
    McuArmTools_SoftwareReset(); /* will perform RESET and does NOT return here! */
#if McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_RPI_PICO
  } else if (McuUtility_strcmp((char*)cmd, "McuArmTools bootloader") == 0) {
    *handled = TRUE;
    reset_usb_boot(0, 0); /* call MSD bootloader, does not return! */
#endif
  }
  return res;
}
#endif

/*
** ===================================================================
**     Method      :  GetKinetisFamilyString (component KinetisTools)
**
**     Description :
**         Determines the Kinetis Familiy based on SIM_SDID register
**     Parameters  : None
**     Returns     :
**         ---             - String describing the Kinetis Family
** ===================================================================
*/
McuArmTools_ConstCharPtr McuArmTools_GetKinetisFamilyString(void)
{
#if McuLib_CONFIG_CPU_IS_KINETIS
  #if McuLib_CONFIG_CORTEX_M==0
  #ifdef SIM_SDID /* normal Kinetis define this */
    int32_t val;

    val = (SIM_SDID>>28)&0x3; /* bits 30..28 */
    if (val>=0 && val<=(int32_t)(sizeof(KinetisM0FamilyStrings)/sizeof(KinetisM0FamilyStrings[0]))) {
      return KinetisM0FamilyStrings[val];
    } else {
      return (McuArmTools_ConstCharPtr)"M0 Family ID out of bounds!";
    }
  #elif defined(SIM_SRSID_FAMID) /* MKE02Z4 defines this, hopefully all other KE too... */
    return (McuArmTools_ConstCharPtr)"KE0x Family"; /* 0000 only KE0x supported */
  #elif defined(SIM_SDID_FAMID) || defined(SIM_SDID_FAMILYID)
    int32_t val;

    val = ((SIM->SDID)>>28)&0xF; /* bits 31..28 */
    if (val>=0 && val<=(int32_t)(sizeof(KinetisM0FamilyStrings)/sizeof(KinetisM0FamilyStrings[0]))) {
      return KinetisM0FamilyStrings[val];
    } else {
      return (McuArmTools_ConstCharPtr)"M0 Family ID out of bounds!";
    }
  #else
    #error "Unknown architecture!"
    return (McuArmTools_ConstCharPtr)"ERROR";
  #endif
  #elif McuLib_CONFIG_CORTEX_M==4
    #ifdef SIM_SDID /* normal Kinetis define this */
    int32_t val;

    val = (SIM_SDID>>4)&0x3; /* bits 6..4 */
    if (val>=0 && val<=(int32_t)(sizeof(KinetisM4FamilyStrings)/sizeof(KinetisM4FamilyStrings[0]))) {
      return KinetisM4FamilyStrings[val];
    } else {
      return (McuArmTools_ConstCharPtr)"M4 Family ID out of bounds!";
    }
    #elif defined(SIM_SDID_FAMID) || defined(SIM_SDID_FAMILYID)
    int32_t val;

    val = ((SIM->SDID)>>4)&0x3; /* bits 6..4 */
    if (val>=0 && val<=(int32_t)(sizeof(KinetisM4FamilyStrings)/sizeof(KinetisM4FamilyStrings[0]))) {
      return KinetisM4FamilyStrings[val];
    } else {
      return (McuArmTools_ConstCharPtr)"M4 Family ID out of bounds!";
    }
    #else
    #error "Unknown architecture!"
    return (McuArmTools_ConstCharPtr)"ERROR";
    #endif
  #elif McuLib_CONFIG_CORTEX_M==7
  return (McuArmTools_ConstCharPtr)"Cortex-M7";
  #else
  #error "Unknown architecture!"
  return (McuArmTools_ConstCharPtr)"ERROR";
  #endif
#elif McuLib_CONFIG_CPU_IS_NORDIC_NRF
  return (McuArmTools_ConstCharPtr)"Nordic nRF";
#elif McuLib_CONFIG_CPU_IS_STM
  return (McuArmTools_ConstCharPtr)"STM32";
#elif McuLib_CONFIG_CPU_IS_IMXRT
  return (McuArmTools_ConstCharPtr)"NXP i.MX RT";
#elif McuLib_CONFIG_CPU_IS_S32K
  return (McuArmTools_ConstCharPtr)"NXP S32K";
#elif McuLib_CONFIG_CPU_IS_LPC
  #if McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC845
  return (McuArmTools_ConstCharPtr)"NXP LPC845";
  #elif McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC804
  return (McuArmTools_ConstCharPtr)"NXP LPC804";
  #elif McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC55S16
  return (McuArmTools_ConstCharPtr)"NXP LPC55S16";
  #elif McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_LPC55S59
  return (McuArmTools_ConstCharPtr)"NXP LPC55S69";
  #elif McuLib_CONFIG_CPU_IS_LPC55xx
  return (McuArmTools_ConstCharPtr)"NXP LPC55xx";
  #else
  return (McuArmTools_ConstCharPtr)"NXP LPC";
  #endif
#elif McuLib_CONFIG_SDK_VERSION_USED==McuLib_CONFIG_SDK_RPI_PICO
  #if McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_RP2040
    return (McuArmTools_ConstCharPtr)"Raspberry Pi Pico, RP2040";
  #else
    return (McuArmTools_ConstCharPtr)"Raspberry Pi Pico";
  #endif
#else
  return (McuArmTools_ConstCharPtr)"UNKNOWN";
#endif
}

/*
** ===================================================================
**     Method      :  GetPC (component KinetisTools)
**
**     Description :
**         returns the program counter
**     Parameters  : None
**     Returns     :
**         ---             - program counter
** ===================================================================
*/
void* McuArmTools_GetPC(void)
{
#ifdef __GNUC__
  void *pc;

  __asm__ __volatile__ ("mov %0, pc" : "=r"(pc));
  return pc;
#else
  #warning "only for GCC"
  return NULL;
#endif
}

/*
** ===================================================================
**     Method      :  GetSP (component KinetisTools)
**
**     Description :
**         returns the stack pointer
**     Parameters  : None
**     Returns     :
**         ---             - stack pointer
** ===================================================================
*/
void* McuArmTools_GetSP(void)
{
#ifdef __GNUC__
  void *sp;

  __asm__ __volatile__ ("mrs %0, msp" : "=r"(sp));
  return sp;
#else
  #warning "only for GCC"
  return NULL;
#endif
}

/*
** ===================================================================
**     Method      :  SetPSP (component KinetisTools)
**
**     Description :
**         sets the process stack pointer
**     Parameters  :
**         NAME            - DESCRIPTION
**         setval          - new PSP value
**     Returns     : Nothing
** ===================================================================
*/
void McuArmTools_SetPSP(void *setval)
{
#ifdef __GNUC__
  __asm__ volatile ("msr psp, %[value]\n\t""dmb\n\t""dsb\n\t""isb\n\t"::[value]"r"(setval):);
  __asm__ volatile ("" ::: "memory");
#else
  #warning "only for GCC implemented"
#endif
}

/*
** ===================================================================
**     Method      :  SetLR (component KinetisTools)
**
**     Description :
**         Sets the link register
**     Parameters  :
**         NAME            - DESCRIPTION
**         setval          - new LR value
**     Returns     : Nothing
** ===================================================================
*/
void McuArmTools_SetLR(uint32_t setval)
{
#ifdef __GNUC__
  __asm__ volatile ("mov lr, %[value]\n\t"::[value]"r"(setval):);
  __asm__ volatile ("" ::: "memory");
#else
  #warning "only for GCC"
#endif
}

/*
** ===================================================================
**     Method      :  InitCycleCounter (component KinetisTools)
**
**     Description :
**         Initializes the cycle counter, available if the core has a
**         DWT (Data Watchpoint and Trace) unit, usually present on
**         M3/M4/M7
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/**
void McuArmTools_InitCycleCounter(void)
{
  ** Implemented as macro in header file
}
*/

/*
** ===================================================================
**     Method      :  ResetCycleCounter (component KinetisTools)
**
**     Description :
**         Reset the cycle counter (set it to zero)
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/**
void McuArmTools_ResetCycleCounter(void)
{
  ** Implemented as macro in header file
}
*/

/*
** ===================================================================
**     Method      :  EnableCycleCounter (component KinetisTools)
**
**     Description :
**         Enables counting the cycles.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/**
void McuArmTools_EnableCycleCounter(void)
{
  ** Implemented as macro in header file
}
*/

/*
** ===================================================================
**     Method      :  DisableCycleCounter (component KinetisTools)
**
**     Description :
**         Disables the cycle counter.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/**
void McuArmTools_DisableCycleCounter(void)
{
  ** Implemented as macro in header file
}
*/

/*
** ===================================================================
**     Method      :  GetCycleCounter (component KinetisTools)
**
**     Description :
**         Return the current cycle counter value
**     Parameters  : None
**     Returns     :
**         ---             - cycle counter
** ===================================================================
*/
/**
uint32_t McuArmTools_GetCycleCounter(void)
{
  ** Implemented as macro in header file
}
*/

/*
** ===================================================================
**     Method      :  Deinit (component KinetisTools)
**
**     Description :
**         Driver de-initialization routine
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuArmTools_Deinit(void)
{
  /* nothing needed */
}

/*
** ===================================================================
**     Method      :  Init (component KinetisTools)
**
**     Description :
**         Driver initialization routine
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuArmTools_Init(void)
{
  /* Nothing needed */
}

/*
** ===================================================================
**     Method      :  GetUsedMainStackSpace (component KinetisTools)
**
**     Description :
**         Returns the used main stack space, based on the overwritten
**         checking pattern.
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Returns the used main stack space, based on the overwritten checking pattern.
 * \return Number of used main stack bytes
 */
uint32_t McuArmTools_GetUsedMainStackSpace(void)
{
  return McuArmTools_GetLinkerMainStackSize()-McuArmTools_GetUnusedMainStackSpace();
}

/*
** ===================================================================
**     Method      :  GetUnusedMainStackSpace (component KinetisTools)
**
**     Description :
**         Calculates the unused stack space, based on the checking
**         pattern.
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Calculates the unused stack space, based on the checking pattern.
 * \return Number of unused main stack space.
 */
uint32_t McuArmTools_GetUnusedMainStackSpace(void)
{
  uint32_t unused = 0; /* number of unused bytes */
  uint32_t *p = (uint32_t*)&McuArmTools_CONFIG_LINKER_SYMBOL_STACK_BASE;

  /* check if the pattern stored on the stack has been changed */
  while (*p==McuArmTools_CONFIG_STACK_CHECK_PATTERN) {
    unused += sizeof(uint32_t); /* count number of unused bytes */
    p++;
  }
  return unused; /* return the number of unused bytes */
}

/*
** ===================================================================
**     Method      :  FillMainStackSpace (component KinetisTools)
**
**     Description :
**         Fill the stack space with the checking pattern, up to the
**         current MSP.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*!
 * \brief Fill the stack space with the checking pattern, up to the current MSP.
 */
void McuArmTools_FillMainStackSpace(void)
{
  uint32_t *base = (uint32_t*)&McuArmTools_CONFIG_LINKER_SYMBOL_STACK_BASE;
  uint32_t *msp = McuArmTools_GetSP()-32; /* get current MSP stack pointer, with a safety margin of 32 */
  /* the current MSP is near the top */
  while(base<msp) { /* fill from the base to the top */
    *base = McuArmTools_CONFIG_STACK_CHECK_PATTERN;
    base++;
  }
}

/*
** ===================================================================
**     Method      :  GetLinkerMainStackSize (component KinetisTools)
**
**     Description :
**         Returns the size of the main (MSP) stack size, using linker
**         symbols for top (higher address) and base (lower address).
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Returns the size of the main (MSP) stack size, using linker symbols for top (higher address) and base (lower address).
 * \return Number of bytes allocated by the linker for the stack
 */
uint32_t McuArmTools_GetLinkerMainStackSize(void)
{
  return (uint32_t)&McuArmTools_CONFIG_LINKER_SYMBOL_STACK_TOP - (uint32_t)&McuArmTools_CONFIG_LINKER_SYMBOL_STACK_BASE;
}

/*
** ===================================================================
**     Method      :  GetLinkerMainStackTop (component KinetisTools)
**
**     Description :
**         Return the stack top, as set in the linker file. The stack
**         grows from the top (higher address) to the base (lower
**         address).
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Return the stack top, as set in the linker file. The stack grows from the top (higher address) to the base (lower address).
 * \return Return the address of the top (last) stack unit
 */
McuArmTools_uint32_t_Ptr McuArmTools_GetLinkerMainStackTop(void)
{
  return (McuArmTools_uint32_t_Ptr)&McuArmTools_CONFIG_LINKER_SYMBOL_STACK_TOP;
}

/*
** ===================================================================
**     Method      :  GetLinkerMainStackBase (component KinetisTools)
**
**     Description :
**         Return the stack bottom, as configured in the linker file.
**         The stack grows from the top (higher address) to the base
**         (lower address).
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
/*!
 * \brief Return the stack bottom, as configured in the linker file. The stack grows from the top (higher address) to the base (lower address).
 * \return Return the address of the top (last) stack unit
 */
McuArmTools_uint32_t_Ptr McuArmTools_GetLinkerMainStackBase(void)
{
  return (McuArmTools_uint32_t_Ptr)&McuArmTools_CONFIG_LINKER_SYMBOL_STACK_BASE;
}


#endif /* McuLib_CONFIG_CPU_IS_ARM_CORTEX_M */
/* END McuArmTools. */

/*!
** @}
*/
