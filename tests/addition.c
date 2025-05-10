#include <stdio.h>
#include <riscv-pk/encoding.h>
// #include <riscv-pk/syscall.h>
// #include </home/nguyesh/risc_v/chipyard/toolchains/riscv-tools/riscv-pk/pk/syscall.c>
#include "marchid.h"
#include <stdint.h>

#define read_from_t0(var) ({ \
  asm volatile ("mv %0, t0" : "=r"(var)); \
  var; })

void enter_user_mode(void) {

    __asm__ volatile ("nop");
    __asm__ volatile ("nop");
    __asm__ volatile ("nop");

    __asm__ volatile ("li t0, 2");
    __asm__ volatile ("li t1, 3;");
    __asm__ volatile ("add t0, t0, t1");

    __asm__ volatile ("nop");
    __asm__ volatile ("nop");
    __asm__ volatile ("nop");

}

// void return_to_supervisor(uint64_t supervisor_return_address) {
//     __asm__ volatile (
//         // Set the return address in mepc
//         "csrw mepc, %0\n\t"

//         // Read the current mstatus, clear MPP bits, and set it to Supervisor Mode (01)
//         "csrr t0, mstatus\n\t"
//         "li t1, 0xFFFFF7FF\n\t"  // Mask to clear MPP (bits 12 and 11)
//         "and t0, t0, t1\n\t"      // Clear MPP
//         "ori t0, t0, (1 << 11)\n\t" // Set MPP to 01 (Supervisor Mode)
//         "csrw mstatus, t0\n\t"    // Update mstatus

//         // Return to the address in mepc and switch privilege mode
//         "mret"
//         :
//         : "r"(supervisor_return_address)
//         : "t0", "t1" // Clobbers
//     );
// }

int main(void) {
    // unsigned long a, b, product;
    // a = 2;
    // b = 3;
    // product = a * b;
    // printf("Product = %li\n", product);
    // __asm__ volatile ("li a0, 10");
    // __asm__ volatile ("addi a0, a0, 10");
    // __asm__ volatile ("addi a0, a0, 10");
    // read_from_a0(a);
    // printf("a = %li\n", a);

    uint64_t mstatus = read_csr(mstatus);
    printf("mstatus =  %li\n", mstatus);

    // uint64_t sstatus = read_csr(sstatus);
    // printf("sstatus =  %li\n", sstatus);

    // uint64_t pmpcfg0 = read_csr(pmpcfg0);
    // printf("pmpcfg0 =  %li\n", pmpcfg0);

    // uint64_t misa = read_csr(misa);
    // printf("misa =  %li\n", misa);


    // __asm__ volatile ("li a0, 2");
    // __asm__ volatile ("li a7, 93");
    // __asm__ volatile ("ori t0, t0, 1");
    // sys_exit(93);
    // SYS_exit;
    // __asm__ volatile ("ecall");
    // __asm__ volatile ("addi a0, a0, 2");

    // uint64_t mstatus;
    uint64_t mepc;
    uint64_t user_entry;
    __asm__ volatile (
      "csrr t0, mstatus\n\t"
      "srli t0, t0, 11\n\t"
      "andi t0, t0, 0x3\n\t"
      );
    unsigned long t0_1;
    read_from_t0(t0_1);
    printf("t0 = %li\n", t0_1);
    __asm__ volatile ("sret");
    __asm__ volatile ("csrr %0, mstatus" : "=r" (mstatus));
    
    __asm__ volatile ("li t0, 0x8"); // set MIE bit
    __asm__ volatile ("csrs mstatus, 0x8");
    __asm__ volatile ("csrr %0, mepc" : "=r" (mepc));
    __asm__ volatile ("lw %0, %1" : "=r" (user_entry) : "m" (*enter_user_mode));
    __asm__ volatile ("csrw mepc, %0" : : "r" (user_entry));
    __asm__ volatile ("csrr t0, mstatus");
    __asm__ volatile ("li t1, 0xFFFFFFF8"); // set MPP to user mode
    __asm__ volatile ("and t0, t0, t1"); // clear MPP bits
    __asm__ volatile ("csrw mstatus, t0");
    
    printf("mstatus =  %li\n", mstatus);
    
    
    __asm__ volatile (
      "csrr t0, mstatus\n\t"
      "srli t0, t0, 11\n\t"
      "andi t0, t0, 0x3\n\t"
      );
    unsigned long t0_2;
    read_from_t0(t0_2);
    printf("t0 = %li\n", t0_2);

    // __asm__ volatile ("csrr t0, mstatus");
    // __asm__ volatile ("li t1, 0xFFFFFFF8"); // set MPP to user mode
    // __asm__ volatile ("and t0, t0, t1"); // clear MPP bits
    // __asm__ volatile ("csrw mstatus, t0");
    // uint64_t return_address = 0x80000000;
    // return_to_supervisor(return_address);
    return 0;
}
