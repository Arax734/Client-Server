#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <curses.h>

void sum_rows(int pipe_write, int rows[], int size) {
    int row_sum = 0;
    int i;

    for (i = 0; i < size; i++) {
        row_sum += rows[i];
    }

    write(pipe_write, &row_sum, sizeof(row_sum));
    close(pipe_write);
}

int main() {
    initscr();
    start_color();

    int num_servers;

    mvprintw(0, 0, "Number of servers: ");
    scanw("%d", &num_servers);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

    int size = num_servers
    int rows[size][size];

    for (int i = 0; i < size; i++) {
        mvprintw(2 + (size + 2) * i, 0, "Server %d:", i + 1);

        for (int j = 0; j < size; j++) {
            mvprintw(3 + (size + 2) * i + j, 0, "Element %d: ", j + 1);
            refresh();
            scanw("%d", &rows[i][j]);
        }
    }

    pid_t server_pid[size];
    int pipefd[size][2];

    for (int i = 0; i < size; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        server_pid[i] = fork();

        if (server_pid[i] == -1) {
            perror("fork");
            exit(1);
        } else if (server_pid[i] == 0) {
            close(pipefd[i][0]);
            sum_rows(pipefd[i][1], rows[i], size);
            close(pipefd[i][1]);
            exit(0);
        }
    }

    int total_sum = 0;
    int row_position = 2;
    int col_position = COLS / 2 + 2;

    for (int i = 0; i < size; i++) {
        int server_sum;
        if (read(pipefd[i][0], &server_sum, sizeof(server_sum)) == -1) {
            perror("read");
            exit(1);
        }
        close(pipefd[i][0]);
        total_sum += server_sum;
        attron(COLOR_PAIR(i + 1));
        mvprintw(row_position, col_position, "Server %d:", i + 1);
        row_position += 2;
        for (int j = 0; j < size; j++) {
            mvprintw(row_position, col_position + 4, "Element %d: %d", j + 1, rows[i][j]);
            row_position++;
        }
        mvprintw(row_position, col_position + 16, "Sum: %d", server_sum);
        row_position += 4;
        attroff(COLOR_PAIR(i + 1));
        if ((i + 1) % 3 == 0) {
            col_position += 30;
            row_position = 2;
        }
    }
    mvprintw(row_position, col_position, "Total sum: %d", total_sum);
    refresh();
    getch();
    endwin();
    for (int i = 0; i < size; i++) {
        waitpid(server_pid[i], NULL, 0);
    }
    return 0;
}