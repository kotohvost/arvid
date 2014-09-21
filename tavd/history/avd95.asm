
PAGE  59,132

;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;€€                                                                      €€
;€€                             AVD95                                    €€
;€€                                                                      €€
;€€      Created:   20-Oct-97                                            €€
;€€      Code type: Windows VxD                                          €€
;€€      Passes:    9          Analysis Options on: M                    €€
;€€                                                                      €€
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

target          EQU   'M6'                      ; Target assembler: MASM-6.11

include  srmacros.inc

.386p

.387

include  vmm.inc



; The following equates show data references outside the range of the program.

d_4000_445F_e   equ     445Fh                   ;*
d_F000_FFFF_e   equ     0FFFFh

;------------------------------------------------------------  seg_a   ----

seg_a           segment para public use32
                assume cs:seg_a  , ds:seg_a

data_a_0000     dw      6 dup (0)
data_a_000C     db      0
                db      77 dup (0)
data_a_005A     dd      00000h
                db      34 dup (0)
data_a_0080     dd      00000h
                db      16 dup (0)
data_a_0094     dd      00000h
                db      8 dup (0)
data_a_00A0     db      0
                db      0, 0, 0
data_a_00A4     dd      00000h
                db      0, 0, 0, 0
data_a_00AC     dd      00000h
                db      12 dup (0)
data_a_00BC     dd      00000h
                db      0, 0, 0, 0
data_a_00C4     dd      00000h
                db      0, 0, 0, 0
data_a_00CC     dd      00000h
data_a_00D0     dd      00000h
data_a_00D4     dd      00000h
                db      24 dup (0)
data_a_00F0     db      0
                db      27 dup (0)
data_a_010C     dd      00000h
data_a_0110     dd      00000h
data_a_0114     dd      00000h
data_a_0118     dd      00000h
                db      12 dup (0)
data_a_0128     dd      00000h
                db      8 dup (0)
data_a_0134     dd      00000h
data_a_0138     dd      00000h
data_a_013C     dd      00000h
data_a_0140     dd      00000h
data_a_0144     dd      00000h
data_a_0148     dd      00000h
                db      0, 0, 0, 0
data_a_0150     dd      00000h
data_a_0154     dd      00000h
                db      8 dup (0)
data_a_0160     dd      00000h
                db      12 dup (0)
data_a_0170     dd      00000h
                db      24 dup (0)
data_a_018C     dd      00000h
data_a_0190     dd      00000h
data_a_0194     dd      00000h
data_a_0198     dd      00000h
data_a_019C     dd      00000h
                db      64 dup (0)
data_a_01E0     db      0
                db      55 dup (0)
data_a_0218     dd      00000h
                db      20 dup (0)
data_a_0230     db      0
                db      79 dup (0)
data_a_0280     db      0
                db      523 dup (0)
hdsk_status_2_  db      0Ah
                db      0, 0, 0, 0, 0, 0
                db      0
hdsk0_cylinder_ db      0
                db      0, 0, 0
                db      'ArVid Device Driver Error'
                db      7 dup (0)
                db       60h, 05h, 00h, 00h, 58h, 05h
                db       00h, 00h
                db       52h, 6Fh
                db      'ot\ArvidStreamer\0000', 0
                db      'ArVid was installed not correctl'
                db      'y!'
                db      0, 0, 0, 0, 0, 0
                db      'ARVID1031'
                db      7 dup (0)
                db       41h, 56h, 44h, 31h, 30h, 33h
                db       31h, 00h
                db      'ARVID1051'
                db      7 dup (0)
                db       41h, 56h, 44h, 31h, 30h, 35h
                db       31h, 00h
                db      'ARVID1052'
                db      7 dup (0)
                db       41h, 56h, 44h, 31h, 30h, 35h
                db       32h, 00h
                db      'ARVID STREAMER'
                db       00h, 00h, 41h, 56h, 44h, 33h
                db       31h, 5Fh, 31h, 00h, 41h, 56h
                db       44h, 33h, 31h, 5Fh, 30h, 00h
                db      'HardwareID'
                db      0, 0
                db      'MatchingDeviceID'
                db      8 dup (0)
                db      'IO Trap ArVid Win95 Device Drive'
                db      'r! Application with IO is ignore'
                db      'd.'
                db       00h, 00h,0D0h, 04h, 0Eh
                db      21 dup (0)
                db       40h, 05h, 0Eh, 00h, 00h, 00h
                db      18 dup (0)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0600      proc    near
                mov     eax,ecx
                mov     dword ptr [ecx],0
                mov     dword ptr [ecx+4],0
                retn
sub_a_0600      endp

                                                ;* No entry point to code
                mov     eax,dword ptr [ecx+4]
                sub     eax,8
                retn
                db      9 dup (0CCh)
                db       8Bh, 01h,0FFh, 60h, 04h,0CCh
                db      0CCh,0CCh, 00h, 00h, 00h, 00h
                db       00h, 04h,0BDh, 1Ah, 01h, 00h
                db       00h, 00h
                db      'AVD95   '
                db       00h, 00h, 00h, 80h, 50h, 07h
                db       00h
                db      33 dup (0)
                db       76h, 65h, 72h, 50h, 50h, 00h
                db       00h, 00h
                db      '1vsR2vsR3vsRp', 8
                db       00h, 00h, 30h, 09h
                db      0, 0
data_a_0680     dd      00000h
                db      20 dup (0)
data_a_0698     dd      00000h
data_a_069C     db      0
                db      0FFh, 00h
                db      18 dup (0FFh)
                db      0C3h
                db      12 dup (0BDh)
                db      0C3h,0FFh,0FFh,0FBh,0F3h,0E3h
                db      0C3h
                db      9 dup (0FBh)
                db      0C1h,0FFh,0FFh,0C3h,0BDh,0BDh
                db      0FDh,0FDh,0FDh,0FDh,0FDh,0FBh
                db      0F7h,0EFh,0DFh,0BFh, 81h,0FFh
                db      0FFh, 81h,0FDh,0FBh,0F7h,0EFh
                db      0DFh, 83h,0FDh,0FDh,0FDh,0FDh
                db      0BDh,0BDh,0C3h,0FFh,0FFh,0F7h
                db      0EFh,0EFh,0DFh,0DFh,0BBh,0BBh
                db       83h,0FBh,0FBh,0FBh,0FBh,0FBh
                db      0C1h,0FFh,0FFh, 81h,0BFh,0BFh
                db      0BFh,0BFh,0BFh, 83h,0FDh,0FDh
                db      0FDh,0FDh,0BDh,0BDh,0C3h,0FFh
                db      0FFh,0C3h,0BDh,0BDh,0BFh,0BFh
                db      0BFh, 83h,0BDh,0BDh,0BDh,0BDh
                db      0BDh,0BDh,0C3h,0FFh,0FFh, 80h
                db      0FEh,0FDh,0FDh,0FBh,0FBh,0C3h
                db      0F7h,0EFh,0EFh,0DFh,0DFh,0BFh
                db      0BFh,0FFh,0FFh,0C3h,0BDh,0BDh
                db      0BDh,0BDh,0DBh,0E7h,0E7h,0DBh
                db      0BDh,0BDh,0BDh,0BDh,0C3h,0FFh
                db      0FFh,0C3h,0BDh,0BDh,0BDh,0BDh
                db      0BDh,0C1h,0FDh,0FDh,0FDh,0FDh
                db      0BDh,0BDh,0C3h,0FFh, 83h,0F8h
                db       23h, 75h, 0Dh, 56h, 52h, 53h
                db       51h,0E8h, 62h, 00h, 00h, 00h
                db       83h,0F8h, 01h,0C3h, 83h,0F8h
                db       1Bh, 75h, 0Bh, 6Ah, 00h,0E8h
                db       00h, 00h, 00h, 00h, 83h,0F8h
                db       01h,0C3h, 83h,0F8h, 1Ch, 75h
                db       0Bh, 6Ah, 00h,0E8h,0C2h, 00h
                db       00h, 00h, 83h,0F8h, 01h,0C3h
loc_a_0782::
                cmp     eax,1
                jne     short loc_a_0795        ; Jump if not equal
                push    628h
                call    sub_a_0791

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0791      proc    near
                cmp     eax,1
                retn
sub_a_0791      endp

loc_a_0795::
                cmp     eax,5
                jne     short loc_a_07A5        ; Jump if not equal
                push    dword ptr 1
                call    sub_a_0840
                cmp     eax,1
                retn
loc_a_07A5::
                clc                             ; Clear carry flag
                retn
                                                ;* No entry point to code
                push    ebp
                call    sub_a_0860
                mov     dword ptr [ebp+1Ch],eax
                retn
                                                ;* No entry point to code
                push    ebp
                call    sub_a_0860
                mov     dword ptr [ebp+1Ch],eax
                retn
                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                mov     eax,dword ptr [esp+4]
                push    esi
                test    eax,eax
                jz      short loc_a_0814        ; Jump if zero
                cmp     eax,0FFFFFFFFh
                je      short loc_a_0832        ; Jump if equal
                mov     eax,dword ptr data_a_0000
                push    eax
                inc     dword ptr data_a_0000
                push    dword ptr 0
                push    dword ptr 0
                call    sub_a_2930
                mov     eax,dword ptr [esp+20h]
                add     esp,0Ch
                mov     esi,dword ptr [eax+20h]
                mov     ecx,dword ptr [eax+0Ch]
                push    dword ptr 0
                mov     eax,dword ptr [eax+18h]
                push    esi
                push    eax
                mov     eax,ecx
;*              and     ecx,0FFh
                db       81h,0E1h,0FFh, 00h, 00h, 00h   ;  Fixup - byte match
                shr     eax,8                   ; Shift w/zeros fill
                push    eax
                push    ecx
                call    sub_a_1160
                mov     word ptr [esi+2],ax
                xor     eax,eax                 ; Zero register
                sti                             ; Enable interrupts
                pop     esi
                retn    10h
loc_a_0814::
                mov     eax,dword ptr data_a_0000
                push    eax
                inc     dword ptr data_a_0000
                push    dword ptr 5
                push    dword ptr 0
                call    sub_a_2930
                add     esp,0Ch
                xor     eax,eax                 ; Zero register
                pop     esi
                retn    10h
loc_a_0832::
                xor     eax,eax                 ; Zero register
                pop     esi
                mov     dword ptr data_a_0000,eax
                retn    10h
                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0840      proc    near
                mov     eax,dword ptr [esp+4]
                push    eax
                call    sub_a_1210
                add     esp,4
                mov     eax,1
                retn    4
sub_a_0840      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0860      proc    near
                xor     eax,eax                 ; Zero register
                retn    4
sub_a_0860      endp

                db      11 dup (0CCh)
                db      0E8h, 0Bh, 00h, 00h, 00h,0E9h
                db       26h, 00h, 00h, 00h,0CCh,0CCh
                db      0CCh,0CCh,0CCh,0CCh, 56h, 57h
                db      0BFh, 00h, 00h, 00h, 00h,0BEh
                db       01h, 00h, 00h, 00h
loc_a_088C::
                mov     ecx,edi
                add     edi,238h
                call    sub_b_01A0
                dec     esi
                jns     loc_a_088C              ; Jump if not sign
                pop     edi
                pop     esi
                retn
                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                push    8B0h
                call    sub_a_2930
                add     esp,4
                retn
                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                push    esi
                mov     al,byte ptr data_a_0000
                push    edi
                test    al,1
                jnz     short loc_a_08DC        ; Jump if not zero
                or      al,1
                mov     edi,470h
                mov     esi,1
                mov     byte ptr data_a_0000,al
loc_a_08CC::
                sub     edi,238h
                mov     ecx,edi
                call    sub_a_08E0
                dec     esi
                jns     loc_a_08CC              ; Jump if not sign
loc_a_08DC::
                pop     edi
                pop     esi
                retn
                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_08E0      proc    near
                push    esi
                mov     esi,ecx
                lea     ecx,dword ptr data_a_0150[ecx]  ; Load effective addr
                call    sub_a_2930
                lea     ecx,dword ptr data_a_0114[esi]  ; Load effective addr
                test    esi,esi
                jnz     short loc_a_08FA        ; Jump if not zero
                xor     ecx,ecx                 ; Zero register
loc_a_08FA::
                call    sub_a_2930
                lea     eax,dword ptr data_a_010C[esi]  ; Load effective addr
                test    esi,esi
                jnz     short loc_a_090B        ; Jump if not zero
                xor     eax,eax                 ; Zero register
loc_a_090B::
                mov     eax,[eax]
                push    eax                     ; PARAMETER_1
                call    sub_a_39FA
                add     esp,4
                test    esi,esi
                jz      short loc_a_0924        ; Jump if zero
                lea     ecx,dword ptr [esi+54h] ; Load effective addr
                call    sub_a_2250
                pop     esi
                retn
loc_a_0924::
                xor     ecx,ecx                 ; Zero register
                call    sub_a_2250
                pop     esi
                retn
sub_a_08E0      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                jmpn    loc_a_0940
                db      11 dup (0CCh)
loc_a_0940::
                mov     word ptr data_a_0000,0
                retn
                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0950      proc    near
                xor     eax,eax                 ; Zero register
                mov     ecx,dword ptr [esp+4]
                mov     ax,data_a_0000
                cmp     eax,ecx
                ja      short loc_a_0963        ; Jump if above
                xor     eax,eax                 ; Zero register
                retn
loc_a_0963::
                lea     eax,dword ptr data_a_0000[ecx*8]        ; Load effective addr
                lea     eax,[eax][eax*8]        ; Load effective addr
                sub     eax,ecx
                lea     eax,dword ptr data_a_0000[eax*8]        ; Load effective addr
                retn
sub_a_0950      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0980      proc    near
                mov     ecx,dword ptr [esp+4]
                call    sub_a_0990
                mov     eax,1
                retn
sub_a_0980      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0990      proc    near
                push    ebx
                push    esi
                push    edi
                mov     edi,ecx
                cmp     dword ptr data_a_0190[ecx],0
                jne     loc_a_0A64              ; Jump if not equal
                cmp     dword ptr data_a_018C[edi],0
                je      loc_a_0A64              ; Jump if equal
                cmp     dword ptr [edi+58h],41Ch
                jge     loc_a_0A64              ; Jump if > or =
loc_a_09BC::
                cmp     dword ptr data_a_0148[edi],0
                mov     eax,dword ptr data_a_0140[edi]
                jz      short loc_a_09DC        ; Jump if zero
                add     eax,1E78h
                lea     esi,dword ptr data_a_0138[edi]  ; Load effective addr
                cmp     [esi],eax
                ja      short loc_a_09F1        ; Jump if above
                jmp     short loc_a_09EB
loc_a_09DC::
                add     eax,141Ch
                lea     esi,dword ptr data_a_0138[edi]  ; Load effective addr
                cmp     [esi],eax
                ja      short loc_a_09F1        ; Jump if above
loc_a_09EB::
                mov     eax,dword ptr data_a_0134[edi]
loc_a_09F1::
                cmp     dword ptr data_a_013C[edi],eax
                je      short loc_a_0A64        ; Jump if equal
                lea     ebx,dword ptr data_a_0150[edi]  ; Load effective addr
                push    ebx
                call    sub_a_1CC0
                test    eax,eax
                jz      short loc_a_0A64        ; Jump if zero
                sti                             ; Enable interrupts
                mov     eax,dword ptr data_a_0170[edi]
                mov     ecx,dword ptr data_a_0140[edi]
                push    eax
                mov     edx,dword ptr data_a_0160[edi]
                push    ecx
                push    edx
                lea     ecx,dword ptr [edi+54h] ; Load effective addr
                call    sub_a_25E0
                cli                             ; Disable interrupts
                mov     ecx,ebx
                call    sub_a_1D20
                cmp     dword ptr data_a_0148[edi],0
                mov     eax,dword ptr data_a_0140[edi]
                jz      short loc_a_0A49        ; Jump if zero
                add     eax,1E78h
                cmp     [esi],eax
                ja      short loc_a_0A58        ; Jump if above
                jmp     short loc_a_0A52
loc_a_0A49::
                add     eax,141Ch
                cmp     [esi],eax
                ja      short loc_a_0A58        ; Jump if above
loc_a_0A52::
                mov     eax,dword ptr data_a_0134[edi]
loc_a_0A58::
                mov     dword ptr data_a_0140[edi],eax
                sti                             ; Enable interrupts
                jmp     loc_a_09BC
loc_a_0A64::
                pop     edi
                pop     esi
                pop     ebx
                retn
sub_a_0990      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0A70      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                push    edi
                mov     edi,dword ptr [esp+18h]
                push    ebp
                and     edi,0FFFFFFFDh
                mov     eax,dword ptr [esp+18h]
                cmp     word ptr [esi+6],0
                jne     loc_a_0B0E              ; Jump if not equal
                cmp     eax,3
                je      short loc_a_0AA8        ; Jump if equal
                cmp     eax,5
                je      loc_a_0DA0              ; Jump if equal
                mov     eax,0FFFFFFFFh
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
loc_a_0AA8::
                xor     ebp,ebp                 ; Zero register
                mov     ecx,esi
                call    sub_a_1320
                cmp     eax,ebp
                mov     ebx,eax
                jnz     short loc_a_0AD3        ; Jump if not zero
                mov     ebp,1
                mov     ecx,esi
                call    sub_a_1380
                test    byte ptr [esp+1Ch],2
                jnz     short loc_a_0AD3        ; Jump if not zero
                mov     ecx,esi
                call    sub_a_13A0
                mov     ebx,eax
loc_a_0AD3::
                mov     ecx,dword ptr [esp+20h]
                test    ebx,ebx
                mov     [ecx],ebx
                jz      short loc_a_0AFF        ; Jump if zero
                test    ebp,ebp
                jnz     short loc_a_0AEC        ; Jump if not zero
                lea     eax,dword ptr [esi+54h] ; Load effective addr
                push    dword ptr 0
                push    eax
                call    sub_a_1EC0
loc_a_0AEC::
                mov     ecx,esi
                call    sub_a_13B0
                mov     eax,0FFFFFFFFh
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
loc_a_0AFF::
                mov     eax,edi
                pop     ebp
                mov     word ptr [esi+6],1
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
loc_a_0B0E::
                sub     eax,3
                cmp     eax,14h
                ja      short loc_a_0B1D        ; Jump if above
                jmp     dword ptr data_a_0DC4[eax*4]    ;*16 entries
loc_a_0B1D::
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0B26::
                mov     eax,0FFFFFFFFh
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0B32::
                mov     ecx,esi
                call    sub_a_13B0
                xor     eax,eax                 ; Zero register
                pop     ebp
                mov     word ptr [esi+6],0
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0B48::
                mov     eax,dword ptr data_a_0194[esi]
                mov     ecx,dword ptr [esp+20h]
                inc     eax
                test    esi,esi
                mov     dword ptr data_a_0194[esi],eax
                mov     dword ptr data_a_0198[esi][eax*8],ecx
                mov     eax,dword ptr data_a_0194[esi]
                mov     dword ptr data_a_019C[esi][eax*8],esi
                jz      short loc_a_0B80        ; Jump if zero
                add     esi,54h
                xor     eax,eax                 ; Zero register
                pop     ebp
                mov     dword ptr [ecx+4],esi
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
loc_a_0B80::
                xor     esi,esi                 ; Zero register
                xor     eax,eax                 ; Zero register
                pop     ebp
                mov     dword ptr [ecx+4],esi
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0B8E::
                mov     eax,dword ptr data_a_0194[esi]
                test    eax,eax
                jl      short loc_a_0B9F        ; Jump if <
                dec     eax
                mov     dword ptr data_a_0194[esi],eax
loc_a_0B9F::
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0BA8::
                mov     eax,dword ptr [esi+58h]
                mov     ecx,dword ptr [esp+20h]
                mov     [ecx],eax
                pop     ebp
                mov     edx,dword ptr data_a_00AC[esi]
                pop     edi
                mov     dword ptr [ecx+4],edx
                mov     eax,dword ptr data_a_00BC[esi]
                pop     esi
                pop     ebx
                mov     dword ptr [ecx+8],eax
                mov     dword ptr [ecx+0Ch],0
                xor     eax,eax                 ; Zero register
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0BD3::
                xor     eax,eax                 ; Zero register
                mov     ecx,dword ptr [esp+20h]
                mov     al,[ecx]
                test    eax,eax
                jz      short loc_a_0BEB        ; Jump if zero
                mov     eax,18h
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
loc_a_0BEB::
                movsx   eax,byte ptr [ecx+1]    ; Mov w/sign extend
                push    eax
                call    sub_a_3300
                add     esp,4
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0C01::
                mov     eax,dword ptr [esp+24h]
                mov     edx,[eax]
                mov     edi,edx
                mov     ecx,edx
                and     edi,3Fh
                and     cl,80h
                mov     dword ptr hdsk_status_2_,edx
                cli                             ; Disable interrupts
                test    cl,cl
                jnz     short loc_a_0C30        ; Jump if not zero
                add     esi,54h
                push    dword ptr 0
                mov     ecx,esi
                call    sub_a_2300
                push    esi
                call    sub_a_1F30
                jmp     short loc_a_0C4E
loc_a_0C30::
                add     esi,54h
                cmp     dword ptr [esi],0
                je      short loc_a_0C48        ; Jump if equal
                mov     eax,[eax]
                mov     ecx,esi
                and     eax,40h
                shr     eax,6                   ; Shift w/zeros fill
                push    eax
                call    sub_a_2300
loc_a_0C48::
                push    esi
                call    sub_a_1F10
