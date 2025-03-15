#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#ifndef container_of
#if __LIST_HAVE_TYPEOF
#define container_of(ptr, type, member)                         \
    __extension__({                                             \
        const typeof(((type *) 0)->member) *__pmember = (ptr);  \
        (type *) ((char *) __pmember - offsetof(type, member)); \
    })
#else
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif
#endif

/* Our simplified version of the Linux kernel list_head */
struct list_head {
    struct list_head *next, *prev;
};

/* Our element structure embedding a list_head and a string value */
typedef struct {
    char *value;
    struct list_head list;
} element_t;

/* Insert element at the head of the list (after dummy head) */
void list_insert_head(struct list_head *head, element_t *elem)
{
    struct list_head *first = head->next;
    head->next = &elem->list;
    elem->list.prev = head;
    elem->list.next = first;
    first->prev = &elem->list;
}

static inline int list_empty(const struct list_head *head)
{
    return (head->next == head);
}


static inline int list_is_singular(const struct list_head *head)
{
    return (!list_empty(head) && head->prev == head->next);
}



/* Initialize a dummy head for the list */
#define INIT_LIST_HEAD(ptr)  \
    do {                     \
        (ptr)->next = (ptr); \
        (ptr)->prev = (ptr); \
    } while (0)




#define list_empty(head) ((head)->next == (head))


void print_result(struct list_head *head)
{
    printf("Current result:[");
    head = head->next;
    while (head) {
        printf("%s ", container_of(head, element_t, list)->value);
        head = head->next;
    }
    printf("]\n");
}


static inline void list_splice_tail(struct list_head *list,
                                    struct list_head *head)
{
    struct list_head *head_last = head->prev;
    struct list_head *list_first = list->next;
    struct list_head *list_last = list->prev;

    if (list_empty(list))
        return;

    head->prev = list_last;
    list_last->next = head;

    list_first->prev = head_last;
    head_last->next = list_first;
}



struct list_head *list_tail(struct list_head *head)
{
    if (!head)
        return NULL;
    while (head->next) {
        // element_t *e = container_of(head, element_t, list);
        // printf("traversing on %s \n", e->value);
        head = head->next;
    }
    return head;
}

void list_add_tail(struct list_head *head, struct list_head *list)
{
    list_tail(head)->next = list;
}



int list_length(struct list_head *head)
{
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}


// add node to the front
void list_add(struct list_head **head, struct list_head *n)
{
    if (*head) {
        n->next = *head;
        *head = n;
    } else {
        n->next = NULL;
        *head = n;
    }
}


void quick_sort(struct list_head *head, bool descend)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return;
    head->prev->next = NULL;
    char *value;
    int i = 0;
    int max_level = 2 * list_length(head->next);
    struct list_head *begin[max_level], *end[max_level];
    struct list_head *left = NULL, *right = NULL;
    struct list_head *result = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(result);
    result->next = NULL, result->prev = NULL;

    begin[0] = head->next;
    end[0] = head->prev;


    printf("Sorting initials done!\n");

    while (i >= 0) {
        struct list_head *L = begin[i], *R = end[i];
        left = NULL, right = NULL;
        if (L != R) {
            struct list_head *pivot = L;
            value = strdup(container_of(pivot, element_t, list)->value);
            struct list_head *p = pivot->next;
            pivot->next = NULL;

            while (p) {
                struct list_head *n = p;
                p = p->next;
                element_t *e = container_of(n, element_t, list);
                list_add(strcmp(e->value, value) > 0 ? &right : &left, n);

                strcmp(e->value, value) > 0
                    ? printf("Added %s to right\n", e->value)
                    : printf("Added %s to left\n", e->value);
            }

            printf("out ");
            begin[i] = left;
            end[i] = list_tail(left);
            printf("working");
            begin[i + 1] = pivot;
            end[i + 1] = pivot;
            printf("working");
            begin[i + 2] = right;
            end[i + 2] = list_tail(right);
            left = right = NULL;
            i += 2;
            printf("partition complete\n");
            free(value);
        } else {
            printf("Gonna add L to result!(i=%d)\n", i);

            if (L)
                list_add_tail(result, L);
            i--;
            print_result(result);
        }
    }

    print_result(result);
    printf("done, we finalizing!\n");

    // left will be used as temp to restore prev pointers
    struct list_head *h = result->next;
    left = result;
    if (descend) {
        while (h) {
            h->prev = left;
            left = h;
            h = h->next;
        }
        result->prev = left;
        left->next = result;
    } else {
        left->prev = left->next;
        while (h) {
            h->prev = h->next;
            h->next = left;
            left = h;
            h = h->prev;
        }
        left->prev = result;
        result->next = left;
    }
    printf("going back to head!\n");
    head->next = head, head->prev = head;
    list_splice_tail(result, head);
    printf("we done!\n");
}


element_t *new_element(const char *str)
{
    element_t *elem = malloc(sizeof(element_t));
    if (!elem) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    elem->value = strdup(str);
    if (!elem->value) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    elem->list.next = elem->list.prev = NULL;
    return elem;
}

/* Print the list starting from head->next */
void print_list(struct list_head *head)
{
    struct list_head *cur;
    printf("l = [");
    for (cur = head->next; cur != head; cur = cur->next) {
        const element_t *elem =
            (element_t *) ((char *) cur - offsetof(element_t, list));
        printf("%s ", elem->value);
    }
    printf("]\n");
}

/* Free the list (including element->value and element) */
void free_list(struct list_head *head)
{
    struct list_head *cur = head->next;
    while (cur != head) {
        const element_t *elem =
            (element_t *) ((char *) cur - offsetof(element_t, list));
        cur = cur->next;
        free(elem->value);
        free(elem);
    }
}

int main()
{
    printf("hello!");
    struct list_head head;
    INIT_LIST_HEAD(&head);


    char *values[] = {"2", "3", "1", "4", "6", "5"};
    int n = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < n; i++) {
        element_t *elem = new_element(values[i]);
        /* Insert at head for simplicity */
        list_insert_head(&head, elem);
    }

    printf("Before sort (original order, for descending):\n");
    print_list(&head);

    quick_sort(&head, true);
    printf("After quick_sort (descending):\n");
    print_list(&head);

    free_list(&head);
    return 0;
}
