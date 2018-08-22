#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#define LIFE_CHARACTER '.'

typedef struct Board
{
    int num_column;
    int num_row;
    char** data;
    char** swap;
} Board_t;

int is_valid(Board_t* board)
{
    return board != NULL && board->data != NULL;
}

void init(int rows, int cols, Board_t* board)
{
    if (board == NULL) return;

    if (rows <= 0 || cols <= 0)
    {
        board->data = NULL;
        board->swap = NULL;
        board->num_column = 0;
        board->num_row = 0;
        return;
    }

    board->num_column = cols;
    board->num_row = rows;

    board->data = (char**)malloc(rows * sizeof(char*));
    board->swap = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; ++i)
    {
        board->data[i] = (char*)calloc(cols, sizeof(char));
        board->swap[i] = (char*)calloc(cols, sizeof(char));
    }
}

void dispose(Board_t* board)
{
    if (!is_valid(board)) return;

    for (int i = 0; i < board->num_row; ++i)
    {
        free(board->data[i]);
        free(board->swap[i]);
    }

    free(board->data);
    board->data = NULL;

    free(board->swap);
    board->swap = NULL;

    board->num_column = 0;
    board->num_row = 0;
}

void randomize_board(Board_t* board, int max_density)
{
    if (!is_valid(board)) return;

    static int checked = 0;
    if (!checked)
    {
        srand(time(NULL)); // guarantee that this will be executed once only
        checked = 1;
    }

    int ran_percentage = board->num_column * board->num_row * (rand() % (max_density % 100)) / 100;

    for (int i = 0; i < ran_percentage; ++i)
    {
        while (1)
        {
            int row = rand() % board->num_row;
            int col = rand() % board->num_column;

            if (board->data[row][col] == 0)
            {
                board->data[row][col] = 1;
                break;
            }
        }
    }
}

int count_around(Board_t *board, int row, int col)
{
    // skip this check as it is guarantee to be valid
    // if (!is_valid(board) || row < 0 || row >= board->num_row || col < 0 || col >= board->num_column)
    //     return 0;

    int count = 0;
    if (row > 0)
    {
        if (col > 0) count += board->data[row - 1][col - 1];

        count += board->data[row - 1][col];

        if (col < board->num_column - 1) count += board->data[row - 1][col + 1];
    }

    if (col > 0) count += board->data[row][col - 1];

    if (col < board->num_column - 1) count += board->data[row][col + 1];

    if (row < board->num_row - 1)
    {
        if (col > 0)
            count += board->data[row + 1][col - 1];
        count += board->data[row + 1][col];
        if (col < board->num_column - 1)
            count += board->data[row + 1][col + 1];
    }

    return count;
}

void calculate_next_state(Board_t *board)
{
    if (!is_valid(board))
        return;

    // 1. Any live cell with fewer than two live neighbors dies, as if by under population.
    // 2. Any live cell with two or three live neighbors lives on to the next generation.
    // 3. Any live cell with more than three live neighbors dies, as if by overpopulation.
    // 4. Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    for (int i = 0; i < board->num_row; ++i)
    {
        for (int j = 0; j < board->num_column; ++j)
        {
            int count = count_around(board, i, j);
            if (board->data[i][j])
            {
                if (count == 2 || count == 3)
                    board->swap[i][j] = 1;
                else
                    board->swap[i][j] = 0;
            }
            else
            {
                if (count == 3)
                    board->swap[i][j] = 1;
                else
                    board->swap[i][j] = 0;
            }
        }
    }
}

void swap_data(Board_t *board)
{
    if (!is_valid(board))
        return;

    char **tmp = board->data;
    board->data = board->swap;
    board->swap = tmp;
}

void print_board_info(Board_t* board)
{
    if (board == NULL) return;

    printf("Board is constructed with size %d X %d\n", board->num_row, board->num_column);
}

void print_board(Board_t* board)
{
    if (!is_valid(board)) return;

    for (int i = 0; i < board->num_row; ++i)
    {
        for (int j = 0; j < board->num_column; ++j)
        {
            if (board->data[i][j])
            {
                printf("%c", LIFE_CHARACTER);
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int get_int_output_from_cmd(const char* cmd)
{
    if (cmd == NULL) return 0;

    FILE* fp;
    char var[32];
    int value = 0;

    fp = popen(cmd, "r");
    if (fgets(var, sizeof(var), fp) != NULL)
    {
        value = atoi(var);
    }

    pclose(fp);

    return value;
}

int main()
{
    int rows = get_int_output_from_cmd("tput lines");
    int cols = get_int_output_from_cmd("tput cols");

    Board_t board;
    init(rows - 1, cols - 1, &board);
    // print_board_info(&board);

    randomize_board(&board, 40);

    while (1)
    {
        print_board(&board);
        calculate_next_state(&board);

        swap_data(&board);
        usleep(200000); // 0.2s
    }

    return EXIT_SUCCESS;
}
