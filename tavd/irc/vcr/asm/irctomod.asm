sub_a_1602	proc	near
		enter	0Ah,0
		push	si
		push	di
/* 010E вывод ИК */
		mov	di,[bp+4]
		jmp	loc_a_1704
loc_a_160E:
/* 1 */
/* start - 0 */
		cmp	word ptr [di+2Eh],0
		je	short loc_a_1624
		mov	word ptr [di+2Eh],0
/* 3000 */
		mov	ax,0BB8h
		xor	dx,dx
		div	word ptr [di+20h]
		mov	[di+1Eh],ax
loc_a_1624:
/* 0 */
		cmp	word ptr [di+1Eh],0
		jne	short loc_a_163B
/* пакеты/паузы */
		les	bx,dword ptr [di+1Ah]
		add	word ptr [di+1Ah],2
/* код из буфера */
	mov	ax,es:[bx]
	mov	[di+1Eh],ax
/* флаг паузы ? */
	xor	word ptr [di+14h],1
loc_a_163B:
/* конец последовательности ? */
		cmp	word ptr [di+1Eh],0
		jne	short loc_a_1649
/* да - закончить преобразование */
	mov	word ptr [di+12h],1
	jmp	loc_a_170D
/* нет - продолжим преобразование */
loc_a_1649:
/* расчитаем количество бит для кода */
/* bx - код */
	mov	bx,[di+1Eh]
	xor	cx,cx
/* модуляция */
	mov	ax,[di+20h]
	xor	dx,dx
	call	DxAxMulCxBx		; (8E9C)
/* модуляция * код */
	mov	[bp-2],dx
	mov	[bp-4],ax
/* сравним с имеющимся пространством */
	mov	ax,[di+24h]
	mov	dx,[di+22h]
	cmp	ax,[bp-2]
	ja	short loc_a_167E
	jc	short loc_a_166E
	cmp	dx,[bp-4]
	jae	short loc_a_167E
loc_a_166E:
/* посчитать для свободного места */
	push	0
/* модуляция */
	push	word ptr [di+20h]
/* оставшийся размер */
	push	word ptr [di+24h]
	push	word ptr [di+22h]
	call	sub_a_8CDD
	jmp	short loc_a_1681
/* место хватает */
loc_a_167E:
/* код */
	mov	ax,[di+1Eh]
loc_a_1681:
/* запомним */
	mov	[bp-0Ah],ax
/* пауза/пакет ? */
	cmp	word ptr [di+14h],0
	je	short loc_a_169B
/* 1 - пауза */
/* ds:buffer - для вывода */
	push	ds
	push	word ptr [di+2Ah]
/* код */
	push	ax
/* модуляция */
	push	word ptr [di+20h]
/* последний выведенный бит */
	push	word ptr [di+28h]
	push	di
	call	sub_a_1BD3
	jmp	short loc_a_16D1

/* пакет */
loc_a_169B:
/* таблица загружена ? */
	cmp	VCRTabl_184,0		; (8C1E:1128=0)
	je	short loc_a_16AD

	mov	bx,VCRTabl_184		; (8C1E:1128=0)
/* тип модуляции */
	cmp	word ptr 0B4h[bx],0	; (8C1E:00B4=0FFFCh)
	jne	short loc_a_16C0

/* отсутствует таблица или модулированный сигнал */
loc_a_16AD:
/* ds:buffer - для вывода */
	push	ds
	push	word ptr [di+2Ah]
/* код */
	push	word ptr [bp-0Ah]
/* модуляция */
	push	word ptr [di+20h]
/* последний выведенный бит */
	push	word ptr [di+28h]
/* структура */
	push	di
	call	IRCToMod		; (1B35)
	jmp	short loc_a_16D1

/* немодулированный сигнал */
loc_a_16C0:
	push	ds
	push	word ptr [di+2Ah]
	push	word ptr [bp-0Ah]
	push	word ptr [di+20h]
	push	word ptr [di+28h]
	push	di
	call	IRCToNoMod		; (1ACA)
/* общий выход после преобразования сигнала */
loc_a_16D1:
	add	sp,0Ch
/* вычесть код из кода */
	mov	ax,[bp-0Ah]
	sub	[di+1Eh],ax
/* подсчитать и вычеслить оставшееся место в буфере */
	mov	bx,[bp-0Ah]
	xor	cx,cx
	mov	ax,[di+20h]
	xor	dx,dx
	call	DxAxMulCxBx		; (8E9C)
	mov	[bp-6],dx
	mov	[bp-8],ax
	sub	[di+22h],ax
	sbb	[di+24h],dx
/* произведем коррекцию указателя буфера */
	push	dx
	push	ax
	push	di
	call	sub_a_1A94
	add	sp,6
/* проверить на окончание буфера */
	mov	ax,[di+22h]
	or	ax,[di+24h]
	jz	short loc_a_170D
loc_a_1704:
/* 1 - 0 */
		cmp	word ptr [di+12h],0
		jne	short loc_a_170D
		jmp	loc_a_160E
/* место кончилось */
loc_a_170D:
		cmp	word ptr [di+12h],3
		jae	short loc_a_175B
		mov	ax,[di+22h]
		or	ax,[di+24h]
		jz	short loc_a_1741
		cmp	word ptr [di+34h],0
		jne	short loc_a_1745
		push	ds
		push	word ptr [di+2Ah]
		push	0
		push	word ptr [di+20h]
		push	word ptr [di+24h]
		push	word ptr [di+22h]
		call	sub_a_8CDD
		push	ax
		push	word ptr [di+20h]
		push	word ptr [di+28h]
		push	di
		call	sub_a_1BD3
		add	sp,0Ch
