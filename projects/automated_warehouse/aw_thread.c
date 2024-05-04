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


struct list blocked_threads;

struct list_elem_with_data {
    struct list_elem elem;
    int data;
};

void list_print(struct list *list) {
    struct list_elem *current = list->head.next;
    while (current != &list->tail) {
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

    struct list_elem_with_data *elem = (struct list_elem_with_data *)malloc(sizeof(struct list_elem_with_data));
    elem->data = thread_current()->tid;

    list_push_back(&blocked_threads, &(elem->elem)); 

    printf("\nList elements: ");
    list_print(&blocked_threads);

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