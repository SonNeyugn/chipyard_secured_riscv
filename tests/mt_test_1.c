#include <riscv-pk/encoding.h>
#include <stdio.h>
#include "marchid.h"
#include <stdint.h>
#include "riscv_macros_mt.h"
#include <riscv-pk/encoding.h>
#include "mmio.h"

// This test is designed to check if every core can read the same value
// from a shared variable and that the value is not corrupted by other cores.

typedef struct {
  int counter;
} atomic_t;

static inline int atomic_xchg(atomic_t *v, int n)
{
  register int c;

  __asm__ __volatile__ (
          "amoswap.w.aqrl %0, %2, %1"
          : "=r" (c), "+A" (v->counter)
          : "r" (n));
  return c;
}

static inline void mb(void)
{
  __asm__ __volatile__ ("fence");
}

void get_lock(atomic_t *lock)
{
  while (atomic_xchg(lock, 1) == 1)
      ;
  mb();
}

void put_lock(atomic_t *lock)
{
  mb();
  atomic_xchg(lock, 0);
}

void setup() {
  INIT_PMP();
  DELEGATE_NO_TRAPS();
  // asm volatile ("csrw mtvec, %0" :: "r"(trap_vector));
  ENABLE_SUPERVISOR();
}


static atomic_t lock = { .counter = 0 };
volatile uint32_t atomic_counter = 0;
volatile uint32_t signal = 0;
volatile uint32_t signal_write = 0;
volatile uint32_t mismatch = 0;
volatile uint32_t shared_data;
static uint32_t t0, t1, t2, t3;


void __main(void) {
  // Get the hart id
  size_t mhartid = read_csr(mhartid);

  while (1) {
    if (mhartid == 0) { // Core 0
      printf("Hello from core 0\n");
      printf("Starting test for cache coherency\n");
      
      shared_data = 0xDEADBEEF;
      printf("Wrote 0xdeadbeef to shared data\n");
      signal_write = 1;
      

      while (signal < 3) {
        // printf("Waiting for signal\n");
      }
      get_lock(&lock);
      printf("All core finish reading\n");
      if (mismatch == 0) {
        printf("Test passed\n");
        exit(0);
      }
      else {
        printf("Test failed\n");
        exit(1);
      }
      put_lock(&lock);
      // asm volatile ("wfi");
      
    }
    else {  // Other cores
      while (signal_write != 1) {
      }

      uint32_t read_data = reg_read32(&shared_data);
      // if (mhartid == 1) printf("Hello from core 1\n");
      if (read_data != 0xDEADBEEF) {
        get_lock(&lock);
        printf("Mismatch - Core %d: read_data = 0x%lx\n", mhartid, read_data);
        signal++;
        mismatch++;
        put_lock(&lock);
        // exit(1);
      }
      else {
        get_lock(&lock);
        signal++;
        printf("Core %d: read_data = 0x%lx\n", mhartid, read_data);
        // printf("Signal = %d\n", signal);
        put_lock(&lock);
      }
      asm volatile ("wfi");
      
      
    }
  }

}

int main() {
  __main();
}