loc_a_0C4E::
                push    edi
                push    esi
                call    sub_a_2340
                sti                             ; Enable interrupts
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0C5F::
                mov     eax,dword ptr [esp+24h]
                mov     edx,dword ptr [esp+20h]
                mov     ecx,[eax]
                push    ecx
                push    edx
                lea     ecx,dword ptr [esi+54h] ; Load effective addr
                call    sub_a_2760
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0C7C::
                mov     eax,dword ptr data_a_0194[esi]
                mov     dword ptr data_a_0190[esi],1
                lea     ecx,dword ptr data_a_0198[esi][eax*8]   ; Load effective addr
                call    sub_a_0E20
                mov     dword ptr data_a_0190[esi],0
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0CA9::
                push    esi
                mov     eax,dword ptr data_a_0194[esi]
                lea     ecx,dword ptr data_a_0198[esi][eax*8]   ; Load effective addr
                call    sub_a_0F60
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0CC3::
                mov     eax,dword ptr data_a_0194[esi]
                lea     ecx,dword ptr data_a_0198[esi][eax*8]   ; Load effective addr
                call    sub_a_10B0
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0CDC::
                mov     eax,dword ptr data_a_0194[esi]
                lea     ecx,dword ptr data_a_0198[esi][eax*8]   ; Load effective addr
                call    sub_a_1100
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0CF5::
                mov     eax,dword ptr data_a_0194[esi]
                lea     ecx,dword ptr data_a_0198[esi][eax*8]   ; Load effective addr
                call    sub_a_1130
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0D0E::
                mov     dword ptr data_a_0110[esi],1
                mov     eax,dword ptr data_a_010C[esi]
                push    eax                     ; PARAMETER_1
                call    sub_a_3A16
                add     esp,4
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0D30::
                lea     ecx,dword ptr [esi+54h] ; Load effective addr
                call    sub_a_1D50
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
                                                ;* No entry point to code
                mov     eax,dword ptr [esp+24h]
                mov     ecx,[eax]
                push    ecx
                lea     ecx,dword ptr data_a_0114[esi]  ; Load effective addr
                call    sub_a_3260
                test    eax,eax
                jz      short loc_a_0D5E        ; Jump if zero
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
loc_a_0D5E::
                mov     eax,dword ptr data_a_0128[esi]
                mov     ecx,dword ptr [esp+20h]
                pop     ebp
                pop     edi
                pop     esi
                mov     [ecx],eax
                mov     eax,6
                pop     ebx
                retn    14h
                                                ;* No entry point to code
                mov     eax,dword ptr [esp+24h]
                mov     ecx,[eax]
                push    ecx
                lea     ecx,dword ptr [esi+54h] ; Load effective addr
                call    sub_a_1E70
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h
                                                ;* No entry point to code
                add     esi,54h
                push    esi
                call    sub_a_2720
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    14h

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0DA0::
                mov     ecx,dword ptr [esp+20h]
                mov     eax,[esi]
                mov     [ecx],eax
                mov     edx,dword ptr [esi+4]
                pop     ebp
                pop     edi
                mov     dword ptr [ecx+4],edx
                mov     ebx,dword ptr [esi+8]
                mov     dword ptr [ecx+8],ebx
                mov     eax,dword ptr [esi+0Ch]
                pop     esi
                pop     ebx
                mov     dword ptr [ecx+0Ch],eax
                xor     eax,eax                 ; Zero register
                retn    14h
sub_a_0A70      endp

                db      90h
data_a_0DC4     dd      offset32 loc_a_0B26
data_a_0DC8     dd      offset32 loc_a_0B32
data_a_0DCC     dd      offset32 loc_a_0DA0
data_a_0DD0     dd      offset32 loc_a_0B48
data_a_0DD4     dd      offset32 loc_a_0B8E
data_a_0DD8     dd      offset32 loc_a_0BA8
data_a_0DDC     dd      offset32 loc_a_0BD3
data_a_0DE0     dd      offset32 loc_a_0C01
data_a_0DE4     dd      offset32 loc_a_0C5F
data_a_0DE8     dd      offset32 loc_a_0C7C
data_a_0DEC     dd      offset32 loc_a_0CA9
data_a_0DF0     dd      offset32 loc_a_0CC3
data_a_0DF4     dd      offset32 loc_a_0CDC
data_a_0DF8     dd      offset32 loc_a_0CF5
data_a_0DFC     dd      offset32 loc_a_0D0E
data_a_0E00     dd      offset32 loc_a_0D30
                db      8 dup (0)
                db       3Fh, 0Dh, 00h, 00h, 76h, 0Dh
                db       00h, 00h, 8Eh, 0Dh, 00h, 00h
                db      0CCh,0CCh,0CCh,0CCh,0CCh,0CCh
                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0E20      proc    near
                push    ebx
                push    esi
                mov     eax,dword ptr [ecx+4]
                push    edi
                push    ebp
                mov     ebx,ecx
                mov     dword ptr data_a_018C[eax],0
                mov     ebp,[ebx]
                test    ebp,ebp
                jz      loc_a_0F40              ; Jump if zero
                mov     eax,dword ptr [ebp+30h]
                dec     eax
                cmp     eax,3
                ja      loc_a_0F40              ; Jump if above
                jmp     dword ptr data_a_0F48[eax*4]    ;*4 entries

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0E51::
                push    ebp
                call    sub_a_17D0
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0E5E::
                push    dword ptr 0
                push    ebp
                call    sub_a_1920
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0E6D::
                push    ebp
                call    sub_a_1870
                mov     edi,dword ptr [ebx+4]
                cmp     dword ptr [edi+58h],41Ch
                jge     loc_a_0F40              ; Jump if > or =
                add     edi,150h
                mov     esi,ebp
                mov     ecx,0Eh
                rep     movsd                   ; Rep when cx >0 Mov [si] to es:[di]
                mov     ecx,dword ptr [ebx+4]
                test    ecx,ecx
                lea     eax,dword ptr [ecx+54h] ; Load effective addr
                jnz     short loc_a_0E9E        ; Jump if not zero
                xor     eax,eax                 ; Zero register
loc_a_0E9E::
                push    dword ptr 3Ch
                mov     dword ptr data_a_0154[ecx],eax
                mov     eax,dword ptr [ebx+4]
                mov     dword ptr data_a_018C[eax],1
                mov     ecx,dword ptr [ebx+4]
                add     ecx,134h
                call    sub_a_1280
                mov     ecx,dword ptr [ebp+34h]
                mov     esi,dword ptr [ebx+4]
                mov     eax,dword ptr data_a_0134[esi]
                mov     dword ptr data_a_0140[esi],eax
                mov     dword ptr data_a_013C[esi],eax
                mov     dword ptr data_a_0148[esi],ecx
                mov     eax,dword ptr data_a_0144[esi]
                add     esi,offset32 data_a_0134
                test    ecx,ecx
                mov     ecx,eax
                lea     eax,[eax][eax*8]        ; Load effective addr
                jz      short loc_a_0F23        ; Jump if zero
                shl     eax,4                   ; Shift w/zeros fill
                sub     edx,edx
                sub     eax,ecx
                mov     ecx,1E78h
                shl     eax,2                   ; Shift w/zeros fill
                lea     eax,[eax][eax*8]        ; Load effective addr
                div     ecx                     ; ax,dx rem=dx:ax/reg
                lea     edx,[eax][eax*8]        ; Load effective addr
                lea     ebx,[edx][edx*8]        ; Load effective addr
                pop     ebp
                pop     edi
                lea     ecx,[eax][ebx*4]        ; Load effective addr
                xor     eax,eax                 ; Zero register
                lea     ecx,[ecx][ecx*2]        ; Load effective addr
                shl     ecx,3                   ; Shift w/zeros fill
                add     ecx,[esi]
                mov     dword ptr [esi+4],ecx
                pop     esi
                pop     ebx
                retn
loc_a_0F23::
                shl     eax,4                   ; Shift w/zeros fill
                pop     ebp
                sub     eax,ecx
                pop     edi
                shl     eax,2                   ; Shift w/zeros fill
                lea     eax,[eax][eax*8]        ; Load effective addr
                add     eax,[esi]
                mov     dword ptr [esi+4],eax
                xor     eax,eax                 ; Zero register
                pop     esi
                pop     ebx
                retn

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0F3A::
                push    ebp
                call    sub_a_18F0
loc_a_0F40::
                xor     eax,eax                 ; Zero register
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn
sub_a_0E20      endp

                db      90h
data_a_0F48     dd      offset32 loc_a_0E51
data_a_0F4C     dd      offset32 loc_a_0E5E
data_a_0F50     dd      offset32 loc_a_0E6D
data_a_0F54     dd      offset32 loc_a_0F3A
                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_0F60      proc    near
                sub     esp,0Ch
                mov     dword ptr [esp+4],ecx
                push    ebx
                push    esi
                cmp     dword ptr [ecx],0
                mov     dword ptr [esp+8],0
                mov     dword ptr [esp+10h],0
                push    edi
                push    ebp
                jz      loc_a_1084              ; Jump if zero
                mov     ebx,dword ptr [esp+20h]
loc_a_0F88::
                mov     eax,dword ptr [esp+14h]
                mov     ecx,[eax]
                mov     eax,dword ptr [ecx+30h]
                dec     eax
                cmp     eax,3
                ja      loc_a_1057              ; Jump if above
                jmp     dword ptr data_a_1098[eax*4]    ;*4 entries

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0FA2::
                push    ecx
                call    sub_a_1990
                jmp     loc_a_1053

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0FAD::
                push    ecx
                call    sub_a_1A30
                jmp     loc_a_1053

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_0FB8::
                cmp     dword ptr [ebx+58h],41Ch
                jge     loc_a_1045              ; Jump if > or =
                mov     dword ptr [esp+10h],0
                mov     esi,dword ptr data_a_013C[ebx]
                cmp     dword ptr data_a_0140[ebx],esi
                je      short loc_a_1057        ; Jump if equal
                mov     dword ptr [esp+10h],1
                mov     ebp,dword ptr [ecx+20h]
                lea     ecx,dword ptr [ebx+54h] ; Load effective addr
                add     ebp,ebp
                call    sub_a_1D40
                mov     edi,eax
                mov     ecx,ebp
                shr     ecx,2                   ; Shift w/zeros fill
                rep     movsd                   ; Rep when cx >0 Mov [si] to es:[di]
                mov     ecx,ebp
                and     ecx,3
                rep     movsb                   ; Rep when cx >0 Mov [si] to es:[di]
                cmp     dword ptr data_a_0148[ebx],0
                mov     eax,dword ptr data_a_013C[ebx]
                jz      short loc_a_102A        ; Jump if zero
                add     eax,1E78h
                cmp     dword ptr data_a_0138[ebx],eax
                ja      short loc_a_103D        ; Jump if above
                mov     eax,dword ptr data_a_0134[ebx]
                mov     dword ptr data_a_013C[ebx],eax
                jmp     short loc_a_1057
loc_a_102A::
                add     eax,141Ch
                cmp     dword ptr data_a_0138[ebx],eax
                ja      short loc_a_103D        ; Jump if above
                mov     eax,dword ptr data_a_0134[ebx]
loc_a_103D::
                mov     dword ptr data_a_013C[ebx],eax
                jmp     short loc_a_1057
loc_a_1045::
                push    ecx
                call    sub_a_19A0
                jmp     short loc_a_1053

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_104D::
                push    ecx
                call    sub_a_18A0
loc_a_1053::
                mov     dword ptr [esp+10h],eax
loc_a_1057::
                cmp     dword ptr [esp+10h],0
                jne     short loc_a_1084        ; Jump if not equal
                mov     dword ptr [esp+18h],8000h
                mov     dword ptr data_a_0110[ebx],1
                mov     eax,dword ptr data_a_010C[ebx]
                push    eax                     ; PARAMETER_1
                call    sub_a_3A16
                add     esp,4
                jmp     loc_a_0F88
loc_a_1084::
                mov     eax,dword ptr [esp+18h]
                or      eax,dword ptr [esp+10h]
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                add     esp,0Ch
                retn    4
sub_a_0F60      endp

                db       8Bh,0FFh
data_a_1098     dd      offset32 loc_a_0FA2
data_a_109C     dd      offset32 loc_a_0FAD
data_a_10A0     dd      offset32 loc_a_0FB8
data_a_10A4     dd      offset32 loc_a_104D
                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_10B0      proc    near
                xor     eax,eax                 ; Zero register
                mov     edx,[ecx]
                test    edx,edx
                jz      short loc_ret_a_10E3    ; Jump if zero
                mov     ecx,dword ptr [edx+30h]
                dec     ecx
                cmp     ecx,3
                ja      short loc_ret_a_10E3    ; Jump if above
                jmp     dword ptr data_a_10E4[ecx*4]    ;*4 entries

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_10C8::
                push    edx
                call    sub_a_1990
                retn

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_10CF::
                push    edx
                call    sub_a_1A30
                retn

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_10D6::
                push    edx
                call    sub_a_19A0
                retn

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_10DD::
                push    edx
                call    sub_a_18A0

loc_ret_a_10E3::
                retn
sub_a_10B0      endp

data_a_10E4     dd      offset32 loc_a_10C8
data_a_10E8     dd      offset32 loc_a_10CF
data_a_10EC     dd      offset32 loc_a_10D6
data_a_10F0     dd      offset32 loc_a_10DD
                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1100      proc    near
                mov     ecx,[ecx]
                test    ecx,ecx
                jz      short loc_a_1125        ; Jump if zero
                mov     eax,dword ptr [ecx+30h]
                cmp     eax,1
                je      short loc_a_1116        ; Jump if equal
                cmp     eax,2
                je      short loc_a_111F        ; Jump if equal
                xor     eax,eax                 ; Zero register
                retn
loc_a_1116::
                push    ecx
                call    sub_a_19E0
                xor     eax,eax                 ; Zero register
                retn
loc_a_111F::
                push    ecx
                call    sub_a_1AE0
loc_a_1125::
                xor     eax,eax                 ; Zero register
                retn
sub_a_1100      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1130      proc    near
                mov     ecx,[ecx]
                test    ecx,ecx
                jz      short loc_a_1155        ; Jump if zero
                mov     eax,dword ptr [ecx+30h]
                cmp     eax,1
                je      short loc_a_1146        ; Jump if equal
                cmp     eax,2
                je      short loc_a_114F        ; Jump if equal
                xor     eax,eax                 ; Zero register
                retn
loc_a_1146::
                push    ecx
                call    sub_a_1A00
                xor     eax,eax                 ; Zero register
                retn
loc_a_114F::
                push    ecx
                call    sub_a_1A00
loc_a_1155::
                xor     eax,eax                 ; Zero register
                retn
sub_a_1130      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1160      proc    near
                push    esi
                push    edi
                mov     esi,dword ptr [esp+0Ch]
                cmp     esi,2
                ja      short loc_a_1189        ; Jump if above
                jz      short loc_a_117C        ; Jump if zero
                cmp     esi,1
                jne     short loc_a_1189        ; Jump if not equal
                mov     eax,101h
                pop     edi
                pop     esi
                retn    14h
loc_a_117C::
                xor     eax,eax                 ; Zero register
                pop     edi
                mov     ax,word ptr hdsk0_cylinder_
                pop     esi
                retn    14h
loc_a_1189::
                cmp     esi,13h
                jb      short loc_a_11CB        ; Jump if below
                jnz     short loc_a_11AB        ; Jump if not zero
                mov     ecx,dword ptr [esp+18h]
                push    dword ptr 0             ; PARAMETER_3
                mov     edx,dword ptr [esp+18h]
                mov     eax,[ecx]
                push    eax                     ; PARAMETER_2
                push    edx                     ; PARAMETER_1
                call    sub_a_3911
                add     esp,0Ch
                pop     edi
                pop     esi
                retn    14h
loc_a_11AB::
                cmp     esi,14h
                jne     short loc_a_11CB        ; Jump if not equal
                mov     ecx,dword ptr [esp+18h]
                push    dword ptr 0             ; PARAMETER_3
                mov     edx,dword ptr [esp+18h]
                mov     eax,[ecx]
                push    eax                     ; PARAMETER_2
                push    edx                     ; PARAMETER_1
                call    sub_a_3935
                add     esp,0Ch
                pop     edi
                pop     esi
                retn    14h
loc_a_11CB::
                mov     edi,dword ptr [esp+10h]
                mov     eax,edi
                and     eax,0FFFFFFFDh
                push    eax
                call    sub_a_0950
                add     esp,4
                test    eax,eax
                jnz     short loc_a_11EB        ; Jump if not zero
                mov     eax,2
                pop     edi
                pop     esi
                retn    14h
loc_a_11EB::
                mov     ecx,dword ptr [esp+18h]
                mov     edx,dword ptr [esp+14h]
                push    ecx
                push    edx
                push    edi
                push    esi
                push    eax
                call    sub_a_0A70
                pop     edi
                pop     esi
                retn    14h
sub_a_1160      endp

                db      14 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1210      proc    near
                push    esi
                xor     esi,esi                 ; Zero register
                cmp     word ptr data_a_0000,si
                je      short loc_a_124D        ; Jump if equal
loc_a_121C::
                push    esi
                call    sub_a_0950
                add     esp,4
                mov     ecx,eax
                call    sub_a_13B0
                push    esi
                call    sub_a_0950
                add     esp,4
                lea     ecx,dword ptr data_a_0134[eax]  ; Load effective addr
                inc     esi
                call    sub_a_1260
                xor     eax,eax                 ; Zero register
                mov     ax,data_a_0000
                cmp     eax,esi
                ja      loc_a_121C              ; Jump if above
loc_a_124D::
                mov     eax,1
                pop     esi
                retn
sub_a_1210      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1260      proc    near
                push    esi
                mov     esi,ecx
                cmp     dword ptr [esi],0
                je      short loc_a_127A        ; Jump if equal
                mov     eax,dword ptr [esi+18h]
                push    eax                     ; PARAMETER_1
                call    sub_a_39D2
                add     esp,4
                mov     dword ptr [esi],0
loc_a_127A::
                pop     esi
                retn
sub_a_1260      endp

                db       90h, 90h, 90h, 90h

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1280      proc    near
                push    edi
                mov     edi,ecx
                cmp     dword ptr [edi],0
                jne     short loc_a_12F4        ; Jump if not equal
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                push    ebx                     ; PARAMETER_3
                lea     ebx,dword ptr [edi+18h] ; Load effective addr
                lea     eax,[esi][esi*4]        ; Load effective addr
                shl     eax,5                   ; Shift w/zeros fill
                add     eax,esi
                lea     ecx,[esi][eax*8]        ; Load effective addr
                shl     ecx,2                   ; Shift w/zeros fill
                push    ecx                     ; PARAMETER_2
                push    ebx                     ; PARAMETER_1
                call    sub_a_39A1
                add     esp,8
                mov     [edi],eax
                test    eax,eax
                jnz     short loc_a_12CF        ; Jump if not zero
loc_a_12AF::
                shr     esi,1                   ; Shift w/zeros fill
                lea     edx,[esi][esi*4]        ; Load effective addr
                shl     edx,5                   ; Shift w/zeros fill
                add     edx,esi
                lea     eax,[esi][edx*8]        ; Load effective addr
                shl     eax,2                   ; Shift w/zeros fill
                push    eax
                push    ebx
                call    sub_a_39A1
                add     esp,8
                mov     [edi],eax
                test    eax,eax
                jz      loc_a_12AF              ; Jump if zero
loc_a_12CF::
                mov     ecx,[edi]
                lea     eax,[esi][esi*8]        ; Load effective addr
                shl     eax,4                   ; Shift w/zeros fill
                sub     eax,esi
                mov     dword ptr [edi+10h],esi
                pop     ebx
                mov     dword ptr [edi+0Ch],ecx
                lea     edx,[eax][eax*8]        ; Load effective addr
                mov     dword ptr [edi+8],ecx
                mov     dword ptr [edi+14h],0
                pop     esi
                lea     eax,[ecx][edx*4]        ; Load effective addr
                mov     dword ptr [edi+4],eax
loc_a_12F4::
                pop     edi
                retn    4
sub_a_1280      endp

                db      8 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1300      proc    near
                mov     ecx,dword ptr [esp+4]
                xor     eax,eax                 ; Zero register
                cmp     byte ptr [ecx],0
                je      short loc_ret_a_1314    ; Jump if equal
loc_a_130B::
                mov     dl,byte ptr [eax+1][ecx]
                inc     eax
                test    dl,dl
                jnz     loc_a_130B              ; Jump if not zero

loc_ret_a_1314::
                retn
sub_a_1300      endp

                db      11 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1320      proc    near
                push    esi
                mov     esi,ecx
                xor     eax,eax                 ; Zero register
                push    edi
                mov     ax,[esi]
                lea     ecx,dword ptr [esi+54h] ; Load effective addr
                push    eax
                push    ecx
                call    sub_a_1EC0
                xor     edx,edx                 ; Zero register
                lea     edi,dword ptr [esi+10h] ; Load effective addr
                mov     dx,[esi]
                push    dword ptr 8
                push    edx
                mov     ecx,edi
                call    sub_a_2860
                mov     ecx,edi
                call    sub_a_2890
                test    eax,eax
                jz      short loc_a_1358        ; Jump if zero
                mov     eax,16h
                pop     edi
                pop     esi
                retn
loc_a_1358::
                xor     eax,eax                 ; Zero register
                lea     ecx,dword ptr data_a_00C4[esi]  ; Load effective addr
                mov     ax,word ptr [esi+4]
                push    eax
                call    sub_a_3740
                neg     eax
                sbb     eax,eax
                pop     edi
                and     al,0E9h
                pop     esi
                add     eax,17h
                retn
sub_a_1320      endp

                db      10 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1380      proc    near
                push    esi
                mov     esi,ecx
                lea     ecx,dword ptr [esi+54h] ; Load effective addr
                call    sub_a_1FA0
;*              add     esi,0C4h
                db       81h,0C6h,0C4h, 00h, 00h, 00h   ;  Fixup - byte match
                push    esi
                call    sub_a_3640
                pop     esi
                retn
sub_a_1380      endp

                db      7 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_13A0      proc    near
                add     ecx,114h
                jmp     loc_a_2B90
sub_a_13A0      endp

                db       90h, 90h, 90h, 90h, 90h

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_13B0      proc    near
                push    esi
                mov     esi,ecx
                lea     eax,dword ptr [esi+54h] ; Load effective addr
                push    eax
                call    sub_a_20E0
                lea     ecx,dword ptr data_a_00C4[esi]  ; Load effective addr
                call    sub_a_3680
                lea     ecx,dword ptr [esi+10h] ; Load effective addr
                call    sub_a_28E0
                pop     esi
                retn
sub_a_13B0      endp

                db      15 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_13E0      proc    near
                push    ebx
                push    esi
                push    edi
                mov     edi,dword ptr [esp+10h]
                push    edi
                call    sub_a_1300
                mov     esi,eax
                add     esp,4
                xor     edx,edx                 ; Zero register
                test    esi,esi
                jbe     short loc_a_141C        ; Jump if below or =
                mov     ecx,dword ptr [esp+14h]
                sub     edi,ecx
