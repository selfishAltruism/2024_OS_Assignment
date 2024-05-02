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

struct cntPurpose {
        struct robot robot;
        int mNum;
        char loadingDock;
};

struct cntPurpose* cntPurposes;

// test code for central control node thread
void test_cnt(void* aux){
        while(1){
                printf("%s", aux);
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

        robots = malloc(sizeof(struct robot) * robotsN);
        cntPurposes = malloc(sizeof(struct cntPurpose) * robotsN);
        int i = 1;

        char *robotsSet;
        size_t len = strlen(argv[2]) + 1;
        char *str = malloc(len); 
        if (str != NULL) {
                memcpy(str, argv[2], len);
        }
        char *token = strtok_r(str, ":", &robotsSet);

        while (token != NULL) {
                int mNum = atoi(token);
                char* robotName;
                robotName = malloc(sizeof(char) + sizeof(int));
                snprintf(robotName, 4, "R%d", i);

                char loadingDock = token[strlen(token)-1];
                printf("Token: %d + %c\n", mNum, loadingDock);
                printf("%s", robotName);

                setRobot(&robots[i], robotName, 5, 5, 0, 0);

                i++;
                token = strtok_r(NULL, ":", &robotsSet);
        }
        free(str);

        // example of create thread
        tid_t* threads = malloc(sizeof(tid_t) * 4);
        int idxs[4] = {1, 2, 3, 4};
        threads[0] = thread_create("CNT", 0, &test_cnt, "dddd\n");
        threads[1] = thread_create("R1", 0, &test_thread, &idxs[1]);
        threads[2] = thread_create("R2", 0, &test_thread, &idxs[2]);
        threads[3] = thread_create("R3", 0, &test_thread, &idxs[3]);

        // if you want, you can use main thread as a central control node
        
}