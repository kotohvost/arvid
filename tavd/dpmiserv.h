#ifndef __DPMISERV_H__
#define __DPMISERV_H__

#define LOCK_MEMORY true
#define UNLOCK_MEMORY false

#ifdef __WATCOMC__

#ifdef __tb
#undef __tb
#endif
#ifdef _dos_ds
#undef _dos_ds
#endif

extern int __tb, _dos_ds;
#endif

extern __dpmi_version_ret dpmi_ver;
extern bool DPMIInit (int Size_tb);
extern bool DPMIFree ();
extern bool LockMemory (void *Addr, dword Size, bool LockFlag);
extern bool IRQHook (dword IRQ, __dpmi_paddr *pIntHandler, __dpmi_paddr *pOldIntHandler, dword Size);
extern bool IRQUnHook (dword IRQ, __dpmi_paddr *pIntHandler, __dpmi_paddr *pOldIntHandler, dword Size);
extern void RestoreInterupt (dword IRQ, __dpmi_paddr *pMyIntHandler, __dpmi_paddr *pOldIntHandler);
extern void SetupInterrupt (dword IRQ, __dpmi_paddr *pMyIntHandler, __dpmi_paddr *pOldIntHandler, dword IntRoutine);
#endif