#include "compiler.h"
#include <_dpmi.h>

#include "mytypes.h"
#include "adriver.h"
#include "dpmiserv.h"
#include "djvidsys.h"
#include "console.h"
#include "galey.h"

  __dpmi_version_ret dpmi_ver;

bool DPMIInit (int Size_tb)
{
#ifdef __WATCOMC__
  if (!_dos_ds)
    __dpmi_allocate_dos_memory ((Size_tb + 0xf) >> 4, &_dos_ds);
#endif
  return true;
}

bool DPMIFree ()
{
#ifdef __WATCOMC__
  if (_dos_ds)
   {
    __dpmi_free_dos_memory (_dos_ds);
    _dos_ds = 0;
   }
#endif
  return true;
}

bool LockMemory (void *Addr, dword Size, bool LockFlag)
{
  if (!(dpmi_ver.flags & 4))
    return true;

  unsigned long BaseAddr;
  dword rc;

#ifdef __WATCOMC__
  struct SREGS sregs;
  segread (&sregs);
  rc = __dpmi_get_segment_base_address (sregs.cs, &BaseAddr);

  if (rc & INTR_CF)
    return false;
#else
  BaseAddr = __djgpp_base_address;
#endif

  __dpmi_meminfo mem_info;
  mem_info.address = (dword)Addr + BaseAddr;
  mem_info.size = Size;

  if (LockFlag)
    rc = __dpmi_lock_linear_region (&mem_info);
  else
    rc = __dpmi_unlock_linear_region (&mem_info);

  if (rc & INTR_CF)
    return false;
  else
    return true;
}

static byte IRQMask;

bool IRQHook (dword IRQ, __dpmi_paddr *pIntHandler, __dpmi_paddr *pOldIntHandler, dword Size)
{
  byte Int;
  bool rc;

#ifdef __GNUC__
  _go32_dpmi_seginfo info;
  info.size = Size;
  info.pm_offset = pIntHandler->offset32;
  info.pm_selector = _my_cs ();
  _go32_dpmi_allocate_iret_wrapper (&info);
  pIntHandler->offset32 = info.pm_offset;
  pIntHandler->selector = info.pm_selector;
#else
  struct SREGS sregs;
  segread (&sregs);
  pIntHandler->selector = sregs.cs;
#endif

  if (IRQ > 7)
   {
    Int = dpmi_ver.slave_pic + (IRQ - 8);
    rc = __dpmi_get_protected_mode_interrupt_vector (Int, pOldIntHandler);

    if (rc & INTR_CF)
      return false;

    IRQMask = inp (0xA1);
    outp (0xA1, IRQMask & ~(1 << (IRQ - 8)));
   }
  else
   {
    Int = dpmi_ver.master_pic + IRQ;
    rc = __dpmi_get_protected_mode_interrupt_vector (Int, pOldIntHandler);

    if (rc & INTR_CF)
      return false;

    IRQMask = inp (0x21);
    outp (0x21, IRQMask & ~(1 << IRQ));
   }

  rc = __dpmi_set_protected_mode_interrupt_vector (Int, pIntHandler);

  if (rc & INTR_CF)
    return false;
  else
    return true;
}

bool IRQUnHook (dword IRQ, __dpmi_paddr *pIntHandler, __dpmi_paddr *pOldIntHandler, dword Size)
{
  byte Int;

  if (IRQ > 7)
   {
    Int = dpmi_ver.slave_pic + (IRQ - 8);
    outp (0xA1, IRQMask);
   }
  else
   {
    Int = dpmi_ver.master_pic + IRQ;
    outp (0x21, IRQMask);
   }

  bool rc = __dpmi_set_protected_mode_interrupt_vector (Int, pOldIntHandler);

#ifdef __GNUC__
  _go32_dpmi_seginfo info;
  info.size = Size;
  info.pm_offset = pIntHandler->offset32;
  info.pm_selector = pIntHandler->selector;
  _go32_dpmi_free_iret_wrapper (&info);
#endif

  if (rc & INTR_CF)
    return false;
  else
   {
    pOldIntHandler->offset32 = 0;
    pOldIntHandler->selector = 0;
    return true;
   }
}

#define TAVD_LOCKED_SIZE ((dword)EndTAVDLockedCode - (dword)BeginTAVDLockedCode)
#define GALEY_LOCKED_SIZE ((dword)EndGALEYLockedCode - (dword)BeginGALEYLockedCode)
#define SCRCTRL_LOCKED_SIZE ((dword)EndSCRCTRLLockedCode - (dword)BeginSCRCTRLLockedCode)

void RestoreInterupt (dword IRQ, __dpmi_paddr *pMyIntHandler, __dpmi_paddr *pOldIntHandler)
{
  if (pOldIntHandler->offset32)
   {
    IRQUnHook (IRQ, pMyIntHandler, pOldIntHandler, TAVD_LOCKED_SIZE);
    LockMemory ((void*)BeginTAVDLockedCode, TAVD_LOCKED_SIZE, UNLOCK_MEMORY);
    LockMemory ((void*)BeginGALEYLockedCode, GALEY_LOCKED_SIZE, UNLOCK_MEMORY);
    LockMemory ((void*)BeginSCRCTRLLockedCode, SCRCTRL_LOCKED_SIZE, UNLOCK_MEMORY);
   }
}

void SetupInterrupt (dword IRQ, __dpmi_paddr *pMyIntHandler, __dpmi_paddr *pOldIntHandler, dword IntRoutine)
{
  __dpmi_get_version (&dpmi_ver);
  PrintWorkStatusBegin ("CPU detected");
  PrintWorkStatusEnd ("%X", ((dword)dpmi_ver.cpu << 8) + 0x80086);
  PrintWorkStatusBegin ("DPMI host found");
  PrintWorkStatusEnd ("%d.%d", dpmi_ver.major, dpmi_ver.minor);
  PrintWorkStatusBegin ("Virtual memory");

  if (dpmi_ver.flags & 4)
    PrintWorkStatusEnd ("Supported");
  else
    PrintWorkStatusEnd ("Unsupported");

  LockMemory ((void*)BeginSCRCTRLLockedCode, SCRCTRL_LOCKED_SIZE, LOCK_MEMORY);
  LockMemory ((void*)BeginGALEYLockedCode, GALEY_LOCKED_SIZE, LOCK_MEMORY);
  LockMemory ((void*)BeginTAVDLockedCode, TAVD_LOCKED_SIZE, LOCK_MEMORY);
  PrintWorkStatusBegin ("Interrupt intercepted");

  pMyIntHandler->offset32 = IntRoutine;
  bool rc = IRQHook (IRQ, pMyIntHandler, pOldIntHandler, TAVD_LOCKED_SIZE);

  if (rc)
    PrintWorkStatusEnd ("Success");
  else
    PrintWorkStatusEnd ("Failed");
}
