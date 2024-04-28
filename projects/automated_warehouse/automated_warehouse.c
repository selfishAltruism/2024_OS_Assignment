#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/timer.h"

#include "projects/automated_warehouse/aw_manager.h"

struct robot* robots;

// test code for central control node thread
void test_cnt(){
        while(1){
                print_map(robots, 4);
                thread_sleep(1000);
                block_thread();
        }
}

// test code for robot thread
void test_thread(void* aux){
        int idx = *((int *)aux);
        int test = 0;
        while(1){
                printf("thread %d : %d\n", idx, test++);
                thread_sleep(idx * 1000);
        }
}

// entry point of simulator
void run_automated_warehouse(char **argv)
{
        init_automated_warehouse(argv); // do not remove this

        printf("implement automated warehouse!\n");

        
        int robotsN = atoi(argv[1]);
        char robotsSets[100] = argv[2];

        printf("%s\n", robotsSets); 

        char *robotsSet = strtok(robotsSets,":");

        while (robotsSet != NULL)
        {
        char letter = *robotsSet;
        robotsSet++;

                int number = 0;
                while (*robotsSet != '\0') { 
                        if (*robotsSet >= '0' && *robotsSet <= '9') { 
                        number = number * 10 + (*robotsSet - '0'); 
                        }
                        robotsSet++; 
                }

                printf("Letter: %c\n", letter);
                printf("Number: %d\n", number);         
                robotsSet = strtok(NULL, ":");      
        }

        // test case robots
        robots = malloc(sizeof(struct robot) * 4);
        setRobot(&robots[0], "R1", 5, 5, 1, 1);
        setRobot(&robots[1], "R2", 0, 2, 0, 0);
        setRobot(&robots[2], "R3", 5, 5, 1, 1);
        setRobot(&robots[3], "R4", 5, 5, 0, 0);

        // example of create thread
        tid_t* threads = malloc(sizeof(tid_t) * 4);
        int idxs[4] = {1, 2, 3, 4};
        threads[0] = thread_create("CNT", 0, &test_cnt, NULL);
        threads[1] = thread_create("R1", 0, &test_thread, &idxs[1]);
        threads[2] = thread_create("R2", 0, &test_thread, &idxs[2]);
        threads[3] = thread_create("R3", 0, &test_thread, &idxs[3]);

        // if you want, you can use main thread as a central control node
        
}