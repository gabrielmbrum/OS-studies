# 31.1 Semaphores: A Definition

It's an object with an integer value that we can manipulate with two routines:

- `sem_wait()`
- `sem_post()`

We first must initialize the semaphore:

```c
#include <semaphore.h>
sem_t s;
sem_init(&s, 0, 1);
```

Third arg: value that will be initialized
Second arg: 0 indicates that the semaphore is shared between threads in the same process

After initialized, we use sem_wait() and sem_post()

```c
int sem_wait (sem_t *s) {
  decrement the value of semaphore s by one
  wait if value of semaphore s is negative
}

int sem_post (sem_t *s) {
  increment the value of semaphore s by one
  if there are one or more threads waiting, wake one
}
```

Aspects of the interface:

- `sem_wait()`:
  - will return right away, or
  - will cause the caller to suspend execution waiting for a post

- `sem_post()`:
  - does not wait
  - rather, it simply increments the value of the s and then
  - if there is a thread waiting, wakes one up

The value of semaphore:
  - when negative, is equal to the number of waiting threads

# 31.2 Binary Semaphores (Locks)

```c
sem_t m;
sem_init(&m, 0, X);

sem_wait(&m);
// critical section
sem_post(&m);
```

X must be 1

The thread has 3 schedular states: running, ready and slepping

# 31.3 Semaphores as Condition Variables

Semaphores are also useful when a thread wants to halt its progress waiting for a condition to become true

Example:
  a thread is waiting for something to happen, while a differente thread is making that something happen and when it does, it wakes up the waiting thread

```c
sem_t s;

void *
child(void *arg) {
  printf("child\n");
  sem_post(&s);
  return NULL;
}

int
main(int argc, char *argv[]) {
  sem_init(&s, 0, X);
  printf("parent: begin\n");
  pthread_t c;
  Pthread_create(c, NULL, child, NULL);
  sem_wait(&s);
  printf("parent: end\n");
  return 0;
}
```

X must be set to 0

# 31.4 The Producer/Consumer (Bounded-Buffer) Problem


## First Atempt

  Our first ateempt at solving the problem introduces two semaphores (empty and full), which indicate when a buffer entry has been emptied or filled

  The put and get routines
  ```c
    int buffer[MAX];
    int fill = 0;
    int use = 0;

    void put (int value) {
      buffer[fill] = value;
      fill = (fill + 1) % max;
    }

    int get() {
      int tmp = buffer[use];
      use = (use + 1) % MAX;
      return tmp;
    }
  ```

  Adding the full and empty conditions
  ```c
    sem_t empty;
    sem_t full;

    void *producer (void *arg) {
      int i;
      for (i = 0; i < loops; i++) {
        sem_wait(&empty);
        put(i);
        sem_post(&full);
      }
    }

    void *consumer (void *arg) {
      int i, tmp = 0;
      while (tmp != 1) {
        sem_wait(&full);
        tmp = get();
        sem_post(&empty);
        printf("%d\n", tmp);
      }
    }

    int main (int argc, char *agrv[]) {
      // ...
      sem_init(&empty, 0, MAX);
      sem_init(&full, 0, 0);
      // ...
    }
  ```

  A race condition, lets imagine there are two producers (Pa and Pb) both calling put() at the same time. assume Pa runs first, it starts to fill the first buffer entry. before Pa increment the fill counter to 1 it's interrupted and Pb runs and both puts its data into the 0th element of the buffer -> OVERWRITE!!!!

## A Solution: Adding Mutual Exclusion
  
  We've forgotten about mutual exclusion, the filling of a buffer and incrementing of the index is a critical section and thus must be guarded.

  Adding mutual exlusion (incorrectly)
  ```c
    sem_t empty;
    sem_t full;
    sem_t muntex;

    void *producer (void *agr) {
      int i;

      for (i = 0; i < loops; i++) {
        sem_wait(&mutex);
        sem_wait(&empty);
        put(i);
        sem_post(&full);
        sem_post(&mutex);
      }
    }

    void *consumer (void *arg) {
      int i;
      for (i = 0; i < loops; i++) {
        sem_wait(&mutex);
        sem_wait(&full);
        int tmp = get();
        sem_post(&empty);
        sem_post(&mutex);
        printf("%d\n", tmp);
      }
    }

    int main (int argc, char *argv[]) {
      // ...
      sem_init(&empty, 0, MAX);
      sem_init(&full, 0, 0);
      sem_init(&mutex, 0, 1);
      // ...
    }
  ```

  This doesnt runs correctly because of deadlock

## Avoiding Deadlock

  consumer holds the mutex and waits for a producer
  producer stay waiting for mutex forever

## Finally, A Working Solution

  We simply must reduce the scope of the lock

  Adding mutual exclusion (correctly)
  ```c
  sem_t empty;
  sem_t full;
  sem_t mutex;

  void *producer (void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
      sem_wait(&empty);
      sem_wait(&mutex);
      put(iv);
      sem_post(&mutex);
      sem_post(&full);
    }
  }

  void *consumer (void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
      sem_wait(&full);
      sem_wait(&mutex);
      int tmp = get();
      sem_post(&mutex);
      sem_post(&empty);
      printf("%d\n", tmp);
    }
  }

  int main (int argc, char *argv[]) {
    // ...
    sem_init(&empty, 0, MAX);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    // ...
  }
  ```

# 31.5 Reader-Writer Locks

PAGE 386
