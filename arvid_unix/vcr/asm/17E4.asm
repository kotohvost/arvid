sub_a_17E4	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
/* [VCRTabl_184 + 038h] - ������ � ��� (001Ah - Akai) */
	mov	dx,[bp+0Ah]
/* [VCRTabl_184 + 0B4h] - ��� ��������� */
	mov	cx,[bp+8]
/* 0 */
	mov	di,[bp+6]
/* 10Eh - ��������� */
	mov	si,[bp+4]
/* ��� ��������� */
	or	cx,cx
	jz	short loc_a_1809
/* ���������������� ������ */
	mov	ax,dx
	imul	ax,ax,154h
/* ������ * 340 */
	mov	bx,0EEh
	xor	dx,dx
	div	bx
/* ������ * 340 / 238 */
	inc	ax
/* +1 */
	jmp	short loc_a_180F

/* �������������� ������ */
loc_a_1809:
	mov	ax,dx
	imul	ax,ax,3
/* ������ * 3 */
	inc	ax
/* +1 */
/* �������� */
loc_a_180F:
	mov	[si+20h],ax
/* ������ 0 */
	or	di,di
	jz	short loc_a_181D
		or	cx,cx
		jnz	short loc_a_181D
		mov	[si+20h],di
loc_a_181D:
	push	si
/* (������ ����� / (����������� * 12)) * (����������� * 12) = 3792 [si+08h] */
	call	sub_a_1A74
	pop	cx
/* ��������� ��������� ������ ��� ������ �� */
	push	word ptr [bp+0Eh]
	push	word ptr [bp+0Ch]
	push	si
	call	SetBufInIRC		; (1832)
	pop	di
	pop	si
	pop	bp
	retn	0Ch
sub_a_17E4	endp