loc_a_13FE::
                mov     al,[edi][ecx]
                mov     bl,[ecx]
;*              cmp     al,bl
                db       3Ah,0C3h               ;  Fixup - byte match
                jnz     short loc_a_1416        ; Jump if not zero
                inc     edx
                inc     ecx
                cmp     edx,esi
                jb      loc_a_13FE              ; Jump if below
                mov     eax,1
                pop     edi
                pop     esi
                pop     ebx
                retn
loc_a_1416::
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                pop     ebx
                retn
loc_a_141C::
                pop     edi
                pop     esi
                mov     eax,1
                pop     ebx
                retn
sub_a_13E0      endp

                db      11 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1430      proc    near
                push    ecx
                push    esi
                mov     esi,dword ptr [esp+10h]
                push    edi
                mov     edi,dword ptr [esp+10h]
                lea     eax,dword ptr [esp+8]   ; Load effective addr
                push    dword ptr 0
                push    eax
                push    esi
                push    dword ptr 1
                push    574h
                push    dword ptr 0
                push    edi
                mov     dword ptr [esp+24h],190h
                call    sub_a_145A

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_145A::
                add     esp,1Ch
                test    eax,eax
                jz      short loc_a_1485        ; Jump if zero
                lea     ecx,dword ptr [esp+8]   ; Load effective addr
                push    dword ptr 0
                push    ecx
                push    esi
                push    dword ptr 1
                push    568h
                push    dword ptr 0
                push    edi
                call    sub_a_1478

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_1478::
                add     esp,1Ch
                test    eax,eax
                jz      short loc_a_1485        ; Jump if zero
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                pop     ecx
                retn
loc_a_1485::
                push    esi
                push    500h
                call    sub_a_13E0
                add     esp,8
                test    eax,eax
                jnz     short loc_a_14DB        ; Jump if not zero
                push    esi
                push    510h
                call    sub_a_13E0
                add     esp,8
                test    eax,eax
                jnz     short loc_a_14DB        ; Jump if not zero
                push    esi
                push    518h
                call    sub_a_13E0
                add     esp,8
                test    eax,eax
                jnz     short loc_a_14DB        ; Jump if not zero
                push    esi
                push    528h
                call    sub_a_13E0
                add     esp,8
                test    eax,eax
                jnz     short loc_a_14DB        ; Jump if not zero
                push    esi
                push    530h
                call    sub_a_13E0
                add     esp,8
loc_a_14DB::
                neg     eax
                sbb     eax,eax
                pop     edi
                and     eax,esi
                pop     esi
                pop     ecx
                retn
sub_a_1430      endp

                db      11 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_14F0      proc    near
                push    edi
                mov     edi,dword ptr [esp+8]
                push    dword ptr 0
                lea     eax,dword ptr [esp+0Ch] ; Load effective addr
                push    edi
                push    eax
                call    sub_b_057C
                add     esp,0Ch
                test    eax,eax
                jz      short loc_a_150D        ; Jump if zero
                xor     eax,eax                 ; Zero register
                pop     edi
                retn
loc_a_150D::
                mov     ecx,dword ptr [esp+8]
                push    esi
                mov     esi,dword ptr [esp+10h]
                push    dword ptr 0
                push    ecx
                push    esi
                push    edi
                call    sub_b_0574
                add     esp,10h
                neg     eax
                sbb     eax,eax
                not     eax
                and     eax,esi
                pop     esi
                pop     edi
                retn
sub_a_14F0      endp

                db       90h, 90h

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1530      proc    near
                sub     esp,1B8h
                push    ebx
                push    ebp
                push    esi
                xor     ebx,ebx                 ; Zero register
                push    edi
                mov     dword ptr [esp+1Ch],ebx
                xor     edi,edi                 ; Zero register
                lea     esi,dword ptr [esp+30h] ; Load effective addr
loc_a_1546::
                mov     dl,bl
;*              push    0
                db       68h, 00h, 00h, 00h, 00h        ;  Fixup - byte match
                add     dl,30h                  ; '0'
                mov     byte ptr [esp+17h],dl
                call    sub_a_1300
                mov     cl,byte ptr [esp+17h]
                add     esp,4
                lea     edx,dword ptr [esp+14h] ; Load effective addr
                mov     byte ptr ds:d_F000_FFFF_e[eax],cl
                push    dword ptr 0
;*              push    0
                db       68h, 00h, 00h, 00h, 00h        ;  Fixup - byte match
                push    edx
                call    sub_a_1577

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_1577::
                add     esp,0Ch
                test    eax,eax
                jnz     short loc_a_1589        ; Jump if not zero
                mov     [esi],ebx
                inc     edi
                add     esi,4
                cmp     edi,2
                jae     short loc_a_158F        ; Jump if above or =
loc_a_1589::
                inc     ebx
                cmp     ebx,0Ah
                jb      loc_a_1546              ; Jump if below
loc_a_158F::
                test    edi,edi
                jnz     short loc_a_15A0        ; Jump if not zero
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                pop     ebp
                pop     ebx
                add     esp,1B8h
                retn
loc_a_15A0::
                jbe     loc_a_16DD              ; Jump if below or =
                lea     eax,dword ptr [esp+30h] ; Load effective addr
                mov     ebp,4B8h
                mov     dword ptr [esp+18h],eax
                mov     esi,offset32 data_a_000C
                mov     dword ptr [esp+24h],edi
loc_a_15BC::
                mov     ecx,dword ptr [esp+18h]
;*              push    0
                db       68h, 00h, 00h, 00h, 00h        ;  Fixup - byte match
                mov     bl,[ecx]
                add     bl,30h                  ; '0'
                call    sub_a_1300
                add     esp,4
                lea     edx,dword ptr [esp+14h] ; Load effective addr
                mov     byte ptr ds:d_F000_FFFF_e[eax],bl
                push    dword ptr 0
;*              push    0
                db       68h, 00h, 00h, 00h, 00h        ;  Fixup - byte match
                push    edx
                call    sub_a_15E9

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_15E9::
                add     esp,0Ch
                test    eax,eax
                jnz     loc_a_16C3              ; Jump if not zero
                mov     ecx,dword ptr [esp+14h]
                lea     eax,dword ptr [esp+38h] ; Load effective addr
                push    eax
                push    ecx
                call    sub_a_1430
                add     esp,8
                test    eax,eax
                jz      loc_a_16C3              ; Jump if zero
                mov     edx,dword ptr [esp+14h]
                push    dword ptr 2
                lea     eax,dword ptr [esp+24h] ; Load effective addr
                push    edx
                push    eax
                call    sub_b_058C
                add     esp,0Ch
                test    eax,eax
                jnz     loc_a_16C3              ; Jump if not zero
                mov     ecx,dword ptr [esp+20h]
                push    eax
                push    eax
                push    dword ptr 2
                lea     edx,dword ptr [esp+38h] ; Load effective addr
                push    ecx
                push    edx
                call    sub_a_163D

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_163D::
                add     esp,14h
                test    eax,eax
                jnz     short loc_a_16C3        ; Jump if not zero
                mov     ecx,dword ptr [esp+2Ch]
                lea     eax,dword ptr [esp+38h] ; Load effective addr
                push    eax
                push    ecx
                call    sub_a_14F0
                add     esp,8
                test    eax,eax
                jz      short loc_a_16C3        ; Jump if zero
                mov     edx,dword ptr [esp+20h]
                xor     edi,edi                 ; Zero register
                mov     di,word ptr [eax+4]
                push    dword ptr 0
                push    dword ptr 0
                push    dword ptr 4
                lea     eax,dword ptr [esp+34h] ; Load effective addr
                push    edx
                push    eax
                call    sub_a_1675

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_1675::
                add     esp,14h
                test    eax,eax
                jnz     short loc_a_16C3        ; Jump if not zero
                mov     edx,dword ptr [esp+28h]
                lea     ecx,dword ptr [esp+38h] ; Load effective addr
                push    ecx
                push    edx
                call    sub_a_14F0
                add     esp,8
                test    eax,eax
                jz      short loc_a_16C3        ; Jump if zero
                xor     ecx,ecx                 ; Zero register
                mov     cx,word ptr [eax+2]
                mov     eax,dword ptr [ebp+0]
                push    eax
                mov     word ptr [esi-4],di
                mov     word ptr [esi-2],0
                mov     [esi],cx
                call    sub_b_00E0
                mov     edx,dword ptr [esp+20h]
                add     esp,4
                inc     edx
                add     esi,238h
                mov     dword ptr [esp+1Ch],edx
                add     ebp,4
loc_a_16C3::
                mov     ecx,dword ptr [esp+18h]
                mov     eax,dword ptr [esp+24h]
                add     ecx,4
                dec     eax
                mov     dword ptr [esp+18h],ecx
                mov     dword ptr [esp+24h],eax
                jnz     loc_a_15BC              ; Jump if not zero
loc_a_16DD::
                mov     ecx,dword ptr [esp+1Ch]
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                test    ecx,ecx
                pop     ebp
                pop     ebx
                setne   al                      ; Set byte if not =
                add     esp,1B8h
                retn
sub_a_1530      endp

                db      13 dup (90h)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1700      proc    near
                xor     eax,eax                 ; Zero register
                mov     dword ptr [ecx+34h],eax
                mov     dword ptr [ecx+20h],eax
                mov     eax,ecx
                retn    4
sub_a_1700      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1710      proc    near
                mov     ecx,dword ptr [esp+4]
                xor     eax,eax                 ; Zero register
                mov     dword ptr [ecx+10h],eax
                mov     dword ptr [ecx+14h],eax
                call    sub_a_1730
                xor     eax,eax                 ; Zero register
                retn    4
sub_a_1710      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1730      proc    near
                push    esi
                mov     esi,ecx
                mov     dword ptr [ecx+24h],1000h
                mov     ecx,dword ptr [esi+4]
                call    sub_a_1D40
                mov     dword ptr [esi+1Ch],eax
                pop     esi
                retn
sub_a_1730      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1750      proc    near
                mov     eax,dword ptr [esp+4]
                push    esi
                mov     esi,ecx
                push    eax
                call    sub_a_1700
                mov     dword ptr [esi+30h],3
                cmp     dword ptr [esp+0Ch],0
                jne     short loc_a_1771        ; Jump if not equal
                push    esi
                call    sub_a_1790
loc_a_1771::
                mov     eax,esi
                pop     esi
                retn    8
sub_a_1750      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1780      proc    near
                mov     ecx,dword ptr [esp+4]
                call    sub_a_1730
                retn    4
sub_a_1780      endp

                db      0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1790      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                push    esi
                call    sub_a_1710
                push    esi
                call    sub_a_1780
                xor     eax,eax                 ; Zero register
                pop     esi
                retn    4
sub_a_1790      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_17B0      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                push    esi
                call    sub_a_1710
                mov     dword ptr [esi+20h],0EA0h
                xor     eax,eax                 ; Zero register
                pop     esi
                retn    4
sub_a_17B0      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_17D0      proc    near
                push    esi
                push    edi
                mov     esi,dword ptr [esp+0Ch]
                mov     eax,dword ptr [esi+34h]
                mov     ecx,dword ptr [esi+4]
                mov     dword ptr [esi+10h],1
                push    eax
                push    ecx
                call    sub_a_22A0
                push    esi
                call    sub_a_1CE0
                mov     ecx,dword ptr [esi+4]
                push    ecx
                call    sub_a_2100
                xor     eax,eax                 ; Zero register
                mov     ecx,dword ptr [esi+4]
                mov     dword ptr [esi+38h],eax
                mov     dword ptr [esi+14h],eax
                call    sub_a_1D40
                mov     edi,eax
                mov     edx,eax
                mov     eax,0AAAAAAAAh
                mov     ecx,6Dh
                rep     stosd                   ; Rep when cx >0 Store ax to es:[di]
                add     edx,1B4h
                mov     eax,100h
                mov     ecx,edx
loc_a_1826::
                mov     word ptr [ecx],0AAAAh
                add     ecx,12h
                dec     eax
                jnz     loc_a_1826              ; Jump if not zero
                add     edx,1200h
                mov     eax,0AAAAAAAAh
                mov     edi,edx
                mov     ecx,24h
                rep     stosd                   ; Rep when cx >0 Store ax to es:[di]
                xor     edi,edi                 ; Zero register
loc_a_1847::
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_2170
                test    eax,eax
                jz      short loc_a_185E        ; Jump if zero
                mov     eax,edi
                inc     edi
                cmp     eax,7530h
                jle     loc_a_1847              ; Jump if < or =
loc_a_185E::
                push    esi
                call    sub_a_1CE0
                pop     edi
                pop     esi
                retn    4
sub_a_17D0      endp

                db      7 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1870      proc    near
                mov     eax,dword ptr [esp+4]
                push    eax
                call    sub_a_1880
                retn    4
sub_a_1870      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1880      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                mov     eax,dword ptr [esi+34h]
                mov     ecx,dword ptr [esi+4]
                push    eax
                push    ecx
                call    sub_a_2280
                push    esi
                call    sub_a_1C60
                pop     esi
                retn    4
sub_a_1880      endp

                db      0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_18A0      proc    near
                push    esi
                push    edi
                mov     esi,dword ptr [esp+0Ch]
                push    esi
                call    sub_a_1CC0
                mov     edi,eax
                test    edi,edi
                jnz     short loc_a_18B9        ; Jump if not zero
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                retn    4
loc_a_18B9::
                sti                             ; Enable interrupts
                mov     eax,dword ptr [esi+10h]
                push    eax
                push    esi
                call    sub_a_1C00
                mov     ecx,dword ptr [esi+4]
                call    sub_a_1D40
                xor     edx,edx                 ; Zero register
                mov     ecx,dword ptr [esi+4]
                mov     dx,[eax]
                cmp     dword ptr [ecx],0
                je      short loc_a_18DF        ; Jump if equal
                mov     ecx,dword ptr [esi+3Ch]
                mov     [eax],cx
loc_a_18DF::
                mov     ecx,esi
                mov     dword ptr [esi+3Ch],edx
                call    sub_a_1D20
                mov     eax,edi
                pop     edi
                pop     esi
                retn    4
sub_a_18A0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_18F0      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                push    esi
                mov     dword ptr [esi+3Ch],0
                call    sub_a_17B0
                push    dword ptr 0
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_22E0
                push    esi
                call    sub_a_1C60
                mov     dword ptr [esi+38h],1
                pop     esi
                retn    4
sub_a_18F0      endp

                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1920      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                mov     ecx,dword ptr [esi+4]
                call    sub_a_1D40
                mov     ebx,eax
                push    esi
                mov     dword ptr data_a_0080[esi],0
                mov     dword ptr [esi+74h],0
                call    sub_a_1BF0
                push    esi
                call    sub_a_1CE0
                push    dword ptr 0
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_22A0
                push    dword ptr 0
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_1D80
                push    1000h
                mov     eax,dword ptr [esi+4]
                push    ebx
                push    eax
                call    sub_a_26A0
                push    dword ptr 0
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_22C0
                push    esi
                call    sub_a_1CE0
                pop     esi
                pop     ebx
                retn    8
sub_a_1920      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1990      proc    near
                mov     eax,dword ptr [esp+4]
                push    eax
                call    sub_a_1D00
                retn    4
sub_a_1990      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_19A0      proc    near
                push    esi
                push    edi
                mov     esi,dword ptr [esp+0Ch]
                push    esi
                call    sub_a_1CC0
                mov     edi,eax
                test    edi,edi
                jnz     short loc_a_19B9        ; Jump if not zero
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                retn    4
loc_a_19B9::
                sti                             ; Enable interrupts
                mov     eax,dword ptr [esi+10h]
                push    eax
                push    esi
                call    sub_a_1C00
                mov     ecx,esi
                call    sub_a_1D20
                mov     eax,edi
                pop     edi
                pop     esi
                retn    4
sub_a_19A0      endp

                db      14 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_19E0      proc    near
                push    esi
                sti                             ; Enable interrupts
                mov     esi,dword ptr [esp+8]
                mov     eax,dword ptr [esi+10h]
                push    eax
                push    esi
                call    sub_a_1C30
                mov     ecx,esi
                call    sub_a_1D20
                pop     esi
                retn    4
sub_a_19E0      endp

                db      0CCh,0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1A00      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                mov     eax,dword ptr [esi+10h]
                mov     ecx,dword ptr [esi+4]
                shl     eax,4                   ; Shift w/zeros fill
                push    eax
                push    ecx
                call    sub_a_1D80
                sti                             ; Enable interrupts
                mov     ecx,esi
                call    sub_a_1D20
                pop     esi
                retn    4
sub_a_1A00      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1A30      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                push    edi
                mov     ebx,1
                mov     ecx,dword ptr [esi+4]
                call    sub_a_1D40
                mov     edi,eax
                mov     eax,12h
                cmp     eax,dword ptr data_a_0080[esi]
                adc     ebx,0FFFFFFFFh
loc_a_1A54::
                push    esi
                call    sub_a_1990
                test    eax,eax
                jz      short loc_a_1AA2        ; Jump if zero
                cmp     dword ptr [esi+70h],0
                je      short loc_a_1ACB        ; Jump if equal
                mov     eax,dword ptr [esi+38h]
                cmp     eax,0EA0h
                jb      short loc_a_1AAA        ; Jump if below
                test    ebx,ebx
                jnz     short loc_a_1A89        ; Jump if not zero
                push    0EA0h
                mov     ecx,dword ptr [esi+20h]
                sub     ecx,eax
                mov     edx,dword ptr [esi+4]
                lea     eax,[edi][ecx*2]        ; Load effective addr
                push    eax
                push    edx
                call    sub_a_26A0
loc_a_1A89::
                sub     dword ptr [esi+38h],0EA0h
                jnz     short loc_a_1A99        ; Jump if not zero
                mov     dword ptr [esi+70h],0
loc_a_1A99::
                mov     ecx,esi
                call    sub_a_1B70
                jmp     short loc_a_1A54
loc_a_1AA2::
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_1AAA::
                test    ebx,ebx
                jnz     short loc_a_1AC4        ; Jump if not zero
                mov     eax,dword ptr [esi+38h]
                mov     ecx,dword ptr [esi+20h]
                push    eax
                sub     ecx,eax
                mov     edx,dword ptr [esi+4]
                lea     eax,[edi][ecx*2]        ; Load effective addr
                push    eax
                push    edx
                call    sub_a_26A0
loc_a_1AC4::
                mov     dword ptr [esi+70h],0
loc_a_1ACB::
                mov     eax,1
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_1A30      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1AE0      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                push    edi
                mov     ecx,dword ptr [esi+4]
                call    sub_a_1D40
                mov     edx,dword ptr [esi+38h]
                mov     ebx,dword ptr [esi+20h]
                mov     ecx,12h
                lea     edi,[ebx][edx]          ; Load effective addr
                cmp     ecx,dword ptr data_a_0080[esi]
                mov     ecx,1
                adc     ecx,0FFFFFFFFh
                cmp     edi,0EA0h
                jb      short loc_a_1B4C        ; Jump if below
                test    ecx,ecx
                jnz     short loc_a_1B29        ; Jump if not zero
                mov     ecx,0EA0h
                sub     ecx,edx
                push    ecx
                push    eax
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_26A0
loc_a_1B29::
                sub     edi,0EA0h
                mov     ecx,esi
                mov     dword ptr [esi+38h],edi
                call    sub_a_1B70
                cmp     dword ptr [esi+38h],0
                je      short loc_a_1B5E        ; Jump if equal
                mov     dword ptr [esi+70h],1
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_1B4C::
                test    ecx,ecx
                jnz     short loc_a_1B5B        ; Jump if not zero
                push    ebx
                push    eax
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_26A0
loc_a_1B5B::
                mov     dword ptr [esi+38h],edi
loc_a_1B5E::
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_1AE0      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1B70      proc    near
                jmpn    loc_a_1B80
                db      11 dup (0CCh)

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_1B80::
loc_a_1B80::
                push    esi
                mov     eax,dword ptr [ecx+4]
                mov     esi,ecx
                push    eax
                call    sub_a_2100
                mov     eax,dword ptr [esi+10h]
                mov     ecx,dword ptr [esi+4]
                inc     eax
                cmp     dword ptr [ecx+60h],eax
                mov     dword ptr [esi+10h],0
                jbe     short loc_a_1BA2        ; Jump if below or =
                mov     dword ptr [esi+10h],eax
loc_a_1BA2::
                mov     eax,dword ptr [esi+10h]
                shl     eax,4                   ; Shift w/zeros fill
                push    eax
                push    ecx
                call    sub_a_1D80
                xor     eax,eax                 ; Zero register
                pop     esi
                retn
sub_a_1B70      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1BC0      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_2120
                mov     eax,dword ptr [esi+10h]
                mov     ecx,dword ptr [esi+4]
                shl     eax,4                   ; Shift w/zeros fill
                push    eax
                push    ecx
                call    sub_a_1D80
                pop     esi
                retn    4
sub_a_1BC0      endp

                db      14 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1BF0      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+4]
                push    ecx
                call    sub_a_21A0
                retn    4
sub_a_1BF0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1C00      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                mov     ecx,dword ptr [esi+4]
                call    sub_a_1D40
                mov     ecx,dword ptr [esi+20h]
                test    ecx,ecx
                jz      short loc_a_1C27        ; Jump if zero
                test    eax,eax
                jz      short loc_a_1C27        ; Jump if zero
                push    ecx
                push    eax
                mov     eax,dword ptr [esp+14h]
                mov     ecx,dword ptr [esi+4]
                push    eax
                call    sub_a_25E0
loc_a_1C27::
                pop     esi
                retn    8
sub_a_1C00      endp

                db      0CCh,0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1C30      proc    near
                mov     eax,dword ptr [esp+4]
                push    esi
                mov     esi,dword ptr [eax+4]
                mov     ecx,dword ptr [eax+20h]
                push    ecx
                mov     ecx,esi
                call    sub_a_1D40
                push    eax
                mov     ecx,esi
                mov     eax,dword ptr [esp+14h]
                push    eax
                call    sub_a_2640
                pop     esi
                retn    8
