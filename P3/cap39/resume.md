# Chapter 39 - Files and Directories

thus far, we have seen that the process (virtualization of the CPU) and the address space (virtualization of the memory).

the abstractions allow a program to run as if it is in its own private world.

in this section a new critical piece is the **persistent storage**. such as HDD and SSD.

✝️ how to manage a persistent device?

---

## 39.1 Files and Directories

### the file 📜

> its a simple linear array of bytes, each of which you can read or write. each file has some kind of **low-level name**. 

the low-level name is often referred to as its **inode number (i-number)**. 

the responsibility of the file system is simply to store such data persistently on disk and make sure that when you request the data again, you get what you put there in the first place. 

file name often has two parts, for example _bar.txt_, that has _bar_ and _txt_ separated by a period. the first part is an arbitrary name, the second indicates the type of the file.

### the directory 📂

> also has a low-level name, but its contents is a list of (user-readable name, low-level name) pairs.

for example, lets say there is a file with the low-level name "10" and with user-readable name of "foo". the dir would have an entry that maps it like ("foo", "10").

each entry in a directory refers to either files or other directories (allows directory tree / directory hierarchy).

the hierarchy starts at **root directory** (in UNIX-based) and uses some kind of **separator** to name subsequent **sub-directories** until the desired file or directory is named.

![an example directory tree](image.png)

valid directories in the example are /, /foo, /bar, /bar/bar,
/bar/foo and valid files are /foo/bar.txt and /bar/foo/bar.txt.

---

## 39.2 The file System Interface

we'll start with the basics of creating, accessing and deleting files. 

along the way, we'll discover the mysterious call that is used to remove files, known as _unlink()_.

---

## 39.3 Creating Files

accomplished with the _open()_ syscall and passing the flag *O_CREAT*.

### code example

```c
  int fd = open("foo", 
                      O_CREAT|O_WRONLY|O_TRUNC, 
                      S_IRUSR|S_IWUSR);
```

the param O_CREAT creates the file **only** if it **does not exist**.

the O_WRONLY ensures that the file can only be written to 

O_TRUNC is to the case of the file already exists, truncates it to a size of zero bytes (removes it content).

### file descriptor

_open()_ returns a **file descriptor**. 

> **file descriptor def.:** is just an integers, private per process and is used in UNIX system to access files. it's a **capability**. 

it's used to read/write the file (assuming that you have permission to do so).

it's like a pointer to an object of type file.

it's managed by the OS on a per-process basis, example:

```c
  struct proc {
    ...
    struct file *ofile[NOFILE]; // open files
    ...
  }
```

---

## 39.4 Reading and Writing Files

```cmdline
  prompt> echo hello > foo
  prompt> cat foo
  hello
  prompt>
```

in this code we redirect the output of the program _echo_ to the file _foo_, which contains "hello".

we then use _cat_ to sse the content of the file.

> how the _cat_ access the file _foo_?

we can use _strace_ command to check this, by tracing the system calls made by a program.

```cmdline
  prompt> strace cat foo
  ...
  open("foo", O_RDONLY|O_LARGEFILE) = 3
  read(3, "hello\n", 4096) = 6
  write(1, "hello\n", 6) = 6
  hello
  read(3, "", 4096) = 0
  close(3) = 0
  ...
  prompt>
```

### open

the first thing that it does is open the file for reading. the flag *O_RDONLY* indicates that the file is opened only for reading. the *O_LARGEFILE* indicates that the 64-bit offset is used. the call _open()_ succeeds and returns a file descriptor, which has value of 3.

> why does the first call to _open()_ returns 3?

this is because the file descriptor 0, 1 and 2 are respectively to the files: standard input, standard output and standard error.

### *read()*

- 1st argument: file descriptor
- 2nd argument: buffer where the result will be placed*
- 3rd argument: the fize of the buffer 

the call returns successfully returning the number of bytes it read.

*: (in the strace result, it shows the results of the read as "hello\n")

### *write()*

a single call to the file descriptor 1 (std out).

is used to write the "hello" to the screen as the _cat_ is meant to do.

maybe _cat_ can call the routine _printf()_ instead of _write()_, which figures out all the formatting details passed to it.

### *close()*

when the read returns 0 the program knows that the file is entirely read, so it calls _close()_.

