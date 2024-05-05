#include <stdio.h>
#include <stdlib.h>

#include "projects/automated_warehouse/aw_message.h"

struct message_box* boxes_from_central_control_node;
struct message_box* boxes_from_robots;

void allocate_message_boxes(int num_robots) {
    boxes_from_central_control_node = (struct message_box *)malloc(num_robots * sizeof(struct message_box));
    
    boxes_from_robots = (struct message_box *)malloc(num_robots * sizeof(struct message_box));
}

void deallocate_message_boxes() {
    free(boxes_from_central_control_node);
    free(boxes_from_robots);
}

struct message empty_message = {
    row: -1,
    col: -1,
    current_payload: -1,
    required_payload: -1,
    cmd: -1
};


void send_message_to_control_node(int id, struct message msg) {
    boxes_from_central_control_node[id].msg = msg;
    boxes_from_central_control_node[id].dirtyBit = 1;
}

struct message receive_message_from_control_node(int id) {
    if (boxes_from_central_control_node[id].dirtyBit) {
        struct message received_msg = boxes_from_central_control_node[id].msg;
        boxes_from_central_control_node[id].dirtyBit = 0;

        return received_msg;
    }
    return empty_message;
}

void send_message_to_robot(int id, struct message msg) {
    boxes_from_robots[id].msg = msg;
    boxes_from_robots[id].dirtyBit = 1;
}

struct message receive_message_from_robot(int id) {
    if (boxes_from_robots[id].dirtyBit) {
        struct message received_msg = boxes_from_robots[id].msg;
        boxes_from_robots[id].dirtyBit = 0;

        return received_msg;
    }
    return empty_message;
}