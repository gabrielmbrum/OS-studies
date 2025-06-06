# introduction

we would like to execute a series of instructions atomically, but due the presence of interrupts (or multiple threads executing on multiple processor concurrency) we couldnt

# 28.1 locks: the basic idea

shared variable example

  balance = balance + 1

to use lock we add somethings

```c
  lock_t mutex;
  ...
  lock(&mutex);
  balance = balance + 1;
  unlock(&mutex);
```

lock is just a variabe of some kind (suck as mutex above), it holds the state of the lock at any instance in time:
  available (free/unlocked): no threads holds the lock
  acquired (locked/held): one thread holds the lock

we could store other information, such as which thread holds the lock, queue for ordering lock acquisition, but this is hidden from the user.

lock():
  1) tries to acquire the lock
  2a) if no thread holds it, the thread acquire and enter the critical section
  2b) if a thread holds it, it waits until the lock be available

unlock():
  1) free the lock
  2a) if no thread are waiting the state of the lock is changed to free
  2b) if there is a thread waiting, it will notice and acquire the lock

# 28.2 Pthread locks

the name provided by POSIX for a lock is mutex (mutual exclusion between threads)

```c

  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

  Pthread_mutex_lock(&lock);
  balance = balance + 1;
  Pthread_mutex_unlock(&lock);
  
```

# 28.3 building a lock

bla bla bla

# 28.4 evaluating locks 

first, we will establish some basic criteria
  1) lock provide a basic task of mutual exclusion (does it work?)
  2) fairness (fair acquiring)
  3) starvation (thus a thread never obtain the lock?)
  4) performance (time overheads added by using the lock)
    a) when a thread is running and grab and release the lock, what is the overhead?
    b) multiple threads are contending for the lock on a single CPU, are the perfomance concerns?
    c) the perform when there are multiple CPUs involved?

# 28.5 controlling interrupts

for single processor systems, a solution was to disable interrupts for critical sections

```c

  void lock() {
  DisableInterrupts();
}
void unlock() {
  EnableInterrupts();
}

```

we ensure the code that entered the critical section will not be interrupted, so will execute atomically
positive point: simplicity
negative points:
  1) we provide to any task the privilege, so we TRUST that it will not takes advantages of it (infinite loop, monopolize the processor
  2) do not work in multiple processors 
  3) can be inefficient (slowly in modern CPUs)

# 28.6 test and set (atomic exchange)

```c

  // first attempt: a simple flag

  typedef struct __lock_t { int flag; } lock_t;

  void init (lock_t *mutex) {
    // 0 -> lock is available, 1 -> held
    mutex->flag = 0;
  }

  void lock (lock_t *mutex) {
    while (mutex->flag == 1)  // TEST the flag
      ;                       // spin wait
    mutex->flag = 1;
  }

  void unlock (lock_t *mutex) {
    mutex->flag = 0;
  }

```

## the code has two problems:
  ### correctness 
    T1 calls lock; spin-waiting; INTERRUPT!! -> T2 calls lock; spin-wait; set flag = 1; INTERRUPT!! -> T1 set flag = 1
    and there are two threads inside the critical section!

  ### performance 
    spin-waiting wastes time waiting for another thread realease a lock 

# 28.7 building a working spin lock

what the test-and-set instruction does?

```c
    int testAndSet (int *ptr, int new) {
      int old = *ptr;
      *ptr = new;
      return old;
    }
```

it returns the old value and set the ptr to the new one

this "test" the old value and "set" the new one

this is able to build a simple spin lock

```c
    typedef struct __lock_t {
      int flag;
    } lock_t;

    void init (lock_t *lock) {
      lock->flag = 0;
    }

    void lock (lock_t *lock) {
      while (testAndSet(&lock->flag, 1) == 1) 
        ;
    }

    void unlock (lock_t *lock) {
      lock->flag = 0;
    }
```

# 28.8 evaluating spin locks

its correct! 
what about fairness?
  spin locks doesnt provide any fairness guarantees -> may lead to starvation

