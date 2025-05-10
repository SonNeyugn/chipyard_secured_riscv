#ifndef _ENV_PHYSICAL_SINGLE_CORE_H
#define _ENV_PHYSICAL_SINGLE_CORE_H

#include "/home/nguyesh/risc_v/chipyard/toolchains/riscv-tools/riscv-tests/env/encoding.h"

//-----------------------------------------------------------------------
// Inline Assembly Macros for C
//-----------------------------------------------------------------------

#define RVTEST_RV64U() asm volatile("" ::: "memory")

#define RVTEST_RV64UF() asm volatile( \
    "csrs mstatus, %[fp]\n\t"         \
    "csrwi fcsr, 0\n\t"               \
    :                                 \
    : [fp] "i" (MSTATUS_FS & (MSTATUS_FS >> 1)) \
    : "memory")

#define RVTEST_RV64UV() asm volatile( \
    "csrs mstatus, %[vec]\n\t"        \
    "csrwi fcsr, 0\n\t"               \
    "csrwi vcsr, 0\n\t"               \
    :                                 \
    : [vec] "i" ((MSTATUS_VS & (MSTATUS_VS >> 1)) | (MSTATUS_FS & (MSTATUS_FS >> 1))) \
    : "memory")

#define CHECK_XLEN() asm volatile(    \
    "li a0, 1\n\t"                    \
    "slli a0, a0, 31\n\t"              \
    "bgez a0, 1f\n\t"                  \
    "j pass\n\t"                       \
    "1:"                               \
    ::: "a0", "memory")

#define INIT_XREG() asm volatile(     \
    "li x1, 0\n\t"                    \
    "li x2, 0\n\t"                    \
    "li x3, 0\n\t"                    \
    "li x4, 0\n\t"                    \
    "li x5, 0\n\t"                    \
    "li x6, 0\n\t"                    \
    "li x7, 0\n\t"                    \
    "li x8, 0\n\t"                    \
    "li x9, 0\n\t"                    \
    "li x10, 0\n\t"                   \
    "li x11, 0\n\t"                   \
    "li x12, 0\n\t"                   \
    "li x13, 0\n\t"                   \
    "li x14, 0\n\t"                   \
    "li x15, 0\n\t"                   \
    "li x16, 0\n\t"                   \
    "li x17, 0\n\t"                   \
    "li x18, 0\n\t"                   \
    "li x19, 0\n\t"                   \
    "li x20, 0\n\t"                   \
    "li x21, 0\n\t"                   \
    "li x22, 0\n\t"                   \
    "li x23, 0\n\t"                   \
    "li x24, 0\n\t"                   \
    "li x25, 0\n\t"                   \
    "li x26, 0\n\t"                   \
    "li x27, 0\n\t"                   \
    "li x28, 0\n\t"                   \
    "li x29, 0\n\t"                   \
    "li x30, 0\n\t"                   \
    "li x31, 0\n\t"                   \
    ::: "memory")

// #define INIT_PMP() asm volatile(      \
//     "csrw mtvec, %[handler]\n\t"       \
//     "li t0, -1\n\t"                    \
//     "csrw pmpaddr1, t0\n\t"            \
//     "li t0, 0x1F\n\t"                   \
//     "slli t0, t0, 8\n\t"               \
//     "csrw pmpcfg0, t0\n\t"             \
//     :                                  \
//     : [handler] "r" (trap_handler)     \
//     : "t0", "memory")

#define INIT_PMP() asm volatile(      \
  "li t0, 0x1FFFFFFFFFFFFF\n\t"      \
  "csrw pmpaddr1, t0\n\t"            \
  "li t0, 0x1F\n\t"                   \
  "slli t0, t0, 8\n\t"               \
  "csrw pmpcfg0, t0\n\t"             \
  ::: "memory")  

