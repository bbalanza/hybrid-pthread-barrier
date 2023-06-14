#include <stdatomic.h>
#include <immintrin.h>
#include <stdlib.h>
#include <sched.h>

#define ATOMIC_INCREMENT(x) __atomic_add_fetch(x, 1, __ATOMIC_ACQ_REL)
#define ATOMIC_LOAD(x) __atomic_load_n(x, __ATOMIC_ACQUIRE)
#define ATOMIC_ZERO(x) __atomic_store_n(x, 0, __ATOMIC_RELEASE)

typedef struct
{
    int *hits;
    int to_hit;
} spinlock_barrier_t;

void spinlock_barrier_init(spinlock_barrier_t *barrier, int barriers_to_hit)
{
    barrier->hits = (int*) calloc(1, sizeof(int));
    barrier->to_hit = barriers_to_hit;
}

void spinlock_barrier_destroy(spinlock_barrier_t *barrier)
{
    free(barrier->hits);
}

// Inspired by https://nullprogram.com/blog/2022/03/13/ but works for an 
// arbitrary number of threads at the expense of an extra atomic_store and atomic_thread_fence
// __mm_pause() and sched_yield from https://probablydance.com/2019/12/30/measuring-mutexes-spinlocks-and-how-bad-the-linux-scheduler-really-is/
static void spinlock_barrier_wait(spinlock_barrier_t *barrier)
{
    int hit = ATOMIC_INCREMENT(barrier->hits);
    if (hit == barrier->to_hit)
    {
        ATOMIC_ZERO(barrier->hits);
    }
    else {
        for (int spin_count = 0; ATOMIC_LOAD(barrier->hits) != 0; ++spin_count){
            if (spin_count < 256)
                _mm_pause();
            else
            {
                sched_yield();
                spin_count = 0;
            }
        }
    }
}