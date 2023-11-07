Kernel Module with a Custom System Call

# The goal of this project is to develop a custom kernel module that implements a system call to read the task_struct entries for processes and output the total number of active processes. Only once the module has been loaded should this system call be used.

1. Initialization: The project entails creating a kernel module that increases the Linux kernel's capability. The module offers a unique system function for finding out how many processes are active at any one time.

2. Application:
    # i. Kernel Module
    The module implements the required functions for the special system call and is written in C. In order to access the task_struct data structure, the header file is necessary.

    # ii. System Call Custom
    The kernel module implements the system call. The task_struct is scanned for entries to determine how many processes are active at any one time.

3. Compiled and loaded: The Linux headers are necessary for the module to be compiled and loaded. The following commands can be used to install them if they are not already there:

# install linux-headers with sudo apt-get
# install linux-headers-generic with sudo apt-get

Using the proper instructions, the module is then built and loaded into the kernel.

4. Usefulness: The custom system call becomes operational when the module has been loaded. It uses the task_struct data structure to count the number of active processes after being invoked.

5. Verification and Testing: Concurrently running processes and using the system call to check the precision of the count can be used to evaluate the system call's operation.

6. Finalization: The project shows how to successfully construct a unique system call inside of a kernel module. The system call successfully accesses the task_struct and returns the total number of processes that are active.