/** @file list.h
 *  @brief an implementation of a doubly circular linked list
 *
 *  Implementation based on ideas from the Linux kernel linked list
 *  https://github.com/torvalds/linux/blob/master/include/linux/list.h
 *  The main idea is instead of forcing data into a special linked list
 *  node, we embed the links in any struct of our choosing. This offers 
 *  a lot of flexibility as a single user-defined struct can be part of 
 *  multiple linked lists and is highly reusable and generic. The second
 *  trick is retrieving the struct address from the list_head struct which
 *  is done with a bit of memory arithmetic.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 */

#ifndef __LIST_H
#define __LIST_H
#include <stddef.h>

#define get_entry(ptr, type, name) \
      ((type *)((char *)(ptr) - offsetof(type, name))) 

struct list_head;
typedef struct list_head {
    struct list_head *next;
    struct list_head *prev;
} list_head;

static inline void init_head(list_head *head) {
    head->next = head;
    head->prev = head;
}

static inline void add_to_list(list_head *new_node, list_head *prev, 
                               list_head *next) {
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
    next->prev = new_node;
}

static inline void add_to_tail(list_head *new_node, list_head *head) {
    add_to_list(new_node, head->prev, head);
}

static inline void add_to_head(list_head *new_node, list_head *head) {
    add_to_list(new_node, head, head->next);
}

static inline void del_entry(list_head *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline list_head *get_first(list_head *head) {
    if (head->next == head) {
        return NULL;
    }
    return head->next;
}

#endif  /* __LIST_H */
