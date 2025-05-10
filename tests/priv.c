#include <stdint.h>
#include <stdio.h>
#include "riscv_macros.h"
#include <riscv-pk/encoding.h>
#include "marchid.h"
#include "mmio.h"

// Define constants

volatile uint32_t test_base[4] = {0, 0, 0, 0};

// Machine Trap Vector Handler
void mtvec_handler() {
  printf("mtvec_handler called\n");
  uint64_t mcause;
  asm volatile ("csrr %0, mcause" : "=r"(mcause));
  printf("  -> mcause: 0x%lx\n", mcause);
  if (mcause == CAUSE_ILLEGAL_INSTRUCTION) {
      handle_illegal_access();
  } else {
      fail();
  }
}

// Handle illegal access
void handle_illegal_access() {
  printf("Illegal access handling\n");
  uint64_t mstatus_mpp = MSTATUS_MPP & (MSTATUS_MPP >> 1);
  asm volatile ("csrs mstatus, %0" :: "r"(mstatus_mpp));
  asm volatile ("li a0, 0");
  success();
}

void trap_vector() {
  TRAP_VECTOR();
}

void fail() {
  printf("Test failed\n");
  exit(1); 
}

int success(void) {
  printf("Test passed\n");
  exit(0);
}


// Attemp to read sstatus from U-mode
// This should cause an exception
void u_mode()
  {
    printf("Switched to user mode\n");

    printf("Attempting to read sstatus\n");
    printf("  -> Should cause an exception\n");
    
    uint64_t sstatus = read_csr(sstatus);
 

    fail(); // Fail if unable to trap illegal access
  }

// Configure and jump to U-mode
void s_mode() 
  {
    printf("Switched to supervisor mode\n");
    // printf("mstatus: 0x%lx\n", read_csr(sstatus));
    // printf("Address of testbase: %p\n", &test_base);

    uint64_t sstatus_spp = SSTATUS_SPP;
    asm volatile ("csrc sstatus, %0" :: "r"(sstatus_spp));
    void *u_mode_addr = &u_mode;
    // printf("Address of u_mode: %p\n", u_mode_addr); // Print the address of u_mode
    asm volatile ("csrw sepc, %0" :: "r"(u_mode_addr));
    asm volatile ("sret");

  }

// Configure and jump to S-mode
void switch_to_s_mode() {
  void *s_mode_addr = &s_mode;
  // printf("Address of s_mode: %p\n", s_mode_addr); // Print the address of s_mode
  asm volatile ("csrw mepc, %0" :: "r"(s_mode_addr));
  asm volatile ("mret");
  fail(); // Fail if unable to jump to S-mode
}


void setup() {
  INIT_PMP();
  DELEGATE_NO_TRAPS();
  asm volatile ("csrw mtvec, %0" :: "r"(trap_vector));
  ENABLE_SUPERVISOR();
}

int main(void) {

  setup();

  printf("Testing trap for access of sstatus from U-mode\n");

  switch_to_s_mode();

  return 0;
}