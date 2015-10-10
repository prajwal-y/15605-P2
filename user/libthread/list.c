/** @file list.c
 *  @brief This function implements the list operations
 *         for a doubly linked list.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include<list.h>

/** @brief initialize the list
 *
 *  Initialize the head by making it point to itself
 *
 *  @param head the head of the doubly linked circular list
 */
void init_head(list_head *head) {
    head->next = head;
    head->prev = head;
}

/** @brief add a node to the list
 *
 *  This is not thread safe and must be protected by a mutex
 *  by the programmer.
 *
 *  @param new_node The new node to be implemented in the list
 *  @param prev the node previous to the node being inserted
 *  @param next the node next to the node being inserted
 */
void add_to_list(list_head *new_node, list_head *prev, 
                               list_head *next) {
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
    next->prev = new_node;
}

/** @brief add a node to the tail of the list
 *
 *  @param new_node The new node to be implemented in the list
 *  @param head The head of the list
 */
void add_to_tail(list_head *new_node, list_head *head) {
    add_to_list(new_node, head->prev, head);
}

/** @brief add a node to the head of the list
 *
 *  @param new_node The new node to be implemented in the list
 *  @param head The head of the list
 */
void add_to_head(list_head *new_node, list_head *head) {
    add_to_list(new_node, head, head->next);
}

/** @brief delete an entry from the list 
 *
 *  Does not free space associated with this node
 *
 *  @param node node to be deleted
 */
void del_entry(list_head *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/** @brief get the first entry in the list
 *
 *  @param head the head of the list
 *  @return the first node in the list
 */
list_head *get_first(list_head *head) {
    if (head->next == head) {
        return NULL;
    }
    return head->next;
}
