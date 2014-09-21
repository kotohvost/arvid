sub_a_1BD3	proc	near
	enter	6,0
	push	si
/* структура */
	mov	ax,[bp+4]
/* последний выведенный бит */
	mov	si,[bp+6]
/* код */
	mov	bx,[bp+0Ah]
	xor	cx,cx
/* модуляция */
	mov	ax,[bp+8]
	xor	dx,dx
	call	DxAxMulCxBx		; (8E9C)
	mov	[bp-2],dx
	mov	[bp-4],ax
/* номер бита */
	or	si,si
	jz	short loc_a_1C13
/* добавим паузу в слово */
	mov	bx,si
	shl	bx,1
/* установленные биты */
	mov	ax,IRCOn_00D0[bx]	; (8C1E:00D0=0)
	les	bx,dword ptr [bp+0Ch]
/* произвести коррекцию */
	and	es:[bx],ax
/* следующее слово */
	add	word ptr [bp+0Ch],2
	mov	ax,10h
	sub	ax,si
	sub	[bp-4],ax
	sbb	word ptr [bp-2],0
loc_a_1C13:
/* пауза */
	mov	dx,[bp-2]
	mov	ax,[bp-4]
	mov	cl,4
	call	sub_a_8DA5
/* /16 */
	mov	[bp-6],ax
/* биты есть ? */
	test	word ptr [bp-4],0Fh
	jz	short loc_a_1C2D
/* да +2 байта */
	mov	ax,2
	jmp	short loc_a_1C2F

loc_a_1C2D:
	xor	ax,ax
loc_a_1C2F:
/* в словах */
	mov	dx,[bp-6]
/* в байтах */
	shl	dx,1
/* плюс для битов */
	add	dx,ax
/* количество */
	push	dx
/* заполнитель */
	push	0
/* буфер */
	push	word ptr [bp+0Eh]
	push	word ptr [bp+0Ch]
/* заполнить побайтно */
	callf	sub_a_A323
	add	sp,8
	xor	ax,ax
	pop	si
	leave
	retn
sub_a_1BD3	endp