#define DELEGATE_NO_TRAPS() asm volatile( \
  "csrwi mie, 0\n\t"                   \
  "la t0, 1f\n\t"                   \
  "csrw mtvec, t0\n\t"               \
  "csrwi medeleg, 0\n\t"               \
  "csrwi mideleg, 0\n\t"               \
  "1:\n\t"                           \
  ::: "t0", "memory")

#define TESTNUM gp
#define TRAP_VECTOR() asm volatile( \
  "csrr t5, mcause\n\t"            \
  "li t6, %[cause_user]\n\t"        \
  "beq t5, t6, write_tohost\n\t"   \
  "li t6, %[cause_super]\n\t"      \
  "beq t5, t6, write_tohost\n\t"   \
  "li t6, %[cause_machine]\n\t"    \
  "beq t5, t6, write_tohost\n\t"   \
  "la t5, mtvec_handler\n\t"       \
  "beqz t5, 1f\n\t"                 \
  "jr t5\n\t"                      \
  "1: csrr t5, mcause\n\t"         \
  "bgez t5, handle_exception\n\t"  \
  "handle_exception:\n\t"         \
  "write_tohost:\n\t"             \
  :                                    \
  : [cause_user] "i" (CAUSE_USER_ECALL), [cause_super] "i" (CAUSE_SUPERVISOR_ECALL), [cause_machine] "i" (CAUSE_MACHINE_ECALL) \
  : "t5", "t6", "memory")

#define ENABLE_SUPERVISOR() asm volatile( \
    "li a0, %[mstatus_mpp]\n\t"        \
    "csrs mstatus, a0\n\t"             \
    "li a0, %[sip_ssip_stip]\n\t"      \
    "csrs mideleg, a0\n\t"             \
    :                                   \
    : [mstatus_mpp] "i" (MSTATUS_MPP & (MSTATUS_MPP >> 1)), [sip_ssip_stip] "i" (SIP_SSIP | SIP_STIP) \
    : "a0", "memory")


#define RVTEST_CODE_BEGIN() asm volatile( \
    ".section .text.init\n\t"            \
    ".align 6\n\t"                     \
    ".weak stvec_handler\n\t"           \
    ".weak mtvec_handler\n\t"           \
    ".globl _start\n\t"               \
    "_start:\n\t"                      \
    "j reset_vector\n\t"               \
    "trap_vector:\n\t"                   \
    "csrr t5, mcause\n\t"                \
    "li t6, CAUSE_USER_ECALL\n\t"        \
    "beq t5, t6, write_tohost\n\t"       \
    "li t6, CAUSE_SUPERVISOR_ECALL\n\t"   \
    "beq t5, t6, write_tohost\n\t"       \
    "li t6, CAUSE_MACHINE_ECALL\n\t"      \
    "beq t5, t6, write_tohost\n\t"       \
    "la t5, mtvec_handler\n\t"          \
    "beqz t5, 1f\n\t"                \
    "jr t5\n\t"                     \
    "1:\n\t"                        \
    "csrr t5, mcause\n\t"             \
    "bgez t5, handle_exception\n\t"   \
    "handle_exception:\n\t"          \
    "1:\n\t"                        \
    "ori TESTNUM, TESTNUM, 1337\n\t"  \
    "write_tohost:\n\t"              \
    "sw TESTNUM, tohost, t5\n\t"      \
    "sw zero, tohost + 4, t5\n\t"    \
    "j write_tohost\n\t"              \
    "reset_vector:\n\t"                \
    "INIT_XREG\n\t"                    \
    "INIT_PMP\n\t"                     \
    "CHECK_XLEN\n\t"                   \
    ::: "memory")

#define RVTEST_PASS() asm volatile(   \
    "fence\n\t"                       \
    "li a7, 93\n\t"                   \
    "li a0, 0\n\t"                    \
    "ecall\n\t"                        \
    ::: "a0", "a7", "memory")

#endif // _ENV_PHYSICAL_SINGLE_CORE_H
