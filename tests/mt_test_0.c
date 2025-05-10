#include <riscv-pk/encoding.h>
#include <stdio.h>
#include "marchid.h"
#include <stdint.h>
#include "riscv_macros_mt.h"
#include <riscv-pk/encoding.h>


// This test is designed to check the atomicity of the LR/SC instructions
// and the atomic operations on a shared variable across multiple cores.

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
  ENABLE_SUPERVISOR();
}



static atomic_t lock = { .counter = 0 };
volatile uint32_t atomic_counter = 0;
volatile uint32_t signal = 0;
static uint32_t t0, t1, t2, t3;

void __main(void) {

  size_t mhartid = read_csr(mhartid);

  while (1) {
    if (mhartid == 0) {
      printf("Initial atomic_counter = %d\n", atomic_counter);
      while (signal < 3) {
        printf("Waiting for signal\n");
      }
      if (atomic_counter == 3) {
        printf("Test passed: atomic_counter = %d\n", atomic_counter);
        exit(0);
      }
      else {
        printf("Test failed: atomic_counter = %d\n", atomic_counter);
        exit(1);
      }
    }
    else {
      asm volatile ("la %0, atomic_counter" : "=r"(t0));
      do {
        atomic_lrsc(t0, t1, t2, t3);
      } while (t2 != 0);
      get_lock(&lock);
      signal++;
      
      put_lock(&lock);
      asm volatile ("wfi");
    }
  }
}

void atomic_lrsc (uint32_t t0, uint32_t t1, uint32_t t2, uint32_t t3) {
  asm volatile (
    "lr.w %0, (%1)\n\t"
    "addi %0, %0, 1\n\t"
    "sc.w %2, %0, (%1)\n\t"
    : "=&r"(t1), "+r"(t0), "=&r"(t2)
    :
    : "memory"
  );
}

int main() {
  __main();
}
