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
  if (mcause == CAUSE_LOAD_ACCESS) {
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
  return 0;
}

// Configure PMP0 for NAPOT and X permissions
void setup_pmp() {
  uint64_t pmpcfg0, pmpaddr0;
  // pmpcfg0 = PMP_NAPOT | PMP_X;
  asm volatile ("csrr %0, pmpcfg0" : "=r"(pmpcfg0));
  // printf("Initial PMPCFG0: 0x%lx\n", pmpcfg0);
  pmpcfg0 |= PMP_NAPOT | PMP_X;
  asm volatile ("csrw pmpcfg0, %0" :: "r"(pmpcfg0));

  // pmpaddr0 = 0x80002010;
  pmpaddr0 = (uint64_t)&test_base[0];
  pmpaddr0 = (pmpaddr0 >> 2) | 0x7;
  asm volatile ("csrw pmpaddr0, %0" :: "r"(pmpaddr0));
  printf("PMP configured\n");
  printf("-- PMPCFG0: 0x%lx\n", pmpcfg0);
  printf("-- PMPADDR0: 0x%lx\n", pmpaddr0);
}

// Attemp to read and write to an address in the protected region
// Read and Write both should cause an exception
void u_mode()
  {
    printf("Switched to user mode\n");

    printf("Attempting to read from test_base[1] at: %p\n", &test_base[1]);
    printf("  -> Should cause an exception\n");
    
    uint32_t test_1 = reg_read32(&test_base[1]); // Read from test base
    printf("Value read from test base 1: 0x%lx\n", test_1);

    printf("Attempting to write to test_base[1] at: %p\n", &test_base[1]);
    
    reg_write64(&test_base[1], 0x12345678);


    fail();
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

  printf("Testing PMP NAPOT with X permissions\n");
  printf("Address of testbase: %p\n", &test_base);
  reg_write64(&test_base[0], 0x88888888);
  reg_write64(&test_base[1], 0x88889999);
  
  printf("Configuring PMP...\n");
  setup_pmp();

  switch_to_s_mode();  
  return 0;
}