sub_a_1C30      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1C60      proc    near
                push    ebx
                push    esi
                push    edi
                push    ebp
                sti                             ; Enable interrupts
                mov     edi,dword ptr [esp+14h]
                xor     ebx,ebx                 ; Zero register
                push    edi
                call    sub_a_1BF0
                mov     eax,dword ptr [edi+4]
                mov     dword ptr [edi+10h],ebx
                mov     esi,dword ptr [eax+60h]
                test    esi,esi
                jle     short loc_a_1CAB        ; Jump if < or =
loc_a_1C7E::
                cmp     ebx,2
                jge     short loc_a_1C8C        ; Jump if > or =
                push    ebx
                push    edi
                call    sub_a_1C30
                jmp     short loc_a_1CA6
loc_a_1C8C::
                mov     ebp,dword ptr [edi+4]
                mov     eax,dword ptr [edi+20h]
                shr     eax,2                   ; Shift w/zeros fill
                mov     ecx,ebp
                push    eax
                call    sub_a_1D40
                push    eax
                mov     ecx,ebp
                push    ebx
                call    sub_a_2640
loc_a_1CA6::
                inc     ebx
                cmp     ebx,esi
                jl      loc_a_1C7E              ; Jump if <
loc_a_1CAB::
                push    edi
                call    sub_a_1BC0
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_1C60      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1CC0      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+4]
                call    sub_a_1D50
                cmp     eax,1
                jg      short loc_ret_a_1CD3    ; Jump if >
                xor     eax,eax                 ; Zero register

loc_ret_a_1CD3::
                retn    4
sub_a_1CC0      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1CE0      proc    near
                mov     eax,dword ptr [esp+4]
                push    eax
                mov     dword ptr [eax+10h],1
                call    sub_a_1BC0
                retn    4
sub_a_1CE0      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D00      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                mov     eax,dword ptr [esi+4]
                push    eax
                call    sub_a_2700
                mov     dword ptr [esi+28h],eax
                pop     esi
                retn    4
sub_a_1D00      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D20      proc    near
                call    sub_a_1B80
                xor     eax,eax                 ; Zero register
                retn
sub_a_1D20      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D30      proc    near
                mov     edx,dword ptr [esp+4]
                xor     eax,eax                 ; Zero register
                in      ax,dx                   ; port 0, DMA-1 bas&add ch 0
                retn    4
sub_a_1D30      endp

                db      0CCh,0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D40      proc    near
                mov     eax,dword ptr [ecx+34h]
                retn
sub_a_1D40      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D50      proc    near
                push    ecx
                call    sub_a_2170
                retn
sub_a_1D50      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D60      proc    near
                mov     ecx,dword ptr [esp+4]
                call    sub_a_1D70
                and     eax,10h
                retn    4
sub_a_1D60      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D70      proc    near
                mov     eax,dword ptr [ecx+1Ch]
                push    eax
                call    sub_a_1D30
                retn
sub_a_1D70      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1D80      proc    near
                mov     eax,dword ptr [esp+8]
                mov     ecx,dword ptr [esp+4]
                mov     edx,dword ptr [ecx+28h]
                mov     dword ptr [ecx+2Ch],eax
                mov     dword ptr [ecx+54h],100h
                out     dx,ax                   ; port 0, DMA-1 bas&add ch 0
                retn    8
sub_a_1D80      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1DA0      proc    near
                mov     eax,dword ptr [esp+4]
                mov     edx,dword ptr [eax+18h]
                mov     eax,dword ptr [esp+8]
                out     dx,ax                   ; port 0, DMA-1 bas&add ch 0
                retn    8
sub_a_1DA0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1DB0      proc    near
                mov     edx,dword ptr [ecx+20h]
                mov     eax,dword ptr [esp+4]
                out     dx,ax                   ; port 0, DMA-1 bas&add ch 0
                retn    4
sub_a_1DB0      endp

                db      0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1DC0      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+24h]
                push    ecx
                call    sub_a_1D30
                retn    4
sub_a_1DC0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1DD0      proc    near
                mov     eax,dword ptr [esp+4]
                mov     edx,dword ptr [eax+24h]
                mov     eax,dword ptr [esp+8]
                out     dx,ax                   ; port 0, DMA-1 bas&add ch 0
                retn    8
sub_a_1DD0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1DE0      proc    near
                retn    4
sub_a_1DE0      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1DF0      proc    near
                push    ebx
                push    esi
                push    edi
                xor     ebx,ebx                 ; Zero register
                push    ecx
                mov     edi,ecx
                call    sub_a_2260
                mov     dword ptr [edi+68h],0
loc_a_1E04::
                push    ebx
                lea     esi,dword ptr [ebx+1]   ; Load effective addr
                push    edi
                call    sub_a_1D80
                push    esi
                push    edi
                call    sub_a_1DD0
                push    esi
                push    edi
                call    sub_a_1DD0
                push    edi
                call    sub_a_1DC0
                push    edi
                call    sub_a_1DC0
                sub     eax,ebx
                cmp     eax,1
                jne     short loc_a_1E58        ; Jump if not equal
                push    dword ptr 0
                push    edi
                call    sub_a_1D80
                push    edi
                call    sub_a_1DC0
                push    edi
                call    sub_a_1DC0
                cmp     eax,1
                jne     short loc_a_1E5E        ; Jump if not equal
                mov     ebx,esi
                cmp     esi,400h
                jb      loc_a_1E04              ; Jump if below
                mov     eax,ebx
                pop     edi
                pop     esi
                pop     ebx
                retn
loc_a_1E58::
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                pop     ebx
                retn
loc_a_1E5E::
                mov     eax,ebx
                mov     dword ptr [edi+68h],ebx
                pop     edi
                pop     esi
                pop     ebx
                retn
sub_a_1DF0      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1E70      proc    near
                push    ebx
                push    esi
                push    edi
                mov     esi,ecx
                push    ebp
                mov     edi,20h
                mov     ebx,6
                mov     ebp,dword ptr [esp+14h]
loc_a_1E84::
                push    esi
                call    sub_a_2720
                push    dword ptr 0
                test    edi,ebp
                push    esi
                jz      short loc_a_1E98        ; Jump if zero
                call    sub_a_22A0
                jmp     short loc_a_1E9D
loc_a_1E98::
                call    sub_a_22E0
loc_a_1E9D::
;*              shr     edi,1                   ; Shift w/zeros fill
                db      0C1h,0EFh, 01h          ;  Fixup - byte match
                push    esi
                call    sub_a_2740
                dec     ebx
                jnz     loc_a_1E84              ; Jump if not zero
                push    dword ptr 0
                push    esi
                call    sub_a_22E0
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_1E70      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1EC0      proc    near
                mov     edx,dword ptr [esp+4]
                mov     eax,8
                mov     dword ptr [edx],0
                mov     dword ptr [edx+60h],eax
                mov     ecx,dword ptr [esp+8]
                mov     dword ptr [edx+64h],eax
                mov     dword ptr [edx+0Ch],0
                mov     dword ptr [edx+20h],0
                test    ecx,ecx
                jz      short loc_ret_a_1F0A    ; Jump if zero
                lea     eax,dword ptr [ecx+2]   ; Load effective addr
                mov     dword ptr [edx+20h],ecx
                add     ecx,6
                mov     dword ptr [edx+24h],eax
                mov     dword ptr [edx+1Ch],ecx
                mov     dword ptr [edx+28h],ecx
                mov     dword ptr [edx+0Ch],401h
                lea     eax,dword ptr [ecx-2]   ; Load effective addr
                mov     dword ptr [edx+18h],eax

loc_ret_a_1F0A::
                retn    8
sub_a_1EC0      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1F10      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+0Ch]
                or      ecx,4
                push    ecx
                mov     dword ptr [eax+0Ch],ecx
                push    eax
                call    sub_a_1DA0
                retn    4
sub_a_1F10      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1F30      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+0Ch]
                and     ecx,0FFFFFFFBh
                push    ecx
                mov     dword ptr [eax+0Ch],ecx
                push    eax
                call    sub_a_1DA0
                retn    4
sub_a_1F30      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1F50      proc    near
                push    ebx
                push    esi
                push    edi
                xor     ebx,ebx                 ; Zero register
                mov     edi,1
                mov     esi,dword ptr [esp+10h]
loc_a_1F5E::
                push    esi
                call    sub_a_1F10
                push    esi
                call    sub_a_1D60
                test    eax,eax
                jz      short loc_a_1F86        ; Jump if zero
                push    esi
                call    sub_a_1F30
                push    esi
                call    sub_a_1D60
                test    eax,eax
                jnz     short loc_a_1F86        ; Jump if not zero
                inc     ebx
                cmp     ebx,32h
                jl      loc_a_1F5E              ; Jump if <
                jmp     short loc_a_1F88
loc_a_1F86::
                xor     edi,edi                 ; Zero register
loc_a_1F88::
                push    esi
                call    sub_a_1F30
                mov     eax,edi
                mov     dword ptr [esi+58h],edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_1F50      endp

                db      7 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_1FA0      proc    near
                push    ebx
                push    esi
                mov     eax,dword ptr [ecx+0Ch]
                push    edi
                mov     ebx,ecx
                xor     esi,esi                 ; Zero register
                xor     edi,edi                 ; Zero register
                and     eax,0FFFFFDFFh
                push    ebx
                mov     dword ptr [ebx+0Ch],eax
                mov     dword ptr [ebx+4],407h
                and     eax,0FFFFFEFFh
                mov     dword ptr [ebx+0Ch],eax
                call    sub_a_1F50
                test    eax,eax
                jnz     short loc_a_1FD5        ; Jump if not zero
                xor     eax,eax                 ; Zero register
                pop     edi
                mov     [ebx],esi
                pop     esi
                pop     ebx
                retn
loc_a_1FD5::
                mov     eax,dword ptr [ebx+0Ch]
                push    ebx
                or      eax,200h
                mov     dword ptr [ebx+0Ch],eax
                or      eax,100h
                mov     dword ptr [ebx+0Ch],eax
                call    sub_a_1F30
                push    ebx
                call    sub_a_1D60
                test    eax,eax
                jnz     short loc_a_1FFD        ; Jump if not zero
                mov     esi,1
loc_a_1FFD::
                test    esi,esi
                jnz     short loc_a_2008        ; Jump if not zero
                mov     dword ptr [ebx+4],41Bh
loc_a_2008::
                mov     eax,dword ptr [ebx+0Ch]
                push    ebx
                and     eax,0FFFFFEFFh
                mov     dword ptr [ebx+0Ch],eax
                or      eax,200h
                mov     dword ptr [ebx+0Ch],eax
                call    sub_a_1F30
                push    ebx
                call    sub_a_1D60
                test    eax,eax
                jz      short loc_a_2030        ; Jump if zero
                mov     edi,1
loc_a_2030::
                test    edi,edi
                jz      short loc_a_206B        ; Jump if zero
                test    esi,esi
                jnz     short loc_a_203F        ; Jump if not zero
                mov     dword ptr [ebx+4],41Ah
loc_a_203F::
                mov     eax,dword ptr [ebx+0Ch]
                push    ebx
                and     eax,0FFFFFDFFh
                mov     dword ptr [ebx+0Ch],eax
                or      eax,100h
                mov     dword ptr [ebx+0Ch],eax
                call    sub_a_1F30
                push    ebx
                call    sub_a_1D60
                test    eax,eax
                jnz     short loc_a_206B        ; Jump if not zero
                mov     dword ptr [ebx+4],41Ch
                xor     edi,edi                 ; Zero register
loc_a_206B::
                mov     eax,dword ptr [ebx+0Ch]
                push    ebx
                and     eax,0FFFFFDFFh
                mov     dword ptr [ebx+0Ch],eax
                and     eax,0FFFFFEFFh
                mov     dword ptr [ebx+0Ch],eax
                call    sub_a_1F30
                test    esi,esi
                jnz     short loc_a_20C8        ; Jump if not zero
                test    edi,edi
                jnz     short loc_a_20C8        ; Jump if not zero
                cmp     dword ptr [ebx+4],41Ch
                mov     dword ptr [ebx],1
                jnz     short loc_a_20AB        ; Jump if not zero
                mov     dword ptr [ebx+5Ch],0E0h
                mov     dword ptr [ebx+4],41Ch
                jmp     short loc_a_20B9
loc_a_20AB::
                mov     dword ptr [ebx+5Ch],20h
                mov     dword ptr [ebx+4],41Bh
loc_a_20B9::
                mov     eax,1
                pop     edi
                pop     esi
                mov     dword ptr [ebx],1
                pop     ebx
                retn
loc_a_20C8::
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                mov     dword ptr [ebx],0
                pop     ebx
                retn
sub_a_1FA0      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_20E0      proc    near
                mov     eax,dword ptr [esp+4]
                cmp     dword ptr [eax+20h],0
                je      short loc_ret_a_20F0    ; Jump if equal
                push    eax
                call    sub_a_2740

loc_ret_a_20F0::
                retn    4
sub_a_20E0      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2100      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+0Ch]
                or      ecx,8
                push    ecx
                push    eax
                call    sub_a_1DA0
                retn    4
sub_a_2100      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2120      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+0Ch]
                or      ecx,10h
                push    ecx
                push    eax
                call    sub_a_1DA0
                retn    4
sub_a_2120      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2140      proc    near
                push    esi
                mov     esi,ecx
                call    sub_a_1D70
                test    byte ptr [esi+0Dh],1
                jnz     short loc_a_2153        ; Jump if not zero
                and     eax,7
                pop     esi
                retn
loc_a_2153::
                mov     ecx,dword ptr [esi+5Ch]
                pop     esi
                and     ecx,eax
                and     eax,7
                shr     ecx,2                   ; Shift w/zeros fill
                or      ecx,eax
                mov     eax,ecx
                retn
sub_a_2140      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2170      proc    near
                push    esi
                push    edi
                mov     esi,dword ptr [esp+0Ch]
loc_a_2176::
                mov     ecx,esi
                call    sub_a_2140
                mov     edi,eax
                mov     ecx,esi
                call    sub_a_2140
                cmp     eax,edi
                jne     loc_a_2176              ; Jump if not equal
                mov     eax,edi
                pop     edi
                pop     esi
                retn    4
sub_a_2170      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_21A0      proc    near
                mov     eax,dword ptr [esp+4]
                push    edi
                mov     edx,dword ptr [eax+38h]
                mov     edi,dword ptr [eax+34h]
                add     edx,edx
                xor     eax,eax                 ; Zero register
                mov     ecx,edx
                shr     ecx,2                   ; Shift w/zeros fill
                rep     stosd                   ; Rep when cx >0 Store ax to es:[di]
                mov     ecx,edx
                and     ecx,3
                rep     stosb                   ; Rep when cx >0 Store al to es:[di]
                pop     edi
                retn    4
sub_a_21A0      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_21D0      proc    near
                sub     esp,8
                push    ebx
                push    esi
                mov     ebx,dword ptr [esp+18h]
                push    edi
                push    ebp
                xor     edi,edi                 ; Zero register
                mov     dword ptr [esp+14h],9
                mov     dword ptr [esp+10h],edi
loc_a_21E9::
                xor     esi,esi                 ; Zero register
                mov     si,[ebx]
                mov     eax,esi
                and     eax,0CCCCh
                cmp     eax,0C0C0h
                jne     short loc_a_2200        ; Jump if not equal
                inc     dword ptr [esp+10h]
loc_a_2200::
                xor     ebp,ebp                 ; Zero register
                xor     edx,edx                 ; Zero register
                mov     eax,0Fh
loc_a_2209::
;*              shr     esi,1                   ; Shift w/zeros fill
                db      0C1h,0EEh, 01h          ;  Fixup - byte match
                mov     ecx,esi
                and     ecx,1
                jz      short loc_a_2218        ; Jump if zero
                test    ebp,ebp
                jz      short loc_a_2218        ; Jump if zero
                inc     edx
loc_a_2218::
                mov     ebp,ecx
                dec     eax
                jnz     loc_a_2209              ; Jump if not zero
                cmp     edx,2
                jbe     short loc_a_2223        ; Jump if below or =
                inc     edi
loc_a_2223::
                add     ebx,2
                dec     dword ptr [esp+14h]
                jnz     loc_a_21E9              ; Jump if not zero
                test    edi,edi
                jnz     short loc_a_2231        ; Jump if not zero
                inc     edi
loc_a_2231::
                cmp     dword ptr [esp+10h],8
                mov     eax,0
                jge     short loc_a_223F        ; Jump if > or =
                mov     eax,edi
loc_a_223F::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                add     esp,8
                retn    8
sub_a_21D0      endp

                db      7 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2250      proc    near
                push    ecx
                call    sub_a_20E0
                retn
sub_a_2250      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2260      proc    near
                mov     eax,dword ptr [esp+4]
                push    dword ptr 1
                push    eax
                mov     dword ptr [eax+30h],0Bh
                call    sub_a_23D0
                retn    4
sub_a_2260      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2280      proc    near
                mov     eax,dword ptr [esp+4]
                push    dword ptr 3
                cmp     dword ptr [esp+0Ch],1
                sbb     ecx,ecx
                push    eax
                and     ecx,0FFFFFFF7h
                add     ecx,0Ah
                mov     dword ptr [eax+30h],ecx
                call    sub_a_23D0
                retn    8
sub_a_2280      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_22A0      proc    near
                mov     eax,dword ptr [esp+4]
                push    dword ptr 1
                cmp     dword ptr [esp+0Ch],1
                sbb     ecx,ecx
                push    eax
                and     ecx,0FFFFFFFEh
                add     ecx,9
                mov     dword ptr [eax+30h],ecx
                call    sub_a_23D0
                retn    8
sub_a_22A0      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_22C0      proc    near
                mov     eax,dword ptr [esp+4]
                push    dword ptr 0
                push    eax
                mov     dword ptr [eax+30h],8
                call    sub_a_23D0
                retn    8
sub_a_22C0      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_22E0      proc    near
                mov     eax,dword ptr [esp+4]
                push    dword ptr 2
                push    eax
                mov     dword ptr [eax+30h],4
                call    sub_a_23D0
                retn    8
sub_a_22E0      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2300      proc    near
                mov     edx,dword ptr [esp+4]
                mov     eax,dword ptr [ecx+30h]
                and     edx,3
                shl     edx,0Bh                 ; Shift w/zeros fill
                cmp     eax,1
                mov     dword ptr [ecx+48h],edx
                jz      short loc_a_231A        ; Jump if zero
                cmp     eax,0Ah
                jne     short loc_ret_a_232E    ; Jump if not equal
loc_a_231A::
                mov     eax,dword ptr [ecx+0Ch]
                and     eax,0FFFFE7FFh
                or      eax,edx
                push    eax
                mov     dword ptr [ecx+0Ch],eax
                push    ecx
                call    sub_a_1DA0

loc_ret_a_232E::
                retn    4
sub_a_2300      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2340      proc    near
                mov     edx,dword ptr [esp+8]
                mov     ecx,dword ptr [esp+4]
                and     edx,7
                mov     eax,dword ptr [ecx+30h]
                shl     edx,5                   ; Shift w/zeros fill
                cmp     eax,1
                mov     dword ptr [ecx+40h],edx
                jz      short loc_a_235E        ; Jump if zero
                cmp     eax,0Ah
                jne     short loc_ret_a_2372    ; Jump if not equal
loc_a_235E::
                mov     eax,dword ptr [ecx+0Ch]
;*              and     eax,0FFFFFF1Fh
                db       25h, 1Fh,0FFh,0FFh,0FFh        ;  Fixup - byte match
                or      eax,edx
                push    eax
                mov     dword ptr [ecx+0Ch],eax
                push    ecx
                call    sub_a_1DA0

loc_ret_a_2372::
                retn    8
sub_a_2340      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2380      proc    near
                cmp     dword ptr [ecx],0
                je      short loc_a_23B2        ; Jump if equal
                or      dword ptr [ecx+0Ch],100h
                mov     dword ptr [ecx+60h],10h
                mov     dword ptr [ecx+64h],10h
                cmp     dword ptr [ecx+4],41Ch
                jne     short loc_ret_a_23C4    ; Jump if not equal
                mov     dword ptr [ecx+60h],40h
                mov     dword ptr [ecx+64h],40h
                retn
loc_a_23B2::
                and     dword ptr [ecx+0Ch],0FFFFFEFFh
                mov     eax,8
                mov     dword ptr [ecx+60h],eax
                mov     dword ptr [ecx+64h],eax

loc_ret_a_23C4::
                retn
sub_a_2380      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_23D0      proc    near
                push    esi
                push    edi
                mov     edi,dword ptr [esp+10h]
                and     edi,3
                cli                             ; Disable interrupts
                mov     esi,dword ptr [esp+0Ch]
                mov     eax,dword ptr [esi+30h]
                dec     eax
                cmp     eax,0Bh
                ja      loc_a_2507              ; Jump if above
                jmp     dword ptr data_a_2550[eax*4]    ;*1 entry
                                                ;* No entry point to code
                mov     eax,dword ptr [esi+40h]
;*              and     eax,0E0h
                db       25h,0E0h, 00h, 00h, 00h        ;  Fixup - byte match
                or      edi,eax
                lea     ecx,dword ptr [esi+0Ch] ; Load effective addr
                mov     edx,8
                mov     dword ptr [esi+60h],edx
                mov     dword ptr [esi+64h],edx
                mov     eax,[ecx]
                and     eax,0FFFFFEFFh
                mov     [ecx],eax
                and     eax,0FFFFFDFFh
                mov     [ecx],eax
loc_a_241A::
                mov     eax,[ecx]
                and     eax,0FFFFE71Ch
                or      eax,edi
                push    eax
                mov     [ecx],eax
                push    esi
                call    sub_a_1DA0
                sti                             ; Enable interrupts
                xor     eax,eax                 ; Zero register
                pop     edi
                pop     esi
                retn    8
                                                ;* No entry point to code
                cmp     dword ptr [esi],0
                je      loc_a_2507              ; Jump if equal
                mov     dword ptr [esi+60h],10h
                lea     ecx,dword ptr [esi+0Ch] ; Load effective addr
                mov     dword ptr [esi+64h],10h
                mov     eax,[ecx]
                or      eax,100h
                cmp     dword ptr [esi+4],41Ch
                mov     [ecx],eax
                jnz     short loc_a_246E        ; Jump if not zero
                mov     dword ptr [esi+60h],40h
                mov     dword ptr [esi+64h],40h
