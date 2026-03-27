#include <linux/module.h> // kernel module載入與卸載、宣告GPL授權
#include <linux/kernel.h> // kermel空間用的函數和宏
#include <linux/init.h> // kernel初始化與退出
#include <linux/proc_fs.h> // proc相關


MODULE_LICENSE("GPL");
MODULE_AUTHOR("WJC");
MODULE_DESCRIPTION("Store seconds in /proc/seconds.");


static unsigned long start_jiffies;

static ssize_t read_seconds(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int sec = (jiffies - start_jiffies) / HZ;
    char buffer[32];
    int len = snprintf(buffer, 32, "%d", sec);
    if (*pos > 0) return 0;
    if (copy_to_user(usr_buf, buffer, len)) return -EFAULT;
    *pos = len;
    return len;
}

static const struct proc_ops my_proc_ops = {
    .proc_read = read_seconds
};

static int __init seconds_init(void) {
    start_jiffies = jiffies;
    struct proc_dir_entry *entry = proc_create("seconds", 0444, NULL, &my_proc_ops);
    if (entry == NULL) {
        printk(KERN_ERR "Error: Could not initialize /proc/seconds\n");
        return -ENOMEM;
    }
    else {
        printk(KERN_INFO "Hello, Kernel! Module loaded.\n");
        return 0;
    }
}

static void __exit seconds_exit(void) {
    remove_proc_entry("seconds", NULL);
    printk(KERN_INFO "Goodbye, Kernel! Module unloaded.\n");
}

module_init(seconds_init);
module_exit(seconds_exit);