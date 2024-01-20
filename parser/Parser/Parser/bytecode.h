#pragma once
enum ByteCode
{
	ldc_w         = 0x13,
	aload         = 0x19,
	return_       = 0xb1,
	invokevirtual = 0xb6,
	invokespecial = 0xb7, 
	invokestatic  = 0xb8
};