loc_a_1741:
		push	di
		call	sub_a_176E
loc_a_1745:
		cmp	word ptr [di+12h],0
		je	short loc_a_175B
		cmp	word ptr [di+34h],0
		jne	short loc_a_1756
		inc	word ptr [di+12h]
		jmp	short loc_a_175B
loc_a_1756:
		mov	word ptr [di+12h],3
loc_a_175B:
		cmp	word ptr [di+12h],3
		jne	short loc_a_1766
		mov	ax,1
		jmp	short loc_a_1768
loc_a_1766:
		xor	ax,ax
loc_a_1768:
		pop	di
		pop	si
		leave
		retn	2
sub_a_1602	endp


IRCToNoMod	proc	near
		push	bp
		mov	bp,sp
		push	si
		push	di
		push	bx
		push	di
		push	si
		push	es
		push	bp
		cld
		mov	dx,[bp+8]
		les	di,dword ptr [bp+0Ch]
		mov	cx,[bp+6]
		mov	ax,[bp+0Ah]
		mov	bp,ax
		or	cx,cx
		jz	short loc_a_1B04
		cmp	cx,10h
		jb	short loc_a_1AF4
		add	di,2
		sub	cx,10h
		jz	short loc_a_1B04
loc_a_1AF4:
		and	cx,0Fh
		mov	bx,cx
		shl	bx,1
		mov	ax,IRCOff_00B0[bx]	; (8C1E:00B0=0FFFFh)
		stosw
		sub	cx,10h
loc_a_1B04:
		add	cx,dx
		mov	ax,cx
		shr	ax,4
		jz	short loc_a_1B18
loc_a_1B0D:
		mov	word ptr es:[di],0FFFFh
		add	di,2
		dec	ax
		jnz	loc_a_1B0D
loc_a_1B18:
		and	cx,0Fh
		dec	bp
		jnz	loc_a_1B04
		or	cx,cx
		jz	short loc_a_1B2C
		mov	bx,cx
		shl	bx,1
		mov	ax,IRCOn_00D0[bx]	; (8C1E:00D0=0)
		stosw
loc_a_1B2C:
		pop	bp
		pop	es
		pop	si
		pop	di
		pop	bx
		pop	di
		pop	si
		pop	bp
		retn
IRCToNoMod	endp

IRCToMod	proc	near
		push	bp
		mov	bp,sp
		push	si
		push	di
		push	bx
		push	di
		push	si
		push	es
		push	bp
		cld
		mov	dx,[bp+8]
		les	di,dword ptr [bp+0Ch]
		mov	cx,[bp+6]
		mov	ax,[bp+0Ah]
		mov	bp,ax
		mov	dh,dl
		shr	dh,1
		sub	dl,dh
		or	cx,cx
		jz	short loc_a_1B75
		cmp	cx,10h
		jb	short loc_a_1B65
		add	di,2
		sub	cx,10h
		jz	short loc_a_1B75
loc_a_1B65:
		and	cx,0Fh
		mov	bx,cx
		shl	bx,1
		mov	ax,IRCOff_00B0[bx]	; (8C1E:00B0=0FFFFh)
		stosw
		sub	cx,10h
loc_a_1B75:
		add	cl,dl
		adc	ch,0
		mov	ax,cx
		shr	ax,4
		jz	short loc_a_1B8C
loc_a_1B81:
		mov	word ptr es:[di],0FFFFh
		add	di,2
		dec	ax
		jnz	loc_a_1B81
loc_a_1B8C:
		and	cx,0Fh
		mov	bx,cx
		jz	short loc_a_1B9E
		shl	bx,1
		mov	ax,IRCOn_00D0[bx]	; (8C1E:00D0=0)
		stosw
		sub	cx,10h
loc_a_1B9E:
		add	cl,dh
		adc	ch,0
		mov	ax,cx
		shr	ax,4
		jz	short loc_a_1BB5
loc_a_1BAA:
		mov	word ptr es:[di],0
		add	di,2
		dec	ax
		jnz	loc_a_1BAA
loc_a_1BB5:
		and	cx,0Fh
		mov	bx,cx
		jz	short loc_a_1BC7
		shl	bx,1
		mov	ax,IRCOff_00B0[bx]	; (8C1E:00B0=0FFFFh)
		stosw
		sub	cx,10h
loc_a_1BC7:
		dec	bp
		jnz	loc_a_1B75
		pop	bp
		pop	es
		pop	si
		pop	di
		pop	bx
		pop	di
		pop	si
		pop	bp
		retn
IRCToMod	endp


IRCOff_00B0	dw	0FFFFh, 0FFFEh, 0FFFCh	; Data table (indexed access)
		dw	0FFF8h, 0FFF0h, 0FFE0h
		dw	0FFC0h, 0FF80h, 0FF00h
		dw	0FE00h, 0FC00h, 0F800h
		dw	0F000h, 0E000h, 0C000h
		dw	8000h
IRCOn_00D0	dw	0, 1, 3			; Data table (indexed access)
		dw	7, 0Fh, 1Fh
		dw	3Fh, 7Fh, 0FFh
		dw	1FFh, 3FFh, 7FFh
		dw	0FFFh, 1FFFh, 3FFFh
		dw	7FFFh
		db	0
