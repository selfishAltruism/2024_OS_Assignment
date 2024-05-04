#include "projects/automated_warehouse/aw_message.h"

void allocate_message_boxes(int num_robots) {
    boxes_from_central_control_node = (struct message_box *)malloc(num_robots * sizeof(struct message_box));
    if (boxes_from_central_control_node == 0) {
        exit(1);
    }
    
    boxes_from_robots = (struct message_box *)malloc(num_robots * sizeof(struct message_box));
    if (boxes_from_robots == 0) {
        free(boxes_from_central_control_node);
        exit(1);
    }
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


void send_message_to_control_node(int tid, struct message msg) {
    boxes_from_central_control_node[tid].msg = msg;
    boxes_from_central_control_node[tid].dirtyBit = 1;
}

struct message receive_message_from_control_node(int tid) {
    if (boxes_from_central_control_node[tid].dirtyBit) {
        struct message received_msg = boxes_from_central_control_node[tid].msg;
        boxes_from_central_control_node[tid].dirtyBit = 0;

        return received_msg;
    }
    return empty_message;
}

void send_message_to_robot(int tid, struct message msg) {
    boxes_from_robots[tid].msg = msg;
    boxes_from_robots[tid].dirtyBit = 1;
}

struct message receive_message_from_robot(int tid) {
    if (boxes_from_robots[tid].dirtyBit) {
        struct message received_msg = boxes_from_robots[tid].msg;
        boxes_from_robots[tid].dirtyBit = 0;

        return received_msg;
    }
    return empty_message;
}