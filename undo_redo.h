#ifndef UNDO_REDO_H
#define UNDO_REDO_H

#define MAX_STACK_SIZE 100   /* max number of states we remember   */
#define MAX_TEXT_LEN   1024  /* max length of the text being edited */

/* ---------- Stack node: stores ONE snapshot of the text ---------- */
typedef struct Node {
    char text[MAX_TEXT_LEN];
    struct Node *next;
} Node;

/* ---------- Stack itself (linked-list based, so no size cap issues) ---------- */
typedef struct {
    Node *top;
    int size;
} Stack;

/* ---------- Core API (this is what the frontend calls) ---------- */

/* Call once at program start */
void initEditor(void);

/* Call once at program end to free memory */
void destroyEditor(void);

/* Apply a new edit: pass the FULL new text after the edit.
   Internally saves the old text on the undo stack. */
void applyEdit(const char *newText);

/* Undo last edit. Returns 1 on success, 0 if nothing to undo. */
int undo(void);

/* Redo last undone edit. Returns 1 on success, 0 if nothing to redo. */
int redo(void);

/* Get the current text (read-only pointer to internal buffer) */
const char *getCurrentText(void);

/* Utility: check stack states (useful for enabling/disabling buttons in UI) */
int canUndo(void);
int canRedo(void);

#endif