loc_a_246E::
                or      eax,200h
                mov     [ecx],eax
                mov     eax,dword ptr [esi+44h]
                and     eax,60h
                or      edi,eax
                jmp     short loc_a_241A
                                                ;* No entry point to code
                cmp     dword ptr [esi],0
                je      loc_a_2524              ; Jump if equal
                mov     dword ptr [esi+60h],10h
                lea     ecx,dword ptr [esi+0Ch] ; Load effective addr
                mov     dword ptr [esi+64h],10h
                mov     eax,[ecx]
                or      eax,100h
                cmp     dword ptr [esi+4],41Ch
                mov     [ecx],eax
                jnz     short loc_a_24B9        ; Jump if not zero
                mov     dword ptr [esi+60h],40h
                mov     dword ptr [esi+64h],40h
loc_a_24B9::
                or      eax,200h
                mov     edx,dword ptr [esi+40h]
;*              and     edx,0E0h
                db       81h,0E2h,0E0h, 00h, 00h, 00h   ;  Fixup - byte match
                mov     [ecx],eax
                mov     eax,dword ptr [esi+48h]
                and     eax,1800h
                or      eax,edx
                or      edi,eax
                jmp     loc_a_241A
                db       8Bh,0CEh,0E8h, 9Fh,0FEh,0FFh
                db      0FFh, 8Dh, 4Eh, 0Ch, 81h, 21h
                db      0FFh,0FDh,0FFh,0FFh,0E9h, 2Bh
                db      0FFh,0FFh,0FFh, 83h,0CFh, 20h
                db       8Bh,0CEh,0E8h, 87h,0FEh,0FFh
                db      0FFh, 8Dh, 4Eh, 0Ch, 81h, 21h
                db      0FFh,0FDh,0FFh,0FFh,0E9h, 13h
                db      0FFh,0FFh,0FFh
loc_a_2507::
                mov     ecx,esi
                call    sub_a_2380
                lea     ecx,dword ptr [esi+0Ch] ; Load effective addr
                mov     eax,dword ptr [esi+44h]
                and     eax,60h
                and     dword ptr [ecx],0FFFFFDFFh
                or      edi,eax
                jmp     loc_a_241A

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_2524::
                mov     ecx,esi
                call    sub_a_2380
                lea     ecx,dword ptr [esi+0Ch] ; Load effective addr
                mov     eax,dword ptr [esi+48h]
                and     eax,1800h
                mov     edx,dword ptr [esi+40h]
;*              and     edx,0E0h
                db       81h,0E2h,0E0h, 00h, 00h, 00h   ;  Fixup - byte match
                and     dword ptr [ecx],0FFFFFDFFh
                or      eax,edx
                or      edi,eax
                jmp     loc_a_241A
sub_a_23D0      endp

                db       8Bh,0FFh
data_a_2550     dd      offset32 loc_a_2524
                db      16 dup (0)
                db      0F2h, 23h, 00h
                db      9 dup (0)
                db       34h, 24h, 00h, 00h, 7Fh, 24h
                db       00h, 00h,0DAh, 24h, 00h, 00h
                db      0EFh, 24h, 00h, 00h

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2580      proc    near
                push    ebx
                push    esi
                push    edi
                push    ebp
                mov     edi,dword ptr [esp+1Ch]
                test    edi,edi
                jz      short loc_a_25D5        ; Jump if zero
                mov     ebx,dword ptr [esp+14h]
                mov     esi,dword ptr [esp+18h]
loc_a_2594::
                mov     eax,dword ptr [ebx+24h]
                push    eax
                call    sub_a_1D30
                mov     ebp,dword ptr [ebx+54h]
                cmp     edi,ebp
                jae     short loc_a_25A6        ; Jump if above or =
                mov     ebp,edi
loc_a_25A6::
                push    ebp                     ; PARAMETER_3
                mov     eax,dword ptr [ebx+20h]
                push    esi                     ; PARAMETER_2
                sub     edi,ebp
                push    eax                     ; PARAMETER_1
                lea     esi,[esi][ebp*2]        ; Load effective addr
                call    sub_a_2789
                add     esp,0Ch
                mov     eax,dword ptr [ebx+54h]
                sub     eax,ebp
                mov     dword ptr [ebx+54h],eax
                jnz     short loc_a_25D1        ; Jump if not zero
                mov     eax,dword ptr [ebx+2Ch]
                inc     eax
                push    eax
                mov     dword ptr [ebx+2Ch],eax
                push    ebx
                call    sub_a_1D80
loc_a_25D1::
                test    edi,edi
                jnz     loc_a_2594              ; Jump if not zero
loc_a_25D5::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    0Ch
sub_a_2580      endp

                db      0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_25E0      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                push    edi
                push    ebp
                mov     edi,ecx
                mov     eax,esi
                shl     eax,4                   ; Shift w/zeros fill
                push    eax
                push    edi
                call    sub_a_1D80
                mov     ebx,dword ptr [esp+1Ch]
                cmp     ebx,1000h
                mov     eax,ebx
                jc      short loc_a_2609        ; Jump if carry Set
                mov     eax,1000h
loc_a_2609::
                sub     ebx,eax
                push    eax
                mov     ebp,dword ptr [esp+1Ch]
                push    ebp
                push    edi
                call    sub_a_2580
                test    ebx,ebx
                jz      short loc_a_2639        ; Jump if zero
                and     esi,0Fh
                add     ebp,2000h
;*              add     esi,0F0h
                db       81h,0C6h,0F0h, 00h, 00h, 00h   ;  Fixup - byte match
                push    esi
                push    edi
                call    sub_a_1D80
                push    ebx
                push    ebp
                push    edi
                call    sub_a_2580
loc_a_2639::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    0Ch
sub_a_25E0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2640      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+0Ch]
                push    edi
                push    ebp
                mov     edi,ecx
                mov     eax,esi
                shl     eax,4                   ; Shift w/zeros fill
                push    eax
                push    edi
                call    sub_a_1D80
                mov     ebx,dword ptr [esp+1Ch]
                cmp     ebx,1000h
                mov     eax,ebx
                jc      short loc_a_2669        ; Jump if carry Set
                mov     eax,1000h
loc_a_2669::
                sub     ebx,eax
                push    eax
                mov     ebp,dword ptr [esp+1Ch]
                push    ebp
                push    edi
                call    sub_a_26A0
                test    ebx,ebx
                jz      short loc_a_2696        ; Jump if zero
;*              add     esi,0F0h
                db       81h,0C6h,0F0h, 00h, 00h, 00h   ;  Fixup - byte match
                add     ebp,2000h
                push    esi
                push    edi
                call    sub_a_1D80
                push    ebx
                push    ebp
                push    edi
                call    sub_a_26A0
loc_a_2696::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    0Ch
sub_a_2640      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_26A0      proc    near
                push    ebx
                push    esi
                push    edi
                push    ebp
                mov     edi,dword ptr [esp+1Ch]
                test    edi,edi
                jz      short loc_a_26EC        ; Jump if zero
                mov     ebx,dword ptr [esp+14h]
                mov     esi,dword ptr [esp+18h]
loc_a_26B4::
                mov     ebp,dword ptr [ebx+54h]
                cmp     ebp,edi
                jbe     short loc_a_26BD        ; Jump if below or =
                mov     ebp,edi
loc_a_26BD::
                push    ebp                     ; PARAMETER_3
                mov     eax,dword ptr [ebx+20h]
                push    esi                     ; PARAMETER_2
                sub     edi,ebp
                push    eax                     ; PARAMETER_1
                lea     esi,[esi][ebp*2]        ; Load effective addr
                call    sub_a_27C7
                add     esp,0Ch
                mov     eax,dword ptr [ebx+54h]
                sub     eax,ebp
                mov     dword ptr [ebx+54h],eax
                jnz     short loc_a_26E8        ; Jump if not zero
                mov     eax,dword ptr [ebx+2Ch]
                inc     eax
                push    eax
                mov     dword ptr [ebx+2Ch],eax
                push    ebx
                call    sub_a_1D80
loc_a_26E8::
                test    edi,edi
                jnz     loc_a_26B4              ; Jump if not zero
loc_a_26EC::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    0Ch
sub_a_26A0      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2700      proc    near
                mov     eax,dword ptr [esp+4]
                push    esi
                push    eax
                mov     esi,dword ptr [eax+60h]
                dec     esi
                call    sub_a_2170
                sub     esi,eax
                dec     esi
                test    esi,esi
                jg      short loc_a_2718        ; Jump if >
                xor     esi,esi                 ; Zero register
loc_a_2718::
                mov     eax,esi
                pop     esi
                retn    4
sub_a_2700      endp

                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2720      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+0Ch]
                or      ecx,400h
                push    ecx
                mov     dword ptr [eax+0Ch],ecx
                push    eax
                call    sub_a_1DA0
                retn    4
sub_a_2720      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2740      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+0Ch]
                and     ecx,0FFFFFBFFh
                push    ecx
                mov     dword ptr [eax+0Ch],ecx
                push    eax
                call    sub_a_1DA0
                retn    4
sub_a_2740      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2760      proc    near
                mov     eax,dword ptr [esp+4]
                mov     dword ptr [ecx+34h],eax
                mov     eax,dword ptr [esp+8]
;*              shr     eax,1                   ; Shift w/zeros fill
                db      0C1h,0E8h, 01h          ;  Fixup - byte match
                mov     dword ptr [ecx+38h],eax
                retn    8
sub_a_2760      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    edi
                push    ds
                pop     es
                mov     edx,dword ptr [ebp+8]
                cld                             ; Clear direction
                mov     edi,dword ptr [ebp+0Ch]
                mov     ecx,dword ptr [ebp+10h]
                rep     insb                    ; Rep when cx >0 Port dx to es:[di]
                pop     edi
                leave                           ; Procedure exit
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2789      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+
PARAMETER_3     =       10h                     ; ebp+

                push    ebp
                mov     ebp,esp
                push    edi
                push    ds
                pop     es
                mov     edx,dword ptr [ebp+PARAMETER_1]
                cld                             ; Clear direction
                mov     edi,dword ptr [ebp+PARAMETER_2]
                mov     ecx,dword ptr [ebp+PARAMETER_3]
                rep     insw                    ; Rep when cx >0 Port dx to es:[di]
                pop     edi
                leave                           ; Procedure exit
                retn
sub_a_2789      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    edi
                push    ds
                pop     es
                mov     edx,dword ptr [ebp+8]
                cld                             ; Clear direction
                mov     edi,dword ptr [ebp+0Ch]
                mov     ecx,dword ptr [ebp+10h]
                rep     insd                    ; Rep when cx >0 Port dx to es:[di]
                pop     edi
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    esi
                mov     edx,dword ptr [ebp+8]
                cld                             ; Clear direction
                mov     esi,dword ptr [ebp+0Ch]
                mov     ecx,dword ptr [ebp+10h]
                rep     outsb                   ; Rep when cx >0 Out [si] to port dx
                pop     esi
                leave                           ; Procedure exit
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_27C7      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+
PARAMETER_3     =       10h                     ; ebp+

                push    ebp
                mov     ebp,esp
                push    esi
                mov     edx,dword ptr [ebp+PARAMETER_1]
                cld                             ; Clear direction
                mov     esi,dword ptr [ebp+PARAMETER_2]
                mov     ecx,dword ptr [ebp+PARAMETER_3]
                rep     outsw                   ; Rep when cx >0 Out [si] to port dx
                pop     esi
                leave                           ; Procedure exit
                retn
sub_a_27C7      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    esi
                mov     edx,dword ptr [ebp+8]
                cld                             ; Clear direction
                mov     esi,dword ptr [ebp+0Ch]
                mov     ecx,dword ptr [ebp+10h]
                rep     outsd                   ; Rep when cx >0 Out [si] to port dx
                pop     esi
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                push    ebp
                mov     ebp,esp
                sub     esp,8
                push    ebx
                push    esi
                push    edi
                mov     dword ptr [ebp-8],edx
                mov     dword ptr [ebp-4],ebx
                pushad                          ; Save all regs
                mov     eax,dword ptr data_a_0000
                cmp     eax,dword ptr [ebp-4]
                je      short loc_a_2827        ; Jump if equal
                push    58Ch
                mov     eax,dword ptr [ebp-8]
                push    dword ptr 0
                push    eax
                push    dword ptr 0
                call    sub_b_00A0
                add     esp,10h
                mov     eax,dword ptr [ebp-4]
                mov     dword ptr data_a_0000,eax
loc_a_2827::
                popad                           ; Restore all regs
                pop     edi
                pop     esi
                pop     ebx
                mov     esp,ebp
                pop     ebp
                retn
                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2830      proc    near
                xor     edx,edx                 ; Zero register
                mov     eax,dword ptr [esp+4]
                mov     dx,word ptr [ecx+2]
                mov     word ptr [ecx+4][edx*8],ax
                xor     eax,eax                 ; Zero register
                mov     ax,word ptr [ecx+2]
                mov     dword ptr [ecx+8][eax*8],27F0h
                inc     word ptr [ecx+2]
                retn    4
sub_a_2830      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2860      proc    near
                push    ebx
                push    esi
                push    edi
                mov     ebx,ecx
                mov     edi,dword ptr [esp+14h]
                push    ebp
                xor     ebp,ebp                 ; Zero register
                mov     word ptr [ebx+2],bp
                cmp     edi,ebp
                je      short loc_a_2889        ; Jump if equal
                mov     esi,dword ptr [esp+14h]
loc_a_2878::
                lea     eax,dword ptr [ebp][esi]        ; Load effective addr
                mov     ecx,ebx
                push    eax
                inc     ebp
                call    sub_a_2830
                cmp     edi,ebp
                ja      loc_a_2878              ; Jump if above
loc_a_2889::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    8
sub_a_2860      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2890      proc    near
                push    ebx
                push    esi
                cmp     word ptr [ecx],0
                push    edi
                mov     esi,ecx
                jnz     short loc_a_28CD        ; Jump if not zero
                xor     edi,edi                 ; Zero register
                cmp     word ptr [esi+2],di
                jbe     short loc_a_28CD        ; Jump if below or =
                lea     ebx,dword ptr [esi+8]   ; Load effective addr
loc_a_28A6::
                mov     eax,[ebx]
                push    eax                     ; PARAMETER_2
                xor     eax,eax                 ; Zero register
                mov     ax,word ptr [ebx-4]
                push    eax                     ; PARAMETER_1
                call    sub_a_3A52
                add     esp,8
                test    eax,eax
                jnz     short loc_a_28CF        ; Jump if not zero
                inc     edi
                xor     eax,eax                 ; Zero register
                inc     word ptr [esi]
                add     ebx,8
                mov     ax,word ptr [esi+2]
                cmp     eax,edi
                ja      loc_a_28A6              ; Jump if above
loc_a_28CD::
                xor     eax,eax                 ; Zero register
loc_a_28CF::
                pop     edi
                pop     esi
                pop     ebx
                retn
sub_a_2890      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_28E0      proc    near
                push    ebx
                push    esi
                push    edi
                mov     esi,ecx
                xor     edi,edi                 ; Zero register
                cmp     [esi],di
                jbe     short loc_a_290B        ; Jump if below or =
                lea     ebx,dword ptr [esi+4]   ; Load effective addr
loc_a_28EF::
                xor     eax,eax                 ; Zero register
                add     ebx,8
                mov     ax,word ptr [ebx-8]
                inc     edi
                push    eax                     ; PARAMETER_1
                call    sub_a_3A6F
                add     esp,4
                xor     eax,eax                 ; Zero register
                mov     ax,[esi]
                cmp     eax,edi
                ja      loc_a_28EF              ; Jump if above
loc_a_290B::
                mov     word ptr [esi],0
                pop     edi
                mov     word ptr [esi+2],0
                pop     esi
                pop     ebx
                retn
sub_a_28E0      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2920      proc    near
                mov     eax,ecx
                retn    4
sub_a_2920      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2930      proc    near
                retn
sub_a_2930      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2940      proc    near
                sub     esp,408h
                mov     dword ptr [esp+4],1
                push    ebx
                push    esi
                mov     eax,[ecx]
                push    edi
                push    ebp
                mov     ebx,ecx
                push    eax
                call    sub_a_2260
                mov     ecx,[ebx]
                call    sub_a_1DF0
                mov     dword ptr [esp+10h],eax
                test    eax,eax
                jnz     short loc_a_2973        ; Jump if not zero
                mov     dword ptr [esp+14h],0
loc_a_2973::
                push    0CCCCh                  ; PARAMETER_1
                call    sub_a_3A7D
                lea     esi,dword ptr [esp+1Ch] ; Load effective addr
                add     esp,4
loc_a_2984::
                add     esi,2
                call    sub_a_3A8B
                mov     word ptr [esi-2],ax
                lea     eax,dword ptr data_a_0218[esp]  ; Load effective addr
                cmp     esi,eax
                jb      loc_a_2984              ; Jump if below
                xor     ebp,ebp                 ; Zero register
                cmp     dword ptr [esp+10h],ebp
                jle     short loc_a_2A01        ; Jump if < or =
loc_a_29A3::
                push    ebp
                mov     eax,[ebx]
                push    eax
                call    sub_a_1D80
                lea     eax,dword ptr [esp+18h] ; Load effective addr
                push    100h
                push    eax
                mov     ecx,[ebx]
                push    ecx
                call    sub_a_26A0
                push    ebp
                mov     ecx,[ebx]
                push    ecx
                call    sub_a_1D80
                lea     ecx,dword ptr data_a_0218[esp]  ; Load effective addr
                push    100h
                push    ecx
                mov     edx,[ebx]
                push    edx
                call    sub_a_2580
                lea     esi,dword ptr [esp+18h] ; Load effective addr
                lea     edi,dword ptr data_a_0218[esp]  ; Load effective addr
                mov     ecx,200h
                repe    cmpsb                   ; Rep zf=1+cx >0 Cmp [si] to es:[di]
                jnz     short loc_a_29F9        ; Jump if not zero
                inc     ebp
                cmp     ebp,dword ptr [esp+10h]
                jl      loc_a_29A3              ; Jump if <
                jmp     short loc_a_2A01
loc_a_29F9::
                mov     dword ptr [esp+14h],0
loc_a_2A01::
                mov     eax,dword ptr [esp+14h]
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                add     esp,408h
                retn    4
sub_a_2940      endp

                db      14 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2A20      proc    near
                sub     esp,8
                lea     eax,dword ptr [esp+0]   ; Load effective addr
                push    eax
                call    sub_a_37FC
                add     esp,0Ch
                retn
sub_a_2A20      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2A40      proc    near
                sub     esp,8
                lea     eax,dword ptr [esp+0]   ; Load effective addr
                push    eax
                call    sub_a_37FC
                sub     eax,dword ptr [esp+10h]
                add     esp,4
                add     esp,8
                retn
sub_a_2A40      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2A60      proc    near
                push    esi
                push    edi
                call    sub_a_2A20
                mov     edi,eax
                mov     eax,dword ptr [esp+0Ch]
                lea     ecx,[eax][eax*8]        ; Load effective addr
                lea     edx,[eax][ecx*4]        ; Load effective addr
                lea     ecx,[eax][edx*4]        ; Load effective addr
                lea     esi,[eax][ecx*8]        ; Load effective addr
loc_a_2A79::
                push    edi
                call    sub_a_2A40
                add     esp,4
                cmp     eax,esi
                jb      loc_a_2A79              ; Jump if below
                pop     edi
                pop     esi
                retn
sub_a_2A60      endp

                db      7 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2A90      proc    near
                push    ebx
                push    esi
                push    edi
                mov     esi,ecx
                push    ebp
                xor     edi,edi                 ; Zero register
                mov     dword ptr [esi+14h],edi
                mov     dword ptr [esi+18h],edi
                cli                             ; Disable interrupts
                push    edi
                mov     eax,[esi]
                push    eax
                call    sub_a_22A0
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,dword ptr [esi+4]
                mov     dword ptr [eax+40h],edi
                call    sub_a_2A20
                mov     ebx,eax
                sti                             ; Enable interrupts
loc_a_2ABE::
                mov     eax,dword ptr [esi+4]
                mov     ebp,dword ptr [eax+40h]
                test    ebp,ebp
                jnz     short loc_a_2AE1        ; Jump if not zero
                push    ebx
                call    sub_a_2A40
                add     esp,4
                cmp     eax,0BA68h
                jbe     loc_a_2ABE              ; Jump if below or =
                mov     dword ptr [esi+18h],1
                jmp     short loc_a_2B49
loc_a_2AE1::
                call    sub_a_2A20
                mov     ebx,eax
loc_a_2AE8::
                mov     eax,dword ptr [esi+4]
                cmp     dword ptr [eax+40h],ebp
                jne     short loc_a_2B0D        ; Jump if not equal
                test    edi,edi
                jnz     loc_a_2AE8              ; Jump if not zero
                push    ebx
                call    sub_a_2A40
                add     esp,4
                cmp     eax,1179Ch
                jbe     loc_a_2AE8              ; Jump if below or =
                mov     dword ptr [esi+18h],2
                jmp     short loc_a_2B49
loc_a_2B0D::
                inc     edi
                push    ebx
                call    sub_a_2A40
                add     esp,4
                sub     edx,edx
                div     edi                     ; ax,dx rem=dx:ax/reg
                cmp     eax,540Ah
                jae     short loc_a_2B2A        ; Jump if above or =
                cmp     edi,1
                ja      short loc_a_2B39        ; Jump if above
                inc     ebp
                jmp     short loc_a_2AE8
loc_a_2B2A::
                cmp     eax,66AEh
                jbe     short loc_a_2B54        ; Jump if below or =
                cmp     edi,1
                ja      short loc_a_2B42        ; Jump if above
                inc     ebp
                jmp     short loc_a_2AE8
loc_a_2B39::
                mov     dword ptr [esi+18h],3
                jmp     short loc_a_2B49
loc_a_2B42::
                mov     dword ptr [esi+18h],4
loc_a_2B49::
                mov     dword ptr [esi+14h],0Bh
                xor     edi,edi                 ; Zero register
                jmp     short loc_a_2B59
loc_a_2B54::
                mov     edi,1
loc_a_2B59::
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                call    sub_a_22A0
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,edi
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_2A90      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2B80      proc    near
                mov     eax,1
                retn
