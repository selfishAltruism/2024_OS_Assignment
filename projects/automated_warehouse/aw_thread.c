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

struct list_elem_with_thread {
    struct list_elem elem;
    struct thread *thread; 
};


/**
 * A function unblocking all blocked threads in "blocked_threads" 
 * It must be called by robot threads
 */
void block_thread(){
    struct list_elem_with_thread *elem = (struct list_elem_with_thread *)malloc(sizeof(struct list_elem_with_thread));
    elem->thread = thread_current();

    list_push_back(&blocked_threads, &(elem->elem)); 

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
    if(blocked_threads.head.next != &blocked_threads.tail) {
        while (1) {
            struct list_elem *popped_elem = list_pop_back(&blocked_threads);
            struct list_elem_with_thread *popped_elem_with_thread = (struct list_elem_with_thread *)popped_elem;
            
            enum intr_level old_level;
            old_level = intr_disable ();
            thread_unblock (popped_elem_with_thread->thread);
            intr_set_level (old_level);

            if(popped_elem->prev == &blocked_threads.head)  break;
        }
    }
}