/*
 *the program first define 4 pgd items,pag_base addr
 *and throught *pgd at base to add offset,the result is every pagetable addr
 *the pagetable_base is 0x2000,and *page_entry add phy_add to result is every page addr
 *the pgd_base to store cr3
 * */

#define NR_PGT 0x4					//4 page global directory
#define PGD_BASE    (unsigned int*)0x1000		//the second pageframe	
#define PAGE_OFFSET (unsigned init)0x2000		

#define PTE_PRE 0X01
#define PTE_RW  0X02
#define PTE_USR 0X04

void page_init()
{
	int pages = NR_PGT;

	unsigned int page_offset = PAGE_OFFSET;
	unsigned int *pgd        = PGD_BASE;
	unsigned int phy_add     = 0x0000;

	unsigned int *pgt_entry = (unsigned int *)0x2000;

	while(pages--)					
	{
		*pgd++ = page_offset | PTE_USR |PTE_RW |PTE_PRE;
		page_offset += 0x1000;
	}
	pgd = PGD_BASE;

	while(phy_add < 0x1000000)
	{
		*pgt_entry++ = phy_add |PTE_PRE | PTE_RW | PTE_USR;
		phy_add += 0x1000;
	}

	__asm__ volatile_("movl %0, %%cr3;"
			  "movl %%cr0,%%eax;"
			  "orl  $0x80000000,%%eax;"
			  "movl %%eax,%%cr0;"::"r"(pgd):"memory","%eax"
			);
}
