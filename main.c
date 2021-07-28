#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define PROCESS_NUM 26

int main( int argc, char *argv[] )  {

    if( argc == 2) {
        FILE *file;
        if((file = fopen(argv[1],"r"))!=NULL)
        {
            fclose(file);
            char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
            char c, caps;
            int y = 0;
            int count;

            // now that we know file exists we can do the count for 26 different processes

            int pids[PROCESS_NUM];
            int pipes[PROCESS_NUM + 1][2];
            int i;
            for (i = 0; i < PROCESS_NUM + 1; i++) {
                if (pipe(pipes[i]) == -1) {
                    printf("Error with creating pipe\n");
                    return 1;
                }
            }

            for (i = 0; i < PROCESS_NUM; i++) {
                pids[i] = fork();
                if (pids[i] == -1) {
                    printf("Error with creating process\n");
                    return 2;
                }
                if (pids[i] == 0) {
                    // Child process
                    int j;
                    for (j = 0; j < PROCESS_NUM + 1; j++) {
                        if (i != j) {
                            close(pipes[j][0]);
                        }
                        if (i + 1 != j) {
                            close(pipes[j][1]);
                        }
                    }

                    int index =0;

                    if (read(pipes[i][0], &index, sizeof(int)) == -1) {
                        printf("Error at reading\n");
                        return 3;
                    }
                    //printf("(%d) Got %d\n", i, x);
                    //printf("(%d) Got %c\n", i, alphabet[x]);

                    // getting capital version of character in alphabet
                    caps = toupper(alphabet[index]);

                    // open file from user input
                    // read from file each character then count occurrence here
                    file = fopen(argv[1], "r");
                        while ((c = fgetc(file)) != EOF)
                        {
                            //character gets cheked to see if it matches the element in the array or the caps version
                            if(c == alphabet[index] || c==caps){
                                // adds up all the counts/occurence
                                count = count + 1;
                            }
                        }
                        // close file because we open it again for each fork
                        fclose(file);

                        printf("%c = %d\n", alphabet[index], count);
                        // moves up to the next index
                        index++;

                    if (write(pipes[i + 1][1], &index, sizeof(int)) == -1) {
                        printf("Error at writing\n");
                        return 4;
                    }
                    close(pipes[i][0]);
                    close(pipes[i + 1][1]);
                    return 0;
                }
            }

            // Main process
            int j;
            for (j = 0; j < PROCESS_NUM + 1; j++) {
                if (j != PROCESS_NUM) {
                    close(pipes[j][0]);
                }
                if (j != 0) {
                    close(pipes[j][1]);
                }
            }
            printf("===== word occurrence =====\n");

            if (write(pipes[0][1], &y, sizeof(int)) == -1) {
                printf("Error at writing\n");
                return 4;
            }
            if (read(pipes[PROCESS_NUM][0], &y, sizeof(int)) == -1) {
                printf("Error at reading\n");
                return 3;
            }

            close(pipes[0][1]);
            close(pipes[PROCESS_NUM][0]);

            for (i = 0; i < PROCESS_NUM; i++) {
                wait(NULL);
            }
            return 0;
        }
        else
        {
            //File not found
            printf("file not found!\n");
        }
    }
    else {
        printf("[Only] one argument expected!\n");
    }
}