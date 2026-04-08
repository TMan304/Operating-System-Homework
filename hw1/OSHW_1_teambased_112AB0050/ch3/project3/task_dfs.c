#include <linux/module.h> // kernel module載入與卸載、宣告GPL授權
#include <linux/kernel.h> // kermel空間用的函數和宏
#include <linux/init.h> // kernel初始化與退出
#include <linux/sched.h> // task_struct
#include <linux/list.h> // list_for_each
#include <linux/rcupdate.h> // rcu_read_lock() 確保遍歷過程中的一致性
#include <linux/sched/task.h> // 獲取pid=0的process


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WJC");
MODULE_DESCRIPTION("Print all tasks in kernel log.");


void print_dfs(struct task_struct *task) {
    printk(KERN_INFO "Task name: %s | State: %u | Process ID: %d\n", task->comm, READ_ONCE(task->__state), task->pid);
    struct task_struct *child;
    struct list_head *child_list;
    list_for_each(child_list, &task->children) {
        child = list_entry(child_list, struct task_struct, sibling);
        print_dfs(child);
    }
}

static int __init task_dfs_init(void) {
    printk(KERN_INFO "Hello, Kernel! Module loaded.\n");
    rcu_read_lock();
    print_dfs(&init_task);
    rcu_read_unlock();
    return 0;
}

static void __exit task_dfs_exit(void) {
    printk(KERN_INFO "Goodbye, Kernel! Module unloaded.\n");
}

module_init(task_dfs_init);
module_exit(task_dfs_exit);