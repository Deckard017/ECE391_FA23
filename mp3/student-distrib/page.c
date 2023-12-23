#include "page.h"
#include "terminal.h"
// create the table and directory
PG_DIR Page_Dir __attribute__((aligned(4096)));
PG_TBL Page_Tbl __attribute__((aligned(4096)));
PG_TBL Page_Tbl_Map __attribute__((aligned(4096)));

void set_PTE_map(int idx, int us, int global, int addr, int pres)
{
    Page_Tbl_Map[idx].present=pres;
    Page_Tbl_Map[idx].r_w = 1; 
    Page_Tbl_Map[idx].user_supervisor = us;
    Page_Tbl_Map[idx].write_through = 0;
    Page_Tbl_Map[idx].cache_disable = 0;  
    Page_Tbl_Map[idx].accessed = 0; 
    Page_Tbl_Map[idx].dirty = 0;    
    Page_Tbl_Map[idx].page_attribute_table=0;
    Page_Tbl_Map[idx].global = global;  
    Page_Tbl_Map[idx].availablr_for_user = 0;
    Page_Tbl_Map[idx].PG_addr = addr;
}





/*
 * set_PDE_kb
 *   DESCRIPTION: Sets the Page Directory Entry with the given parameters
 *   INPUTS: Page_Dir -- the page directory to be set
 *           idx -- the index of the page directory entry
 *           pres -- the present flag
 *           us -- the user/supervisor flag
 *           ps -- the page size flag
 *           global -- the global flag
 *           addr -- the address of the page directory entry
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies the Page Directory Entry
 */
void set_PDE_kb(int idx, int pres, int us, int ps, int global, int addr){
    Page_Dir[idx].kb.present = pres;
    Page_Dir[idx].kb.r_w = 1; 
    Page_Dir[idx].kb.user_supervisor = us;
    Page_Dir[idx].kb.write_through = 0;
    Page_Dir[idx].kb.cache_disable = 0;  
    Page_Dir[idx].kb.accessed = 0; 
    Page_Dir[idx].kb.dirty = 0;    
    Page_Dir[idx].kb.page_size = ps; 
    Page_Dir[idx].kb.global = global;  
    Page_Dir[idx].kb.availablr_for_user = 0;
    Page_Dir[idx].kb.PG_addr = addr;
}

/*
    * set_PDE_mb
    *   DESCRIPTION: Sets the Page Directory Entry with the given parameters
    *   INPUTS: Page_Dir -- the page directory to be set
    *           idx -- the index of the page directory entry
    *           pres -- the present flag
    *           us -- the user/supervisor flag
    *           ps -- the page size flag
    *           global -- the global flag
    *           addr -- the address of the page directory entry
    *   OUTPUTS: none
    *   RETURN VALUE: none
    *   SIDE EFFECTS: modifies the Page Directory Entry
*/
void set_PDE_mb( int idx, int pres, int us, int ps, int global, int addr){
    Page_Dir[idx].mb.present = pres;
    Page_Dir[idx].mb.r_w = 1; 
    Page_Dir[idx].mb.user_supervisor = us;
    Page_Dir[idx].mb.write_through = 0;
    Page_Dir[idx].mb.cache_disable = 0;  
    Page_Dir[idx].mb.accessed = 0; 
    Page_Dir[idx].mb.dirty = 0;    
    Page_Dir[idx].mb.page_size = ps; 
    Page_Dir[idx].mb.global = global;  
    Page_Dir[idx].mb.availablr_for_user = 0;
    Page_Dir[idx].mb.PAT = 0;
    Page_Dir[idx].mb.reserved2 = 0;
    Page_Dir[idx].mb.PG_addr = addr;
}



/*
 * set_PTE
 *   DESCRIPTION: Sets the Page Table Entry with the given parameters
 *   INPUTS: Page_Tbl -- the page table to be set
 *           idx -- the index of the page table entry
 *           us -- the user/supervisor flag
 *           global -- the global flag
 *           addr -- the address of the page table entry
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies the Page Table Entry
 */
