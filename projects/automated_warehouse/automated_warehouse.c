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
        struct robot *robot;
        int mNum;
        char loadingDock;
};

// test code for central control node thread
void test_cnt(void* aux){
        struct cntPurpose info = *((struct cntPurpose *)aux);
        while(1){
                printf("%s", aux);
                print_map(robots, 4);
                thread_sleep(1000);
        }
}

// test code for robot thread
void test_thread(void* aux){
        struct cntPurpose info = *((struct cntPurpose *)aux);
        int test = 0;
        while(1){
                printf("thread %s : %d\n", info.robot->name, test++);
                thread_sleep(1000);
                block_thread();
        }
}

// entry point of simulator
void run_automated_warehouse(char **argv)
{
        init_automated_warehouse(argv); // do not remove this

        printf("implement automated warehouse!\n");

        //robot define
        int robotsN = atoi(argv[1]);

        robots = malloc(sizeof(struct robot) * robotsN);
        

        //thread define
        tid_t* threads = malloc(sizeof(tid_t) * (robotsN + 1));


        //define cntPurposes
        struct cntPurpose* cntPurposes;
        cntPurposes = malloc(sizeof(struct cntPurpose) * (robotsN));


        int i = 0;
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

                snprintf(robotName, 4, "R%d", i+1);

                char loadingDock = token[strlen(token)-1];

                //will be delete part
                printf("%s", robotName);
                printf(": %d + %c\n", mNum, loadingDock);

                cntPurposes[i].robot = &robots[i];
                cntPurposes[i].mNum = mNum;
                cntPurposes[i].loadingDock = loadingDock;
                
                //set robot & thread
                setRobot(&robots[i], robotName, 5, 5, 0, 0);
                threads[i + 1] = thread_create(robotName, 0, &test_thread, &cntPurposes[i]);

                i++;
                token = strtok_r(NULL, ":", &robotsSet);
        }

        threads[0] = thread_create("CNT", 0, &test_cnt, cntPurposes);

        free(str);  
}