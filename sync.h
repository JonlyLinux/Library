#ifndef SYNC_H
#define SYNC_H

typedef struct atomic_s {
    volatile int atomic;
} atomic_t;

/* atomic operations */
/* add sub and or xor nand */
#define atomic_fetch_and_op(OP, A, V) __sync_fetch_and_##OP##(&(A)->atomic, (V))
#define atomic_op_and_fetch(OP, A, V) __sync_##OP##_and_fetch(&(A)->atomic, (V))

/* atomic compare and set called CAS */
#define vcas(PTR, OLDVAL, NEWVAL) __sync_val_cmp_and_set((PTR), (OLDVAL), (NEWVAL))
#define bcas(PTR, OLDVAL, NEWVAL) __sync_bool_cmp_and_set((PTR), (OLDVAL), (NEWVAL))

/* atomic set and return */
#define atomic_set_and_ret(PTR, VAL) __sync_lock_test_and_set((PTR), (VAL))
#define atomic_set_zero(PTR)         __sync_lock_release((PTR))

#endif // SYNC_H
