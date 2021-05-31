#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MASTER_THREADS 4
#define MASTER_RANK 0
#define HORROR_RANK 1
#define COMEDY_RANK 2
#define FANTASY_RANK 3
#define SCIFI_RANK 4

char *input_file;
char *output_file;
pthread_barrier_t barrier;

int totalParagraphsM = 0;
int *orderM;

char *trim_space(char *str) {
    char *end;

    while (isspace(*str)) {
        str = str + 1;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }

    *(end+1) = '\0';
    return str;
}

void *read_and_send_to_workers(void *arg) {
    long id = *(long*) arg;

    char *type;

    if (id + 1 == HORROR_RANK) type = "horror";
    else if (id + 1 == COMEDY_RANK) type = "comedy";
    else if (id + 1 == FANTASY_RANK) type = "fantasy";
    else if (id + 1 == SCIFI_RANK) type = "science-fiction";

    FILE *fptr = fopen(input_file, "r");

    int no_paragraphs = 0;
    size_t len = 0;
    char *line;

    if (id == 0) {
        totalParagraphsM = 0;
        while (getline(&line, &len, fptr) != -1) {
            if (strcmp(trim_space(line), "horror") == 0 || strcmp(trim_space(line), "comedy") == 0
            || strcmp(trim_space(line), "fantasy") == 0 || strcmp(trim_space(line), "science-fiction") == 0) {
                totalParagraphsM++;
            }
        }

        fseek(fptr, 0, SEEK_SET);
        orderM = calloc(totalParagraphsM * sizeof(int), 1);
        int w = 0;
        while (getline(&line, &len, fptr) != -1) {
            if (strcmp(trim_space(line), "horror") == 0) {
                orderM[w++] = HORROR_RANK;
            } else if (strcmp(trim_space(line), "comedy") == 0) {
                orderM[w++] = COMEDY_RANK;
            } else if (strcmp(trim_space(line), "fantasy") == 0) {
                orderM[w++] = FANTASY_RANK;
            } else if (strcmp(trim_space(line), "science-fiction") == 0) {
                orderM[w++] = SCIFI_RANK;
            }
        }

        MPI_Send(&totalParagraphsM, 1, MPI_INT, HORROR_RANK, 2, MPI_COMM_WORLD);
        MPI_Send(&totalParagraphsM, 1, MPI_INT, COMEDY_RANK, 2, MPI_COMM_WORLD);
        MPI_Send(&totalParagraphsM, 1, MPI_INT, FANTASY_RANK, 2, MPI_COMM_WORLD);
        MPI_Send(&totalParagraphsM, 1, MPI_INT, SCIFI_RANK, 2, MPI_COMM_WORLD);
        MPI_Send(orderM, totalParagraphsM, MPI_INT, HORROR_RANK, 3, MPI_COMM_WORLD);
        MPI_Send(orderM, totalParagraphsM, MPI_INT, COMEDY_RANK, 3, MPI_COMM_WORLD);
        MPI_Send(orderM, totalParagraphsM, MPI_INT, FANTASY_RANK, 3, MPI_COMM_WORLD);
        MPI_Send(orderM, totalParagraphsM, MPI_INT, SCIFI_RANK, 3, MPI_COMM_WORLD);
        
        fseek(fptr, 0, SEEK_SET);
    }

    while (getline(&line, &len, fptr) != -1) {
        if (strcmp(trim_space(line), type) == 0) {
            no_paragraphs++;
        }
    }

    MPI_Send(&no_paragraphs, 1, MPI_INT, id + 1, 0, MPI_COMM_WORLD);

    char **paragraphs = calloc(no_paragraphs * sizeof(char*), 1);
    int *lengths = calloc(no_paragraphs * sizeof(int), 1);

    fseek(fptr, 0, SEEK_SET);

    int counter;
    int i = 0;

    while (getline(&line, &len, fptr) != -1) {
        if (strcmp(trim_space(line), type) == 0) {
            counter = 0;
            while(getline(&line, &len, fptr) != -1 && strcmp(line, "\n") != 0) {
                counter += strlen(line);
            }
            lengths[i] = counter;
            paragraphs[i] = calloc(lengths[i] * sizeof(char), 1);
            i++;
        }
    }
    
    fseek(fptr, 0, SEEK_SET);

    i = 0;
    int j = 0;
    while (getline(&line, &len, fptr) != -1) {
        if (strcmp(trim_space(line), type) == 0) {
            while(getline(&line, &len, fptr) != -1 && strcmp(line, "\n") != 0) {
                strcat(paragraphs[i], line);
            }
        
            MPI_Send(&lengths[i], 1, MPI_INT, id + 1, 1, MPI_COMM_WORLD);
            MPI_Send(paragraphs[i], lengths[i], MPI_CHAR, id + 1, 1, MPI_COMM_WORLD);
            i++;
        }
    }

    pthread_barrier_wait(&barrier);
    if (id == 0) {
        FILE *fp = fopen(output_file, "w");
        for (int i = 0; i < totalParagraphsM; i++) {
            int len = 0;
            char *curr_par;
            MPI_Recv(&len, 1, MPI_INT, orderM[i], i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            curr_par = calloc(len * sizeof(char), 1);
            MPI_Recv(curr_par, len, MPI_CHAR, orderM[i], i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            fputs(curr_par, fp);
        }
    }

    pthread_exit(NULL);
}

int main (int argc, char *argv[]) { 
    int  numtasks, rank, provided;
    long id;
    pthread_barrier_init(&barrier, NULL, MASTER_THREADS);

    input_file = argv[1];
    output_file = argv[2];

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    if (rank == MASTER_RANK) {

        pthread_t threadsM[MASTER_THREADS];
        long argumentsM[MASTER_THREADS];

        for (id = 0; id < MASTER_THREADS; id++) {
            argumentsM[id] = id;
            pthread_create(&threadsM[id], NULL, read_and_send_to_workers, &argumentsM[id]);
        }

        for (id = 0; id < MASTER_THREADS; id++) {
            pthread_join(threadsM[id], NULL);
        }
        
    } else if (rank == HORROR_RANK) {
        int no_paragraphs;
        char **paragraphs;
        char consonants[46];
        int i = 0;
        for (char c = 'b'; c <= 'z'; c++) {
            if (c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u') {
                consonants[i] = c;
                i++;
            }
        }

        for (char c = 'B'; c <= 'Z'; c++) {
            if (c != 'A' && c != 'E' && c != 'I' && c != 'O' && c != 'U') {
                consonants[i] = c;
                i++;
            }
        }
        
        int totalParagraphs = 0;

        MPI_Recv(&no_paragraphs, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&totalParagraphs, 1, MPI_INT, MASTER_RANK, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *order = calloc(totalParagraphs * sizeof(int), 1);
        MPI_Recv(order, totalParagraphs, MPI_INT, MASTER_RANK, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        int *paragraph_len = calloc(no_paragraphs * sizeof(int), 1);
        paragraphs = calloc(no_paragraphs * sizeof(char*), 1);

        for (int i = 0; i < no_paragraphs; i++) {
            MPI_Recv(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            paragraphs[i] = calloc(paragraph_len[i] * sizeof(char), 1);
            MPI_Recv(paragraphs[i], paragraph_len[i], MPI_CHAR, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            char *current_paragraph = calloc((2 * paragraph_len[i] + 100) * sizeof(char), 1);
            strcat(current_paragraph, "horror\n");

            for (int j = 0; j < paragraph_len[i]; j++) {

                char str[2];
                str[0] = paragraphs[i][j];
                str[1] = '\0';
                strcat(current_paragraph, str);
                for (int k = 0; k < 46; k++) {
                    if (consonants[k] == paragraphs[i][j]) {
                        str[0] = tolower(paragraphs[i][j]);
                        strcat(current_paragraph, str);
                        break;
                    }
                }
            }

            strcat(current_paragraph, "\n");
            paragraph_len[i] *= 2;
            paragraph_len[i] += 100;

            for (int j = 0; j <= totalParagraphs; j++) {
                if (order[j] == HORROR_RANK) {
                    MPI_Send(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, j, MPI_COMM_WORLD);
                    MPI_Send(current_paragraph, paragraph_len[i], MPI_CHAR, MASTER_RANK, j, MPI_COMM_WORLD);
                    order[j] = -1;
                    break;
                }
            }
        }
    } else if (rank == COMEDY_RANK) {
        int no_paragraphs;
        char **paragraphs;
        int totalParagraphs;

        MPI_Recv(&no_paragraphs, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&totalParagraphs, 1, MPI_INT, MASTER_RANK, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *order = calloc(totalParagraphs * sizeof(int), 1);
        MPI_Recv(order, totalParagraphs, MPI_INT, MASTER_RANK, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *paragraph_len = calloc(no_paragraphs * sizeof(int), 1);
        paragraphs = calloc(no_paragraphs * sizeof(char*), 1);

        for (int i = 0; i < no_paragraphs; i++) {
            MPI_Recv(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            paragraphs[i] = calloc(paragraph_len[i] * sizeof(char), 1);
            MPI_Recv(paragraphs[i], paragraph_len[i], MPI_CHAR, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            char *current_paragraph = calloc((paragraph_len[i] + 100) * sizeof(char), 1);
            strcat(current_paragraph, "comedy\n");

            int count = 1;
            for (int j = 0; j < paragraph_len[i]; j++) {
                if (paragraphs[i][j] == ' ' || paragraphs[i][j] == '\n') {
                    char str[1];
                    str[0] = paragraphs[i][j];
                    strcat(current_paragraph, str);
                    count = 1;
                } else if (count % 2 == 0) {
                    if (isdigit(paragraphs[i][j])) {
                        char str[1];
                        str[0] = paragraphs[i][j];
                        strcat(current_paragraph, str);
                    } else{
                        char str[1];
                        str[0] = toupper(paragraphs[i][j]);
                        strcat(current_paragraph, str);
                    }
                    count++;
                } else {
                    char str[1];
                    str[0] = paragraphs[i][j];
                    strcat(current_paragraph, str);
                    count++;
                }
            }

            strcat(current_paragraph, "\n");
            for (int j = 0; j < totalParagraphs; j++) {
                if (order[j] == COMEDY_RANK) {
                    paragraph_len[i] += 100;
                    MPI_Send(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, j, MPI_COMM_WORLD);
                    MPI_Send(current_paragraph, paragraph_len[i], MPI_CHAR, MASTER_RANK, j, MPI_COMM_WORLD);
                    order[j] = -1;
                    break;
                }
            }
        }

    } else if (rank == FANTASY_RANK) {
        int no_paragraphs;
        char **paragraphs;
        int totalParagraphs;

        MPI_Recv(&no_paragraphs, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&totalParagraphs, 1, MPI_INT, MASTER_RANK, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *order = calloc(totalParagraphs * sizeof(int), 1);
        MPI_Recv(order, totalParagraphs, MPI_INT, MASTER_RANK, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *paragraph_len = calloc(no_paragraphs * sizeof(int), 1);
        paragraphs = calloc(no_paragraphs * sizeof(char*), 1);

        for (int i = 0; i < no_paragraphs; i++) {
            MPI_Recv(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            paragraphs[i] = calloc(paragraph_len[i] * sizeof(char), 1);
            MPI_Recv(paragraphs[i], paragraph_len[i], MPI_CHAR, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            char *current_paragraph = calloc((paragraph_len[i] + 100) * sizeof(char), 1);
            strcat(current_paragraph, "fantasy\n");

            int count = 1;
            for (int j = 0; j < paragraph_len[i]; j++) {
                if (paragraphs[i][j] == ' ' || paragraphs[i][j] == '\n') {
                    char str[1];
                    str[0] = paragraphs[i][j];
                    strcat(current_paragraph, str);
                    count = 1;
                } else if (count == 1) {
                    char str[1];
                    str[0] = toupper(paragraphs[i][j]);
                    strcat(current_paragraph, str);
                    count++;
                } else {
                    char str[1];
                    str[0] = paragraphs[i][j];
                    strcat(current_paragraph, str);
                    count++;
                }
            }

            strcat(current_paragraph, "\n");
            for (int j = 0; j < totalParagraphs; j++) {
                if (order[j] == FANTASY_RANK) {
                    paragraph_len[i] += 100;
                    MPI_Send(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, j, MPI_COMM_WORLD);
                    MPI_Send(current_paragraph, paragraph_len[i], MPI_CHAR, MASTER_RANK, j, MPI_COMM_WORLD);
                    order[j] = -1;
                    break;
                }
            }
        }

    } else if (rank == SCIFI_RANK) {
        int no_paragraphs;
        char **paragraphs;
        int totalParagraphs;

        MPI_Recv(&no_paragraphs, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&totalParagraphs, 1, MPI_INT, MASTER_RANK, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *order = calloc(totalParagraphs * sizeof(int), 1);
        MPI_Recv(order, totalParagraphs, MPI_INT, MASTER_RANK, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *paragraph_len = calloc(no_paragraphs * sizeof(int), 1);
        paragraphs = calloc(no_paragraphs * sizeof(char*), 1);

        for (int i = 0; i < no_paragraphs; i++) {
            MPI_Recv(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            paragraphs[i] = calloc(paragraph_len[i] * sizeof(char), 1);
            MPI_Recv(paragraphs[i], paragraph_len[i], MPI_CHAR, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            char *current_paragraph = calloc((paragraph_len[i] + 100) * sizeof(char), 1);
            strcat(current_paragraph, "science-fiction\n");

            int count = 1;
            for (int j = 0; j < paragraph_len[i]; j++) {
                if (count == 7) {
                    int prev = j;
                    while (paragraphs[i][j] != ' ' && paragraphs[i][j] != '\n') {
                        j++;
                    }
                    j--;
                    for (int k = j; k >= prev; k--) {
                        char str[1];
                        str[0] = paragraphs[i][k];
                        strcat(current_paragraph, str);
                    }
                    count = 1;
                } else if (paragraphs[i][j] == ' ' || paragraphs[i][j] == '\n') {
                    char str[1];
                    str[0] = paragraphs[i][j];
                    strcat(current_paragraph, str);
                    count++;
                } else {
                    char str[1];
                    str[0] = paragraphs[i][j];
                    strcat(current_paragraph, str);
                }
            }
    
            strcat(current_paragraph, "\n");

            for (int j = 0; j < totalParagraphs; j++) {
                if (order[j] == SCIFI_RANK) {
                    paragraph_len[i] += 100;
                    MPI_Send(&paragraph_len[i], 1, MPI_INT, MASTER_RANK, j, MPI_COMM_WORLD);
                    MPI_Send(current_paragraph, paragraph_len[i], MPI_CHAR, MASTER_RANK, j, MPI_COMM_WORLD);
                    order[j] = -1;
                    break;
                }
            }
        }
    }

    pthread_barrier_destroy(&barrier);
    MPI_Finalize();
    return 0;
}
