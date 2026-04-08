#include <linux/module.h> // kernel module載入與卸載、宣告GPL授權
#include <linux/kernel.h> // kermel空間用的函數和宏
#include <linux/init.h> // kernel初始化與退出
#include <linux/sched.h> // task_struct
#include <linux/sched/signal.h> // for_each_process


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WJC");
MODULE_DESCRIPTION("Print all tasks in kernel log.");


static int __init task_liner_init(void) {
    printk(KERN_INFO "Hello, Kernel! Module loaded.\n");
    struct task_struct *task;
    for_each_process (task) {
        printk(KERN_INFO "Task name: %s | State: %u | Process ID: %d\n", task->comm, READ_ONCE(task->__state), task->pid);
    }
    return 0;
}

static void __exit task_liner_exit(void) {
    printk(KERN_INFO "Goodbye, Kernel! Module unloaded.\n");
}

module_init(task_liner_init);
module_exit(task_liner_exit);