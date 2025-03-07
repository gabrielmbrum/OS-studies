// the registers xv6 will save and restore
// to sopt and subsequently restart a process 
struct context {
  int eip;
  int esp;
  int ebx;
  int ecx;
  int edx;
  int esi;
  int edi;
  int ebp;
};

// the different states a procces can be in
enum proc_state { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// the information xv6 tracks about each process
// including its register context and state
struct proc {
  char *mem;                    // start of process memory
  uint sz;                      // size of process memory
  char *kstack;                  // bottom of kernel stack for this process

  enum proc_state state;        // process state
  int pid;                      // process id
  struct proc *parent;          // parent process
  void *chan;                   // if non-zero, sleeping on chan
  int killed;                   // if non-zero, have been killed
  struct file *ofile[NOFILE];   // open files
  struct inode *cwd;            // current directory
  struct context context;       // switch here to run process
  struct trapframe *tf;         //trap frame for the current interrupt
};