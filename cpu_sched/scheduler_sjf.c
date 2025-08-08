// scheduler_sjf.c
#include "scheduler.h"
#include "scheduler_sjf.h"
#include "doubly_linked_list.h"

/**
 * \brief Enqueues a process in READY state
 *
 * \param rq  The scheduler's run queue
 * \param pid The process to be enqueued
 */
void stud_SJF_start(struct run_queue* rq, int pid){
    if (stud_rq_find(rq, pid))
        return;
    struct task *t = stud_task_create(pid, READY);
    if (!t) return;
    stud_rq_enqueue_sorted(rq, t);
}

/**
 * \brief Elects and starts a process from the run queue. The running process MUST be placed
 *        at the head of `rq` if it is not already there.
 *
 * \param rq  The scheduler's run queue
 */
void stud_SJF_elect(struct run_queue* rq){
    struct task *cur = rq->head;
    /* Suche erstes READY */
    while (cur && cur->state != READY)
        cur = cur->next;
    if (!cur)
        return;
    /* Wenn nicht schon Kopf, vorziehen */
    if (cur != rq->head) {
        if (cur->prev) cur->prev->next = cur->next;
        if (cur->next) cur->next->prev = cur->prev;
        cur->next = rq->head;
        rq->head->prev = cur;
        cur->prev = NULL;
        rq->head = cur;
    }
    cur->state = RUNNING;
    rq->time_counter = 0;
}

/**
 * \brief Terminates the current running process (i.e. rq->head) and places it at the BACK
 *        of the run_queue.
 *
 * \param rq  The scheduler's run queue
 */
void stud_SJF_terminate(struct run_queue* rq){
    if (stud_rq_empty(rq))
        return;
    struct task *t = rq->head;  /* muss RUNNING sein */
    t->state = TERMINATED;
    t->runtime = 0;
    rq->head = t->next;
    if (rq->head)
        rq->head->prev = NULL;
    t->prev = t->next = NULL;
    rq->n_tasks--;
    stud_rq_enqueue(rq, t);
    stud_SJF_elect(rq);
}

/**
 * \brief Performs a clock tick as specified by SJF.
 *
 * \param rq  The scheduler's run queue
 */
void stud_SJF_clock_tick(struct run_queue* rq){
    struct task *t = rq->head;
    if (t && t->state == RUNNING)
        t->runtime++;
}

/**
 * \brief Sets the state of the running process to BLOCKED, moves it to the BACK of the
 *        run_queue, and elects and runs a new process.
 *
 * \param rq  The scheduler's run queue
 */
void stud_SJF_wait(struct run_queue* rq){
    if (stud_rq_empty(rq))
        return;
    struct task *t = rq->head;
    t->state = BLOCKED;
    rq->head = t->next;
    if (rq->head)
        rq->head->prev = NULL;
    t->prev = t->next = NULL;
    rq->n_tasks--;
    stud_rq_enqueue(rq, t);
    stud_SJF_elect(rq);
}

/**
 * \brief Sets the state of `pid` to READY, if it exists, and sorts it into the
 *        run_queue as specified by SJF.
 *
 * \param rq  The scheduler's run queue
 * \param pid The process to be woken up
 */
void stud_SJF_wake_up(struct run_queue* rq, int pid){
    struct task *t = stud_rq_find(rq, pid);
    if (t && t->state == BLOCKED) {
        t->state = READY;
        /* Aus Liste entfernen */
        if (t->prev) t->prev->next = t->next;
        else         rq->head = t->next;
        if (t->next) t->next->prev = t->prev;
        rq->n_tasks--;
        t->prev = t->next = NULL;
        stud_rq_enqueue_sorted(rq, t);
    }
}

/**
 * \brief Event handler for SJF
 *
 * \param rq    The scheduler's run queue
 * \param event The event to be handled
 * \param pid   Depending on `event`, the `pid` of the target process.
 *              If the `event` doesn't need this, it is ignored.
 */
void stud_SJF(struct run_queue* rq, enum events event, int pid) {
    switch(event) {
        case start:      stud_SJF_start(rq, pid);     break;
        case terminate:  stud_SJF_terminate(rq);      break;
        case clock_tick: stud_SJF_clock_tick(rq);     break;
        case wait:       stud_SJF_wait(rq);           break;
        case wake_up:    stud_SJF_wake_up(rq, pid);   break;
        default:         /* ignored */                break;
    }
}
