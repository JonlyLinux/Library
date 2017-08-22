#ifndef BUILTIN_H
#define BUILTIN_H

#define likely(cond) __builtin_expect(cond, 1)
#define unlikely(cond) __builtin_expect(cond, 0)

#endif // BUILTIN_H
