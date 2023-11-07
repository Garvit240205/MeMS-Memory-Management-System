/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<math.h>


/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096

// int mems_va;
// int mems_pa;

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/

typedef struct Node {
    void* add_start;
    void* add_end;
    struct Node* next;
    struct Node* prev;
    struct ChainNode* child;
    int pages;
    size_t size;
} Node;

typedef struct ChainNode {
    void* add_start;
    void* add_end;
    struct ChainNode* next;
    struct ChainNode* prev;
    int type;
    size_t size;
} ChainNode;

typedef struct {
    void* mems_virt_addr;
    void* mems_phys_addr;
} mems_mapping;

mems_mapping mappings[100000]={0};
int mappings_count = 0;

Node* head=NULL;
void* node_allocate=NULL;
void* arr[1000];
int indexing=0;
int check=0;

void mems_init(){

    // head=NULL;
    int fd = open("/dev/zero", O_RDWR);
    node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
    // mems_va=0;
}


/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/

void mems_finish(){
    Node* start=head;
    while(start->next!=NULL){
        start=start->next;
        if (munmap(start->add_start,start->pages*PAGE_SIZE) == -1) {
            perror("munmap failed");
        }
    }
    head->next=NULL;
    while(indexing>=0){
        if (munmap(arr[indexing],PAGE_SIZE) ==-1){
            perror("munmap failed");
        }
        indexing--;
    }

}


/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 

