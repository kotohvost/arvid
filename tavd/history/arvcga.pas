Program arvid;
{ Copyright 1998 Mike Jangolenko jangolen@infopro.spb.su}
Uses CRT{,graph};

Const
     aport=$250;
     bit : array[1..22] of byte =(31,0,0,0,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff,0,0);
Var i,j : integer;
      n,x,y : integer;
      a0,a1,a2,a3,a4,a5,a6,a7 : byte;
 grDriver: Integer;
 grMode: Integer;
 ErrCode: Integer;


BEGIN
{  grDriver :=1{ detect};
{ InitGraph(grDriver, grMode,'C:\BP\BGI');
} asm
{    mov AL,$35
    push ax}
    mov ax,$0010
    int $10
  end;

 {ErrCode := GraphResult;
 if ErrCode = grOk then
 begin
 end
 else
   Writeln('Graphics error:', GraphErrorMsg(ErrCode));

   {запись экранов}

  portw[aport+4]:=$11;
{  for i:=0 to $7f do begin
      portw[aport+6]:=i;
      FOR j:=0 to $ff do begin portw[aport]:=$0000; readkey;end;
                  end;

  for i:=0 to 50*1 do begin
                       while (port[aport+4] and $07)>6 do;
                       portw[aport+4]:=$09;
                       readkey;
                       end;


  {прием}

  n:=0;
  portw[aport+4]:=$95;
     while not keypressed do
               begin
               for i:=0 to 10 do
                   begin
                   portw[aport+6]:=i;
                   for j:=0 to $ff do
                       begin
                       n:=(i)*$100+j;
                       y:=n div 9;
                       x:=(n- y*9)*$8;
                       n:=(y{ div 2})*$50+{2000*(y and $1}+(x);

{                       bit[6]:=port[aport+2];}

{                       a:=port[aport+2];}
                       asm
                          mov DX,$252
                          in  AL,DX

                          xor AH,AH
                          rcl AL,1
                          jnc @l1
                          or  AH,$0f
@l1:                      rcl AL,1
                          jnc @l2
                          or  AH,$f0
@l2:                      mov [a0],AH

                          xor AH,AH
                          rcl AL,1
                          jnc @l3
                          or  AH,$0f
@l3:                      rcl AL,1
                          jnc @l4
                          or  AH,$f0
@l4:                      mov [a1],AH

                          xor AH,AH
                          rcl AL,1
                          jnc @l5
                          or  AH,$0F
@l5:                      rcl AL,1
                          jnc @l6
                          or  AH,$f0
@l6:                      mov [a2],AH

                          xor AH,AH
                          rcl AL,1
                          jnc @l7
                          or  AH,$0f
@l7:                      rcl AL,1
                          jnc @l8
                          or  AH,$f0
@l8:                      mov [a3],AH

                          mov DX,$251
                          in  AL,DX

                          xor AH,AH
                          rcl AL,1
                          jnc @m1
                          or  AH,$0f
@m1:                      rcl AL,1
                          jnc @m2
                          or  AH,$f0
@m2:                      mov [a4],AH

                          xor AH,AH
                          rcl AL,1
                          jnc @m3
                          or  AH,$0f
@m3:                      rcl AL,1
                          jnc @m4
                          or  AH,$f0
@m4:                      mov [a5],AH

                          xor AH,AH
                          rcl AL,1
                          jnc @m5
                          or  AH,$0f
@m5:                      rcl AL,1
                          jnc @m6
                          or  AH,$f0
@m6:                      mov [a6],AH

                          xor AH,AH
                          rcl AL,1
                          jnc @m7
                          or  AH,$0f
@m7:                      rcl AL,1
                          jnc @m8
                          or  AH,$f0
@m8:                      mov [a7],AH


                       end;
{                       bit[5]:=a3;
                       bit[6]:=a2;
                       bit[7]:=a1;
                       bit[8]:=a0;
                       bit[9]:=a7;
                       bit[10]:=a6;
                       bit[11]:=a5;
                       bit[12]:=a4;
                       putimage(x,y,bit,normalput);}
                       mem[$a000:(n)]:=a3;
                       mem[$a000:(n+1)]:=a2;
                       mem[$a000:(n+2)]:=a1;
                       mem[$a000:(n+3)]:=a0;
                       mem[$a000:(n+4)]:=a7;
                       mem[$a000:(n+5)]:=a6;
                       mem[$a000:(n+6)]:=a5;
                       mem[$a000:(n+7)]:=a4;

                       portw[aport+4]:=$73;
                       end;
                   end;
               end;
   {CloseGraph;}
 asm
{    mov AL,$35
    push ax}
    mov ax,$0002
    int $10
  end;

END.
