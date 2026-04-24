#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "manager.h"

int main() {
    char line[256];
    char cmd[10];
    int arg1, arg2;
    int result;
    int first_output = 1;
    
    while (fgets(line, sizeof(line), stdin)) {

        // Strip CRLF
        line[strcspn(line, "\r\n")] = 0;

        // Skip blank lines
        if (line[0] == '\0')
            continue;

        if (sscanf(line, "%s", cmd) != 1)
            continue;
        
        if (strcmp(cmd, "in") == 0) {
            init();
        
            if (!first_output)
                printf("\n");   // only print newline between sequences
        
            printf("%d ", running_pid);  // print 0 with NO leading space
        
            first_output = 0;
            continue;
        }


        if (strcmp(cmd, "cr") == 0) {
            sscanf(line, "%*s %d", &arg1);
            result = create(arg1);
        }
        else if (strcmp(cmd, "de") == 0) {
            sscanf(line, "%*s %d", &arg1);
            result = destroy(arg1);
        }
        else if (strcmp(cmd, "rq") == 0) {
            sscanf(line, "%*s %d %d", &arg1, &arg2);
            result = request(arg1, arg2);
        }
        else if (strcmp(cmd, "rl") == 0) {
            sscanf(line, "%*s %d %d", &arg1, &arg2);
            result = release(arg1, arg2);
        }
        else if (strcmp(cmd, "to") == 0) {
            result = timeout();
        }
        else {
            result = -1;
        }
        
        if (result == -2) result = -1;
        printf("%d ", result);
    }
    return 0;
}
