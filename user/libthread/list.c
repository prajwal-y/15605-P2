#include<list.h>

void init_head(list_head *head) {
    head->next = head;
    head->prev = head;
}

void add_to_list(list_head *new_node, list_head *prev, 
                               list_head *next) {
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
    next->prev = new_node;
}

void add_to_tail(list_head *new_node, list_head *head) {
    add_to_list(new_node, head->prev, head);
}

void add_to_head(list_head *new_node, list_head *head) {
    add_to_list(new_node, head, head->next);
}

void del_entry(list_head *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

list_head *get_first(list_head *head) {
    if (head->next == head) {
        return NULL;
    }
    return head->next;
}
