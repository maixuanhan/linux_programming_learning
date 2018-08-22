#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LIFE_CHARACTER '#'

typedef struct Board
{
    int num_column;
    int num_row;
    char** data;
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
        board->num_column = 0;
        board->num_row = 0;
        return;
    }

    board->num_column = cols;
    board->num_row = rows;

    board->data = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; ++i)
    {
        board->data[i] = (char*)calloc(cols, sizeof(char));
    }
}

void dispose(Board_t* board)
{
    if (!is_valid(board)) return;

    for (int i = 0; i < board->num_row; ++i)
    {
        free(board->data[i]);
    }

    free(board->data);
    board->data = NULL;
    board->num_column = 0;
    board->num_row = 0;
}

void randomize_board(Board_t* board)
{
    if (!is_valid(board)) return;

    static int checked = 0;
    if (!checked)
    {
        srand(time(NULL)); // guarantee that this will be executed once only
        checked = 1;
    }

    int ran_percentage = board->num_column * board->num_row * (rand() % 20) / 100; // maximum 30%

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

    randomize_board(&board);

    print_board(&board);

    return EXIT_SUCCESS;
}