void* mems_malloc(size_t size){
    
    if(head==NULL){
        int fd = open("/dev/zero", O_RDWR);
       
        head=node_allocate;
        node_allocate+=sizeof(Node);
        check+=sizeof(Node);
        if(check+sizeof(Node)>PAGE_SIZE){
            node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
            arr[indexing]=node_allocate;
            indexing++;
        }

        // if (head == MAP_FAILED) {
        //     perror("mmap failed");
        // }
        void* addr;
        int quotient = (int)ceil((double)size / PAGE_SIZE);
        if(quotient==0) addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
        // else if(quotient*PAGE_SIZE<=PAGE_SIZE) addr = mmap(NULL, (quotient)*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        else {
            // quotient+=1;
            addr = mmap(NULL, (quotient)*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        }
        // else if(size>PAGE_SIZE && size<=PAGE_SIZE*2) addr = mmap(NULL, PAGE_SIZE*2, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        // else if(size>PAGE_SIZE*2 && size<=PAGE_SIZE*3) addr = mmap(NULL, PAGE_SIZE*3, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        // else if(size>PAGE_SIZE*3) addr = mmap(NULL, PAGE_SIZE*4, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        if (addr == MAP_FAILED) {
            perror("mmap failed");
        }
        int temp_size;
        if(quotient==0) {
            quotient+=1;
            temp_size=PAGE_SIZE;
        }
        else temp_size=quotient*PAGE_SIZE;
        Node* main_node1=node_allocate;
        node_allocate+=sizeof(Node);
        check+=sizeof(Node);
        if(check+sizeof(Node)>PAGE_SIZE){
            node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
            arr[indexing]=node_allocate;
            indexing++;
        }
        ChainNode* chain1_node=node_allocate;
        node_allocate+=sizeof(ChainNode);
        check+=sizeof(ChainNode);
        if(check+sizeof(ChainNode)>PAGE_SIZE){
            node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
            arr[indexing]=node_allocate;
            indexing++;
        }

        // if (main_node1 == MAP_FAILED) {
        //     perror("mmap failed");
        // }


        head->next=main_node1;
        main_node1->pages=quotient;
        main_node1->size=main_node1->pages*PAGE_SIZE;
        main_node1->child=chain1_node;
        main_node1->add_start=addr;
        main_node1->add_end=addr+main_node1->size-1;
        // if(temp_size<size){
        //     chain1_node->next=NULL;
        //     chain1_node->size=temp_size;
        //     chain1_node->type=1;//process
        //     chain1_node->add_start=addr;
        //     chain1_node->add_end=addr+temp_size-1;
        //     ChainNode* temp=mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        //     chain1_node->next=temp;
        //     temp->prev=chain1_node;
        //     temp->size=size-temp_size;
        //     temp->type=0;//Hole
        //     temp->add_start=chain1_node->add_end+1;
        //     temp->add_end=chain1_node->add_end+size-1;
        //     temp->next=NULL;
        //     return main_node1->add_start;
        // }
        void* toReturn;
        if(temp_size==size){
            chain1_node->next=NULL;
            chain1_node->size=temp_size;
            chain1_node->type=1;
            chain1_node->add_start=addr;
            chain1_node->add_end=addr+temp_size-1;
            void* mems_virt_addr = (void*) mappings_count;
            mappings[mappings_count].mems_virt_addr = mems_virt_addr;
            mappings[mappings_count].mems_phys_addr = addr;
            mappings_count++;
            toReturn=mems_virt_addr;
            for(int i=1;i<size;i++){
                mems_virt_addr = (void*) mappings_count;
                mappings[mappings_count].mems_virt_addr = mems_virt_addr;
                mappings[mappings_count].mems_phys_addr = addr+i;
                mappings_count++;
            }
            
            // return main_node1->add_start;
            return toReturn;
        }
        else{
            chain1_node->next=NULL;
            chain1_node->size=size;
            chain1_node->type=1;//process
            chain1_node->add_start=addr;
            chain1_node->add_end=addr+size-1;//to think
            temp_size-=size;
            void* mems_virt_addr = (void*) mappings_count;
            mappings[mappings_count].mems_virt_addr = mems_virt_addr;
            mappings[mappings_count].mems_phys_addr = addr;
            mappings_count++;
            toReturn=mems_virt_addr;
            for(int i=1;i<size;i++){
                void* mems_virt_addr = (void*) mappings_count;
                mappings[mappings_count].mems_virt_addr = mems_virt_addr;
                mappings[mappings_count].mems_phys_addr = addr+i;
                mappings_count++;
            }
        }
        int cnt=0;

        // while(temp_size>=size){
        //     ChainNode* temp=mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        //     temp->size=size;
        //     temp->next=NULL;
        //     temp->type=1;//process
        //     chain1_node->next=temp;
        //     temp_size-=size;
        //     temp->prev=chain1_node;
        //     temp->add_start=chain1_node->add_end+1;
        //     temp->add_end=chain1_node->add_end+temp->size;
        //     chain1_node=temp;
        //     // cnt++;
        // }
        
        if(temp_size!=0){
            // cnt+=1;
            ChainNode* temp=node_allocate;
            node_allocate+=sizeof(ChainNode);
            check+=sizeof(ChainNode);
            if(check+sizeof(ChainNode)>PAGE_SIZE){
                node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
                arr[indexing]=node_allocate;
                indexing++;
            }

            temp->size=temp_size;
            temp->next=NULL;
            temp->type=0;//Hole
            chain1_node->next=temp;
            temp->prev=chain1_node;
            temp->add_start=chain1_node->add_end+1;
            temp->add_end=chain1_node->add_end+temp->size-1;
            chain1_node=temp;
            void* mems_virt_addr = (void*) mappings_count;
            mappings[mappings_count].mems_virt_addr = mems_virt_addr;
            mappings[mappings_count].mems_phys_addr = temp->add_start;
            mappings_count++;
            for(int i=1;i<temp_size;i++){
                void* mems_virt_addr = (void*) mappings_count;
                mappings[mappings_count].mems_virt_addr = mems_virt_addr;
                mappings[mappings_count].mems_phys_addr = temp->add_start+i;
                mappings_count++;
            }
            temp_size-=size;
            
            
        }
        return toReturn;
    }
    else{
        int fd = open("/dev/zero", O_RDWR);//as MAP_ANONYMOUS was not available
        Node* start=head;
        Node* temp;
        while(start->next!=NULL){
            start=start->next;
            if(start->child){
                ChainNode* inner1=start->child;
                while(inner1!=NULL){
                    if(inner1->type==1){
                        inner1=inner1->next;
                    }
                    else{
                        if(inner1->size>size){
                            ChainNode* new=node_allocate;
                            node_allocate+=sizeof(ChainNode);
                            check+=sizeof(ChainNode);
                            if(check+sizeof(ChainNode)>PAGE_SIZE){
                                node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
                                arr[indexing]=node_allocate;
                                indexing++;
                            }

                            ChainNode* inner1_prev=inner1->prev;
                            new->size=size;
                            new->type=1;//process
                            new->add_start=inner1->add_start;
                            new->add_end=inner1->add_start+new->size-1;
                            new->next=inner1;
                            if(inner1->prev) new->prev=inner1->prev;
                            else new->prev=NULL;

                            int addrStart=0;
                            int totSize=inner1->size;
                            for(int i=0;i<100000;i++){
                                if(mappings[i].mems_phys_addr==inner1->add_start){
                                    addrStart=i;
                                    break;
                                }
                            }

                            if(inner1_prev){
                                inner1_prev->next=new;
                            }
                            inner1->prev=new;
                            inner1->size-=size;
                            inner1->type=0;//Hole
                            inner1->add_start=new->add_end+1;
                            inner1->add_end=inner1->add_start+inner1->size-1;
                            for(int i=0;i<new->size;i++){
                                void* mems_virt_addr = (void*) addrStart;
                                mappings[addrStart].mems_virt_addr = mems_virt_addr;
                                mappings[addrStart].mems_phys_addr = new->add_start+i;
                                addrStart++;
                            }

                            for(int i=0;i<inner1->size;i++){
                                void* mems_virt_addr = (void*) addrStart;
                                mappings[addrStart].mems_virt_addr = mems_virt_addr;
                                mappings[addrStart].mems_phys_addr = inner1->add_start+i;
                                addrStart++;
                            }
                            if(start->child==inner1){
                                start->child=new;
                            }
                            for(int i=0;i<100000;i++){
                                if(mappings[i].mems_phys_addr==new->add_start){
                                    return mappings[i].mems_virt_addr;
                                }
                            }
                            return NULL;
                        }
                        else if(inner1->size==size){
                            inner1->type=1;//process
                            for(int i=0;i<100000;i++){
                                if(mappings[i].mems_phys_addr==inner1->add_start){
                                    return mappings[i].mems_virt_addr;
                                }
                            }
                            return start->add_start;
                        }
                        else inner1=inner1->next;
                    }
                }
            }
        }
        void* addr;
        int quotient = (int)ceil((double)size / PAGE_SIZE);
        if(quotient==0) addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
        // else if(quotient*PAGE_SIZE<=PAGE_SIZE) addr = mmap(NULL, quotient*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        else {
            // quotient+=1;
            addr = mmap(NULL, (quotient)*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        }
        // else if(size>PAGE_SIZE && size<=PAGE_SIZE*2) addr = mmap(NULL, PAGE_SIZE*2, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        // else if(size>PAGE_SIZE*2 && size<=PAGE_SIZE*3) addr = mmap(NULL, PAGE_SIZE*3, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        // else if(size>PAGE_SIZE*3) addr = mmap(NULL, PAGE_SIZE*4, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        if (addr == MAP_FAILED) {
            perror("mmap failed");
        }
        int temp_size;
        if(quotient==0) {
            quotient+=1;
            temp_size=PAGE_SIZE;
        }
        else temp_size=quotient*PAGE_SIZE;
        ChainNode* inner=start->child;
        while(inner->next!=NULL){
            inner=inner->next;
        }
        Node* main_node1=node_allocate;
        node_allocate+=sizeof(Node);
        check+=sizeof(Node);
        if(check+sizeof(Node)>PAGE_SIZE){
            node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
            arr[indexing]=node_allocate;
            indexing++;
        }

        ChainNode* chain1_node=node_allocate;
        node_allocate+=sizeof(ChainNode);
        check+=sizeof(ChainNode);
        if(check+sizeof(ChainNode)>PAGE_SIZE){
            node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
            arr[indexing]=node_allocate;
            indexing++;
        }
        // if (main_node1 == MAP_FAILED) {
        //     perror("mmap failed");
        // }

        

        start->next=main_node1;
        main_node1->pages=quotient;
        main_node1->size=main_node1->pages*PAGE_SIZE;
        main_node1->child=chain1_node;
        main_node1->add_start=addr;
        main_node1->add_end=addr+main_node1->size-1;
        // if(temp_size<size){
        //     chain1_node->next=NULL;
        //     chain1_node->size=temp_size;
        //     chain1_node->type=1;//process
        //     chain1_node->add_start=addr;
        //     chain1_node->add_end=addr+temp_size-1;
        //     ChainNode* temp=mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        //     chain1_node->next=temp;
        //     temp->prev=chain1_node;
        //     temp->size=size-temp_size;
        //     temp->type=0;//Hole
        //     temp->add_start=chain1_node->add_end+1;
        //     temp->add_end=chain1_node->add_end+size-1;
        //     temp->next=NULL;
        //     return main_node1->add_start;
        // }
        void* toReturn;
        if(temp_size==size){
            chain1_node->next=NULL;
            chain1_node->size=temp_size;
            chain1_node->type=1;//process
            chain1_node->add_start=addr;
            chain1_node->add_end=addr+temp_size-1;
            void* mems_virt_addr = (void*) mappings_count;
            mappings[mappings_count].mems_virt_addr = mems_virt_addr;
            mappings[mappings_count].mems_phys_addr = addr;
            mappings_count++;
            toReturn=mems_virt_addr;
            for(int i=1;i<size;i++){
                mems_virt_addr = (void*) mappings_count;
                mappings[mappings_count].mems_virt_addr = mems_virt_addr;
                mappings[mappings_count].mems_phys_addr = addr+i;
                mappings_count++;
            }
            
            // return main_node1->add_start;
            return mems_virt_addr;
        }
        else{
            chain1_node->next=NULL;
            chain1_node->size=size;
            chain1_node->type=1;//process
            chain1_node->add_start=addr;
            chain1_node->add_end=addr+size-1;//to think
            temp_size-=size;
            void* mems_virt_addr = (void*) mappings_count;
            mappings[mappings_count].mems_virt_addr = mems_virt_addr;
            mappings[mappings_count].mems_phys_addr = addr;
            mappings_count++;
            toReturn=mems_virt_addr;
            for(int i=1;i<size;i++){
                void* mems_virt_addr = (void*) mappings_count;
                mappings[mappings_count].mems_virt_addr = mems_virt_addr;
                mappings[mappings_count].mems_phys_addr = addr+i;
                mappings_count++;
            }
        }
        int cnt=0;
        
        // while(temp_size>=size){
        //     ChainNode* temp=mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        //     temp->size=size;
        //     temp->next=NULL;
        //     temp->type=1;//process
        //     chain1_node->next=temp;
        //     temp_size-=size;
        //     temp->prev=chain1_node;
        //     temp->add_start=chain1_node->add_end+1;
        //     temp->add_end=chain1_node->add_end+temp->size;
        //     chain1_node=temp;
        //     // cnt++;
        // }
        if(temp_size!=0){
            // cnt+=1;
            ChainNode* temp=node_allocate;
            node_allocate+=sizeof(ChainNode);
            check+=sizeof(ChainNode);
            if(check+sizeof(ChainNode)>PAGE_SIZE){
                node_allocate=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE,fd, 0);
                arr[indexing]=node_allocate;
                indexing++;
            }

            temp->size=temp_size;
            temp->next=NULL;
            temp->type=0;//Hole
            chain1_node->next=temp;
            temp->prev=chain1_node;
            temp->add_start=chain1_node->add_end+1;
            temp->add_end=chain1_node->add_end+temp->size-1;
            chain1_node=temp;
            void* mems_virt_addr = (void*) mappings_count;
            mappings[mappings_count].mems_virt_addr = mems_virt_addr;
            mappings[mappings_count].mems_phys_addr = temp->add_start;
            mappings_count++;
            for(int i=1;i<temp_size;i++){
                void* mems_virt_addr = (void*) mappings_count;
                mappings[mappings_count].mems_virt_addr = mems_virt_addr;
                mappings[mappings_count].mems_phys_addr = temp->add_start+i;
                mappings_count++;
            }
            temp_size-=size;
            
            
        }
        return toReturn;
    }
}


