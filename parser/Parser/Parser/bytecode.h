#pragma once
enum ByteCode
{
	iconst_0      = 0x03,
	iconst_1      = 0x04,
	bipush        = 0x10,
	ldc_w         = 0x13,
	iload         = 0x15,
	aload         = 0x19,
	return_       = 0xb1,
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
	ineg		  = 0x74
};
