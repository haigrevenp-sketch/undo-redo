/* ============================================================
   Micro Project - 02 : Undo-Redo Text Editor using Stack
   ------------------------------------------------------------
   Demonstrates LIFO (stack) operations by letting the user
   type / delete text, then Undo or Redo those changes.

   Design:
     - undoStack : stores previous versions of the text
     - redoStack : stores versions that were undone
     - Every edit (insert/delete) PUSHes the OLD text onto
       undoStack before applying the change, and CLEARS redoStack
       (standard editor behaviour - a new edit kills the redo
       history).
     - Undo  -> pop from undoStack, push current text to
                redoStack, restore popped text.
     - Redo  -> pop from redoStack, push current text to
                undoStack, restore popped text.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEXT   1024   /* max length of the document text   */
#define MAX_STACK  100    /* max number of undo/redo steps     */

/* ---------- Stack of text snapshots ---------- */
typedef struct {
    char items[MAX_STACK][MAX_TEXT];
    int  top;                       /* index of top element, -1 = empty */
} Stack;

void stackInit(Stack *s) {
    s->top = -1;
}

int stackIsEmpty(Stack *s) {
    return s->top == -1;
}

int stackIsFull(Stack *s) {
    return s->top == MAX_STACK - 1;
}

void stackPush(Stack *s, const char *text) {
    if (stackIsFull(s)) {
        /* drop the oldest entry to make room (shift down) */
        for (int i = 0; i < MAX_STACK - 1; i++)
            strcpy(s->items[i], s->items[i + 1]);
        s->top = MAX_STACK - 2;
    }
    strcpy(s->items[++(s->top)], text);
}

/* pops top element into 'out'; returns 0 on success, -1 if empty */
int stackPop(Stack *s, char *out) {
    if (stackIsEmpty(s)) return -1;
    strcpy(out, s->items[(s->top)--]);
    return 0;
}

void stackClear(Stack *s) {
    s->top = -1;
}

/* ---------- Editor state ---------- */
char currentText[MAX_TEXT] = "";
Stack undoStack, redoStack;

/* Call this BEFORE making any change to currentText */
void saveStateForUndo(void) {
    stackPush(&undoStack, currentText);
    stackClear(&redoStack);   /* new edit invalidates redo history */
}

void insertText(const char *addition) {
    if (strlen(currentText) + strlen(addition) >= MAX_TEXT - 1) {
        printf("Text too long, cannot insert.\n");
        return;
    }
    saveStateForUndo();
    strcat(currentText, addition);
    printf("Inserted.\n");
}

void deleteLastWord(void) {
    int len = strlen(currentText);
    if (len == 0) {
        printf("Nothing to delete.\n");
        return;
    }
    saveStateForUndo();

    /* trim trailing spaces first */
    while (len > 0 && currentText[len - 1] == ' ') {
        currentText[--len] = '\0';
    }
    /* remove characters back to the previous space (or start) */
    while (len > 0 && currentText[len - 1] != ' ') {
        currentText[--len] = '\0';
    }
    printf("Last word deleted.\n");
}

void undo(void) {
    char prev[MAX_TEXT];
    if (stackPop(&undoStack, prev) == -1) {
        printf("Nothing to undo.\n");
        return;
    }
    stackPush(&redoStack, currentText);
    strcpy(currentText, prev);
    printf("Undo done.\n");
}

void redo(void) {
    char next[MAX_TEXT];
    if (stackPop(&redoStack, next) == -1) {
        printf("Nothing to redo.\n");
        return;
    }
    stackPush(&undoStack, currentText);
    strcpy(currentText, next);
    printf("Redo done.\n");
}

void showText(void) {
    printf("\n--- Current Text ---\n");
    if (strlen(currentText) == 0)
        printf("(empty)\n");
    else
        printf("%s\n", currentText);
    printf("--------------------\n");
    printf("Undo stack size: %d | Redo stack size: %d\n\n",
           undoStack.top + 1, redoStack.top + 1);
}

void printMenu(void) {
    printf("\n===== Undo-Redo Text Editor (Stack based) =====\n");
    printf("1. Insert text\n");
    printf("2. Delete last word\n");
    printf("3. Undo\n");
    printf("4. Redo\n");
    printf("5. Show current text\n");
    printf("6. Exit\n");
    printf("Choose an option: ");
}

int main(void) {
    stackInit(&undoStack);
    stackInit(&redoStack);

    int choice;
    char buffer[MAX_TEXT];

    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            /* clear bad input */
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
        getchar(); /* consume leftover newline before fgets */

        switch (choice) {
            case 1:
                printf("Enter text to insert: ");
                fgets(buffer, MAX_TEXT, stdin);
                buffer[strcspn(buffer, "\n")] = '\0'; /* strip newline */
                insertText(buffer);
                showText();
                break;

            case 2:
                deleteLastWord();
                showText();
                break;

            case 3:
                undo();
                showText();
                break;

            case 4:
                redo();
                showText();
                break;

            case 5:
                showText();
                break;

            case 6:
                printf("Exiting editor. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice, try again.\n");
        }
    }

    return 0;
}
