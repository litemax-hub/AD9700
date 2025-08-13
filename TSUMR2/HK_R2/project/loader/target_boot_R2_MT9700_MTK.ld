ENTRY(__reset)

/* kernel memory pool reserved for the use of malloc by kernel ONLY */
KERNEL_POOL_SIZE = 40000; /* 256KB */

/*
PM_R2 
345MHz (for local dimming, Leona 300MHz)
code run on SPI. IQMEM 0KB, DQMEM 160KB, ICACHE 8KB, no DCACHE.
SRAM-base IMI 8KB (Leger2 8KB, TX x1, RX x1)
4K與AP溝通+4K給SECU運算用(RX x1), no port fast switching.
*/

RAM_START = 0x000000;
RAM_SIZE = 0x00200000;

MEMORY
{
		ram  : ORIGIN = 0x00000000, LENGTH = 0x00200000
		sram : ORIGIN = 0xB0000000, LENGTH = 0x4000
		imi : ORIGIN = 0x90000000, LENGTH = 0xEE400
}

SECTIONS
{
    .vectors : { . = .; KEEP(*(.reset)) KEEP(*(.vectors)) } > ram
    .interp ALIGN (0x8) : { *(.interp) } > ram
    .hash : AT ((LOADADDR (.interp) + SIZEOF (.interp) + 8 - 1) & ~ (8 - 1)) { *(.hash) } > ram
    .dynsym : AT ((LOADADDR (.hash) + SIZEOF (.hash) + 8 - 1) & ~ (8 - 1)) { *(.dynsym) } > ram
    .dynstr : AT ((LOADADDR (.dynsym) + SIZEOF (.dynsym) + 8 - 1) & ~ (8 - 1)) { *(.dynstr) } > ram
    .gnu.version : AT ((LOADADDR (.dynstr) + SIZEOF (.dynstr) + 8 - 1) & ~ (8 - 1)) { *(.gnu.version) } > ram
    .gnu.version_d : AT ((LOADADDR (.gnu.version) + SIZEOF (.gnu.version) + 8 - 1) & ~ (8 - 1)) { *(.gnu.version_d) } > ram
    .gnu.version_r : AT ((LOADADDR (.gnu.version_d) + SIZEOF (.gnu.version_d) + 8 - 1) & ~ (8 - 1)) { *(.gnu.version_r) } > ram
    .plt : AT ((LOADADDR (.gnu.version_r) + SIZEOF (.gnu.version_r) + 8 - 1) & ~ (8 - 1)) { *(.plt) } > ram
    .rel.text : { *(.rel.text) *(.rel.text.*) *(.rel.gnu.linkonce.t*) } > ram
    .rela.text : { *(.rela.text) *(.rela.text.*) *(.rela.gnu.linkonce.t*) } > ram
    .rel.data : { *(.rel.data) *(.rel.data.*) *(.rel.gnu.linkonce.d*) } > ram
    .rela.data : { *(.rela.data) *(.rela.data.*) *(.rela.gnu.linkonce.d*) } > ram
    .rel.rodata : { *(.rel.rodata) *(.rel.rodata.*) *(.rel.gnu.linkonce.r*) } > ram
    .rela.rodata : { *(.rela.rodata) *(.rela.rodata.*) *(.rela.gnu.linkonce.r*) } > ram
    .rel.got : { *(.rel.got) } > ram .rela.got : { *(.rela.got) } > ram
    .rel.ctors : { *(.rel.ctors) } > ram
    .rela.ctors : { *(.rela.ctors) } > ram
    .rel.dtors : { *(.rel.dtors) } > ram
    .rela.dtors : { *(.rela.dtors) } > ram
    .rel.init : { *(.rel.init) } > ram
    .rela.init : { *(.rela.init) } > ram
    .rel.fini : { *(.rel.fini) } > ram
    .rela.fini : { *(.rela.fini) } > ram
    .rel.bss : { *(.rel.bss) } > ram
    .rela.bss : { *(.rela.bss) } > ram
    .rel.plt : { *(.rel.plt) } > ram
    .rela.plt : { *(.rela.plt) } > ram
    .rel.dyn : { *(.rel.dyn) } > ram
    .boot_data ALIGN (0x1000): { . = .; *(.boot_data) } > ram /* for system_init data */
    /* 
    . = ALIGN(8192); 
    */
    __readonly_start = .;
    .init ALIGN (0x8) : { . = .; KEEP (*(.init)) } > ram = 0
    .fini ALIGN (0x4) : { . = .; KEEP (*(.fini)) } > ram =0
    .text ALIGN(0x10):
    {
        __stext = .; *(.boot_from_spi_start.*);
        *(EXCLUDE_FILE(*Platform.o *aes.o *asn1parse.o *asn1write.o *bignum.o *ctr_drbg.o *entropy.o *entropy_poll.o *md.o *md_wrap.o *md5.o *oid.o *padlock.o *ripemd160.o *rsa.o *sha1.o *sha256.o *sha512.o *timing.o *aescrypt2.o *rsa_verify.o) .text*)
        *(.gnu.warning) *(.gnu.linkonce.t.*)
    } > ram
    . = ALIGN(0x10); 
    __etext = .; PROVIDE (etext = .);

    __IMI_lma = .;
	.IMI :
    AT ( (LOADADDR(.text) + SIZEOF(.text) + (16-1)) & ~(16-1) )
	{
        __IMI_start = .;
		KEEP(*(.IMI*));
	    . = ALIGN(0x10);
       *Platform.o(.text* .rodata*) *aes.o(.text* .rodata*) *asn1parse.o(.text* .rodata*) *asn1write.o(.text* .rodata*) *bignum.o(.text* .rodata*) *ctr_drbg.o(.text* .rodata*) *entropy.o(.text* .rodata*) *entropy_poll.o(.text* .rodata*) *md.o(.text* .rodata*) *md_wrap.o(.text* .rodata*) *md5.o(.text* .rodata*) *oid.o(.text* .rodata*) *padlock.o(.text* .rodata*) *ripemd160.o(.text* .rodata*) *rsa.o(.text* .rodata*) *sha1.o(.text* .rodata*) *sha256.o(.text* .rodata*) *sha512.o(.text* .rodata*) *timing.o(.text* .rodata*) *aescrypt2.o(.text* .rodata*) *rsa_verify.o(.text* .rodata*)
        __IMI_end = .;
	} > imi 
	
    . = (LOADADDR(.IMI) + SIZEOF(.IMI) + (16-1)) & ~(16-1) ;
    .rodata1 ALIGN (0x8) : { . = .; *(.rodata1) *(.rodata1*) } > ram
    .rodata ALIGN (0x8) :
    {
        . = .;
        *(EXCLUDE_FILE(*Platform.o *aes.o *asn1parse.o *asn1write.o *bignum.o *ctr_drbg.o *entropy.o *entropy_poll.o *md.o *md_wrap.o *md5.o *oid.o *padlock.o *ripemd160.o *rsa.o *sha1.o *sha256.o *sha512.o *timing.o *rsa_verify.o) .rodata*)
        *(.gnu.linkonce.r*)
    } > ram
    . = ALIGN(0x10); __readonly_end = .;
    .tspdat ALIGN (0x10) : { . = .; *(.tspdat) } > ram
    .fixup ALIGN (0x4) : { __FIXUP_START__ = ABSOLUTE(.); *(.fixup) __FIXUP_END__ = ABSOLUTE(.);} > ram
    .gcc_except_table ALIGN (0x4) : { __EXCEPT_START__ = ABSOLUTE(.); *(.gcc_except_table) __EXCEPT_END__ = ABSOLUTE(.);} > ram
    .eh_frame ALIGN (0x8) : { . = .; __EH_FRAME_BEGIN__ = .; KEEP(*(.eh_frame)) __FRAME_END__ = .; . = . + 8; } > ram = 0
    .ctors ALIGN (0x8) : { . = .; KEEP (*crtbegin.o(.ctors)) KEEP (*_ctors.o(.ctors)) ___CTOR_LIST__ = .; PROVIDE (__CTOR_LIST__ = .); KEEP (*(.ctors)); KEEP (*(SORT(.ctors.*))) ___CTOR_END__ = .; PROVIDE (___CTOR_END__ = .); } > ram
    .dtors ALIGN (0x8) : { . = .; KEEP (*crtbegin.o(.dtors)) __DTOR_LIST__ = .; PROVIDE (__DTOR_LIST__ = .); KEEP (*(SORT(.dtors.*))) __DTOR_END__ = .; KEEP (*(.dtors)); PROVIDE (__DTOR_END__ = .); } > ram
    .devtab ALIGN (0x8) : { . = .; KEEP(*( SORT (.ecos.table.*))) ; } > ram
    .got ALIGN (0x8) : { . = .; *(.got.plt) *(.got*) } > ram
    .dynamic ALIGN (0x8) : { . = .; *(.dynamic) } > ram
    .bin ALIGN (0x8) : { . = .; *(bin.data) } > ram

/*
    __rom_data_start = (LOADADDR(.IMI) + SIZEOF(.IMI) + (16-1)) & ~(16-1) ;
    __rom_data_end   = __readonly_end;
*/
    . = ALIGN(0x10); 
    ___data_start_s = .; /*data start - source address*/
    .data : AT(___data_start_s) { ___data_start_d = ABSOLUTE(.); *(.data*) *(.data.*) *(.gnu.linkonce.d*) *( .2ram.*) . = ALIGN (8); SORT(CONSTRUCTORS) } > sram
    .sdata ALIGN (0x8) : AT(LOADADDR(.data) + SIZEOF(.data)) { __SDATA_START__ = ABSOLUTE(.); . = .; *(.sdata) *(.sdata.*) *(.gnu.linkonce.s*) } > sram
    .lit8 ALIGN (0x8) : AT(LOADADDR(.sdata) + SIZEOF(.sdata)) { . = .; *(.lit8) } > sram
    ___data_end_s = LOADADDR(.lit8) + SIZEOF(.lit8);    

    /* put sbss/bss together for reset.s to initialize to 0 */
    .sbss ALIGN (0x8) (NOLOAD) : { __sbss_start = ABSOLUTE (.); __SBSS_START__ = ABSOLUTE(.); *(.sbss.*) __SBSS_END__ = ABSOLUTE(.); __SBSSx_START__ = ABSOLUTE(.); *(.sbss*) __SBSSx_END__ = ABSOLUTE(.); *(.scommon*) __sbss_end = ABSOLUTE (.); } > sram
    .bss ALIGN (0x10) (NOLOAD) : { __bss_start = ABSOLUTE (.); . = .; *(.dynbss*) *(.bss) *(.bss.*) *(COMMON) . = ALIGN(0x10); __bss_end = ABSOLUTE (.); } > sram

     	__text_length = .;

        /* for sbrk.c */
        . = ALIGN (0x8);
        ___heap = .; /*___heap_end = RAM_START + RAM_SIZE;*/
        ___heap_end = ___heap + 0x100;
        
	. = ALIGN(0x8);
	.tmp : AT(LOADADDR(.bss) + SIZEOF(.bss))
	{
		KEEP(*(.qmem))
	} > sram

    . = LOADADDR(.tmp) + SIZEOF(.tmp);
    __boot_end = ALIGN(0x6000);
    .padding : AT(__boot_end - 8)
    {
        _MY_MARK = .; LONG(0xAABBCCDD) LONG(0x0)
    } > ram

	_RISC32_DCACHE_SIZE = 8192;
	_RISC32_DCACHE_LINE_SIZE = 16;
	_RISC32_DCACHE_WAYS = 4;

    PROVIDE(_boot_stack     = 0xB000C000);    /* t2:  0xC0001000 */
    PROVIDE(_risc32_regbase = 0xA0201E00);    /* T2: 0xA0001E00 ; T3:0xA0201E00  */
    PROVIDE(___qmem_addr      = 0xB0000000);    /*for mvf QMemory*/
}
