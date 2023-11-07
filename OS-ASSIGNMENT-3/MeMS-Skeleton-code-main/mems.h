/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this
macro to make the output of all system same and conduct a fair evaluation.
*/
#define PAGE_SIZE 4096
// Memory Segment Types
#define HOLE 0
#define PROCESS 1

// Structure for a memory segment
typedef struct Segment
{
    int type;              // HOLE or PROCESS
    size_t size;           // Size of the segment
                           // Start address of the segment (array)
    struct Segment *next;  // Next segment in the sub-chain
    struct Segment *prev;  // Previous segment in the sub-chain
    void *virtual_address; // MeMS virtual address
    void *start_address[];
} Segment;

// Structure for a sub-chain
typedef struct SubChain
{
    Segment *head;         // Head of the sub-chain
    Segment *tail;         // Tail of the sub-chain
    struct SubChain *next; // Next sub-chain
    struct SubChain *prev; // Previous sub-chain
} SubChain;

// Structure for a main chain
typedef struct MainChain
{
    SubChain *sub_chains;   // Sub-chains in the main chain
    struct MainChain *next; // Next main chain
    struct MainChain *prev; // Previous main chain
    void *virtual_address;  // MeMS virtual address
} MainChain;

// Function to allocate memory for a new segment
typedef struct MainNode
{
    SubChain *sub_chain;   // The Sub Chain associated with this Main Node
    struct MainNode *next; // Next Main Node
    struct MainNode *prev; // Previous Main Node
    void *virtual_address; // MeMS virtual address
} MainNode;

// Data structure for MeMS virtual to physical address mapping
typedef struct MemMapping
{
    void *virtual_address;   // MeMS virtual address
    void *physical_address;  // MeMS physical address
    struct MemMapping *next; // Next mapping
} MemMapping;

MainChain *main_chain = NULL;
void *mems_get(void *v_ptr);
MainChain *allocate_main_chain();
MemMapping *mapping_head = NULL;

static void *virtual_address_counter = (void *)1000;
static void *next_virtual_address = (void *)1000; // Starting virtual address, adjust as needed

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/

// Include this for mmap-related functions and constants

// Define a structure to represent the segments in the free list

/*
Initializes all the required parameters for the MeMS system.
*/

void mems_init()
{
    // Initialize the MeMS system parameters
    main_chain = NULL; // Initialize the free list
    // virtual_address_counter = (void *)0X1; // Initialize the starting MeMS virtual address

    // Initialize additional global variables
    mapping_head = NULL; // Initialize the mapping list

    // Optionally, create the initial MainChain or perform other setup tasks
    

    // You can add other initialization tasks here as needed
}

