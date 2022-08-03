/*
 * asm.c
 *
 *  Created on: 2021. 4. 28.
 *      Author: kjm-hsh
 */



void asm_rst(void)
{
    __asm("    MOV R0, #0x0\n"
          "    LDR SP, [R0]\n"
          "    MOV R0, #0x4\n"
          "    LDR PC, [R0]");
}
