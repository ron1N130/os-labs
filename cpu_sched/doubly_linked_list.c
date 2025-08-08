// doubly_linked_list.c
#include <stdbool.h>
#include <stdlib.h>
#include "scheduler.h"
#include "doubly_linked_list.h"

/**
 * \brief Checks whether a run_queue is empty
 *
 * \param rq Pointer to the run_queue
 *
 * \returns `true` iff the run_queue is empty
 */
bool stud_rq_empty(struct run_queue const *rq) {
    return rq->n_tasks == 0;
}

/**
 * \brief Creates a task
 *
 * \param pid   The pid of the new task
 * \param state Default state of the task
 *
 * \return A pointer to the new task, `NULL` if failed
 */
struct task *stud_task_create(int pid, enum states state) {
    struct task *t = malloc(sizeof(*t));
    if (!t) return NULL;
    t->pid = pid;
    t->state = state;
    t->prev = t->next = NULL;
    t->runtime = 0;
    return t;
}

/**
 * \brief Frees/destroys a task This function assumes the task has already been
 *        removed from any run queue. It does not handle linked list operations
 *        or modifications to the run queue.
 *
 * \param task Pointer to the task to-be-destroyed
 */
void stud_task_free(struct task *task) {
    free(task);
}

/**
 * \brief Frees all the elements of the `run_queue` and empties it.
 *        `rq` itself should NOT be freed. After calling this function,
 *        `rq` must simply be an empty `run_queue`.
 *
 * \param rq Pointer to the run_queue to-be-destroyed
 */
void stud_rq_destroy(struct run_queue *rq) {
    struct task *cur = rq->head;
    while (cur) {
        struct task *next = cur->next;
        free(cur);
        cur = next;
    }
    rq->head = NULL;
    rq->n_tasks = 0;
}

/**
 * \brief Tries to find a task in a run_queue by its PID
 *
 * \param rq  Pointer to the run_queue
 * \param pid PID of the wanted task
 *
 * \returns Pointer to the task, `NULL` if failed
 */
struct task *stud_rq_find(struct run_queue *rq, int pid) {
    for (struct task *cur = rq->head; cur; cur = cur->next)
        if (cur->pid == pid)
            return cur;
    return NULL;
}

/**
 * \brief Returns the head of the `run_queue`.
 *
 * \param rq An element of the list
 */
struct task *stud_rq_head(struct run_queue *rq) {
    return rq->head;
}

/**
 * \brief Returns the tail of the `run_queue`.
 *
 * \param rq An element of the list
 */
struct task *stud_rq_tail(struct run_queue *rq) {
    struct task *cur = rq->head;
    if (!cur) return NULL;
    while (cur->next)
        cur = cur->next;
    return cur;
}

/**
 * \brief Enqueues a task on the given run_queue. The head stays where it is
 *        and the new task is put at the END of the list.
 *
 * \param rq   Pointer to the run_queue
 * \param task The task to-be-enqueued
 *
 * \returns `true` iff successful
 */
bool stud_rq_enqueue(struct run_queue *rq, struct task *task) {
    if (!rq || !task) return false;
    task->prev = task->next = NULL;
    if (!rq->head) {
        rq->head = task;
    } else {
        struct task *tail = stud_rq_tail(rq);
        tail->next = task;
        task->prev = tail;
    }
    rq->n_tasks++;
    return true;
}

/**
 * \brief Enqueues a task into the run_queue sorted by ascending runtime.
 *
 * \param rq   Pointer to the run_queue
 * \param task The task to be inserted
 *
 * \returns `true` if successful
 */
bool stud_rq_enqueue_sorted(struct run_queue* rq, struct task* task){
    if (!rq || !task) return false;
    task->prev = task->next = NULL;
    if (!rq->head) {
        rq->head = task;
    } else {
        struct task *cur = rq->head;
        while (cur && cur->runtime <= task->runtime)
            cur = cur->next;
        if (!cur) {
            struct task *tail = stud_rq_tail(rq);
            tail->next = task;
            task->prev = tail;
        } else if (cur == rq->head) {
            task->next = cur;
            cur->prev = task;
            rq->head = task;
        } else {
            task->next = cur;
            task->prev = cur->prev;
            cur->prev->next = task;
            cur->prev = task;
        }
    }
    rq->n_tasks++;
    return true;
}

/**
 * \brief Prepends a task on the given run_queue. The head of the list MUST be
 *        moved to the new task!
 *
 * \param rq   Pointer the run_queue
 * \param task The task to-be-prepended
 *
 * \returns `true` iff successful
 */
bool stud_rq_prepend(struct run_queue *rq, struct task *task) {
    if (!rq || !task) return false;
    task->prev = NULL;
    task->next = rq->head;
    if (rq->head)
        rq->head->prev = task;
    rq->head = task;
    rq->n_tasks++;
    return true;
}

/**
 * \brief Computes the length of a `run_queue`
 *
 * \param rq The run_queue
 *
 * \return The length of `rq`
 */
size_t stud_rq_length(struct run_queue *rq) {
    return rq->n_tasks;
}
