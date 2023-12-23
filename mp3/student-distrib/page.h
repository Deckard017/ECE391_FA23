# ifndef PAGE_H
# define PAGE_H
#include "types.h"
#include "lib.h"
#include "x86_desc.h"

/* Number of entries in the page directory */
#define Page_Dir_Entry_num 1024
/* Number of entries in the page table */
#define Page_Tbl_Entry_num 1024
/* Video memory address */
#define VIDEO 0xB8000
/* Function to set a Page Directory Entry for 4KB pages */
void set_PDE_kb(int idx, int pres, int us, int ps, int global, int addr);
/* Function to set a Page Directory Entry for 4MB pages */
void set_PDE_mb(int idx, int pres, int us, int ps, int global, int addr);
/* Function to set a Page Table Entry */
void set_PTE(int idx, int us, int global, int addr, int pres);

void set_PTE_map(int idx, int us, int global, int addr, int pres);

void page_init();
void vidmem_remap(uint8_t term_id);

/* Union for 4KB Page Directory Entry */
typedef union Page_Dir_kb_Entry_T {
    uint32_t val;
    struct {
        uint32_t present                        :1;  /* Page present in memory */
        uint32_t r_w                            :1;  /* Read-write permissions */
        uint32_t user_supervisor                :1;  /* User or supervisor level */
        uint32_t write_through                  :1;  /* Page write through */
        uint32_t cache_disable                  :1;  /* Cache disabled */
        uint32_t accessed                       :1;  /* Page has been accessed */
        uint32_t dirty                          :1;  /* Page has been written to */
        uint32_t page_size                      :1;  /* Page size */
        uint32_t global                         :1;  /* Global page */
        uint32_t availablr_for_user             :3;  /* Available for user programs */
        uint32_t PG_addr                        :20; /* Page address */
    } __attribute__ ((packed));
} Page_Dir_kb_Entry_T;

/* Union for 4MB Page Directory Entry */
typedef union Page_Dir_mb_Entry_T
{
    uint32_t val;
    struct {
        uint32_t present                        :1;  /* Page present in memory */
        uint32_t r_w                            :1;  /* Read-write permissions */
        uint32_t user_supervisor                :1;  /* User or supervisor level */
        uint32_t write_through                  :1;  /* Page write through */
        uint32_t cache_disable                  :1;  /* Cache disabled */
        uint32_t accessed                       :1;  /* Page has been accessed */
        uint32_t dirty                          :1;  /* Page has been written to */
        uint32_t page_size                      :1;  /* Page size */
        uint32_t global                         :1;  /* Global page */
        uint32_t availablr_for_user             :3;  /* Available for user programs */
        uint32_t PAT                            :1;  /* Page attribute table index */
        uint32_t reserved2                      :9;  /* Reserved for future use */
        uint32_t PG_addr                        :10; /* Page address */
    } __attribute__ ((packed));
} Page_Dir_mb_Entry_T;

/* Union for Page Directory Entry */
typedef union Page_dir_Entry_T
{
    Page_Dir_kb_Entry_T kb; /* 4KB Page Directory Entry */
    Page_Dir_mb_Entry_T mb; /* 4MB Page Directory Entry */
}Page_Dir_Entry_T;




/* Union for Page Table Entry */
typedef union Page_Tbl_Entry_T {
    uint32_t val;
    struct {
        uint32_t present                        :1;  /* Page present in memory */
        uint32_t r_w                            :1;  /* Read-write permissions */
        uint32_t user_supervisor                :1;  /* User or supervisor level */
        uint32_t write_through                  :1;  /* Page write through */
        uint32_t cache_disable                  :1;  /* Cache disabled */
        uint32_t accessed                       :1;  /* Page has been accessed */
        uint32_t dirty                          :1;  /* Page has been written to */
        uint32_t page_attribute_table           :1;  /* Page attribute table index */
        uint32_t global                         :1;  /* Global page */
        uint32_t availablr_for_user             :3;  /* Available for user programs */
        uint32_t PG_addr                        :20; /* Page address */
    } __attribute__ ((packed));
} Page_Tbl_Entry_T;

/* Page Directory and Page Table */
typedef Page_Dir_Entry_T PG_DIR[Page_Dir_Entry_num];
typedef Page_Tbl_Entry_T PG_TBL[Page_Tbl_Entry_num];
extern PG_DIR Page_Dir; /* Page Directory */
extern PG_TBL Page_Tbl; /* Page Table */


#endif
