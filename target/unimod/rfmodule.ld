MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}
_crc_offset = 0x1000; /* CRC is located at 0x08005000 */
/*Edited by RF-Module - Eclipse lives in the Debug/Release directory so ../ is needed*/
/*Can't work out if/how you can change this in Eclipse with the ARM Plugin*/
INCLUDE ../target/common/devo/devo.ld
