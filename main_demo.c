#include <stdio.h>
#include <string.h>
#include "undo_redo.h"

int main(void) {
    initEditor();

    applyEdit("Hello");
    applyEdit("Hello World");
    applyEdit("Hello World!");

    printf("Current : %s\n", getCurrentText());   /* Hello World! */

    undo();
    printf("After undo: %s\n", getCurrentText());  /* Hello World */

    undo();
    printf("After undo: %s\n", getCurrentText());  /* Hello */

    redo();
    printf("After redo: %s\n", getCurrentText());  /* Hello World */

    printf("Can undo? %s\n", canUndo() ? "yes" : "no");
    printf("Can redo? %s\n", canRedo() ? "yes" : "no");

    destroyEditor();
    return 0;
}