/*
This function will be called at the end of the MeMS system and its main job is to unmap the
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish()
{
    MainChain *current_main = main_chain;

    while (current_main != NULL)
    {
        SubChain *current_sub = current_main->sub_chains;

        while (current_sub != NULL)
        {
            Segment *current_segment = current_sub->head;

            while (current_segment != NULL)
            {
                // Remove the MemMapping associated with the segment
                 for (size_t i = 0; i < current_segment->size; i++) {
                    void *virtual_address = current_segment->start_address[i];
                    
                    // Find and remove the corresponding MemMapping from the list
                    MemMapping *current_mapping = mapping_head;
                    MemMapping *prev_mapping = NULL;
                    
                    while (current_mapping != NULL) {
                        if (current_mapping->virtual_address == virtual_address) {
                            if (prev_mapping != NULL) {
                                prev_mapping->next = current_mapping->next;
                            } else {
                                mapping_head = current_mapping->next;
                            }
                            
                            int unmap_result = munmap(current_mapping, sizeof(MemMapping));
                            if (unmap_result != 0) {
                                fprintf(stderr, "Error unmapping MemMapping\n");
                                // Handle the munmap error, if needed
                            }
                            break;
                        }
                        
                        prev_mapping = current_mapping;
                        current_mapping = current_mapping->next;
                    }
                }

                Segment *next_segment = current_segment->next;

                // Unmap the memory segment
                int unmap_segment_result = munmap(current_segment, current_segment->size);
                if (unmap_segment_result != 0)
                {
                    // Handle the munmap error, e.g., print an error message
                    fprintf(stderr, "Error unmapping segment\n");
                    // Optionally, you can set a flag to indicate an issue with unmapping
                }

                current_segment = next_segment;
            }

            SubChain *next_sub = current_sub->next;
            // Unmap the SubChain structure
            int unmap_subchain_result = munmap(current_sub, sizeof(SubChain));
            if (unmap_subchain_result != 0)
            {
                // Handle the munmap error, e.g., print an error message
                fprintf(stderr, "Error unmapping SubChain\n");
                // Optionally, you can set a flag to indicate an issue with unmapping
            }

            current_sub = next_sub;
        }

        MainChain *next_main = current_main->next;
        // Unmap the MainChain structure
        int unmap_mainchain_result = munmap(current_main, sizeof(MainChain));
        if (unmap_mainchain_result != 0)
        {
            // Handle the munmap error, e.g., print an error message
            fprintf(stderr, "Error unmapping MainChain\n");
            // Optionally, you can set a flag to indicate an issue with unmapping
        }

        current_main = next_main;
    }

    // After verifying that all memory is unmapped, set main_chain to NULL
    main_chain = NULL;

    // Release any other resources and perform cleanup as needed
}

int global_start_address = 1000; // Global start address

Segment *allocate_segment(size_t size, int type, SubChain *sub_chain)
{
    // Calculate the size of the segment (rounded up to the nearest PAGE_SIZE)
    size_t segment_size = sizeof(Segment) + size * sizeof(void *);

    // Allocate memory for the segment and the start_address array using mmap
    void *segment_start = mmap(NULL, size + size * sizeof(void *), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (segment_start == MAP_FAILED)
    {
        fprintf(stderr, "Failed to allocate memory for a segment using mmap\n");
        exit(1);
    }

    // Create a new segment
    Segment *segment = (Segment *)segment_start;
    segment->type = type;
    segment->size = size;

    // Set the start_address array to point to the allocated memory region

    // Populate the start_address array with addresses in the desired range
    for (size_t i = 0; i < size; i++)
    {
        segment->start_address[i] = (void *)(global_start_address + i);
    }

    segment->next = NULL;
    segment->prev = sub_chain->tail;

    // Update the mapping (allocate without malloc) for each element in the start_address array
    for (size_t i = 0; i < size; i++)
    {
        MemMapping *new_mapping = mmap(NULL, sizeof(MemMapping), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        new_mapping->virtual_address = segment->start_address[i]; // MeMS virtual address
        new_mapping->physical_address = segment_start + i;        // MeMS physical address
        new_mapping->next = mapping_head;
        mapping_head = new_mapping;
    }

    // Update the segment pointers
    if (sub_chain->head == NULL)
    {
        sub_chain->head = segment;
    }
    if (sub_chain->tail != NULL)
    {
        sub_chain->tail->next = segment;
    }
    sub_chain->tail = segment;

    // Increment the virtual address counter
    if (type == 1 || segment->type == PROCESS)
    {
        global_start_address += size;
    }

    return segment;
}

SubChain *allocate_sub_chain()
{
    // Use mmap to allocate memory for the SubChain
    void *sub_chain_memory = mmap(NULL, sizeof(SubChain), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (sub_chain_memory == MAP_FAILED)
    {
        fprintf(stderr, "Failed to allocate memory for a sub-chain using mmap\n");
        exit(1);
    }

    SubChain *sub_chain = (SubChain *)sub_chain_memory;
    sub_chain->head = NULL;
    sub_chain->tail = NULL;
    sub_chain->next = NULL;
    sub_chain->prev = NULL;

    // Update the global start address based on the last segment's ending value in the previous sub-chain
    if (main_chain != NULL)
    {
        MainChain *current_main = main_chain;
        while (current_main->next != NULL)
        {
            current_main = current_main->next;
            break;
        }
        if (current_main->sub_chains != NULL)
        {
            SubChain *previous_sub = current_main->sub_chains;
            if (previous_sub->tail != NULL)
            {
                Segment *last_segment = previous_sub->tail;
                global_start_address = (int)last_segment->start_address[last_segment->size - 1] + 1;
            }
            previous_sub = previous_sub->next;
        }

        return sub_chain;
    }
}

MainNode *allocate_main_node()
{
    // Calculate the size of the main node (rounded up to the nearest PAGE_SIZE)
    size_t main_node_size = (sizeof(MainNode) + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;

    // Use mmap to allocate memory for the MainNode
    void *main_node_memory = mmap(NULL, main_node_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (main_node_memory == MAP_FAILED)
    {
        fprintf(stderr, "Failed to allocate memory for a main node using mmap\n");
        exit(1);
    }

    MainNode *main_node = (MainNode *)main_node_memory;
    main_node->sub_chain = NULL;
    main_node->virtual_address = next_virtual_address; // Assign virtual address
    // next_virtual_address = (void *)((char *)next_virtual_address + main_node_size);
    main_node->next = NULL;
    main_node->prev = NULL;

    // Set the global start address based on the last segment's ending value in the previous sub-chain
    if (main_chain != NULL && main_chain->prev != NULL && main_chain->prev->sub_chains != NULL)
    {
        SubChain *previous_sub = main_chain->prev->sub_chains;
        while (previous_sub->next != NULL)
        {
            previous_sub = previous_sub->next;
        }
        Segment *last_segment = previous_sub->tail;
        if (last_segment != NULL)
        {
            global_start_address = (int)last_segment->start_address[last_segment->size - 1];
        }
    }

    return main_node;
}

MainChain *allocate_main_chain()
{
    size_t main_chain_size = (sizeof(MainChain) + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;

    // Use mmap to allocate memory for the MainChain
    void *main_chain_memory = mmap(NULL, sizeof(MainNode), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (main_chain_memory == MAP_FAILED)
    {
        fprintf(stderr, "Failed to allocate memory for a main chain using mmap\n");
        exit(1);
    }

    MainChain *main_node = (MainChain *)main_chain_memory;
    main_node->sub_chains = NULL;
    main_node->virtual_address = next_virtual_address; // Assign virtual address
    // next_virtual_address = (void *)((char *)next_virtual_address + main_chain_size);
    main_node->next = NULL;
    main_node->prev = NULL;

    return main_node;
}
void attach_main_node(MainNode *new_main_node)
{
    if (main_chain == NULL)
    {
        // If the global Main Chain is empty, create the first Main Node
        MainChain *new_main_chain = allocate_main_chain();
        new_main_chain->sub_chains = new_main_node->sub_chain;
        main_chain = new_main_chain;

        // Set the global start address based on the last segment's ending value in the previous sub-chain
        if (new_main_chain->prev != NULL && new_main_chain->prev->sub_chains != NULL)
        {
            SubChain *previous_sub = new_main_chain->prev->sub_chains;
            while (previous_sub->next != NULL)
            {
                previous_sub = previous_sub->next;
            }
            Segment *last_segment = previous_sub->tail;
            if (last_segment != NULL)
            {
                global_start_address = (int)last_segment->start_address[last_segment->size - 1];
            }
        }
    }
    else
    {
        // Find the last Main Node in the global Main Chain
        MainChain *current_main_chain = main_chain;
        while (current_main_chain->next != NULL)
        {
            current_main_chain = current_main_chain->next;
        }

        // Attach the new Main Node to the last Main Chain
        current_main_chain->sub_chains = new_main_node->sub_chain;

        // Set the global start address based on the last segment's ending value in the previous sub-chain
        if (new_main_node->sub_chain->prev != NULL)
        {
            Segment *last_segment = new_main_node->sub_chain->prev->tail;
            if (last_segment != NULL)
            {
                global_start_address = (int)last_segment->start_address[last_segment->size - 1];
            }
        }
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

void *mems_get(void *v_ptr)
{
    // Check if the mapping_head is properly initialized
    if (mapping_head == NULL)
    {
        fprintf(stderr, "Error: mapping_head is not properly initialized.\n");
        return NULL;
    }

    // Convert virtual addresses and physical addresses to long integers for human readability
    MemMapping *current_mapping = mapping_head;
    long v_ptr_value = (long)v_ptr; // Convert v_ptr to long once
    long v_addr;                    // Declare v_addr here

    while (current_mapping != NULL)
    {
        v_addr = (long)current_mapping->virtual_address; // Assign v_addr inside the loop
        long p_addr = (long)current_mapping->physical_address;

        if (current_mapping->virtual_address == v_ptr)
        {
            printf("\nMapping found - Pointer : %ld  , Virtual Address: %ld, Physical Address: %ld\n", v_ptr_value, v_addr, p_addr);
            return current_mapping->physical_address;
        }
        // printf("not found - Current Mapping Virtual Address: %ld, v_ptr: %ld\n", v_addr, v_ptr_value);
        current_mapping = current_mapping->next;
    }

    // MeMS virtual address not found
    // fprintf(stderr, "Error: MeMS virtual address not found - Current Mapping Virtual Address: %ld, v_ptr: %ld\n", v_addr, v_ptr_value);
    return NULL;
}
SubChain *reverse_sub_chain(SubChain *head)
{
    SubChain *prev = NULL;
    SubChain *current = head;
    SubChain *next = NULL;

    while (current != NULL)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}
MainChain *reverse_main_chain(MainChain *head)
{
    MainChain *prev = NULL;
    MainChain *current = head;
    MainChain *next = NULL;

    while (current != NULL)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}

void *mems_malloc(size_t size)
{
    // Calculate the size of the main chain (rounded up to the nearest PAGE_SIZE)
    size_t main_chain_size = (size + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;

    MainChain *reversed_main_chain = reverse_main_chain(main_chain);

    MainChain *current_main = reversed_main_chain;

    while (current_main != NULL)
    {
        SubChain *reversed_sub_chain = reverse_sub_chain(current_main->sub_chains);
        SubChain *current_sub = reversed_sub_chain;

        while (current_sub != NULL)
        {
            int last_segment_ending_value = -1;
            Segment *current_segment = current_sub->head;
            Segment *prev_segment = NULL;

            while (current_segment != NULL)
            {
                // Check if the segment is a HOLE and has enough size
                if (current_segment->type == HOLE && current_segment->size >= size)
                {
                    // Allocate memory in this hole
                    void *allocated_address = current_segment->start_address[0];

                    // If there's unused space in the hole, create a new segment for it
                    if (current_segment->size > size)
                    {
                        Segment *new_segment = allocate_segment(current_segment->size - size, HOLE, current_sub);
                        new_segment->start_address[0] = current_segment->start_address[size - 1] + 1;
                        for (int i = 1; i < new_segment->size; i++)
                        {
                            new_segment->start_address[i] = new_segment->start_address[0] + i;
                        }

                        // Update the size and the type of the current segment
                        current_segment->next = new_segment;
                        current_segment->size = size;
                        current_segment->type = PROCESS;
                        global_start_address += size;
                    }
                    else
                    {
                        // If the hole is fully used, mark it as PROCESS
                        current_segment->type = PROCESS;
                        global_start_address += size;
                    }

                    reverse_sub_chain(reversed_sub_chain);   // Restore the sub-chain to its original order
                    reverse_main_chain(reversed_main_chain); // Restore the main chain to its original order
                    return allocated_address;
                }

                current_segment = current_segment->next;
            }

            current_sub = current_sub->next;
        }

        current_main = current_main->next;
    }

    reverse_main_chain(reversed_main_chain); // Restore the main chain to its original order

    // If no suitable hole is found, create a new main node and attach it to the global main chain
    MainNode *new_main_node = allocate_main_node();
    new_main_node->next = main_chain; // Attach the new Main Node to the current global Main Chain
    main_chain = new_main_node;       // Update the global Main Chain to the new Main Node

    // Create a new sub-chain within the new main node with a single segment
    SubChain *new_sub_chain = allocate_sub_chain();
    new_main_node->sub_chain = new_sub_chain;

    // Allocate memory in the sub-chain for the requested size
    void *allocated_address = allocate_segment(size, PROCESS, new_sub_chain)->start_address[0];

    // Calculate the remaining hole size within the segment
    size_t remaining_hole_size = main_chain_size - size;

    if (remaining_hole_size > 0)
    {
        // If there is unused space in the segment, create a new HOLE segment for it
        Segment *new_hole_segment = allocate_segment(remaining_hole_size, HOLE, new_sub_chain);
        global_start_address += size;
    }

    return allocated_address;
}

/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
int custom_ceil(double num)
{
    int rounded = (int)num;
    if (num > rounded)
    {
        rounded++;
    }
    return rounded;
}
void print_main_chain_backwards(MainChain *main_chain)
{
    if (main_chain == NULL)
    {
        return;
    }

    // Recursively print the main chain in reverse order
    print_main_chain_backwards(main_chain->next);

    // Print the main chain information
    printf("MAIN [%d:%d]-> ",
           (int)main_chain->sub_chains->head->start_address[0], 
           (int)main_chain->sub_chains->tail->start_address[main_chain->sub_chains->tail->size - 1]);

    SubChain *current_sub = main_chain->sub_chains;
    int sub_chain_index = 0;

    while (current_sub != NULL)
    {
        if (current_sub->head == NULL)
        {
            // Skip printing if the sub-chain is empty
            current_sub = current_sub->next;
            continue;
        }

        // Print sub-chain index

        // Print segments in the order they appear
        Segment *current_segment = current_sub->head;

        while (current_segment != NULL)
        {
            printf("%c[%d:%d] <-> ",
                   (current_segment->type == HOLE) ? 'H' : 'P',
                   (int)current_segment->start_address[0],
                   (int)current_segment->start_address[current_segment->size - 1]);

            current_segment = current_segment->next;
        }

        printf("NULL\n");

        current_sub = current_sub->next;
        sub_chain_index++;
    }
}


void mems_print_stats()
{

    printf("\n----- MeMS SYSTEM STATS -----\n");
    if (main_chain == NULL)
    {
        printf("Main Chain is empty.\n");
        return;
    }

    // Print the main chain and its sub-chains in reverse order
    print_main_chain_backwards(main_chain);

    // Count the number of used and unused pages
    double total_pages_used = 0;
    int total_pages_unused = 0;
    int main_chain_length = 0;

    MainChain *current_main = main_chain;

    while (current_main != NULL)
    {
        main_chain_length++;

        SubChain *current_sub = current_main->sub_chains;
        int sub_chain_length = 0;
        int pages_used = 0;
        int pages_unused = 0;

        while (current_sub != NULL)
        {
            if (current_sub->head == NULL)
            {
                // Skip empty sub-chains
                current_sub = current_sub->next;
                continue;
            }

            // Count the number of pages used and unused in the sub-chain

            Segment *current_segment = current_sub->head;

            while (current_segment != NULL)
            {
                if (current_segment->type == PROCESS)
                {
                    pages_used += (current_segment->size);
                }
                else if (current_segment->type == HOLE)
                {
                    pages_unused += (int)(current_segment->size);
                }

                current_segment = current_segment->next;
            }

            total_pages_used += pages_used;
            total_pages_unused += pages_unused;

            current_sub = current_sub->next;
        }

        current_main = current_main->next;
    }
    double num = total_pages_used / PAGE_SIZE;
    printf("Main Chain Length: %d\n", main_chain_length);
    printf("Pages used: %d\n", custom_ceil(num));
    printf("Space unused: %d\n", total_pages_unused);


    int sub_chain_lengths[main_chain_length];
    current_main = main_chain;
    int i = 0;

    while (current_main != NULL)
    {
        SubChain *current_sub = current_main->sub_chains;
        int sub_chain_length = 0;

        while (current_sub != NULL)
        {
            Segment *current_segment = current_sub->head;

            while (current_segment != NULL)
            {
                sub_chain_length++;
                current_segment = current_segment->next;
            }

            current_sub = current_sub->next;
        }

        sub_chain_lengths[i] = sub_chain_length;
        i++;

        current_main = current_main->next;
    }

    // Reverse the sub_chain_lengths array
    int temp;
    int start = 0;
    int end = main_chain_length - 1;

    while (start < end)
    {
        temp = sub_chain_lengths[start];
        sub_chain_lengths[start] = sub_chain_lengths[end];
        sub_chain_lengths[end] = temp;
        start++;
        end--;
    }

    printf("Sub-chain Length array: [");
    for (i = 0; i < main_chain_length; i++)
    {
        printf("%d, ", sub_chain_lengths[i]);
    }
    printf("]\n");
    printf("-------------------------------------\n");
}

/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/


void mems_free(void *v_ptr)
{
    MainChain *current_main = main_chain;

    while (current_main != NULL)
    {
        SubChain *current_sub = current_main->sub_chains;

        while (current_sub != NULL)
        {
            Segment *current_segment = current_sub->head;
            Segment *prev_segment = NULL;

            while (current_segment != NULL)
            {
                if (current_segment->start_address[0] == v_ptr)
                {

                    current_segment->type = HOLE;

                    if (prev_segment != NULL && prev_segment->type == HOLE)
                    {
                        // Merge current_segment with the previous HOLE segment
                        prev_segment->size += current_segment->size;
                        prev_segment->next = current_segment->next;
                        if (munmap(current_segment, sizeof(current_segment) == -1))
                        {
                            perror("munmap current_segment");
                            // Handle error if necessary
                        }
                        return;
                    }

                    if (current_segment->next != NULL && current_segment->next->type == HOLE)
                    {
                        // Merge current_segment with the next HOLE segment
                        current_segment->size += current_segment->next->size;
                        current_segment->next = current_segment->next->next;
                        if (munmap(current_segment->next, sizeof(current_segment->next)) == -1)
                        {
                            perror("munmap next_segment");
                            // Handle error if necessary
                        }
                        return;
                    }
                }

                prev_segment = current_segment;
                current_segment = current_segment->next;
            }

            current_sub = current_sub->next;
        }

        current_main = current_main->next;
    }
}