/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/

void *patova(void *v_ptr){
    for(int i=0;i<mappings_count;i++){
        if(mappings[i].mems_phys_addr==v_ptr){
            return mappings[i].mems_virt_addr;
        }
    }
    return NULL;  
}

void *mainreturner(ChainNode* v){
    unsigned long start = patova(v->add_start);
    unsigned long end;

    while(v!=NULL){
        end = patova(v->add_end);
        v=v->next;
    }
    printf("MAIN[%lu:%lu]-> ", start, end);
}

void mems_print_stats(){
    printf("------------MeMS SYSTEM STATS------------\n");
    Node* start=head;
    Node* temp;
    int freemem;
    int totPages=0;

    unsigned long spaceunused = 0;
    int subchainlength[10000];
    int subchaincounter = 0;
    int nullcount = 0;
    int mainchain = 0;

    while(start->next!=NULL){
            start=start->next;
            totPages+=start->pages;
            //printf("No of Pages :%d\n",start->pages);
            if(start->child){
                ChainNode* inner1=start->child;
                mainreturner(inner1);
                while(inner1!=NULL){
                    if(inner1->type==1){
                        printf("P[%lu:%lu] <-> ", (unsigned long)patova(inner1->add_start), (unsigned long)patova(inner1->add_end));
                        subchaincounter = subchaincounter + 1;
                        //printf("This is a Process of size %lu and type Process, add start: %lu, add end: %lu.\n",inner1->size,(unsigned long)inner1->add_start,(unsigned long)inner1->add_end);
                        inner1=inner1->next;

                        if(inner1==NULL){
                            subchainlength[nullcount] = subchaincounter;
                            nullcount = nullcount + 1;
                            subchaincounter = 0;
                            printf("NULL");
                        }
                    }
                    else{
                        printf("H[%lu:%lu] <-> ", (unsigned long)patova(inner1->add_start), (unsigned long)patova(inner1->add_end));
                        subchaincounter = subchaincounter + 1;
                        spaceunused = spaceunused + (unsigned long)patova(inner1->add_end) - (unsigned long)patova(inner1->add_start) + 1;
                        //printf("This is a Process of size %lu and type Hole, add start: %lu, add end: %lu.\n",inner1->size,(unsigned long)inner1->add_start,(unsigned long)inner1->add_end);
                        freemem+=inner1->size;
                        inner1=inner1->next;   

                        if(inner1==NULL){
                            subchainlength[nullcount] = subchaincounter;
                            nullcount = nullcount + 1;
                            subchaincounter = 0;
                            printf("NULL");
                        }
                    }

                    
                }
                mainchain = mainchain + 1;
            }
            printf("\n");
    }
    printf("Pages used: %d\n",totPages);
    printf("Space unused: %lu\n", spaceunused);
    printf("Main chain length: %d\n", mainchain);

    printf("Sub-chain Length Array: ");
    if(nullcount==0){
        printf("NONE");
    }
    else{
        int loop = 0;
        while(subchainlength[loop]!=0){
            printf("%d, ", subchainlength[loop]);
            loop = loop + 1;
        }   
    }
    printf("\n");
    printf("-------------------------------------------------\n");
}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
    for(int i=0;i<100000;i++){
        if(mappings[i].mems_virt_addr==v_ptr){
            return mappings[i].mems_phys_addr;
        }
    }
    perror("No Physical address present corresponding to this.");
    return NULL;  
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    int fd = open("/dev/zero", O_RDWR);//as MAP_ANONYMOUS was not available
    if (fd == -1) {
        perror("open failed");
    }
    Node* start = head;
    Node* temp;
    int freemem;
    void* phyAdd;
    for(int i =0; i < 100000; i++){
        if(mappings[i].mems_virt_addr == v_ptr){
            phyAdd = mappings[i].mems_phys_addr;
            break;
        }
    }

    while(start->next != NULL){
        start = start->next;
        if(start->child){
            ChainNode* inner1 = start->child;
            int cnt=0;
            while(inner1 != NULL){
                if(inner1->add_start == phyAdd){
                    inner1->type = 0; //Hole
                    ChainNode* temp=start->child;
                    int nodecnt=0;
                    while(temp!=NULL){
                        temp=temp->next;
                        cnt++;
                    }
                    if(inner1->next!=NULL && inner1->prev!=NULL && inner1->prev->type==0 && inner1->next->type==0 && nodecnt==3){
                        if (munmap(start->add_start,start->pages*PAGE_SIZE) == -1) {
                            perror("munmap failed");
                        }
                        start->child=NULL;
                        if(start->prev==NULL){
                            head->next=start->next;
                            return;
                        }//may want to remove mapping too.
                        start->prev->next=start->next;
                        start->next->prev=start->prev;
                        start->next=NULL;
                        start->prev=NULL;
                        break;
                    }
                    else if(inner1->next!=NULL && inner1->next->type == 0){
                            void* addStart = inner1->add_start;
                            int vaStart;

                            for(int i = 0; i < 100000; i++){
                                if(mappings[i].mems_phys_addr == inner1->add_start){
                                    vaStart =(int) mappings[i].mems_virt_addr;
                                    break;
                                }
                            }
                            ChainNode* temp=inner1->next;
                            int totSize = inner1->size + inner1->next->size;
                            if(cnt==0 && temp->next!=NULL){
                                inner1->next->prev=inner1->prev;
                                inner1->next->add_start=addStart;
                                inner1->next->size=totSize;
                                inner1->next=NULL;
                                
                            }
                            else if(cnt!=0){
                                inner1->prev->next=inner1->next;
                                inner1->next->prev=inner1->prev;
                                inner1->next->add_start=addStart;
                                inner1->next->size=totSize;
                                inner1->next=NULL;
                                inner1->prev=NULL;
                            }

                            if(temp->next==NULL && temp->prev==NULL){
                                if (munmap(inner1->prev->add_start,start->pages*PAGE_SIZE) == -1) {
                                    perror("munmap failed");
                                }
                                start->child=NULL;
                                if(start->prev==NULL){
                                    head->next=start->next;
                                    return;
                                }//may want to remove mapping too.
                                start->prev->next=start->next;
                                start->next->prev=start->prev;
                                start->next=NULL;
                                start->prev=NULL;
                                break;
                            }
                            
                            
                            for(int i = 0; i < totSize; i++){
                                void* mems_virt_addr = (void*) vaStart;
                                mappings[vaStart].mems_virt_addr = mems_virt_addr;
                                mappings[vaStart].mems_phys_addr = addStart + i;
                                vaStart++;
                            }
                            break;
                    }
                    else if(inner1->prev!=NULL && inner1->prev->type == 0){
                            void* addStart = inner1->add_start;
                            int vaStart;

                            for(int i = 0; i < 100000; i++){
                                if(mappings[i].mems_phys_addr == inner1->prev->add_start){
                                    vaStart =(int) mappings[i].mems_virt_addr;
                                    break;
                                }
                            }
                            ChainNode* temp=inner1->prev;
                            int totSize = inner1->size + inner1->prev->size;
                            if(cnt==1 && inner1->next!=NULL){
                                inner1->prev->next=inner1->next;
                                inner1->next->prev=inner1->prev;
                                inner1->prev->add_end=inner1->add_end;
                                inner1->prev->size=totSize;
                                inner1->next=NULL;
                                inner1->prev=NULL;
                            }
                            else if(cnt!=1){
                                inner1->prev->next=inner1->next;
                                inner1->next->prev=inner1->prev;
                                // inner1->prev->add_start=inner1->prev->add_start;
                                inner1->prev->add_end=inner1->add_end;
                                inner1->prev->size=totSize;
                                inner1->next=NULL;
                                inner1->prev=NULL;
                            }

                            if(temp->next==NULL && temp->prev==NULL){
                                if (munmap(inner1->prev->add_start,start->pages*PAGE_SIZE) == -1) {
                                    perror("munmap failed");
                                }
                                start->child=NULL;
                                if(start->prev==NULL){
                                    head->next=start->next;
                                    return;
                                }//may want to remove mapping too.
                                start->prev->next=start->next;
                                start->next->prev=start->prev;
                                start->next=NULL;
                                start->prev=NULL;
                                break;
                            }
                            
                            
                            for(int i = 0; i < totSize; i++){
                                void* mems_virt_addr = (void*) vaStart;
                                mappings[vaStart].mems_virt_addr = mems_virt_addr;
                                mappings[vaStart].mems_phys_addr = addStart + i;
                                vaStart++;
                            }
                            break;
                    }
                    else{
                        return;
                    }
                }
                else{
                    cnt++;
                    inner1=inner1->next;
                }
            }
        }
    }

}