#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "undo_redo.h"

/* ---------- Internal state ---------- */
static Stack undoStack;
static Stack redoStack;
static char currentText[MAX_TEXT_LEN];

/* ---------- Low-level stack operations (LIFO) ---------- */

static void stackInit(Stack *s) {
    s->top = NULL;
    s->size = 0;
}

static void push(Stack *s, const char *text) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    strncpy(node->text, text, MAX_TEXT_LEN - 1);
    node->text[MAX_TEXT_LEN - 1] = '\0';
    node->next = s->top;
    s->top = node;
    s->size++;

    /* Optional cap so memory doesn't grow forever */
    if (s->size > MAX_STACK_SIZE) {
        /* Remove the oldest node (bottom of stack) */
        Node *cur = s->top;
        Node *prev = NULL;
        while (cur->next != NULL) {
            prev = cur;
            cur = cur->next;
        }
        if (prev) {
            prev->next = NULL;
            free(cur);
            s->size--;
        }
    }
}

/* Pops top of stack into outText. Returns 1 on success, 0 if empty. */
static int pop(Stack *s, char *outText) {
    if (s->top == NULL) return 0;

    Node *node = s->top;
    strncpy(outText, node->text, MAX_TEXT_LEN - 1);
    outText[MAX_TEXT_LEN - 1] = '\0';

    s->top = node->next;
    free(node);
    s->size--;
    return 1;
}

static void stackClear(Stack *s) {
    Node *cur = s->top;
    while (cur != NULL) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    s->top = NULL;
    s->size = 0;
}

static int stackIsEmpty(Stack *s) {
    return s->top == NULL;
}

/* ---------- Public API ---------- */

void initEditor(void) {
    stackInit(&undoStack);
    stackInit(&redoStack);
    currentText[0] = '\0';
}

void destroyEditor(void) {
    stackClear(&undoStack);
    stackClear(&redoStack);
}

void applyEdit(const char *newText) {
    /* Save current state so we can undo back to it */
    push(&undoStack, currentText);

    /* A fresh edit invalidates the redo history */
    stackClear(&redoStack);

    /* Commit the new text */
    strncpy(currentText, newText, MAX_TEXT_LEN - 1);
    currentText[MAX_TEXT_LEN - 1] = '\0';
}

int undo(void) {
    if (stackIsEmpty(&undoStack)) return 0;

    /* Save current state to redo before going back */
    push(&redoStack, currentText);

    char prevText[MAX_TEXT_LEN];
    pop(&undoStack, prevText);
    strncpy(currentText, prevText, MAX_TEXT_LEN - 1);
    currentText[MAX_TEXT_LEN - 1] = '\0';
    return 1;
}

int redo(void) {
    if (stackIsEmpty(&redoStack)) return 0;

    /* Save current state to undo before moving forward */
    push(&undoStack, currentText);

    char nextText[MAX_TEXT_LEN];
    pop(&redoStack, nextText);
    strncpy(currentText, nextText, MAX_TEXT_LEN - 1);
    currentText[MAX_TEXT_LEN - 1] = '\0';
    return 1;
}

const char *getCurrentText(void) {
    return currentText;
}

int canUndo(void) {
    return !stackIsEmpty(&undoStack);
}

int canRedo(void) {
    return !stackIsEmpty(&redoStack);
}
