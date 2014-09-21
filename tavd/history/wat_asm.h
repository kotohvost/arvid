//void bout(short,unsigned char);
#pragma aux bout parm [dx][al]=\
 "out dx,al";

//void bout(short,short);
#pragma aux wout parm [dx][ax]=\
 "out dx,ax";

//unsigned char bin (short);
#pragma aux bin parm [dx]=\
"xor eax,eax"\
"in  al,dx"\
value [eax];

//unsigned short win(short);
#pragma aux win parm [dx]=\
"in  ax,dx"\
value [ax];

//void stosb(char *adr,int lenght);
#pragma aux stos parm [edi][edx]=\
"xor eax,eax	 "\
"mov ecx,edx	 "\
"shr ecx,2	 "\
"repne stosd	 "\
"mov cl,dl	 "\
"and cl,3	 "\
"repne stosb	 "\
modify [edi eax ecx];

//void stosb(char *adr,int lenght,int byte_);
#pragma aux stosb parm [edi][ecx][eax]=\
"repne stosb"\
modify [edi ecx];
