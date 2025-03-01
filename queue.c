#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head) {}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return NULL;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    struct list_head *pos;
    short int cnt = 0;

    list_for_each (pos, head) {
        cnt++;
    }
    return cnt;
}

/* Delete the middle node in queue */
// https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    /* head is a slow pointer, while fast is a fast pointer */
    struct list_head *fast = head->next, *slow = head->next;

    while (fast != head && fast->next != head) {
        // head ? printf("slow: %d, ", head->val) : printf("slow: NULL");
        // fast ? printf("fast: %d\n", fast->val) : printf("fast: NULL");

        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    return true;
}

/* Delete all nodes that have duplicate string */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head **pp = &head->next;
    struct list_head *fast;

    while (*pp != head) {
        const element_t *n1 = list_entry(*pp, element_t, list);
        fast = (*pp)->next;

        while (fast != head) {
            const element_t *n2 = list_entry(fast, element_t, list);

            if (strcmp(n1->value, n2->value) == 0) {
                // Save reference since we're freeing the dupe
                struct list_head *next_fast = fast->next;
                list_del(fast);
                fast = next_fast;
                continue;
            }

            fast = fast->next;
        }

        if ((*pp)->next == fast)
            pp = &(*pp)->next;
        else
            *pp = fast;
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
