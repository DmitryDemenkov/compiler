#pragma once
enum ByteCode
{
	iconst_0      = 0x03,
	iconst_1      = 0x04,
	bipush        = 0x10,
	ldc_w         = 0x13,
	iload         = 0x15,
	aload         = 0x19,
	pop           = 0x57,
	dup           = 0x59,
	dup_x1        = 0x5a,
	dup_x2        = 0x5b,
	new_          = 0xbb,
	newarray      = 0xbc,
	anewarray	  = 0xbd,
	arraylength   = 0xbe,
	aastore       = 0x53,
	iastore       = 0x4f,
	aaload        = 0x32,
	iaload        = 0x2e,
	ireturn       = 0xac,
	areturn       = 0xb0,
	return_       = 0xb1,
	getstatic     = 0xb2,
	putstatic	  = 0xb3,
	getfield      = 0xb4,
	putfield      = 0xb5,
	invokevirtual = 0xb6,
	invokespecial = 0xb7, 
	invokestatic  = 0xb8,
	istore        = 0x36,
	astore        = 0x3a,
	iadd          = 0x60,
	isub		  = 0x64,
	imul		  = 0x68,
	idiv		  = 0x6c,
	irem		  = 0x70,
	ineg		  = 0x74,
	ifeq          = 0x99,
	ifne          = 0x9a,
	if_icmpeq     = 0x9f,
	if_icmpne     = 0xa0,
	if_icmplt     = 0xa1,
	if_icmpge     = 0xa2,
	if_icmpgt     = 0xa3,
	if_icmple     = 0xa4,
	goto_         = 0xa7
};
