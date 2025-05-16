# Intro

**<crux> How to create and control threads? </crux>**

## (27.1) Thread Creation

First thing is to create new threads:                                                                                    

```c
#include <pthread.h>

int
pthread_create (
  pthread_t *thread,
  const pthread_attr_t *attr,
  void *(*start_routine) (void*),                            
  void *arg);
```

where:

- `thread`: will interact with this thread, it's initialized in this function
- `attr`: used to specify any atrributes this thread might have (stack size, scheduling priority), it's initialized with `pthread_attr_init()`
- `function pointer`: tells us a function name (`start_routine`) and which returns a value of type void (can change `(void*)` to `(int)` or any other type)
- `arg`: passed to the function where the thread begins execution. 

Why do we need these void pointers?
- allows us to pass in any type of arguemnt
- the return is the same

### Example: Creating a Thread

```c
#include <pthread.h>

typedef struct __myarg_t {
  int a;
  int b;
} myarg_t;

void *mythread (void *arg) {
  myarg_t *m = (myarg_t *) arg;
  printf("%d %d\n", m->a, m->b);
  return NULL;
}

int
main (int argc, char *argv[]) {
  pthread_t p;
  int rc;

  myarg_t args;
  args.a = 10;
  args.b = 20;
  rc = pthread_create(&p, NULL, mythread, &args);
  ...
}
```

## (27.2) Thread Completion

What if you want to wait for a thread to be completed?
You must call `pthread_join()` !

```c
int pthread_join (pthread_t thread, void **value_ptr);
```

where:

- `thread`: used to specify which thread to wait for
- `**value_ptr`: returns the value you expect

### Example: Waiting for Thread Completion

```c
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

typedef strcut __myarg_t {
  int a;
  int b;
} myarg_t;

typedef struct __myret_t {
  int x;
  int y;
} myret_t;

void *mythread (void *arg) {
  myarg_t *m = (myarg_t *) arg;
  printf("%d %d\n", m->a, m->b);
  myret_t *r = Malloc(sizeof(myret_t));
  r->x = 1;
  r->y = 2;
  return (void *) r;
}

int
main (int argc, char *argv[]) {
  int rc;
  pthread_t p;
  myret_t *m;

  myarg_t args;
  args.a = 10;
  args.b = 20;
  Pthread_create(%p, NULL, mythread, &args);
  Pthread_join(p, (void **) &m);
  printf("returned %d %d\n", m->x, m->y);
  return 0;
}
```

`myret_t` is used to return the values from the join.

Life can be simpler by passing a single value, as the example below:

```c
// simpler argument passing to a thread

void *mythread (void *arg) {
  int m = (int) arg;
  printf("%d\n", m);
  return (void *) (arg + 1);
}

int
main (int argc, char *argv[]) {
  int rc, m;
  pthread_t p;

  Pthread_create(%p, NULL, mythread, (void *) 100);
  Pthread_join(p, (void **) &m);
  printf("returned %d\n", m);
  return 0;
}
```

But we have to take care to NEVER return a pointer which refers to something allocated on the thread's call stack!!!! as this example:

```c
void *mythread (void *arg) {
  myarg_t *m = (myarg_t *) arg;
  printf("%d %d\n", m->a, m->b);
  myret_r r; // ALLOCATED ON STACK ! BAD !!1!11!!!
  r.x = 1;
  r.y = 2;
  return (void *) &r;
}
```

Using `_create` and then `_join` it's a strange way to create a thread. There is an easier way to do this: procedure call!

## (27.3) Locks

Beyond creation and join, the most useful set of functions are the mutual exclusion to a critical section via locks:

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

Example:
```c
pthread_mutex_t lock;
pthread_mutex_lock(&lock);
x = x + 1;
pthread_mutex_unlock(&lock);
```

When a thread calls `pthread_mutex_lock()`:
  - if no other thread holds the lock, the thread acquires the lock and accesses the critical section
  - else, it waits until the thread with the lock unlocks it (this may cause the thread to be stuck for a while)

Unfortunately, this mechanism is broken due to:

1) Lack of proper initialization: there is no validation of correct values to begin and work with when `(un)lock` are called.

  There are two solutions to this:
  a) Using `PTHREAD_MUTEX_INITALIZER`:
    
  
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  

    This sets the lock to the default values and makes the lock usable.

  b) Using `pthread_mutex_init()`:

    
    int rc = pthread_mutex_init(&lock, NULL);
    assert(rc == 0); // always check success
  

2) Fails to check errors code when calling lock and unlock (these routines can also fail!)
  If a non-processed error occurs, multiple threads can access the critical section all at once.

  a) Minimal approach: wrappers

  ```c
  // use this to clean code but checking failures
  // only use if exiting program is OK upon failure

  void Pthread_mutex_lock(pthread_mutex_t *mutex) {
    int rc = pthread_mutex_lock(mutex);
    assert(rc == 0);
  }
  ```

It's important to notice that lock and unlock aren't the only routines that pthreads has to interact with locks, there are two more that are also relevant:

  - `// returns failure if the lock is already held`
  ```c
  int pthread_mutex_trylock(pthread_mutex_t *mutex);
  ```

  - `// returns after a timeout of after acquiring the lock`
  ```c
  int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                              struct timespec *abs_timeout);
  ```

## (27.4) Condition Variables

Condition variables are useful when some kind of signaling must take place between threads, for instance, if one thread is waiting for another to do something before it can continue.

```c
// puts the calling thread to sleep (just wait)
// usually when something in the program has changed that the now sleeping thread might care about
// has a 2nd parameter for the lock that is released
int pthread_cond_wait(pthread_cond_t *cond,
                        pthread_mutex_t *mutex);

int pthread_cond_signal(pthread_cond_t *cond);
```

### Example: Using Wait
  
```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t init = PTHREAD_COND_INTIALIZAR;

Pthread_mutex_lock(&lock);
while (initialized == 0)
  Pthread_cond_wait(&init, &lock);
Pthread_mutex_unlock(&lock);

/*
  after init and lock and cond, a thread checks if the initialized has yet been set to something other than zero
    if not
      thread simply wait
*/
```

### Example: Code to Wake a Thread

```c
Pthread_mutex_lock(&lock);
initialized = 1;
Pthread_cond_signal(&init);
Pthread_mutex_unlock(&lock);

/*
  1st : make sure that has the lock held 
  2nd : 2nd argument of wait is for releasing the lock
*/
```

## (27.5) Compiling and Running

```bash
prompt> gcc -o main main.c -Wall -pthread
```
