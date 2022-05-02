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
void parseline (char prompt[100], char* argv[3]) {

    char * str = strtok(prompt, " ");
    int i=0;
    while(str != NULL){
        argv[i] = str;
        str = strtok(NULL, " ");
        i++;
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
        //execute(args);
    } while (1);
}

//memory struct

struct Memory {
    int address, data;
};

//page table struct
struct PageTable {
    int v_page_num, valid_bit, dirty_bit, page_num, time_stamp;
};

struct Memory main_memory[32];
struct Memory virtual_memory[128];
//virtual memory system will have 16 pages; each page contains 8 addresses
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

// read <virtual_addr>: This command prints the contents of a memory address. The command
// takes one argument which is the virtual address of the memory location to be read. When a page fault
// occurs, “A Page Fault Has Occurred\n” is printed to the screen before the contents of a memory
// address.

void read (struct Memory virtual_address) {
    //read the address of the memory address parameter
    int page; 
    int v_address;

    //look through each page in the page table
    for (page; page < sizeof(p_table); page++) {
        //if virtual address exists in page
        for (v_address; sizeof(virtual_memory); v_address++){
            //TODO: look through each address in virtual memory 
            // if (virtual_address.address == &p_table[page]) {
            //print the virtual address
            prinf(virtual_address);
        }
    }
    //if the address does not exist on any page, print an error message
    printf("A Page Fault Has Occurred\n");
}

// showmain <ppn>: This command prints the contents of a physical page in the main memory. The
// command takes one argument which is the number of the physical page to be printed. Since each
// page contains eight addresses, eight contents of addresses should be printed and together with their
// associated physical addresses. You can see the format from the example below which shows the
// contents of physical page 1 that includes the value 100 at address 8, the value 101 at address 9, the
// value 102 at address 10, and so on.

void showmain (int ppn) {

}

int main(int argc, char** argv) {
    init();
    if (argv[1] == NULL || strcmp (argv[1], "FIFO") == 0)
        fifo = 1;
    else if (strcmp (argv[1], "LRU") == 0)
        lru = 1;
    loop();
    return 0;
}