sub_a_2B80      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2B90      proc    near
loc_a_2B90::
                push    esi
                mov     esi,ecx
                call    sub_a_2B80
                test    eax,eax
                jnz     short loc_a_2BAA        ; Jump if not zero
                mov     dword ptr [esi+14h],14h
                mov     eax,14h
                pop     esi
                retn
loc_a_2BAA::
                push    dword ptr 0
                mov     ecx,esi
                call    sub_a_2940
                test    eax,eax
                jnz     short loc_a_2BC5        ; Jump if not zero
                mov     dword ptr [esi+14h],6
                mov     eax,6
                pop     esi
                retn
loc_a_2BC5::
                mov     ecx,esi
                call    sub_a_2B80
                test    eax,eax
                jnz     short loc_a_2BDE        ; Jump if not zero
                mov     dword ptr [esi+14h],15h
                mov     eax,15h
                pop     esi
                retn
loc_a_2BDE::
                push    dword ptr 0
                mov     ecx,esi
                call    sub_a_2A90
                test    eax,eax
                jnz     short loc_a_2BF9        ; Jump if not zero
                mov     dword ptr [esi+14h],0Bh
                mov     eax,0Bh
                pop     esi
                retn
loc_a_2BF9::
                xor     eax,eax                 ; Zero register
                pop     esi
                retn
sub_a_2B90      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2C00      proc    near
                push    ebx
                push    esi
                mov     eax,[ecx]
                push    edi
                mov     esi,ecx
                mov     edi,1
                push    dword ptr 0
                push    eax
                mov     dword ptr [esi+14h],0
                call    sub_a_22A0
                cli                             ; Disable interrupts
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,[esi]
                push    eax
                call    sub_a_2170
                test    eax,eax
                jz      short loc_a_2C42        ; Jump if zero
                mov     dword ptr [esi+14h],1
                xor     edi,edi                 ; Zero register
                sti                             ; Enable interrupts
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_2C42::
                mov     ebx,1
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                dec     eax
                cmp     eax,ebx
                jb      short loc_a_2C79        ; Jump if below
loc_a_2C51::
                mov     eax,[esi]
                push    eax
                call    sub_a_2100
                mov     eax,[esi]
                push    eax
                call    sub_a_2170
                cmp     eax,ebx
                je      short loc_a_2C6E        ; Jump if equal
                mov     ecx,ebx
                sub     ecx,eax
                cmp     ecx,1
                jne     short loc_a_2C9F        ; Jump if not equal
loc_a_2C6E::
                inc     ebx
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                dec     eax
                cmp     eax,ebx
                jae     loc_a_2C51              ; Jump if above or =
loc_a_2C79::
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,[esi]
                push    eax
                call    sub_a_2170
                test    eax,eax
                jz      short loc_a_2C96        ; Jump if zero
                mov     dword ptr [esi+14h],1
                xor     edi,edi                 ; Zero register
loc_a_2C96::
                sti                             ; Enable interrupts
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_2C9F::
                mov     dword ptr [esi+14h],2
                xor     edi,edi                 ; Zero register
                sti                             ; Enable interrupts
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_2C00      endp

                db      15 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2CC0      proc    near
;*              sub     esp,90h
                db       81h,0ECh, 90h, 00h, 00h, 00h   ;  Fixup - byte match
                push    ebx
                push    esi
                push    edi
                mov     esi,ecx
                lea     ecx,dword ptr [esp+14h] ; Load effective addr
                push    ebp
                call    sub_a_2CD5

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_2CD5::
                lea     ecx,dword ptr [esp+5Ch] ; Load effective addr
                call    sub_a_2CDE

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_2CDE::
                lea     ecx,dword ptr [esp+18h] ; Load effective addr
                push    dword ptr 0
                mov     dword ptr [esp+18h],1
                mov     dword ptr [esi+14h],0
                call    sub_a_2CF8

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_2CF8::
                lea     ecx,dword ptr [esp+5Ch] ; Load effective addr
                push    dword ptr 1
                call    sub_a_2D03

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_2D03::
                mov     eax,[esi]
                push    eax
                call    sub_a_2260
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                call    sub_a_1D80
                cmp     dword ptr data_a_00A4[esp],0
                je      short loc_a_2D2D        ; Jump if equal
                cmp     dword ptr data_a_00A4[esp],1
                jne     loc_a_2E0E              ; Jump if not equal
loc_a_2D2D::
                xor     edi,edi                 ; Zero register
loc_a_2D2F::
                push    edi
                mov     eax,[esi]
                push    eax
                add     edi,10h
                call    sub_a_1D80
                mov     eax,dword ptr [esp+50h]
                mov     ecx,dword ptr [esp+1Ch]
                push    eax
                mov     edx,[esi]
                push    ecx
                push    edx
                call    sub_a_26A0
;*              cmp     edi,80h
                db       81h,0FFh, 80h, 00h, 00h, 00h   ;  Fixup - byte match
                jc      loc_a_2D2F              ; Jump if carry Set
                mov     dword ptr [esp+10h],0
loc_a_2D5D::
                mov     edi,dword ptr [esp+10h]
                xor     ebx,ebx                 ; Zero register
loc_a_2D63::
                push    edi
                mov     eax,edi
                inc     edi
                mov     ecx,[esi]
                push    ecx
                xor     ebp,ebp                 ; Zero register
                call    sub_a_1D80
loc_a_2D71::
                mov     eax,[esi]
                push    eax
                call    sub_a_1DC0
                mov     eax,[esi]
                push    eax
                call    sub_a_1DC0
                test    eax,eax
                jnz     loc_a_2E93              ; Jump if not zero
                push    0FFFFFFFFh
                mov     ecx,[esi]
                inc     ebp
                call    sub_a_1DB0
                cmp     ebp,100h
                jb      loc_a_2D71              ; Jump if below
                inc     ebx
                cmp     ebx,10h
                jb      loc_a_2D63              ; Jump if below
                add     dword ptr [esp+10h],10h
;*              cmp     dword ptr [esp+10h],80h
                db       81h, 7Ch, 24h, 10h, 80h, 00h   ;  Fixup - byte match
                db       00h, 00h               ;  Fixup - byte match
                jc      loc_a_2D5D              ; Jump if carry Set
                mov     dword ptr [esp+10h],0
loc_a_2DB8::
                mov     edi,dword ptr [esp+10h]
                xor     ebx,ebx                 ; Zero register
loc_a_2DBE::
                push    edi
                mov     eax,edi
                inc     edi
                mov     ecx,[esi]
                push    ecx
                xor     ebp,ebp                 ; Zero register
                call    sub_a_1D80
loc_a_2DCC::
                mov     eax,[esi]
                push    eax
                call    sub_a_1DC0
                mov     eax,[esi]
                push    eax
                call    sub_a_1DC0
                cmp     eax,0FFFFh
                jne     loc_a_2EA4              ; Jump if not equal
                push    dword ptr 0
                mov     ecx,[esi]
                inc     ebp
                call    sub_a_1DB0
                cmp     ebp,100h
                jb      loc_a_2DCC              ; Jump if below
                inc     ebx
                cmp     ebx,10h
                jb      loc_a_2DBE              ; Jump if below
                add     dword ptr [esp+10h],10h
;*              cmp     dword ptr [esp+10h],80h
                db       81h, 7Ch, 24h, 10h, 80h, 00h   ;  Fixup - byte match
                db       00h, 00h               ;  Fixup - byte match
                jc      loc_a_2DB8              ; Jump if carry Set
loc_a_2E0E::
                cmp     dword ptr data_a_00A4[esp],0
                je      short loc_a_2E26        ; Jump if equal
                cmp     dword ptr data_a_00A4[esp],2
                jne     loc_a_2EC4              ; Jump if not equal
loc_a_2E26::
                mov     edi,8
loc_a_2E2B::
                lea     ecx,dword ptr [esp+18h] ; Load effective addr
                push    dword ptr 5
                call    sub_b_02E0
                dec     edi
                jnz     loc_a_2E2B              ; Jump if not zero
                xor     edi,edi                 ; Zero register
loc_a_2E3B::
                push    edi
                mov     eax,[esi]
                push    eax
                call    sub_a_1D80
                mov     eax,dword ptr [esp+50h]
                mov     ecx,dword ptr [esp+1Ch]
                push    eax
                mov     edx,[esi]
                push    ecx
                push    edx
                call    sub_a_26A0
                push    edi
                mov     ecx,[esi]
                push    ecx
                call    sub_a_1D80
                mov     ecx,dword ptr data_a_0094[esp]
                mov     edx,dword ptr [esp+60h]
                push    ecx
                mov     eax,[esi]
                push    edx
                push    eax
                call    sub_a_2580
                mov     ecx,dword ptr [esp+60h]
                push    ecx
                lea     ecx,dword ptr [esp+1Ch] ; Load effective addr
                call    sub_b_0260
                test    eax,eax
                jz      short loc_a_2EB5        ; Jump if zero
                add     edi,10h
;*              cmp     edi,80h
                db       81h,0FFh, 80h, 00h, 00h, 00h   ;  Fixup - byte match
                jc      loc_a_2E3B              ; Jump if carry Set
                jmp     short loc_a_2EC4
loc_a_2E93::
                mov     dword ptr [esp+14h],0
                mov     dword ptr [esi+14h],5
                jmp     short loc_a_2EC4
loc_a_2EA4::
                mov     dword ptr [esp+14h],0
                mov     dword ptr [esi+14h],5
                jmp     short loc_a_2EC4
loc_a_2EB5::
                mov     dword ptr [esp+14h],0
                mov     dword ptr [esi+14h],6
loc_a_2EC4::
                lea     ecx,dword ptr [esp+5Ch] ; Load effective addr
                call    sub_a_2ECD

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_2ECD::
                lea     ecx,dword ptr [esp+18h] ; Load effective addr
                call    sub_a_2ED6

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_a_2ED6::
                mov     eax,dword ptr [esp+14h]
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
;*              add     esp,90h
                db       81h,0C4h, 90h, 00h, 00h, 00h   ;  Fixup - byte match
                retn    4
sub_a_2CC0      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_2EF0      proc    near
                push    ebx
                push    esi
                mov     eax,[ecx]
                push    edi
                mov     esi,ecx
                mov     edi,1
                push    dword ptr 0
                push    eax
                mov     dword ptr [esi+14h],0
                xor     ebx,ebx                 ; Zero register
                call    sub_a_22A0
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                dec     eax
                jz      short loc_a_2F30        ; Jump if zero
loc_a_2F1D::
                mov     eax,[esi]
                inc     ebx
                push    eax
                call    sub_a_2100
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                dec     eax
                cmp     eax,ebx
                ja      loc_a_2F1D              ; Jump if above
loc_a_2F30::
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                lea     edx,[eax][eax*4]        ; Load effective addr
                lea     eax,[eax][edx*4]        ; Load effective addr
                push    eax
                call    sub_a_2A60
                add     esp,4
                mov     ecx,[esi]
                push    ecx
                call    sub_a_2170
                test    eax,eax
                jz      short loc_a_2F61        ; Jump if zero
                mov     dword ptr [esi+14h],7
                xor     edi,edi                 ; Zero register
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_2F61::
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                xor     ebx,ebx                 ; Zero register
                call    sub_a_22C0
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                dec     eax
                jz      short loc_a_2F90        ; Jump if zero
loc_a_2F7D::
                mov     eax,[esi]
                inc     ebx
                push    eax
                call    sub_a_2100
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                dec     eax
                cmp     eax,ebx
                ja      loc_a_2F7D              ; Jump if above
loc_a_2F90::
                mov     eax,[esi]
                mov     eax,dword ptr [eax+60h]
                lea     edx,[eax][eax*4]        ; Load effective addr
                lea     eax,[eax][edx*4]        ; Load effective addr
                push    eax
                call    sub_a_2A60
                add     esp,4
                mov     ecx,[esi]
                push    ecx
                call    sub_a_2170
                test    eax,eax
                jz      short loc_a_2FC1        ; Jump if zero
                mov     dword ptr [esi+14h],9
                xor     edi,edi                 ; Zero register
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_2FC1::
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                call    sub_a_2280
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,[esi]
                mov     ecx,dword ptr [eax+60h]
                lea     eax,[ecx][ecx*4]        ; Load effective addr
                lea     ecx,[ecx][eax*4]        ; Load effective addr
                push    ecx
                call    sub_a_2A60
                add     esp,4
                mov     ebx,[esi]
                push    ebx
                call    sub_a_2170
                sub     eax,dword ptr [ebx+60h]
                cmp     eax,0FFFFFFFFh
                je      short loc_a_3008        ; Jump if equal
                mov     dword ptr [esi+14h],8
                xor     edi,edi                 ; Zero register
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
loc_a_3008::
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                call    sub_a_22E0
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                mov     eax,[esi]
                mov     ecx,dword ptr [eax+60h]
                lea     eax,[ecx][ecx*4]        ; Load effective addr
                lea     ecx,[ecx][eax*4]        ; Load effective addr
                push    ecx
                call    sub_a_2A60
                add     esp,4
                mov     ebx,[esi]
                push    ebx
                call    sub_a_2170
                sub     eax,dword ptr [ebx+60h]
                cmp     eax,0FFFFFFFFh
                je      short loc_a_3047        ; Jump if equal
                mov     dword ptr [esi+14h],0Ah
                xor     edi,edi                 ; Zero register
loc_a_3047::
                mov     eax,edi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_a_2EF0      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3050      proc    near
                mov     cx,word ptr [esp+4]
                xor     eax,eax                 ; Zero register
                mov     edx,10h
loc_a_305C::
                test    cl,1
                jz      short loc_a_3062        ; Jump if zero
                inc     eax
loc_a_3062::
;*              shr     cx,1                    ; Shift w/zeros fill
                db      0C1h,0E9h, 01h, 4Ah     ;  Fixup - byte match
                jnz     loc_a_305C              ; Jump if not zero
                retn
sub_a_3050      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3070      proc    near
                sub     esp,50h
                mov     dword ptr [esp+8],1
                push    ebx
                push    esi
                push    edi
                mov     esi,ecx
                lea     ecx,dword ptr [esp+18h] ; Load effective addr
                push    ebp
                call    sub_b_0510
                push    dword ptr 0
                mov     eax,[esi]
                mov     dword ptr [esi+14h],0
                push    eax
                call    sub_a_22E0
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                push    dword ptr 64h
                call    sub_a_2A60
                add     esp,4
                mov     eax,[esi]
                push    eax
                call    sub_a_2100
                push    190h
                call    sub_a_2A60
                add     esp,4
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
loc_a_30CA::
                mov     eax,[esi]
                push    eax
                call    sub_a_2170
                cmp     eax,6
                jb      loc_a_30CA              ; Jump if below
                mov     dword ptr [esp+14h],0
                mov     dword ptr [esp+10h],10h
loc_a_30E7::
                mov     eax,dword ptr [esp+10h]
                mov     ecx,[esi]
                push    eax
                push    ecx
                call    sub_a_1D80
                mov     ecx,dword ptr [esp+20h]
                push    0EA0h
                push    ecx
                mov     edx,[esi]
                push    edx
                xor     ebx,ebx                 ; Zero register
                mov     edi,0EA0h
                call    sub_a_2580
                mov     ebp,dword ptr [esp+20h]
loc_a_3111::
                mov     ax,word ptr [ebp+0]
                test    ax,ax
                jz      short loc_a_3125        ; Jump if zero
                push    eax
                call    sub_a_3050
                add     esp,4
                add     ebx,eax
loc_a_3125::
                add     ebp,2
                dec     edi
                jnz     loc_a_3111              ; Jump if not zero
                cmp     ebx,640h
                jbe     short loc_a_3143        ; Jump if below or =
                add     dword ptr [esp+10h],10h
                inc     dword ptr [esp+14h]
                cmp     dword ptr [esp+10h],30h
                jbe     loc_a_30E7              ; Jump if below or =
loc_a_3143::
                cmp     dword ptr [esp+14h],1
                jbe     short loc_a_3159        ; Jump if below or =
                mov     dword ptr [esp+18h],0
                mov     dword ptr [esi+14h],0Fh
loc_a_3159::
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                call    sub_a_22A0
                lea     ecx,dword ptr [esp+1Ch] ; Load effective addr
                call    sub_b_0560
                mov     eax,dword ptr [esp+18h]
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                add     esp,50h
                retn
sub_a_3070      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3180      proc    near
                sub     esp,4
                mov     dword ptr [esp+0],1
                push    ebx
                push    esi
                mov     eax,[ecx]
                push    edi
                push    ebp
                mov     esi,ecx
                push    dword ptr 0
                push    eax
                mov     dword ptr [esi+14h],0
                call    sub_a_2280
                push    dword ptr 3
                mov     eax,[esi]
                push    eax
                call    sub_a_2340
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
                push    dword ptr 0
                mov     eax,[esi]
                push    eax
                call    sub_a_2280
;*              push    0D2h
                db       68h,0D2h, 00h, 00h, 00h        ;  Fixup - byte match
                call    sub_a_2A60
                add     esp,4
                mov     eax,[esi]
                push    eax
                call    sub_a_2120
;*              push    0D2h
                db       68h,0D2h, 00h, 00h, 00h        ;  Fixup - byte match
                call    sub_a_2A60
                add     esp,4
                mov     eax,[esi]
                cmp     dword ptr [eax],1
                sbb     edi,edi
                and     edi,0FFFFFFFDh
                add     edi,9
                jz      short loc_a_3200        ; Jump if zero
                xor     ebx,ebx                 ; Zero register
loc_a_31F1::
                push    ebx
                mov     eax,[esi]
                push    eax
                add     ebx,10h
                call    sub_a_1D80
                dec     edi
                jnz     loc_a_31F1              ; Jump if not zero
loc_a_3200::
                mov     ecx,[esi]
                call    sub_a_1D40
                mov     ebx,eax
                push    0A0Eh
                push    ebx
                mov     eax,[esi]
                push    eax
                xor     edi,edi                 ; Zero register
                mov     ebp,11Eh
                call    sub_a_2580
loc_a_321E::
                push    ebx
                mov     eax,[esi]
                push    eax
                call    sub_a_21D0
                test    eax,eax
                jnz     short loc_a_322C        ; Jump if not zero
                inc     edi
loc_a_322C::
                add     ebx,12h
                dec     ebp
                jnz     loc_a_321E              ; Jump if not zero
;*              cmp     edi,0C8h
                db       81h,0FFh,0C8h, 00h, 00h, 00h   ;  Fixup - byte match
                jnc     short loc_a_3249        ; Jump if carry=0
                mov     dword ptr [esi+14h],10h
                mov     dword ptr [esp+10h],0
loc_a_3249::
                mov     eax,dword ptr [esp+10h]
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                add     esp,4
                retn
sub_a_3180      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3260      proc    near
                xor     eax,eax                 ; Zero register
                mov     edx,dword ptr [esp+4]
                cmp     edx,9
                ja      short loc_ret_a_32D2    ; Jump if above
                jmp     dword ptr data_a_32D8[edx*4]    ;*10 entries

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_3272::
                call    sub_a_2B90
                cmp     eax,1
                sbb     eax,eax
                neg     eax
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_3281::
                push    dword ptr 0
                call    sub_a_2C00
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_328B::
                push    dword ptr 0
                call    sub_a_2CC0
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_3295::
                push    dword ptr 1
                call    sub_a_2CC0
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_329F::
                push    dword ptr 2
                call    sub_a_2CC0
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_32A9::
                push    dword ptr 0
                call    sub_a_2EF0
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_32B3::
                call    sub_a_3070
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_32BB::
                call    sub_a_3180
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_32C3::
                mov     eax,1
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_32CB::
                push    dword ptr 0
                call    sub_a_2A90

loc_ret_a_32D2::
                retn    4
sub_a_3260      endp

                db       8Dh, 49h, 00h
data_a_32D8     dd      offset32 loc_a_3272
data_a_32DC     dd      offset32 loc_a_3281
data_a_32E0     dd      offset32 loc_a_328B
data_a_32E4     dd      offset32 loc_a_3295
data_a_32E8     dd      offset32 loc_a_329F
data_a_32EC     dd      offset32 loc_a_32A9
data_a_32F0     dd      offset32 loc_a_32B3
data_a_32F4     dd      offset32 loc_a_32BB
data_a_32F8     dd      offset32 loc_a_32C3
data_a_32FC     dd      offset32 loc_a_32CB

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3300      proc    near
                sub     esp,4
                push    esi
                push    edi
                mov     esi,dword ptr [esp+10h]
                test    esi,esi
                jl      loc_a_33EF              ; Jump if <
                cmp     esi,4
                jg      loc_a_33EF              ; Jump if >
                xor     ecx,ecx                 ; Zero register
loc_a_331C::
                mov     eax,ecx
                mov     edx,0CF8h
                and     eax,0FFFFFFFCh
                or      eax,80003800h
                out     dx,eax                  ; port 0CF8h ??i/o non-standard
                mov     edx,ecx
                inc     ecx
                and     edx,3
                add     edx,0CFCh
                cmp     ecx,4
                in      al,dx                   ; port 0, DMA-1 bas&add ch 0
                mov     byte ptr [esp+7][ecx],al
                jc      loc_a_331C              ; Jump if carry Set
                cmp     dword ptr [esp+8],70008086h
                je      short loc_a_337E        ; Jump if equal
                xor     ecx,ecx                 ; Zero register
loc_a_334E::
                mov     eax,ecx
                mov     edx,0CF8h
                and     eax,0FFFFFFFCh
                or      eax,80003800h
                out     dx,eax                  ; port 0CF8h ??i/o non-standard
                mov     edx,ecx
                inc     ecx
                and     edx,3
                add     edx,0CFCh
                cmp     ecx,4
                in      al,dx                   ; port 0, DMA-1 bas&add ch 0
                mov     byte ptr [esp+7][ecx],al
                jc      loc_a_334E              ; Jump if carry Set
                cmp     dword ptr [esp+8],122E8086h
                jne     short loc_a_33EF        ; Jump if not equal
loc_a_337E::
                xor     edi,edi                 ; Zero register
