/****************************************************************************
 * The linker command script
 *  Created on: 04.06.2018
 *      Author: Georgi Angelov
 * -Xlinker --defsym=_RO_BASE_=0x40000000 or 0x43000000
 ****************************************************************************/

PHDRS { APP PT_LOAD; }

SECTIONS
{    
    . = _RO_BASE_;
    .text :
    {
        KEEP( *( .preamble ) );
        KEEP( *( .library ) ); /* QAPI LIBRARIES */
        PROVIDE_HIDDEN ( __preinit_array_start = . );
        KEEP( *(SORT(.preinit_array.*)) );
        KEEP( *(.preinit_init_array*) );
        PROVIDE_HIDDEN ( __preinit_array_end = . );
        PROVIDE_HIDDEN ( __init_array_start = . );
        KEEP( *(SORT(.init_array.*)) );
        KEEP( *(.init_array*) );
        PROVIDE_HIDDEN ( __init_array_end = . );
        PROVIDE_HIDDEN ( __fini_array_start = . );
        KEEP( *(SORT(.fini_array.*)) );
        KEEP( *(.fini_array*) );
        PROVIDE_HIDDEN ( __fini_array_end = . );  
        *( .text .text.* i.* )
        *( .glue_7t )
        *( .glue_7 )
        *( .ctors )
        *( .dtors )
        *( .gnu.linkonce.t.* )
        *( .gnu.linkonce.r.* )
        *( .gcc_except_table )
        *( .rodata .rodata* .constdata* .conststring*  ) 	                                
    } : APP
    
    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } : APP
    PROVIDE_HIDDEN( __exidx_start = . );    
	.ARM.exidx : { *( .ARM.exidx* .gnu.linkonce.armexidx.* ) } : APP
    PROVIDE_HIDDEN( __exidx_end = . );    

    . = ALIGN(8);   
    PROVIDE_HIDDEN( _RO_BASE_END_ = . ); 
    PROVIDE_HIDDEN(  _etext = . );
    PROVIDE_HIDDEN( __etext = . );    
	PROVIDE( IMAGE_ER_RO_LENGTH = _RO_BASE_END_ - _RO_BASE_ );

/*--------------------------------------------------------------------*/	

    .data ALIGN(8) : 
    {
        . = ALIGN(8);
        PROVIDE_HIDDEN( _DATA_BEGIN_ = . );
        KEEP( *( .data .data.* ) );
        . = ALIGN(8);
        PROVIDE_HIDDEN( _DATA_END_ = . );
    } : APP   
    PROVIDE( IMAGE_ER_RW_LENGTH = _DATA_END_ - _DATA_BEGIN_ );

/*--------------------------------------------------------------------*/

    .bss ALIGN(8) : 
    {
        . = ALIGN(8);
        PROVIDE_HIDDEN( _BSS_BEGIN_ = . );
        *( .bss .bss.* .common COMMON )
        . = ALIGN(8);  
        PROVIDE_HIDDEN( _BSS_END_ = . );                    
    } : APP    
    PROVIDE( IMAGE_ER_ZI_ZI_LENGTH = _BSS_END_ - _BSS_BEGIN_ );

/*--------------------------------------------------------------------*/
    
    PROVIDE (     end = . ); 
    PROVIDE (    _end = . );
    PROVIDE (   __end = . );
    PROVIDE ( __end__ = . );
}

