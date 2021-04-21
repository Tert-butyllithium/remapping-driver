# Share memory by remapping address

A simple way to use mmap\&proc shared memory between kernel and userspace

All code based on [this](https://stackoverflow.com/questions/36762974/how-to-use-mmapproc-shared-memory-between-kernel-and-userspace) anster from Stack Overflow


## Known issues:

- cannot allocate more 1 page... (precisely, writing the address more than 1 page will crash)
- slow, incomprehensible slow (writing to these memory regions incurs incomprehensible overhead)
