# Q1 writeup

- For large datasets, the mmap approach is generally more efficient due to its ability to leverage virtual memory and reduce the number of system calls. The reduced overhead and optimized I/O operations can lead to significant performance improvements.

- In our program, we measured the time taken to process the file using both methods. The results showed that the mmap approach was faster than the fread approach. This is expected because mmap allows for more efficient file access and reduces the overhead associated with multiple system calls and memory copying.

##### Conclusion

For processing large datasets, the mmap approach is generally more efficient than the fread approach. The ability to map the file directly into memory and leverage virtual memory optimizations can lead to faster file access and reduced overhead. However, mmap may introduce complexity in handling file access patterns and requires careful management of memory-mapped regions. For smaller datasets or simpler applications, fread may still be a suitable and straightforward choice.
