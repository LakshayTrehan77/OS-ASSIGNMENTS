#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/kernel.h>

static int __init count_running_tasks_init(void) {
    struct task_struct *task;
    int running_tasks = 0;

    printk(KERN_INFO "Counting currently running tasks:\n");

    for_each_process(task) {
        if (task->__state == TASK_RUNNING) {
            running_tasks++;
        }
    }

    printk(KERN_INFO "Number of running tasks: %d\n", running_tasks);

    return 0;
}

static void __exit count_running_tasks_exit(void) {
    // Cleanup code (if needed)
}

module_init(count_running_tasks_init);
module_exit(count_running_tasks_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Count Running Tasks Kernel Module");
MODULE_AUTHOR("Your Name");
