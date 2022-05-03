#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
int fifo = 0, lru = 0;
int least_recent[] = {0, 0, 0, 0}; 
int next_main_mem = 0; 
int clock = 0;
void parseline (char prompt[100], char* argv[3]) {

    char * str = strtok(prompt, " ");
    int i=0;
    while(str != NULL){
        argv[i] = str;
        str = strtok(NULL, " ");
        i++;
    }
}

//memory struct
struct Memory {
    int address, data;
};

//page table struct
struct PageTable {
    int v_page_num, valid_bit, dirty_bit, page_num, time_stamp;
};

//each page contains 8 addresses
//main memory has 4 pages
struct Memory main_memory[32];
//virtual memory system will have 16 pages;
struct Memory virtual_memory[128];
struct PageTable p_table[16];

void init() {
    //Initalize main_memory, virtual_memory, and p_table
    int i;
    for (i = 0; i < sizeof(main_memory)/sizeof(main_memory[0]); i++) {
        main_memory[i].data = -1;
        main_memory[i].address = i;
    }
    for (i = 0; i < sizeof(virtual_memory)/sizeof(virtual_memory[0]); i++) {
        virtual_memory[i].data = -1;
        virtual_memory[i].address = i;
    }
    for (i = 0; i < sizeof(p_table)/sizeof(p_table[0]); i++) {
        p_table[i].v_page_num = p_table[i].page_num = i;
        p_table[i].valid_bit = p_table[i].dirty_bit = 0;
        p_table[i].time_stamp = 0;
    }
}
void print_ptable(){
    int i; 
    for (i = 0; i < sizeof(p_table)/sizeof(p_table[0]); i++) {
        printf("%d:%d:%d:%d\n", p_table[i].v_page_num,  p_table[i].valid_bit,  p_table[i].dirty_bit, p_table[i].page_num); 
    }
}

// this function will handle the lru and fifo by returning the value in main memory that should be accessed
int return_page_num(){
    int pageNum;
    next_main_mem ++; 
    //if page fault occurs
    if (next_main_mem > 4){
        //FIFO
        if (fifo == 1) {
            //return page number of the page that has been the longest in the main memory
            pageNum = least_recent[0];
            // handling changing the ptable when page is no longer in main memory
            p_table[pageNum/8].valid_bit = 0;
            p_table[pageNum/8].dirty_bit = 0;
            return pageNum;
        }
        //LRU - use global clock
        else if (lru == 1) {
            //return page number of the page that is the least recently used/accessed in the main memory
            pageNum = clock;
            // handling changing the ptable when page is no longer in main memory
            p_table[pageNum/8].valid_bit = 0;
            p_table[pageNum/8].dirty_bit = 0;
            return pageNum;
        }
    }
    return next_main_mem - 1; 
}
// read <virtual_addr>: This command prints the contents of a memory address. The command
// takes one argument which is the virtual address of the memory location to be read. When a page fault
// occurs, “A Page Fault Has Occurred\n” is printed to the screen before the contents of a memory
// address.
void read_f (int address ) {
    // every time write is called, update clock to be the address most recently accessed
    clock = p_table[address/8].page_num;
    //read the address of the memory address parameter
    if (p_table[address/8].valid_bit == 0){
        printf("A Page Fault Has Occurred\n");
        int pn = return_page_num(); 
        least_recent[pn] = next_main_mem;
        p_table[address/8].valid_bit = 1;
        p_table[address/8].page_num = pn;
        int i; 
        for (i = 0; i < 8; i++){
            main_memory[(pn*8)+i].data = virtual_memory[((address/8)*8) + i].data;
        }

    }
    printf("%i\n" , virtual_memory[address].data); 
}

// The command writes data to a memory location. The command
// takes two arguments, the virtual address to write the data to, and an integer which will be written to
// the address. When a page fault occurs, “A Page Fault Has Occurred\n” is printed on the screen.
void write_f (int address, int data){
    //every time write is called, update clock to be the address most recently accessed
    clock = p_table[address/8].page_num;
    if (p_table[address/8].valid_bit == 0){
        printf("A Page Fault Has Occurred\n");
        int pn = return_page_num(); 
        p_table[address/8].valid_bit = 1;
        p_table[address/8].dirty_bit = 1;
        p_table[address/8].page_num = pn;
        int i; 
        virtual_memory[address].data = data; 
        for (i = 0; i < 8; i++){
            main_memory[(pn*8)+i].data = virtual_memory[((address/8)*8) + i].data;
        }
    }
    else{
        int pn = p_table[address/8].page_num; 
        virtual_memory[address].data = data; 
        main_memory[(pn*8)+(address%8)].data = data; 
    }
}

// showmain <ppn>: This command prints the contents of a physical page in the main memory. The
// command takes one argument which is the number of the physical page to be printed. Since each
// page contains eight addresses, eight contents of addresses should be printed and together with their
// associated physical addresses. You can see the format from the example below which shows the
// contents of physical page 1 that includes the value 100 at address 8, the value 101 at address 9, the
// value 102 at address 10, and so on.

void showmain (int ppn) {
    int i;
    for(i = (ppn*8); i < (ppn*8) + 8; i++){
        printf("%i:%i\n", main_memory[i].address, main_memory[i].data); 
    }
}
void loop() {
    char input[100];
    char* args[3];
    do {
        printf("> ");
        fgets(input, 80, stdin);
        input[strlen(input)-1] = '\0';
        parseline(input, args);
        if (strcmp(args[0], "quit") == 0){
            exit(0);
        }
        else if (strcmp(args[0], "showptable") == 0){
            print_ptable();
        }
        else if (strcmp(args[0], "read") == 0){
            read_f(atoi(args[1]));
        }
        else if (strcmp(args[0], "showmain") == 0){
            showmain(atoi(args[1]));
        }
    } while (1);
}



int main(int argc, char** argv) {
    printf("TODO: \n - return_page_num function which handles lru and fifo \n - handling changing the ptable when page is no longer in main memory\n");
    if (argv[1] == NULL || strcmp (argv[1], "FIFO") == 0)
        fifo = 1;
    else if (strcmp (argv[1], "LRU") == 0)
        lru = 1;
    init();
    loop();

    return 0;
}