what about perfomance?
  in single CPU its painful :( because when a thread has the lock, all the next threads that the CPU picks that want the lock will just spend its time slice
  in multiple CPUs, while thread B waits on CPU 2, thread A is running on CPU 1

# 28.9 compare and swap

```c
  int compareAndSwap (int *ptr, int expected, int new) {
    int actual = *ptr;
    if (actual == expected) 
      *ptr = new;
    return actual;
  }

```

lock example:
  
```c
  void lock (lock_t *lock) {
    while (compareAndSwap(&lock->flag, 0, 1) == 1)
      ;
  }

```

# 28.10 load-linked and store-conditional

on the MIPS architecture the load-linked and store-conditional instrucions can be used to build locks and other concurrent structures

```c
  int loadLinked (int *ptr) {
    return *ptr;
  }

  int storeConditional (int *ptr, int value) {
    if (no one has updated *ptr since the loakLinked to this address) {
      *ptr = value;
      return 1; // success!
    } else {
      return 0;
    }
  }
```

```c
  void lock (lock_t *lock) {
    while (1) {
      while (loadLinked(&lock->flag) == 1)
        ; // spin until is zero
      if (storeConditional(&lock->flag, 1) == 1)
        return; // if no one acquired the lock, success and return! otherwise, try all it over again
    }
  }
```

```c
  // shorter version
  
  void lock (lock_t *lock) {
    while (loadLinked(&lock->flag) == 1 || !storeConditional(&lock->flag, 1))
      ;
  }
```


# 28.11 fetch and add

```c
  int fetchAndAdd (int *ptr) {
    int old = *ptr;
    *ptr = old + 1;
    return old;
  }
```

ticket lock concept: instead of a single value, uses a ticket and turn variable in combination to build a lock.
when a thread whishes to acquire the lock, it does a fechAndAdd on the ticket value.
that value is now the thread "turn".
the global lock->turn determines which thread's turn it is.
when (myturn == turn) it is that thread's turn to enter the critical section.

unlock simply increments the turn, so the next waiting thread can now enters de cs.

```c

  typedef struct __lock_t {
    int ticket;
    int turn;
  } lock_t;

  void lock_init (lock_t *lock) {
    lock->ticket = 0;
    lock->turn = 0;
  }

  void lock (lock_t *lock) {
     int myturn = fetchAndAdd(&lock->ticket);
     while (&lock->turn != myturn)
      ;
  }

  void unlock (lock_t *lock) {
    fetchAndAdd(&lock->turn);
  }

```

it ensures progress to all threads!!! -> fairness!!! :)

# 28.12 too much spinning

too much spinning -> N threads, N-1 time slices waisted waiting for a critical section 

# 28.13 a simple approach: just yield baby!

what to do when a context switch occurs in a critical section and threads starts to spin endlessly?

if you are going to spin, just "double the CPU and pass to the next one"!

```c

  void init () {
    flag = 0;
  }
  
  void lock () {
    while (testAndSet(&flag, 1) == 1)
      yield();  // gives up the CPU
  }

  void unlock() {
    flag = 0;
  }

```

moves the thread from runnnig to ready state

in a case of 2 threads on 1 CPU works well!

the cost changes from spinning to context switching (imagine 100 threads and so 99 context switch)

# 28.14 using queues : sleeping instead of spinning

will can use a queue and a support from Solaris, using park() and unparck()

  park(): put a calling thread to sleep 
  unparl(ThreadID): wake a particular thread as designated by its id

```c

  typedef struct __lock_t {
    int flag;
    int guard;
    queue_t *q;
  } lock_t;

  void lock_init (lock_t *m) {
    m->flag = 0;
    m->guard = 0;
    queue_init(m->q);
  }

  void lock (lock_t *m) {
    while (testAndSet(&m->guard, 1) == 1)
      ;
    if (m->flag == 0) {
      m->flag = 1;
      m->guard = 0;
    } else {
      queue_add(m->q, getid());
      m->guard = 0;
      park();
    }
  }

  void unlock (lock_t *m) {
    while (testAndSet(&m->guard, 1) == 1) 
      ;
    if (queue_empty(m->q))
      m->flag = 0;
    else
      unpark(queue_remove(m->q));
    m->guard = 0;
  }

```

we combine test-and-set with a queue of lock waiters to a make a more efficient lock, beyond that, the queue avoids starvation
the guard is used as a spin-lock around the flag and queue manipulation
this approach doesnt avoid spin-waiting for complete, but the time spend with it is limited (few instructions inside lock and unlock)
in lock, when a thread cannot access, add ourselves to a queue (getting the thread id), setting guard to 0 and yield

wakeup/waiting race
  with the wrong timing, a thread A will be about to be park (will sleep until the lock is no longer held)
  happens a switch at this time to a thread B holding the lock and releases it.
  switch to tA, that is about to park and will sleep forever.

this problem was solved by solaris with setpark()

  queue_add(m->q, getid());
  setpark();
  m->guard = 0;

# 28.15 different OS, diffent support

linux provides something called a futex which is similar to the Solaris interface

```c

  void mutex_lock (int *mutex) {
    int v;
    // bit 31 was clear, we got the mutex
    if (atomic_bit_test_set(mutex, 31) == 0)
      return;
    atomic_increment(mutex);

    while(1) {
      if (atomic_bit_test_set(mutext, 31) == 0) {
        atomic_decrement(mutex);
        return;
      }
      // we have to wait now, first make sure the futex value we are monitoring is truly negative (i.e locked)
      v = *mutex;
      if (v >= 0)
        continue;
      futex_wait(mutex, v);
    }
  }

  void mutex_unlock(int *mutex) {
    // adding 0x80000000 to the counter results in 0 if and only if there are not other interested threads
    if (atomic_add_zero(mutex, 0x80000000))
      return;

    // there are other threads waiting for the mutex, wake one of them up
    futex_wake(mutex);
  }

```

# 28.16 two-phase locks

spinning can be useful when a lock is about to be released

  1st phase: the lock spins for a while, hoping it can acquire the lock
  2nd phase: the caller is put to sleep and woken up when the lock becomes free later

it's like the linux version, but with a variable quantify of spins waiting

it's a hybrid approach