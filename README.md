## Introduction
The code manages memory in a hierarchical structure, where a main chain (linked list of nodes) contains sub-chains (linked list of segments) representing process (allocated) and hole (free) segments. The code keeps track of mappings between MeMS virtual and physical addresses in the mappings array. It is important to note that the code is designed to work on a specific platform or system with certain assumptions about page size (PAGE_SIZE) and system-specific features. The code uses mmap for memory allocation and deallocation. Additionally, the mappings array is used to maintain the MeMS virtual-to-physical address mapping.

NOTE: Please add -lm in your own makefile or use the attached makefile, it uses -lm.

This is a test run of the code.
![run](https://github.com/devansh2610/CSE231-OS-Assignment3/assets/133692296/e1fb4733-fb4a-400e-960d-9b29cd5b471d)

## Structures
mems_mapping Structure: This structure is used to store mappings between MeMS virtual addresses and physical addresses.

Node Structure: Represents a node in the main chain of memory segments.
Contains information about the allocated memory region, such as the start and end addresses, the number of pages, the size, and a reference to a sub-chain of segments.

ChainNode Structure: Represents a node in a sub-chain of memory segments (PROCESS or HOLE).
Contains information about a specific memory segment, such as the start and end addresses, size, and type.

mappings Array: An array of mems_mapping structures used to store MeMS virtual-to-physical address mappings.
The mappings_count variable keeps track of the number of mappings.


## Variables
node_allocate: A pointer used to allocate nodes and chain nodes within a page.

head: A pointer to the head of the main chain.

arr Array: An array used to store pointers to allocated pages when needed.

indexing: An index variable used to keep track of the current page being allocated.

check: A variable to keep track of the amount of space allocated within a page.

These are the main functions in the code:


## Functions
mems_init():
This function initializes the MeMS system.
It opens the /dev/zero device file, which provides null-terminated strings, for memory allocation.
It uses mmap to allocate an initial block of memory (of size PAGE_SIZE) to be used as the head of the free list.


mems_finish():
This function is called at the end of the MeMS system to release all allocated memory.
It iterates through the linked list of allocated memory blocks (nodes) and uses munmap to deallocate them.
It also deallocates any additional memory blocks created using mmap and stored in the arr array.


mems_malloc(size_t size):
This function allocates memory of the specified size.
It first checks if the free list (head) is empty. If it's empty, it allocates a new block of memory using mmap.
The allocated memory is divided into nodes, where each node represents a block of memory with its size and type (process or hole).
If a block of memory in the free list is large enough to satisfy the request, it is used to allocate memory. If not, additional memory is allocated using mmap, and the excess memory is added to the free list as a hole.
The function returns the MeMS virtual address for the allocated memory.


mems_print_stats():
This function prints statistics about the MeMS system, including:
The number of pages used by the MeMS system.
The amount of memory that is unused, i.e., the memory in the free list (holes).
Details about the main chain (nodes) and sub-chains (process or hole segments) within the main chain.
It calculates and prints information such as total pages used, space unused, main chain length, and sub-chain lengths.


mems_get(void* v_ptr):
This function takes a MeMS virtual address (v_ptr) and returns the corresponding MeMS physical address.
It searches through the mappings array to find the physical address that matches the virtual address.


mems_free(void* v_ptr):
This function frees the memory pointed to by the given MeMS virtual address.
It locates the corresponding node and segment (process or hole) in the memory structure and marks it as a hole. If adjacent segments are also holes, they are merged into a single hole.
If the entire node is freed and its child (sub-chain) is empty, it is removed from the main chain.

## Problem Statement
                                                (MeMS) Memory Management System

Implement a custom memory management system (MeMS) using the C programming language. MeMS should utilize the system calls mmap and munmap for memory allocation and deallocation, respectively. The system must satisfy the following constraints and requirements outlined below:


Constraints and Requirements:

MeMS can solely use the system calls mmap and munmap for memory management. The use of any other memory management library functions such as malloc, calloc, free, and realloc are STRICTLY PROHIBITED.
MeMS should request memory from the OS using mmap in multiples of the system's PAGE_SIZE, which can be determined using the command getconf PAGE_SIZE. For most Linux distributions, the PAGE_SIZE is 4096 bytes (4 KB); however, it might differ for other systems.
MeMS should deallocate memory only through munmap and deallocation should only occur in multiples of PAGE_SIZE.
As the value of PAGE_SIZE can differ from system to system hence use the macro “PAGE_SIZE” provided in the template wherever you need the value of PAGE_SIZE in your code so that this size can be modified if required for evaluation purposes.
The user program must use the functions provided by MeMS for memory allocation and deallocation. It is not allowed to use any other memory management library functions, including malloc, calloc, free, realloc, mmap, and munmap.
Although MeMS requests memory from the OS in multiples of PAGE_SIZE, it only allocates that much amount of memory to the user program as requested by the user program. MeMS maintains a free list data structure to keep track of the heap memory which MeMS has requested from the OS. This free list keeps track of two items:
memory allocated to each user program. We will call this memory as PROCESS in the free list (details below).
Memory which has not been allocated to any user program. We will call this memory as a HOLE in the free list (details below).

Free List Structure:

Free List is represented as a doubly linked list. Let's call this doubly linked list as the main chain of the free list. The main features of the main chain are:

Whenever MeMS requests memory from the OS (using mmap), it adds a new node to the main chain.
Each node of the main chain points to another doubly linked list which we call as sub-chain. This sub-chain can contain multiple nodes. Each node corresponds to a segment of memory within the range of the memory defined by its main chain node. Some of these nodes (segments) in the sub-chain are mapped to the user program. We call such nodes (segments) as PROCESS nodes. Rest of the nodes in the sub-chain are not mapped to the user program and are called as HOLES or HOLE nodes.
Whenever the user program requests for memory from MeMS, MeMS first tries to find a sufficiently large segment in any sub-chain of any node in the main chain. If a sufficiently large segment is found, MeMS uses it to allocate memory to the user program and updates the segment’s type from HOLE to PROCESS. Else, MeMS requests the OS to allocate more memory on the heap (using mmap) and add a new node corresponding to it in the main chain.


The structure of free list looks like below:





The main features of the chain (sub-chain) are:

Each chain is broken into segments.
Each segment represents a part of memory and represents whether that segment is of type PROCESS i.e. is mapped to the user process or is of type HOLE i.e. not allocated/mapped to the user program.
The segments of type HOLE can be reallocated to any new requests by the user process. In this scenario, if some space remains after allocation then the remaining part becomes a new segment of type HOLE in that sub-chain. Graphaphically it looks something like below:


In the above picture, the Node1 of sub-chain-4 is reused by the user process but only 600 bytes out of 1000 bytes are used. Hence a HOLE of 400 bytes is created and the node of 600 bytes is marked as PROCESS and the MeMS virtual address corresponding to 600 bytes node is returned to the user process for further use.


NOTE: You must handle the corner cases and make sure that your system should avoids memory fragmentation within the free list.


MeMS Virtual Address and MeMS Physical Address:


Let us call the address (memory location) returned by mmap as the MeMS physical address. In reality, the address returned by mmap is actually a virtual address in the virtual address space of the process in which MeMS is running. But for the sake of this assignment, since we are simulating memory management by the OS, we will call the virtual address returned by mmap as MeMS physical address.


Just like a call to mmap returns a virtual address in the virtual address space of the calling process, a call to mems_malloc will return a MeMS virtual address in the MeMS virtual address space of the calling process. For the sake of this assignment, MeMS manages heap memory for only one process at a time.


Just like OS maintains a mapping from virtual address space to physical address space, MeMS maintains a mapping from MeMS virtual address space to MeMS physical address space. So, for every MeMS physical address (which is provided by mmap), we need to assign a MeMS virtual address. As you may understand, this MeMS virtual address has no meaning outside the MeMS system.


Any time the user process wants to write/store anything to the heap, it has to make use of the MeMS virtual address. But we cannot directly write using MeMS virtual address as the OS does not have any understanding of MeMS virtual address space. Therefore, we first need to get the MeMS physical address for that MeMS virtual address. Then, the user process needs to use this MeMS physical address to write on the heap.


For example in the below figure

MeMS virtual address 0 corresponds to MeMS physical address 1000
MeMS virtual address 500 corresponds to MeMS physical address 1500
MeMS virtual address 1024 corresponds to MeMS physical address 5024




We can get the MeMS physical address (i.e. the actual address returned by mmap) corresponding to a MeMS virtual address by using the function mems_get function (see below for more details).



Function Implementations:

Implement the following functions within MeMS:

void mems_init(): Initializes all the required parameters for the MeMS system. The main parameters to be initialized are
the head of the free list i.e. the pointer that points to the head of the free list
the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing

Returns: Nothing

void mems_finish(): This function will be called at the end of the MeMS system and its main job is to unmap the allocated memory using the munmap system call.
Input Parameter: Nothing

Returns: Nothing

void* mems_malloc(size_t size): Allocates memory of the specified size by reusing a segment from the free list if a sufficiently large segment is available. Else, uses the mmap system call to allocate more memory on the heap and updates the free list accordingly.
Parameter: The size of the memory the user program wants

Returns: MeMS Virtual address (that is created by MeMS)

void mems_free(void* ptr): Frees the memory pointed by ptr by marking the corresponding sub-chain node in the free list as HOLE. Once a sub-chain node is marked as HOLE, it becomes available for future allocations.
Parameter: MeMS Virtual address (that is created by MeMS)

Returns: nothing

void mems_print_stats(): Prints the total number of mapped pages (using mmap) and the unused memory in bytes (the total size of holes in the free list). It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing

Returns: Nothing but should print the necessary information on STDOUT

void *mems_get(void*v_ptr): Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)

Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).

