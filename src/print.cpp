#include <nitro.h>
#include "core.h"
#include "print.h"

#include <nitro/code32.h>

static asm void print_asm_end();

static asm void print_asm()
{
	mov r12, r12
	b print_asm_end
}

//This looks like a function, but it actually a trick to allocate room for the
//debug string data
static asm void msgdata()
{
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
}

static asm void print_asm_end()
{
	bx lr
}

#include <nitro/codereset.h>

void NOCASH_Printf(char* msg, ...)
{
	va_list argptr;
	va_start(argptr, msg);

	uint8_t* p = (uint8_t*)&msgdata;
	((uint16_t*)p)[0] = 0x6464;
	((uint16_t*)p)[1] = 0;
	p += 4;
	OS_VSPrintf((char*)p, msg, argptr);
	print_asm();
	va_end(va);
}