void set_PTE(int idx, int us, int global, int addr, int pres){
    Page_Tbl[idx].present = pres;
    Page_Tbl[idx].r_w = 1;    
    Page_Tbl[idx].user_supervisor = us;
    Page_Tbl[idx].write_through = 0;
    Page_Tbl[idx].cache_disable = 0;
    Page_Tbl[idx].accessed = 0;
    Page_Tbl[idx].dirty = 0;
    Page_Tbl[idx].page_attribute_table = 0;
    Page_Tbl[idx].global = global;
    Page_Tbl[idx].availablr_for_user = 0;
    Page_Tbl[idx].PG_addr = addr;
}



/*
 * set_cr
 *   DESCRIPTION: Sets the control registers for paging
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: modifies the control registers
 */
void set_cr(){
    asm volatile(
       "movl %0, %%eax;             \
        movl %%eax, %%cr3;          \
        movl %%cr4, %%eax;          \
        orl $0x00000010, %%eax;     \
        movl %%eax, %%cr4;          \
        movl %%cr0, %%eax;          \
        orl $0x80000000, %%eax;     \
        movl %%eax, %%cr0;"
        :
        : "r" (&Page_Dir)
        : "%eax"
    );
}

/*
    * page_init
    *   DESCRIPTION: Initializes the page directory and page table
    *   INPUTS: none
    *   OUTPUTS: none
    *   RETURN VALUE: none
    *   SIDE EFFECTS: modifies the page directory and page table
*/
void page_init(void){
    int i;
    for(i = 0; i < Page_Dir_Entry_num; i++){//initialize page directory
        if(i==0){
            set_PDE_kb( i, 1, 0, 0, 0,(int) Page_Tbl>>12);//4kb
        } 
        
        else if(i==1){
            set_PDE_mb( i, 1, 0, 1, 1, 1);//initializing kernel 4MB page to 1

        }
        else if (i==33) {//33->0x8400000
            set_PDE_kb( i, 1, 1, 0, 0, (int) Page_Tbl_Map>>12);//initializing video memory to 4kb
        }
        else{
            set_PDE_mb( i, 0, 0, 0, 0, 0);//initializing rest of the page directory to 0
        }
    }
    for(i = 0; i < Page_Tbl_Entry_num; i++){//initialize page table
        if(i==(VIDEO)>>12){//video memory
            set_PTE( i, 0, 0, i, 1); //set 4kb video memory
        }else{
            set_PTE( i, 0, 0, i, 0); //set other memory to 0
        }
        set_PTE_map(i,1,0,i,0);
    }
    set_PTE((VIDEO>>12), 0, 0, (VIDEO>>12), 1);
    set_PTE((VIDEO>>12)+1, 0, 0, (VIDEO>>12)+1, 1);
    set_PTE_map((VIDEO>>12)+1, 1, 0, (VIDEO>>12)+1, 1);
    set_PTE((VIDEO>>12)+2, 0, 0, (VIDEO>>12)+2, 1);
    set_PTE_map((VIDEO>>12)+2, 1, 0,( VIDEO>>12)+2, 1);
    set_PTE((VIDEO>>12)+3, 0, 0, (VIDEO>>12)+3, 1);
    set_PTE_map((VIDEO>>12)+3, 1, 0, (VIDEO>>12)+3, 1);
    set_cr();//set control registers
}



void vidmem_remap(uint8_t term_id){
    if(term_id==cur_terminal){
        set_PTE(VIDEO>>12, 0, 0, VIDEO>>12, 1);
        set_PTE_map(VIDEO>>12, 1, 0, VIDEO>>12, 1);
    }else{
        set_PTE(VIDEO>>12, 0, 0, (VIDEO>>12)+term_id+1, 1);
        set_PTE_map(VIDEO>>12, 1, 0, (VIDEO>>12)+term_id+1, 1);
    }
    asm volatile(
       "movl %0, %%eax;             \
        movl %%eax, %%cr3;"
        :
        : "r" (&Page_Dir)
        : "%eax"
    );
}
