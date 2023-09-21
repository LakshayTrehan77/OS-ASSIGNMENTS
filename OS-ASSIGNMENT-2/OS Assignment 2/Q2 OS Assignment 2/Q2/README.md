Linux Scheduling Policies Benchmarking
# This project's goal is to use three processes that execute counting tasks to illustrate the use and effectiveness of three alternative Linux scheduling rules (SCHED_OTHER, SCHED_RR, and SCHED_FIFO).

This project creates three processes that carry out counting tasks in order to test Linux's scheduling policies. Each process compares its performance using a different scheduling approach.

1. Processess Functioning

Process Number 1 (SCHED_OTHER, nice: 0) - Fork() is used to start this procedure. It launches a different C file for counting using the execl() system function. Scheduling Guidelines: SCHED_OTHER Priority: Priority 0 (standard)
(SCHED_RR, default priority).
Process Number 2 - It utilizes fork() to start and uses execl() to run the counting program, same as Process 1.
Scheduling Guidelines: Priority SCHED_RR: Default priority
(SCHED_FIFO, default priority).
Process Number 3 - Once more, fork() was used, and execl() was used for counting. Scheduling Guidelines: SCHED_FIFO
Priority: The default setting

2. Benchmarking: We utilize clock timestamps received by calling clock_gettime() to benchmark the processes. Prior to forking and following each process termination (identified by waitpid()), timestamps are kept.

3. Data Analysis: To create histograms indicating the completion times for each scheduling policy, the recorded data is evaluated. The histograms are plotted using Python (matplotlib).

4. Result: The histograms show how long it takes each process to finish the counting task while using various scheduling rules. Comparative study demonstrates how well each policy manages the processes.

5. The execution time is stored in an another file (execution_time.txt) for creating the graph. This is done to make the code more structured and error free.

The y axis has taken the data from execution.txt.
The x axis has taken the data from policies.txt

In the parent program, scheduling policies of child processes are mentioned in the policies.txt and their execution times of the corresponding child process are mentioned in execution_time.txt

6. Conclusion: The scheduling policy that handles processes with counting tasks more effectively may be determined based on the benchmarking findings. The conclusions will offer guidance on selecting the best scheduling policy for certain applications.