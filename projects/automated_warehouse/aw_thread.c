#include "projects/automated_warehouse/aw_thread.h"

//
// You need to read carefully thread/synch.h and thread/synch.c
// 
// In the code, a fucntion named "sema_down" implements blocking thread and 
// makes list of blocking thread
// 
// And a function named "sema_up" implements unblocing thread using blocking list
//
// You must implement blocking list using "blocking_threads" in this code.
// Then you can also implement unblocking thread.
//


struct list blocked_threads = {
    .head = { .prev = NULL, .next = &blocked_threads.tail },
    .tail = { .prev = &blocked_threads.head, .next = NULL }
};

struct list_elem_with_data {
    struct list_elem elem;
    int data;
};

void list_push_back(struct list_elem_with_data *elem) {
    elem->elem.prev = blocked_threads.tail.prev;
    elem->elem.next = &blocked_threads.tail;
    blocked_threads.tail.prev->next = &(elem->elem);
    blocked_threads.tail.prev = &(elem->elem);
}

//will be delete part
void list_print() {
    struct list_elem *current = blocked_threads.head.next;
    while (current != &blocked_threads.tail) {
        struct list_elem_with_data *elem_with_data = (struct list_elem_with_data *)current;
        printf("%d ", elem_with_data->data);
        current = current->next;
    }
    printf("\n");
}

/**
 * A function unblocking all blocked threads in "blocked_threads" 
 * It must be called by robot threads
 */
void block_thread(){
    //will be delete part
    printf("%d\n", thread_current ()-> tid);
    
    struct list_elem_with_data elem = {{NULL, NULL}, thread_current ()-> tid};
    list_push_back(&elem);

    printf("\nList elements: ");
    list_print();

    // Code below is example
    enum intr_level old_level;
    old_level = intr_disable ();
    thread_block ();
    intr_set_level (old_level);
}

/**
 * A function unblocking all blocked threads in "blocked_threads" 
 * It must be called by central control thread
 */
void unblock_threads(){
    // you must implement this
}