> ✝️ each process maintainas an array of file descriptor, each of which refers to an entry in the system-wide **open file table**. each entry tracks which underlying file the descriptor refers to, the current offset, and other details such is readable or writable.

---

## 39.5 Reading and Writing, but Not Sequentially

sometimes, it is useful to be able to read or write to a specific offset within a file. 

if you build an index over a text document, and use it to look up a specific word, you may end up reading from some random offsets.

```
  off_t lseek(int fildes, off_t offset, int whence);
```

this functions just aims to changes a variable in OS memory that tracks, for a particular process, at which offset its next r/w will start

the first argument is a file descriptor, the second is the offset and the third is for historical reasons (determines exactly how the seek is performed).

_whence_ cases:
- SEEK_SET: the offset is set to offset bytes
- SEEK_CUR: the offset is set to its current location plus offset bytes.
- SEEK_END: the offset is set to the size of the file plus offset bytes.

ways to update the offset;
- when a read or write of _N_ bytes takes place, then offset = offset + _N_
- explicity with _lseek_

example of file struct:

```c
  struct file {
    int ref;
    char readable;
    char writable;
    struct inode *ip;
    uint off; //offset
  };
```

all the currently opened files in the system together are referred to as the **open file table**.  

```c
  struct {
    struct spinlock lock;
    struct file file[NFILE];
  } ftable;
```

### examples

#### reading a file of 300 bytes

![example 1](image-1.png)

important things to note:
- offset is initialized to zero
- offset is incremented by each _read()_ 
- returns zero from _read()_ when the file is entirely read

##### reading the same file twice

![example 2](image-2.png)

two file descriptors are allocated (3 and 4) and each refers to a different entry in the open file table (OFT). in the imagem OFT[10] and OFT[11] refers to the entries of the files

#### *lseek()*
process uses _lseek()_ to reposition the current offset before reading

![example 3](image-3.png)

here _lseek()_ call first sets the current offset to 200, the subsequent _read()_ then reads the next 50 bytes and updates the current offset accordingly.

---

## 39.6 Shared File Table Entries: *fork()* and *dup()*

each logical reading/writing of a file is independent, and each has its own current offset while it accesses the given file.

however, there are a few interesting cases where an entry in the open file table is _shared_. 

### *fork*

one of those cases occurs when a parent process creates a child process with _fork()_.

![fork-seek.c](image-4.png)

in this code, a parent creates a child and then waits for it to complete. the child adjusts the current offset via a call to _lseek()_ and then exits. finally the parent checks the current offset and prints out its value.

this is the output of the code 👇

![output do fork-seek.c](image-5.png)

this show the relationships that connect: each process's private descriptor array, the shared OFT entry and the reference from it to the underlying file-system inode. 

![processes sharing an OFTE](image-6.png)

first use of **reference count** here.  when a file table entry is shared, its refcount is incremented and only when both processes close the file will the entry be removed.

sharing open file table entries across parent and child is useful, if you create a number of processes that are cooperatively working on a task, they can write to the same output file.

### *dup*

one other case of sharing OFTE is with the use of _dup()_ system call.

allows a process to create a new file descriptor that refers to the same underlying open file as an existing descriptor.

code example

```c
  int main(int argc, char *argv[]) {
    int fd = open("README", O_RDONLY);
    assert(fd >= 0);
    int fd2 = dup(fd);
    // now fd and fd2 can be used interchangeably
    return 0;
  }
```

this is useful when writing a UNIX shell and performing operations like output redirection.

---

## 39.7 Writing Immediately with *fsync()*

the file system, for performance reasons, will **buffer** such writes in memory for some time and then write after this timestamp.

from the perspective of the calling application, writes is quickly completed and only in rare cases will data be lost.

for a DBMS, development of a correct recovery protocol requires the ability to force writes to disk from time to time.

to support these, the API provide the _fsync(int fd)_.

this call makes the file system responds by forcing all **dirty** data to disk for the file specified.

code snippet:
```c
  int fd = open("foo", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR); // opens the file
  assert(fd > -1);
  int rc = write(fd, buffer, size);
  assert(rc == size);
  rc = fsync(fd);
  assert(rc == 0);
```

sometimes, its also necessary to _fsync()_ the directory that contains the file _foo_. adding this ensures not only that the file itself is on disk, but that the file, if newly created, also is durably a part of the directory.

---

## 39.8 Renaming Files