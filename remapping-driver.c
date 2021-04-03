#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include "address_translate.h"

#define PROC_MEMSHARE_DIR "memshare"
#define PROC_MEMSHARE_INFO "phymem_info"
#define PROC_MMAP_FILE "mmap"


/* alloc one page. 4096 bytes */
#define PAGE_ORDER 0
/* this value can get from PAGE_ORDER */
#define PAGES_NUMBER 1
#define BUF_SIZE 4096

struct proc_dir_entry *proc_memshare_dir;
// unsigned long kernel_memaddr = 0;
// char kernel_memaddr[BUF_SIZE];
char kernel_memaddr[BUF_SIZE];
unsigned long kernel_memsize = 0;

int proc_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long phyaddr;
    // page = virt_to_phys((void *)kernel_memaddr) >> PAGE_SHIFT;
    phyaddr = get_phyaddr_at(kernel_memaddr);

    if (remap_pfn_range(vma, vma->vm_start, phyaddr, (vma->vm_end - vma->vm_start),
                        vma->vm_page_prot))
    {
        printk("remap failed...");
        return -1;
    }
    vma->vm_flags |= (VM_DONTDUMP | VM_DONTEXPAND);
    printk("remap_pfn_rang page:[%lu] ok.\n", phyaddr);
    return 0;
}

static int proc_show_meminfo(struct seq_file *m, void *v)
{
    seq_printf(m, "%08lx %lu\n", get_phyaddr_at(kernel_memaddr), kernel_memsize);
    return 0;
}

static int proc_open_meminfo(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show_meminfo, NULL);
}

static const struct file_operations read_phymem_info_fops = {
    .owner = THIS_MODULE,
    .open = proc_open_meminfo,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release};

static const struct file_operations proc_mmap_fops = {
    .owner = THIS_MODULE,
    .mmap = proc_mmap};

static void write_something(unsigned long addr)
{
    char *buffer_addr = (void *)addr;
    int i = 0;
    // for (; i < 4096 * (1 << PAGE_ORDER); i++)
    // {
    //     buffer_addr[i] = 'a' + i % 26;
    // }
    char *src = "114514";
    strncpy(buffer_addr, src, strlen(src));
}

static int __init init(void)
{
    /* build proc dir "memshare"and two proc files: phymem_addr, phymem_size in the dir */
    proc_memshare_dir = proc_mkdir(PROC_MEMSHARE_DIR, NULL);
    proc_create_data(PROC_MEMSHARE_INFO, 0, proc_memshare_dir, &read_phymem_info_fops, NULL);
    proc_create_data(PROC_MMAP_FILE, 0, proc_memshare_dir, &proc_mmap_fops, NULL);

    /* alloc one page */
    // kernel_memaddr = __get_free_pages(GFP_KERNEL, PAGE_ORDER);
    // kernel_memaddr = kmalloc(BUF_SIZE,GFP_KERNEL);
    if (!kernel_memaddr)
    {
        printk("Allocate memory failure!/n");
    }
    else
    {
        // SetPageReserved(virt_to_page(kernel_memaddr));
        // kernel_memsize = PAGES_NUMBER * PAGE_SIZE;
        kernel_memsize = BUF_SIZE;
        printk("Allocate memory success!. The phy mem addr=%08lx, size=%lu\n", get_phyaddr_at(kernel_memaddr), kernel_memsize);
        write_something(kernel_memaddr);
    }
    return 0;
}

static void __exit fini(void)
{
    printk("Clear page reserved");
    // ClearPageReserved(virt_to_page(kernel_memaddr));
    
    printk("free pages...");
    // free_pages(kernel_memaddr, PAGE_ORDER);
    // kfree(kernel_memaddr);
    printk("remove ...");
    remove_proc_entry(PROC_MEMSHARE_INFO, proc_memshare_dir);
    remove_proc_entry(PROC_MMAP_FILE, proc_memshare_dir);
    remove_proc_entry(PROC_MEMSHARE_DIR, NULL);
    return;
}
module_init(init);
module_exit(fini);