loc_a_3380::
                lea     ecx,dword ptr [edi+4Ch] ; Load effective addr
                mov     edx,0CF8h
                mov     eax,ecx
                inc     edi
                and     eax,0FFFFFFFCh
                and     ecx,3
                or      eax,80003800h
                cmp     edi,1
                out     dx,eax                  ; port 0CF8h ??i/o non-standard
                lea     edx,dword ptr ds:[0CFCh][ecx]   ; Load effective addr
                in      al,dx                   ; port 0CF8h ??i/o non-standard
                mov     byte ptr [esp+7][edi],al
                jc      loc_a_3380              ; Jump if carry Set
                test    esi,esi
                jz      short loc_a_33C1        ; Jump if zero
                mov     eax,dword ptr [esp+8]
                xor     eax,esi
                and     eax,3
                xor     eax,dword ptr [esp+8]
                or      eax,4
                mov     dword ptr [esp+8],eax
                jmp     short loc_a_33C6
loc_a_33C1::
                and     dword ptr [esp+8],0FFFFFFF8h
loc_a_33C6::
                xor     ecx,ecx                 ; Zero register
loc_a_33C8::
                lea     esi,dword ptr [ecx+4Ch] ; Load effective addr
                mov     edx,0CF8h
                mov     eax,esi
                inc     ecx
                and     eax,0FFFFFFFCh
                and     esi,3
                or      eax,80003800h
                cmp     ecx,1
                out     dx,eax                  ; port 0CF8h ??i/o non-standard
                lea     edx,dword ptr ds:[0CFCh][esi]   ; Load effective addr
                mov     al,byte ptr [esp+7][ecx]
                out     dx,al                   ; port 0CF8h ??i/o non-standard
                jc      loc_a_33C8              ; Jump if carry Set
loc_a_33EF::
                pop     edi
                pop     esi
                add     esp,4
                retn
sub_a_3300      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3400      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                push    ebx
                push    esi
                push    edi
                cli                             ; Disable interrupts
                pusha                           ; Save all regs
                push    es
                push    ds
                pop     es
                mov     eax,dword ptr [ebp+PARAMETER_1]
                mov     eax,dword ptr data_a_0000[eax*4]
                test    eax,eax
                jz      short loc_a_3423        ; Jump if zero
                push    eax
                call    sub_a_36B0
                add     esp,4
loc_a_3423::
                sti                             ; Enable interrupts
                pop     es
                popa                            ; Restore all regs
                pop     edi
                pop     esi
                pop     ebx
                pop     ebp
                retn    4
sub_a_3400      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                push    dword ptr 0             ; PARAMETER_1
                call    sub_a_3400
                retn
                db      8 dup (0CCh)
                db       6Ah, 01h,0E8h,0B9h,0FFh,0FFh
                db      0FFh,0C3h
                db      8 dup (0CCh)
                db       6Ah, 02h,0E8h,0A9h,0FFh,0FFh
                db      0FFh,0C3h
                db      8 dup (0CCh)
                db       6Ah, 03h,0E8h, 99h,0FFh,0FFh
                db      0FFh,0C3h
                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3470      proc    near
                mov     eax,dword ptr [esp+4]
                sub     esp,4
                sub     eax,0Ah
                cmp     eax,5
                ja      short loc_a_34AF        ; Jump if above
                jmp     dword ptr data_a_34BC[eax*4]    ;*3 entries

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_3486::
                xor     eax,eax                 ; Zero register
                add     esp,4
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_348E::
                mov     eax,1
                add     esp,4
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_3499::
                mov     eax,2
                add     esp,4
                retn    4
                                                ;* No entry point to code
                mov     eax,3
                add     esp,4
                retn    4
loc_a_34AF::
                mov     eax,dword ptr [esp+0]
                add     esp,4
                retn    4
sub_a_3470      endp

                db       8Dh, 49h, 00h
data_a_34BC     dd      offset32 loc_a_3486
data_a_34C0     dd      offset32 loc_a_348E
data_a_34C4     dd      offset32 loc_a_3499
                db      8 dup (0)
                db      0A4h, 34h, 00h, 00h,0CCh
                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_34E0      proc    near
                mov     eax,dword ptr [esp+4]
                sub     esp,4
                sub     eax,0Ah
                cmp     eax,5
                ja      short loc_a_3522        ; Jump if above
                jmp     dword ptr data_a_352C[eax*4]    ;*3 entries

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_34F6::
                mov     eax,3430h
                add     esp,4
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_3501::
                mov     eax,3440h
                add     esp,4
                retn    4

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_a_350C::
                mov     eax,3450h
                add     esp,4
                retn    4
                                                ;* No entry point to code
                mov     eax,3460h
                add     esp,4
                retn    4
loc_a_3522::
                mov     eax,dword ptr [esp+0]
                add     esp,4
                retn    4
sub_a_34E0      endp

data_a_352C     dd      offset32 loc_a_34F6
data_a_3530     dd      offset32 loc_a_3501
data_a_3534     dd      offset32 loc_a_350C
                db      8 dup (0)
                db       17h, 35h, 00h, 00h,0CCh,0CCh
                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3550      proc    near
                push    ebx
                push    esi
                mov     esi,dword ptr [esp+10h]
                push    edi
                test    esi,esi
                jz      short loc_a_3589        ; Jump if zero
                mov     edi,dword ptr [esp+10h]
                push    esi
                mov     word ptr [edi+20h],si
                call    sub_a_3470
                mov     ebx,eax
                push    esi
                call    sub_a_34E0
                mov     dword ptr [edi+24h],eax
                test    ebx,ebx
                jl      short loc_a_357F        ; Jump if <
                mov     dword ptr data_a_0000[ebx*4],edi
loc_a_357F::
                mov     ecx,edi
                mov     dword ptr [edi+14h],esi
                call    sub_a_35D0
loc_a_3589::
                pop     edi
                pop     esi
                pop     ebx
                retn    8
sub_a_3550      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3590      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                cmp     dword ptr [esi+14h],0
                jne     short loc_a_35C0        ; Jump if not equal
                mov     eax,dword ptr [esp+14h]
                mov     ecx,dword ptr [esp+10h]
                mov     edx,dword ptr [esp+0Ch]
                mov     [esi],eax
                push    edx
                mov     dword ptr [esi+4],ecx
                push    esi
                call    sub_a_3550
                cmp     dword ptr [esi+18h],0
                jne     short loc_a_35C0        ; Jump if not equal
                mov     dword ptr [esi+18h],1
loc_a_35C0::
                pop     esi
                retn    10h
sub_a_3590      endp

                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_35D0      proc    near
                push    esi
                lea     eax,dword ptr [ecx+20h] ; Load effective addr
                mov     esi,ecx
                push    eax                     ; PARAMETER_1
                call    sub_a_383E
                add     esp,4
                mov     dword ptr [esi+3Ch],eax
                cmp     eax,0FFFFFFFFh
                jne     short loc_a_35EE        ; Jump if not equal
                mov     dword ptr [esi+14h],0
loc_a_35EE::
                pop     esi
                retn
sub_a_35D0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_35F0      proc    near
                push    esi
                mov     esi,ecx
                cmp     dword ptr [ecx+14h],0
                je      short loc_a_3626        ; Jump if equal
                push    esi
                call    sub_a_3660
                mov     eax,dword ptr [esi+14h]
                push    eax
                call    sub_a_3470
                mov     dword ptr data_a_0680[eax*4],0
                mov     eax,dword ptr [esi+3Ch]
                push    eax                     ; PARAMETER_1
                call    sub_a_389D
                mov     dword ptr [esi+14h],0
                add     esp,4
loc_a_3626::
                cmp     dword ptr [esi+18h],0
                je      short loc_a_3633        ; Jump if equal
                mov     dword ptr [esi+18h],0
loc_a_3633::
                pop     esi
                retn
sub_a_35F0      endp

                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3640      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+3Ch]
                push    ecx                     ; PARAMETER_1
                call    sub_a_3830
                add     esp,4
                retn    4
sub_a_3640      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3660      proc    near
                mov     eax,dword ptr [esp+4]
                mov     ecx,dword ptr [eax+3Ch]
                push    ecx                     ; PARAMETER_1
                call    sub_a_3822
                add     esp,4
                retn    4
sub_a_3660      endp

                db      13 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3680      proc    near
                push    esi
                mov     esi,ecx
                cmp     dword ptr [ecx+14h],0
                je      short loc_a_3696        ; Jump if equal
                push    esi
                call    sub_a_3660
                mov     ecx,esi
                call    sub_a_35F0
loc_a_3696::
                pop     esi
                retn
sub_a_3680      endp

                db      8 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_36A0      proc    near
                mov     eax,dword ptr [ecx+3Ch]
                push    eax
                call    sub_a_388F
                add     esp,4
                retn
sub_a_36A0      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_36B0      proc    near
                push    esi
                mov     esi,dword ptr [esp+8]
                mov     eax,dword ptr [esi+8]
                push    eax
                call    sub_a_1DE0
                cli                             ; Disable interrupts
                mov     ecx,esi
                call    sub_a_36A0
                mov     eax,dword ptr [esi+44h]
                test    eax,eax
                jnz     short loc_a_3713        ; Jump if not zero
                inc     eax
                mov     dword ptr [esi+44h],eax
                sti                             ; Enable interrupts
                mov     ecx,dword ptr [esi+40h]
                push    ecx
                push    dword ptr 0Ah
                push    dword ptr 0
                call    sub_a_2930
                add     esp,0Ch
                mov     eax,dword ptr [esi+10h]
                inc     dword ptr [esi+40h]
                test    eax,eax
                jz      short loc_a_36F5        ; Jump if zero
                push    eax
                call    sub_a_0980
                add     esp,4
loc_a_36F5::
                mov     eax,dword ptr [esi+0Ch]
                cmp     dword ptr [eax+4],0
                je      short loc_a_3710        ; Jump if equal
                mov     dword ptr [eax+4],0
                mov     eax,[eax]
                push    eax                     ; PARAMETER_1
                call    sub_a_3A08
                add     esp,4
loc_a_3710::
                dec     dword ptr [esi+44h]
loc_a_3713::
                sti                             ; Enable interrupts
                mov     eax,1
                pop     esi
                retn
sub_a_36B0      endp

                db      0CCh,0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3720      proc    near
                mov     eax,dword ptr [esp+4]
                push    eax                     ; PARAMETER_1
                call    sub_a_38EB
                add     esp,4
                retn
sub_a_3720      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                mov     eax,dword ptr [esp+4]
                push    eax                     ; PARAMETER_1
                call    sub_a_38FE
                add     esp,4
                retn
                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3740      proc    near
                mov     eax,dword ptr [esp+4]
                push    esi
                mov     esi,ecx
                push    dword ptr 0
                push    36B0h
                push    eax
                push    esi
                call    sub_a_3590
                mov     eax,dword ptr [esi+14h]
                pop     esi
                retn    4
sub_a_3740      endp

                db      0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3760      proc    near
                push    esi
                push    15B3h
                mov     esi,ecx
                push    dword ptr 46h
                push    dword ptr 4
                call    sub_a_2930
                add     esp,0Ch
                mov     ecx,esi
                call    sub_a_3780
                mov     eax,esi
                pop     esi
                retn
sub_a_3760      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3780      proc    near
                xor     eax,eax                 ; Zero register
                mov     dword ptr [ecx+8],eax
                mov     dword ptr [ecx+10h],eax
                mov     dword ptr [ecx+14h],eax
                mov     dword ptr [ecx+40h],eax
                mov     word ptr [ecx+22h],ax
                mov     dword ptr [ecx+44h],eax
                mov     dword ptr [ecx+18h],eax
                mov     dword ptr [ecx+24h],eax
                mov     dword ptr [ecx+28h],eax
                mov     dword ptr [ecx+2Ch],eax
                mov     dword ptr [ecx+30h],eax
                mov     dword ptr [ecx+34h],eax
                mov     dword ptr [ecx+38h],1F4h
                mov     word ptr [ecx+20h],10h
                retn
sub_a_3780      endp

                db      0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_37B8      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+

                push    ebp
                mov     ebp,esp
                push    esi
                push    ebx
                push    edi
                mov     eax,1000h
                mov     ecx,dword ptr [ebp+PARAMETER_1]
                mov     edi,dword ptr [ebp+PARAMETER_2]
                VMMcall Get_Cur_VM_Handle       ; Win-VMM function   fn=10001h
                VxDcall SHELL_SYSMODAL_Message  ; Win-SHELL Service  fn=170003h
                pop     edi
                pop     ebx
                pop     esi
                leave                           ; Procedure exit
                retn
sub_a_37B8      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    esi
                push    ebx
                push    edi
                mov     eax,1001h
                mov     ecx,dword ptr [ebp+8]
                mov     edi,dword ptr [ebp+0Ch]
                VMMcall Get_Cur_VM_Handle       ; Win-VMM function   fn=10001h
                VxDcall SHELL_SYSMODAL_Message  ; Win-SHELL Service  fn=170003h
                pop     edi
                pop     ebx
                pop     esi
                leave                           ; Procedure exit
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_37FC      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+

                push    ebp
                mov     ebp,esp
                VxDcall VTD_Get_Real_Time       ; Win-VTD Service    fn=50007h
                mov     ecx,dword ptr [ebp+PARAMETER_1]
                mov     [ecx],eax
                mov     dword ptr [ecx+4],edx
                leave                           ; Procedure exit
                retn
sub_a_37FC      endp

                                                ;* No entry point to code
                VMMcall Get_Last_Updated_System_Time    ; Win-VMM function   fn=100CFh
                xor     edx,edx                 ; Zero register
                mov     ecx,36Fh
                shl     eax,4                   ; Shift w/zeros fill
                div     ecx                     ; ax,dx rem=dx:ax/reg
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3822      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                VxDcall VPICD_Physically_Mask   ; Win-Debug Service  fn=30008h
                leave                           ; Procedure exit
                retn
sub_a_3822      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3830      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                VxDcall VPICD_Physically_Unmask ; Win-Debug Service  fn=30009h
                leave                           ; Procedure exit
                retn
sub_a_3830      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_383E      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                push    edi
                mov     edi,dword ptr [ebp+PARAMETER_1]
                VxDcall DEBUG_Fault             ; Win-Debug Service  fn=30001h
                jnc     short loc_a_3852        ; Jump if carry=0
                mov     eax,0FFFFFFFFh
loc_a_3852::
                pop     edi
                leave                           ; Procedure exit
                retn
sub_a_383E      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    si
                mov     eax,dword ptr [ebp+8]
                mov     esi,dword ptr [ebp+0Ch]
                VMMcall Hook_v86_Int_Chain      ; Win-VMM function   fn=10041h
                mov     eax,0
                adc     eax,0
                pop     si
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    si
                mov     eax,dword ptr [ebp+8]
                mov     esi,dword ptr [ebp+0Ch]
                VMMcall Unhook_V86_Int_Chain    ; Win-VMM function   fn=10118h
                mov     eax,0
                adc     eax,0
                pop     si
                leave                           ; Procedure exit
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_388F      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                VxDcall VPICD_Phys_EOI          ; Win-Debug Service  fn=30004h
                leave                           ; Procedure exit
                retn
sub_a_388F      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_389D      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                VxDcall VPICD_Force_Default_Behavior    ; Win-Debug Service  fn=30011h
                leave                           ; Procedure exit
                retn
sub_a_389D      endp

                                                ;* No entry point to code
                pushad                          ; Save all regs
                cmp     dword ptr data_a_0000,0
                jne     short loc_a_38E9        ; Jump if not equal
                push    dword ptr 0
                push    10000h
                push    0A0000h
                VMMcall _MapPhysToLinear        ; Win-VMM function   fn=1006Ch
                add     esp,0Ch
                mov     dword ptr data_a_0000,eax
                push    dword ptr 0
                push    1000h
                push    0B8000h
                VMMcall _MapPhysToLinear        ; Win-VMM function   fn=1006Ch
                add     esp,0Ch
                mov     data_a_0698,eax
loc_a_38E9::
                popad                           ; Restore all regs
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_38EB      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                push    dword ptr 0
                push    dword ptr [ebp+PARAMETER_1]
                VMMcall _HeapAllocate           ; Win-VMM function   fn=1004Fh
                add     esp,8
                leave                           ; Procedure exit
                retn
sub_a_38EB      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_38FE      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                push    dword ptr 0
                push    dword ptr [ebp+PARAMETER_1]
                VMMcall _HeapFree               ; Win-VMM function   fn=10051h
                add     esp,8
                leave                           ; Procedure exit
                retn
sub_a_38FE      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3911      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+
PARAMETER_3     =       10h                     ; ebp+

                push    ebp
                mov     ebp,esp
                mov     edx,dword ptr [ebp+PARAMETER_1]
                mov     eax,edx
                add     edx,dword ptr [ebp+PARAMETER_2]
                shr     eax,0Ch                 ; Shift w/zeros fill
                shr     edx,0Ch                 ; Shift w/zeros fill
                sub     edx,eax
                inc     edx
                push    dword ptr [ebp+PARAMETER_3]
                push    edx
                push    eax
                VMMcall _LinPageLock            ; Win-VMM function   fn=10063h
                add     esp,0Ch
                leave                           ; Procedure exit
                retn
sub_a_3911      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3935      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+
PARAMETER_3     =       10h                     ; ebp+

                push    ebp
                mov     ebp,esp
                mov     edx,dword ptr [ebp+PARAMETER_1]
                mov     eax,edx
                add     edx,dword ptr [ebp+PARAMETER_2]
                shr     eax,0Ch                 ; Shift w/zeros fill
                shr     edx,0Ch                 ; Shift w/zeros fill
                sub     edx,eax
                inc     edx
                push    dword ptr [ebp+PARAMETER_3]
                push    edx
                push    eax
                VMMcall _LinPageUnlock          ; Win-VMM function   fn=10064h
                add     esp,0Ch
                leave                           ; Procedure exit
                retn
sub_a_3935      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                mov     edx,dword ptr [ebp+8]
                mov     eax,edx
                add     edx,dword ptr [ebp+0Ch]
                shr     eax,0Ch                 ; Shift w/zeros fill
                mov     ecx,dword ptr [ebp+10h]
                shr     edx,0Ch                 ; Shift w/zeros fill
                sub     edx,eax
                inc     edx
                mov     [ecx],edx
                push    40000000h
                push    edx
                push    eax
                VMMcall _LinPageLock            ; Win-VMM function   fn=10063h
                add     esp,0Ch
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+8]
                shr     eax,0Ch                 ; Shift w/zeros fill
                push    40000000h
                push    dword ptr [ebp+0Ch]
                push    eax
                VMMcall _LinPageUnlock          ; Win-VMM function   fn=10064h
                add     esp,0Ch
                leave                           ; Procedure exit
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_39A1      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+
PARAMETER_3     =       10h                     ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_2]
                shr     eax,0Ch                 ; Shift w/zeros fill
                inc     eax
                push    dword ptr 8
                push    dword ptr 0
                push    dword ptr 0
                push    dword ptr 0
                push    dword ptr 0
                push    dword ptr 0
                push    dword ptr 1
                push    eax
                VMMcall _PageAllocate           ; Win-VMM function   fn=10053h
                add     esp,20h
                mov     ecx,eax
                or      ecx,edx
                jz      short loc_a_39D0        ; Jump if zero
                mov     ecx,dword ptr [ebp+PARAMETER_1]
                mov     [ecx],eax
                mov     eax,edx
loc_a_39D0::
                leave                           ; Procedure exit
                retn
sub_a_39A1      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_39D2      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                push    dword ptr 0
                push    dword ptr [ebp+PARAMETER_1]
                VMMcall _PageFree               ; Win-VMM function   fn=10055h
                add     esp,8
                leave                           ; Procedure exit
                retn
sub_a_39D2      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_39E5      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     ecx,dword ptr [ebp+PARAMETER_1]
                VMMcall Create_Semaphore        ; Win-VMM function   fn=10025h
                jnc     short loc_a_39F8        ; Jump if carry=0
                mov     eax,0FFFFFFFFh
loc_a_39F8::
                leave                           ; Procedure exit
                retn
sub_a_39E5      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_39FA      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                VMMcall Destroy_Semaphore       ; Win-VMM function   fn=10026h
                leave                           ; Procedure exit
                retn
sub_a_39FA      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3A08      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                VMMcall Signal_Semaphore        ; Win-VMM function   fn=10028h
                leave                           ; Procedure exit
                retn
sub_a_3A08      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3A16      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                mov     ecx,10h
                VMMcall Wait_Semaphore          ; Win-VMM function   fn=10027h
                leave                           ; Procedure exit
                retn
sub_a_3A16      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    edi
                mov     edi,dword ptr [ebp+8]
                VMMcall Remove_Mult_IO_Handlers ; Win-VMM function   fn=10117h
                pop     edi
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    edi
                mov     edi,dword ptr [ebp+8]
                VMMcall Install_Mult_IO_Handlers        ; Win-VMM function   fn=10095h
                mov     eax,0
                jnc     short loc_a_3A4F        ; Jump if carry=0
                mov     eax,edx
loc_a_3A4F::
                pop     edi
                leave                           ; Procedure exit
                retn

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3A52      proc    near

PARAMETER_1     =       8                       ; ebp+
PARAMETER_2     =       0Ch                     ; ebp+

                push    ebp
                mov     ebp,esp
                push    esi
                mov     esi,dword ptr [ebp+PARAMETER_2]
                mov     edx,dword ptr [ebp+PARAMETER_1]
                VMMcall Install_IO_Handler      ; Win-VMM function   fn=10096h
                mov     eax,0
                jnc     short loc_a_3A6C        ; Jump if carry=0
                mov     eax,dword ptr [ebp+PARAMETER_1]
loc_a_3A6C::
                pop     esi
                leave                           ; Procedure exit
                retn
sub_a_3A52      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3A6F      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     edx,dword ptr [ebp+PARAMETER_1]
                VMMcall Remove_IO_Handler       ; Win-VMM function   fn=10116h
                leave                           ; Procedure exit
                retn
sub_a_3A6F      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3A7D      proc    near

PARAMETER_1     =       8                       ; ebp+

                push    ebp
                mov     ebp,esp
                mov     eax,dword ptr [ebp+PARAMETER_1]
                mov     data_a_0000,ax
                leave                           ; Procedure exit
                retn
sub_a_3A7D      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_a_3A8B      proc    near
                mov     ax,data_a_0000
                shl     ax,1                    ; Shift w/zeros fill
                jnc     short loc_a_3A9A        ; Jump if carry=0
                xor     ax,100Bh
