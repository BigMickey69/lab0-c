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
    struct list_head *h = malloc(sizeof(struct list_head));
    if (!h)
        return NULL;
    INIT_LIST_HEAD(h);
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    if (list_empty(head)) {
        free(head);
        return;
    }

    struct list_head *pos, *tmp;

    list_for_each_safe (pos, tmp, head) {
        element_t *entry = list_entry(pos, element_t, list);
        free(entry->value);
        list_del(pos);
        free(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }

    struct list_head *h = &e->list;

    h->next = head->next;
    head->next->prev = h;
    head->next = h;
    h->prev = head;

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }

    struct list_head *h = &e->list;

    h->prev = head->prev;
    h->next = head;
    head->prev->next = h;
    head->prev = h;

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = container_of(head->next, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&e->list);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = container_of(head->prev, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&e->list);
    return e;
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
    element_t *e = container_of(slow, element_t, list);
    free(e->value);
    free(e);
    return true;
}

/* Delete all nodes that have duplicate string */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *cur = head->next, *next, *n;

    while (cur != head) {
        bool deleted = false;
        next = n = cur->next;

        while (n != head) {
            struct list_head *nexter = n->next;
            const element_t *e_cur = list_entry(cur, element_t, list);
            element_t *e = list_entry(n, element_t, list);

            if (strcmp(e_cur->value, e->value) == 0) {
                deleted = true;
                list_del(n);
                q_release_element(e);
            }
            n = nexter;
        }

        if (deleted) {
            next = cur->next;
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
        }
        cur = next;
    }
    return true;
}


/* Swap every two adjacent nodes */
// https://leetcode.com/problems/swap-nodes-in-pairs/
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head **pp = &head->next;

    while (*pp != head && (*pp)->next != head) {
        struct list_head *p1 = *pp;
        struct list_head *p2 = p1->next;

        p1->next = p2->next;
        p2->next->prev = p1;

        p2->next = p1;
        p2->prev = p1->prev;
        p1->prev = p2;

        p2->prev->next = p2;

        *pp = p2;

        pp = &p1->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *p = head;
    do {
        struct list_head *temp = p->next;
        p->next = p->prev;
        p->prev = temp;
        p = temp;
    } while (p != head);
}

/* Reverse the nodes of the list k at a time */
// https://leetcode.com/problems/reverse-nodes-in-k-group/
void q_reverseK(struct list_head *head, int k)
{
    if (k <= 1 || list_empty(head))
        return;

    struct list_head new;
    INIT_LIST_HEAD(&new);

    short counter = 0;
    for (struct list_head *cur = head->next; !(cur == head && counter != 0);
         cur = cur->next) {
        counter++;
        if (counter != k)
            continue;

        while (cur != head) {
            struct list_head *temp = cur->prev;
            list_move_tail(cur, &new);
            cur = temp;
        }
        counter = 0;
        cur = head;
    }

    list_splice_tail_init(head, &new);
    list_splice_init(&new, head);
}

/* Sort elements of queue in ascending/descending order */
/* Iterating through nodes and create an array, then quicksort
 * said array is a faster way to sort, but it takes more memory
 * and is harder to implement. Bubblesort keeps things simple */

/* EDIT: I decided to just use quicksort*/

/* EDIT 2: quicksort is almost impossible due to malloc being disabled,
 * so the new method of choice is mergesort! */

struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right,
                                 bool descend)
{
    struct list_head head;
    struct list_head *h = &head;
    if (!left && !right) {
        return NULL;
    }
    while (left && right) {
        const char *l_v = list_entry(left, element_t, list)->value;
        const char *r_v = list_entry(right, element_t, list)->value;

        if (descend ? strcmp(l_v, r_v) >= 0 : strcmp(l_v, r_v) <= 0) {
            h->next = left;
            left = left->next;
            h = h->next;
        } else {
            h->next = right;
            right = right->next;
            h = h->next;
        }
    }
    h->next = left ? left : right;
    return head.next;
}


struct list_head *merger(struct list_head *head, bool descend)
{
    if (!head->next) {
        return head;
    }

    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }

    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = merger(head, descend);
    struct list_head *right = merger(mid, descend);

    return merge_two_list(left, right, descend);
}

void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    head->prev->next = NULL;
    head->next = merger(head->next, descend);
    struct list_head *cur;
    struct list_head *prev_node = head;
    for (cur = head->next; cur->next != NULL; cur = cur->next) {
        cur->prev = prev_node;
        prev_node = cur;
    }

    cur->next = head;
    head->prev = cur;
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
// https://leetcode.com/problems/remove-nodes-from-linked-list/
int q_ascend(struct list_head *head)
{
    if (list_empty(head))
        return -1;

    if (list_is_singular(head))
        return 1;

    char *max = strdup(list_entry(head->prev, element_t, list)->value);

    short int size = 1;
    struct list_head *cur = head->prev->prev;
    while (cur != head) {
        element_t *e = list_entry(cur, element_t, list);

        if (strcmp(max, e->value) > 0) {
            size++;
            free(max);
            max = strdup(e->value);
            cur = cur->prev;
        } else {
            struct list_head *temp = cur->prev;
            list_del(cur);
            q_release_element(e);
            cur = temp;
        }
    }
    free(max);
    return size;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (list_empty(head))
        return -1;

    if (list_is_singular(head))
        return 1;

    char *max = strdup(list_entry(head->prev, element_t, list)->value);

    short int size = 1;
    struct list_head *cur = head->prev->prev;
    while (cur != head) {
        element_t *e = list_entry(cur, element_t, list);

        if (strcmp(max, e->value) < 0) {
            size++;
            free(max);
            max = strdup(e->value);
            cur = cur->prev;
        } else {
            struct list_head *temp = cur->prev;
            list_del(cur);
            q_release_element(e);
            cur = temp;
        }
    }
    free(max);
    return size;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
// Supporting function
void merge_lists(struct list_head *h1, struct list_head *h2, bool descend)
{
    struct list_head merged;
    INIT_LIST_HEAD(&merged);

    while (!list_empty(h1) && !list_empty(h2)) {
        element_t *e1 = list_entry(h1->next, element_t, list);
        element_t *e2 = list_entry(h2->next, element_t, list);
        if (descend ? strcmp(e1->value, e2->value) >= 0
                    : strcmp(e1->value, e2->value) <= 0) {
            list_move_tail(&e1->list, &merged);
        } else {
            list_move_tail(&e2->list, &merged);
        }
    }

    if (!list_empty(h1)) {
        list_splice_tail_init(h1, &merged);
    }

    if (!list_empty(h2)) {
        list_splice_tail_init(h2, &merged);
    }

    list_splice_init(&merged, h1);
}

// https://leetcode.com/problems/merge-k-sorted-lists/
/* Watched this video by neetcode for better solution:
 * https://www.youtube.com/watch?v=q5a5OiGbT6Q */
int q_merge(struct list_head *head, bool descend)
{
    if (list_empty(head))
        return 0;

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);

    for (struct list_head *cur = head->next->next; cur != head;
         cur = cur->next) {

        queue_contex_t *second = list_entry(cur, queue_contex_t, chain);

        first->size += second->size;
        second->size = 0;
        merge_lists(first->q, second->q, descend);
    }
    return first->size;
}