loc_a_3A9A::
                mov     data_a_0000,ax
                cdwe                            ; Word to double word
                retn
sub_a_3A8B      endp

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    ebx
                mov     eax,dword ptr [ebp+8]
                mov     ecx,0A0h
                mul     ecx                     ; dx:ax = reg * ax
                mov     edx,dword ptr [ebp+10h]
                add     eax,dword ptr [ebp+0Ch]
                add     eax,dword ptr [ebp+0Ch]
                mov     dh,3
                add     eax,dword ptr data_a_0698
                add     dl,30h                  ; '0'
                mov     [eax],dx
                pop     ebx
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    ebx
                mov     eax,dword ptr [ebp+8]
                mov     ecx,640h
                mul     ecx                     ; dx:ax = reg * ax
                add     eax,dword ptr [ebp+0Ch]
                mov     ebx,eax
                add     ebx,dword ptr data_a_0000
                mov     ecx,2
                lea     edx,dword ptr data_a_0000       ; Load effective addr
                mov     eax,dword ptr [ebp+10h]
                shl     eax,4                   ; Shift w/zeros fill
                add     edx,eax

locloop_a_3AF6::
                mov     ax,[edx]
                mov     [ebx],al
                mov     byte ptr [ebx+50h],ah
                mov     ax,word ptr [edx+2]
                mov     byte ptr data_a_00A0[ebx],al
                mov     byte ptr data_a_00F0[ebx],ah
                mov     ax,word ptr [edx+4]
                mov     byte ptr data_a_0140[ebx],al
                mov     byte ptr data_a_0190[ebx],ah
                mov     ax,word ptr [edx+6]
                mov     byte ptr data_a_01E0[ebx],al
                mov     byte ptr data_a_0230[ebx],ah
                add     edx,8
                add     ebx,offset32 data_a_0280
                loopd   locloop_a_3AF6          ; Loop if ecx > 0

                pop     ebx
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    ebx
                push    esi
                push    edi
                mov     eax,dword ptr [ebp+8]
                mov     ecx,2800h
                mul     ecx                     ; dx:ax = reg * ax
                mov     ebx,dword ptr [ebp+0Ch]
                shl     ebx,3                   ; Shift w/zeros fill
                add     eax,ebx
                mov     ebx,eax
                add     ebx,dword ptr data_a_0000
                lea     edx,dword ptr data_a_0000       ; Load effective addr
                mov     eax,dword ptr [ebp+10h]
                shl     eax,4                   ; Shift w/zeros fill
                add     edx,eax
                mov     edi,edx
                add     edi,10h
loc_a_3B6F::
                xor     eax,eax                 ; Zero register
                mov     ecx,8
                mov     al,[edx]
loc_a_3B78::
                rol     al,1                    ; Rotate
                mov     esi,eax
                and     esi,1
                mov     ah,byte ptr data_a_069C[esi]
                mov     [ebx],ah
                inc     ebx
                dec     ecx
                jnz     loc_a_3B78              ; Jump if not zero
                inc     edx
                cmp     edx,edi
                jae     short loc_a_3B98        ; Jump if above or =
                add     ebx,278h
                jmp     short loc_a_3B6F
loc_a_3B98::
                pop     edi
                pop     esi
                pop     ebx
                leave                           ; Procedure exit
                retn
                db      0CCh,0CCh,0CCh, 00h

seg_a           ends



;------------------------------------------------------------  seg_c   ----

seg_c           segment para public use32


                org     10000h

                db      'Cnt:            Port:           '
                db      'Irq:             '
                db      0, 0, 0, 0

seg_c           ends



;------------------------------------------------------------  seg_d   ----

seg_d           segment para public use32
                assume cs:seg_d  , ds:seg_a


                org     20000h

                                                ;* No entry point to code
                push    ebp
                mov     ebp,esp
                push    dword ptr [ebp+8]
                VxDcall IOS_Register            ; Block Device/IOS   fn=100007h
                add     esp,4
                leave                           ; Procedure exit
                retn
                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                sbb     al,byte ptr [edi+53h][ecx*2]
                xor     edi,[eax]
                and     byte ptr ss:[ecx+72h],al
;*              jbe     short loc_d_0089        ;*Jump if below or =
                db       76h, 69h               ;  Fixup - byte match
                db      'd device driver'
                db      0
loc_d_0030::
                add     [ecx],cl
                inc     ecx
                push    esi
                inc     esp
                cmp     dword ptr ds:d_4000_445F_e,esi
                add     [eax],eax
                db      17410 dup (0)

seg_d           ends



;------------------------------------------------------------  seg_b   ----

seg_b           segment para public use32
                assume cs:seg_b  , ds:seg_a


                org     0E0000h

                                                ;* No entry point to code
                call    sub_a_2930
                mov     eax,dword ptr [esp+4]
                push    eax
                call    sub_b_0160
                add     esp,4
                retn    4
                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0020      proc    near
                push    esi
                mov     eax,20202020h
                mov     esi,dword ptr [esp+8]
                push    edi
                mov     edi,dword ptr [esp+10h]
                mov     [edi],eax
                mov     dword ptr [edi+4],eax
                add     edi,0Ah
                mov     word ptr [edi-2],ax
loc_b_003B::
                mov     ecx,0Ah
                mov     eax,esi
                sub     edx,edx
                dec     edi
                div     ecx                     ; ax,dx rem=dx:ax/reg
                add     dl,30h                  ; '0'
                mov     eax,esi
                mov     byte ptr [edi+1],dl
                sub     edx,edx
                div     ecx                     ; ax,dx rem=dx:ax/reg
                mov     esi,eax
                test    esi,esi
                jnz     loc_b_003B              ; Jump if not zero
                pop     edi
                pop     esi
                retn    8
sub_b_0020      endp

                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0060      proc    near
                mov     ecx,dword ptr [esp+8]
                push    ebx
                mov     dword ptr [ecx],20202020h
                mov     dword ptr [ecx+4],20202020h
                add     ecx,8
                mov     eax,dword ptr [esp+8]
loc_b_0079::
                mov     edx,eax
                mov     bl,37h                  ; '7'
                and     dl,0Fh
                cmp     dl,0Ah
                jge     short loc_b_0087        ; Jump if > or =
                mov     bl,30h                  ; '0'
loc_b_0087::
                shr     eax,4                   ; Shift w/zeros fill
                add     dl,bl
                mov     [ecx],dl
                dec     ecx
                test    eax,eax
                jnz     loc_b_0079              ; Jump if not zero
                pop     ebx
                retn    8
sub_b_0060      endp

                db      9 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_00A0      proc    near
                mov     eax,dword ptr [esp+4]
                push    10004h
                push    eax
                call    sub_b_0020
                mov     eax,dword ptr [esp+8]
                push    10015h
                push    eax
                call    sub_b_0060
                mov     eax,dword ptr [esp+0Ch]
                push    10024h
                push    eax
                call    sub_b_0020
                mov     eax,dword ptr [esp+10h]
                push    eax                     ; PARAMETER_2
                push    10000h                  ; PARAMETER_1
                call    sub_a_37B8
                add     esp,8
                retn
sub_b_00A0      endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_00E0      proc    near
                mov     ax,data_a_0000
;*              cmp     ax,2
                db       3Dh, 02h, 00h, 73h     ;  Fixup - byte match
                or      byte ptr [esi+40h],ah
                mov     data_a_0000,ax

loc_ret_b_00F4::
                retn
sub_b_00E0      endp

                db      11 dup (0CCh)
loc_b_0100::
                lea     eax,dword ptr [ecx+54h] ; Load effective addr
                mov     dword ptr data_a_00D4[ecx],ecx
                mov     dword ptr data_a_00CC[ecx],eax
                mov     dword ptr data_a_0114[ecx],eax
                lea     eax,dword ptr data_a_00C4[ecx]  ; Load effective addr
                test    ecx,ecx
                jnz     short loc_b_0121        ; Jump if not zero
                xor     eax,eax                 ; Zero register
loc_b_0121::
                mov     dword ptr data_a_0118[ecx],eax
                lea     eax,dword ptr data_a_010C[ecx]  ; Load effective addr
                mov     dword ptr data_a_00D0[ecx],eax
                retn
                db      12 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0140      proc    near
                xor     eax,eax                 ; Zero register
                mov     word ptr [ecx+4],ax
                mov     byte ptr [ecx+8],al
                mov     [ecx],ax
                mov     word ptr [ecx+2],ax
                mov     word ptr [ecx+6],ax
                jmpn    loc_b_0100
sub_b_0140      endp

                db      7 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0160      proc    near
                push    esi
                mov     eax,0
                test    eax,eax
                jz      short loc_b_0173        ; Jump if zero
                mov     word ptr data_a_0000,0
loc_b_0173::
                mov     esi,8
loc_b_0178::
                test    esi,esi
                jz      short loc_b_0183        ; Jump if zero
                mov     ecx,esi
                call    sub_b_01A0
loc_b_0183::
                mov     ecx,esi
                add     esi,238h
                call    sub_b_0140
                cmp     esi,478h
                jb      loc_b_0178              ; Jump if below
                call    sub_a_1530
                pop     esi
                retn
sub_b_0160      endp

                db      0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_01A0      proc    near
                push    ebx
                push    esi
                push    edi
                xor     ebx,ebx                 ; Zero register
                push    ebp
                mov     ebp,ecx
                mov     word ptr [ebp+10h],bx
                mov     word ptr [ebp+12h],bx
                mov     byte ptr [ebp+8],bl
                mov     word ptr [ebp+4],bx
                lea     ecx,dword ptr ss:data_a_00C4[ebp]       ; Load effective addr
                mov     word ptr [ebp+0],bx
                mov     word ptr [ebp+2],bx
                mov     word ptr [ebp+6],bx
                call    sub_a_3760
                push    ebx                     ; PARAMETER_1
                mov     dword ptr ss:data_a_0110[ebp],ebx
                call    sub_a_39E5
                add     esp,4
                lea     ecx,dword ptr ss:data_a_0114[ebp]       ; Load effective addr
                mov     dword ptr ss:data_a_010C[ebp],eax
                push    ebx
                call    sub_a_2920
                mov     dword ptr ss:data_a_0134[ebp],ebx
                mov     dword ptr ss:data_a_0140[ebp],ebx
                push    dword ptr 1
                mov     dword ptr ss:data_a_013C[ebp],ebx
                push    ebx
                lea     ecx,dword ptr ss:data_a_0150[ebp]       ; Load effective addr
                mov     dword ptr ss:data_a_0144[ebp],3Ch
                lea     edi,dword ptr ss:data_a_0198[ebp]       ; Load effective addr
                mov     dword ptr ss:data_a_0148[ebp],ebx
                mov     dword ptr ss:data_a_0138[ebp],4B690h
                mov     esi,13h
                call    sub_a_1750
                mov     dword ptr ss:data_a_018C[ebp],ebx
                mov     dword ptr ss:data_a_0190[ebp],ebx
                mov     dword ptr ss:data_a_0194[ebp],0FFFFFFFFh
loc_b_024A::
                mov     ecx,edi
                add     edi,8
                call    sub_a_0600
                dec     esi
                jns     loc_b_024A              ; Jump if not sign
                mov     eax,ebp
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn
sub_b_01A0      endp

                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0260      proc    near
                mov     edx,dword ptr [ecx+38h]
                push    esi
                push    edi
                mov     eax,ecx
                add     edx,edx
                mov     esi,dword ptr [esp+0Ch]
                mov     ecx,edx
                mov     edi,dword ptr [eax+4]
                shr     ecx,2                   ; Shift w/zeros fill
                repe    cmpsd                   ; Rep zf=1+cx >0 Cmp [si] to es:[di]
                jnz     short loc_b_0280        ; Jump if not zero
                mov     ecx,edx
                and     ecx,3
                repe    cmpsb                   ; Rep zf=1+cx >0 Cmp [si] to es:[di]
loc_b_0280::
                mov     eax,1
                jz      short loc_b_0289        ; Jump if zero
                xor     eax,eax                 ; Zero register
loc_b_0289::
                pop     edi
                pop     esi
                retn    4
sub_b_0260      endp

                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0290      proc    near
                push    esi
                mov     eax,dword ptr [ecx+3Ch]
                imul    eax,[ecx+40h]           ; reg = reg * mem
                push    edi
                test    eax,eax
                jz      short loc_b_02B2        ; Jump if zero
                mov     dx,word ptr [esp+0Ch]
                xor     esi,esi                 ; Zero register
loc_b_02A4::
                mov     edi,dword ptr [ecx+4]
                add     esi,2
                mov     word ptr [edi-2][esi],dx
                dec     eax
                jnz     loc_b_02A4              ; Jump if not zero
loc_b_02B2::
                pop     edi
                pop     esi
                retn    4
sub_b_0290      endp

                db      9 dup (0CCh)
                db      0B8h, 01h, 00h, 00h, 00h,0C2h
                db       04h, 00h
                db      8 dup (0CCh)
                db       33h,0C0h,0C2h, 04h, 00h
                db      11 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_02E0      proc    near
                mov     eax,dword ptr [esp+4]
                push    ebx
                push    esi
                dec     eax
                push    edi
                mov     esi,ecx
                push    ebp
                cmp     eax,0Ch
                mov     ecx,dword ptr [esi+4]
                ja      short loc_b_02FA        ; Jump if above
                jmp     dword ptr data_b_0408[eax*4]    ;*4 entries
loc_b_02FA::
                xor     eax,eax                 ; Zero register
                jmp     loc_b_03C7

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_b_0301::
                mov     eax,0FFFFh;Loop check ? - UV
                jmp     loc_b_03C7

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_b_030B::
                mov     eax,0AAAAh;Loop check ? - UV
                jmp     loc_b_03C7

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_b_0315::
                mov     eax,5555h;Loop check ? - UV
                jmp     loc_b_03C7

;ƒƒƒƒƒ Indexed Entry Point ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ

loc_b_031F::
                push    dword ptr 1             ; PARAMETER_1
                call    sub_a_3A7D
                add     esp,4
                mov     eax,dword ptr [esi+3Ch]
                xor     ebx,ebx                 ; Zero register
                imul    eax,[esi+40h]           ; reg = reg * mem
                lea     edi,dword ptr data_a_0000[eax*2]        ; Load effective addr
                test    edi,edi
                jz      short loc_b_034E        ; Jump if zero
loc_b_033D::
                inc     ebx
                call    sub_a_3A8B
                mov     ecx,dword ptr [esi+4]
                cmp     edi,ebx
                mov     byte ptr [ecx-1][ebx],al
                ja      loc_b_033D              ; Jump if above
loc_b_034E::
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
                                                ;* No entry point to code
                mov     edx,dword ptr [esi+3Ch]
                xor     eax,eax                 ; Zero register
                imul    edx,[esi+40h]           ; reg = reg * mem
                test    edx,edx
                jz      loc_b_034E              ; Jump if zero
loc_b_0362::
                mov     [ecx][eax*2],ax
                inc     eax
                cmp     eax,edx
                jb      loc_b_0362              ; Jump if below
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
                                                ;* No entry point to code
                mov     eax,3333h;Loop check ? - UV
                jmp     short loc_b_03C7
                                                ;* No entry point to code
                mov     eax,0EEEEh;Loop check ? - UV
                jmp     short loc_b_03C7
                                                ;* No entry point to code
                xor     eax,eax                 ; Zero register
                cmp     dword ptr [esi+40h],eax
                jbe     loc_b_034E              ; Jump if below or =
                xor     dx,dx                   ; Zero register
loc_b_038A::
                xor     edi,edi                 ; Zero register
                cmp     dword ptr [esi+3Ch],edi
                jbe     short loc_b_03A6        ; Jump if below or =
loc_b_0391::
                lea     ebx,[edx][edi]          ; Load effective addr
                mov     ebp,dword ptr [esi+3Ch]
                imul    ebp,eax                 ; reg = reg * reg
                add     ebp,edi
                inc     edi
                mov     [ecx][ebp*2],bx
                cmp     dword ptr [esi+3Ch],edi
                ja      loc_b_0391              ; Jump if above
loc_b_03A6::
                movzx   edx,dx                  ; Mov w/zero extend
                add     dx,100h
                inc     eax
                cmp     dword ptr [esi+40h],eax
                ja      loc_b_038A              ; Jump if above
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
                                                ;* No entry point to code
                mov     eax,0F0F0h; Loop check in - UV
                jmp     short loc_b_03C7
                db      0B8h,0C3h,0C3h, 00h, 00h ; Loop check out - UV
loc_b_03C7::
                push    eax
                mov     ecx,esi
                call    sub_b_0290
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_b_02E0      endp

                                                ;* No entry point to code
                mov     edi,dword ptr [esi+3Ch]
                xor     edx,edx                 ; Zero register
                imul    edi,[esi+40h]           ; reg = reg * mem
                test    edi,edi
                jz      loc_b_034E              ; Jump if zero
loc_b_03E7::
                mov     eax,edx
                and     eax,1
                cmp     eax,1
                mov     eax,0
                adc     eax,0FFFFFFFFh
                inc     edx
                mov     word ptr [ecx-2][edx*2],ax
                cmp     edx,edi
                jb      loc_b_03E7              ; Jump if below
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn    4
data_b_0408     dd      offset32 loc_b_0301
data_b_040C     dd      offset32 loc_b_030B
data_b_0410     dd      offset32 loc_b_0315
data_b_0414     dd      offset32 loc_b_031F
                db       00h, 00h, 00h, 00h, 55h, 03h
                db       0Eh, 00h, 00h, 00h, 00h, 00h
                db       72h, 03h, 0Eh, 00h, 79h, 03h
                db       0Eh, 00h, 80h, 03h, 0Eh, 00h
                db      0BBh, 03h, 0Eh, 00h,0C2h, 03h
                db       0Eh, 00h,0D6h, 03h, 0Eh, 00h
                db      0CCh,0CCh,0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0440      proc    near
                push    ebx
                push    esi
                push    edi
                xor     ebx,ebx                 ; Zero register
                mov     dword ptr [ecx+8],ebx
                mov     dword ptr [ecx],0
                mov     edi,dword ptr [esp+10h]
                mov     esi,ecx
                lea     eax,dword ptr data_a_005A[edi*2]        ; Load effective addr
                mov     dword ptr [esi+38h],edi
                mov     dword ptr [esi+3Ch],8
                mov     dword ptr [esi+40h],100h
                push    eax
                call    sub_a_3720
                add     esp,4
                mov     dword ptr [esi+4],eax
loc_b_0478::
                mov     eax,dword ptr [esi+4]
                add     ebx,2
                mov     word ptr [eax-2][ebx],2AAAh
                cmp     ebx,10h
                jb      loc_b_0478              ; Jump if below
                lea     eax,dword ptr [edi+8]   ; Load effective addr
                lea     ecx,dword ptr [edi+9]   ; Load effective addr
                cmp     eax,ecx
                jae     short loc_b_04B0        ; Jump if above or =
                lea     ecx,dword ptr data_a_0000[eax*2]        ; Load effective addr
                sub     edi,eax
                add     edi,9
loc_b_04A0::
                mov     eax,dword ptr [esi+4]
                add     ecx,2
                mov     word ptr [eax-2][ecx],2AAAh
                dec     edi
                jnz     loc_b_04A0              ; Jump if not zero
loc_b_04B0::
                lea     edi,dword ptr [esi+0Ch] ; Load effective addr
                xor     eax,eax                 ; Zero register
                add     dword ptr [esi+4],10h
                mov     ecx,0Ah
                rep     stosd                   ; Rep when cx >0 Store ax to es:[di]
                mov     eax,esi
                pop     edi
                pop     esi
                pop     ebx
                retn    4
sub_b_0440      endp

                db      8 dup (0CCh)
                db       56h, 8Bh,0F1h,0E8h, 18h, 00h
                db       00h, 00h,0F6h, 44h, 24h, 08h
                db       01h, 74h, 09h, 56h,0E8h, 00h
                db       00h, 00h, 00h, 83h,0C4h, 04h
                db       8Bh,0C6h, 5Eh,0C2h, 04h, 00h
                db      0CCh,0CCh
loc_b_04F0::
                mov     eax,dword ptr [ecx+4]
                mov     dword ptr [ecx],0
                sub     eax,10h
                push    eax
                call    sub_b_0502

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0502      proc    near
                add     esp,4
                retn
sub_b_0502      endp

                db      10 dup (0CCh)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0510      proc    near
                push    esi
                push    1000h
                mov     esi,ecx
                call    sub_b_0440
                mov     eax,esi
                mov     dword ptr [esi],0
                mov     dword ptr [esi+3Ch],8
                mov     dword ptr [esi+40h],200h
                pop     esi
                retn
sub_b_0510      endp

                db      11 dup (0CCh)
                db       56h, 8Bh,0F1h,0E8h, 18h, 00h
                db       00h, 00h,0F6h, 44h, 24h, 08h
                db       01h, 74h, 09h, 56h,0E8h, 00h
                db       00h, 00h, 00h, 83h,0C4h, 04h
                db       8Bh,0C6h, 5Eh,0C2h, 04h, 00h
                db      0CCh,0CCh

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0560      proc    near
                mov     dword ptr [ecx],0
                jmpn    loc_b_04F0
sub_b_0560      endp

                                                ;* No entry point to code
                int     3                       ; Debug breakpoint
                VxDjmp  ?? undocumented sub-function    ; Vonfig Manager     fn=33803Eh
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_b_0574      proc    near
                VxDjmp  _CONFIGMG_Get_Res_Des_Data      ; Vonfig Manager     fn=338022h
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_b_057C::
                VxDjmp  _CONFIGMG_Get_Res_Des_DataSize  ; Vonfig Manager     fn=338021h
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                VxDjmp  _CONFIGMG_Get_Next_Res_Des      ; Vonfig Manager     fn=33801Fh
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_b_058C::
                VxDjmp  _CONFIGMG_Get_First_Log_Conf    ; Vonfig Manager     fn=33801Ah
                int     3                       ; Debug breakpoint
                int     3                       ; Debug breakpoint
                VxDjmp  _CONFIGMG_Locate_DevNode        ; Vonfig Manager     fn=338002h
                db      0
sub_b_0574      endp


seg_b           ends



                end
