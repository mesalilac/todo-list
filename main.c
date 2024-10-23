#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char *task;
    int is_done;
} Todo;

typedef struct
{
    Todo **list;
    int size;

    /// If true save todos to file
    /// To prevent unnecessary writes
    bool changed;
} Todos;

Todos *load_todos(char *file_name)
{
    Todos *todos = malloc(sizeof(Todos));
    todos->size  = 0;

    FILE *fptr = fopen(file_name, "r");
    if (fptr == NULL)
    {
        if (errno == ENOENT)
        {
            return todos;
        }

        else
        {
            fprintf(
                stderr,
                "ERROR: Failed to open file '%s': errno %i\n",
                file_name,
                errno
            );
            exit(1);
        }
    }

    todos->list = malloc(sizeof(Todo) * 100);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fptr)) != -1)
    {
        todos->list[todos->size]          = malloc(sizeof(Todo));
        todos->list[todos->size]->task    = malloc(sizeof(char) * 100);
        todos->list[todos->size]->is_done = 0;

        char *tmp = malloc(sizeof(char) * 100);
        int tmp_i = 0;

        int i = 0;
        while (line[i] != '\0')
        {
            if (line[i] == '|')
            {
                tmp[tmp_i] = '\0';

                strcpy(todos->list[todos->size]->task, tmp);

                tmp_i = 0;
            }
            else if (line[i + 2] == '\0')
            {
                // NOTE: it's line[i+2] because the line ends with \n\0
                if (line[i] == '1')
                {
                    todos->list[todos->size]->is_done = 1;
                }
                else if (line[i] == '0')
                {
                    todos->list[todos->size]->is_done = 0;
                }
            }
            else
            {
                tmp[tmp_i] = line[i];
                tmp_i++;
            }

            i++;
        }

        free(tmp);
        todos->size++;
    }

    fclose(fptr);
    if (line)
        free(line);

    return todos;
}

bool save_todos(char *file_name, Todos *todos)
{
    FILE *fptr = fopen(file_name, "w");
    if (fptr == NULL)
    {
        return false;
    }

    for (int i = 0; i < todos->size; ++i)
        fprintf(fptr, "%s|%i\n", todos->list[i]->task, todos->list[i]->is_done);

    fclose(fptr);

    return true;
}

void print_help()
{
    printf("list\tList all todos\n");
    printf("add\tAdd a new task\n");
    printf("done\tMark task as done\n");
    printf("todo\tMark task as todo\n");
    printf("help\tPrint this message\n");
}

int main()
{
    /* char *file_name = "todos-save.list"; */
    char *file_name           = "todos.list";
    char *file_name_save_file = "todos.list.save";
    Todos *todos              = load_todos(file_name);

    bool quit = false;

    print_help();

    while (!quit)
    {
        char *command = malloc(sizeof(char) * 10);
        printf("> ");
        scanf("%s", command);
        getchar();

        if (strcmp(command, "list") == 0)
        {
            for (int i = 0; i < todos->size; ++i)
            {
                printf(
                    "#%i %s %s\n",
                    i,
                    todos->list[i]->is_done == true ? "DONE" : "    ",
                    todos->list[i]->task
                );
            }
        }
        else if (strcmp(command, "add") == 0)
        {
            char *task = malloc(sizeof(char) * 100);

            printf("task: ");
            fgets(task, 100, stdin);
            task[strcspn(task, "\n")] = 0;

            todos->list[todos->size]          = malloc(sizeof(Todo));
            todos->list[todos->size]->task    = malloc(strlen(task) + 1);
            todos->list[todos->size]->is_done = 0;

            strcpy(todos->list[todos->size]->task, task);

            todos->size++;
            free(task);
            todos->changed = true;
        }
        else if (strcmp(command, "done") == 0)
        {
            int index = 0;
            printf("task index: ");
            scanf("%i", &index);
            getchar();

            if (index > todos->size || index < 0)
            {
                printf("Task %i does not exists!\n", index);
                free(command);
                continue;
            }

            todos->list[index]->is_done = true;
            todos->changed              = true;
        }
        else if (strcmp(command, "todo") == 0)
        {
            int index = 0;
            printf("task index: ");
            scanf("%i", &index);
            getchar();

            if (index > todos->size || index < 0)
            {
                printf("Task %i does not exists!\n", index);
                free(command);
                continue;
            }

            todos->list[index]->is_done = false;
            todos->changed              = true;
        }
        else if (strcmp(command, "quit") == 0)
            quit = true;
        else if (strcmp(command, "help") == 0)
            print_help();
        else
        {
            printf("Invaild command '%s'\n", command);
        }

        free(command);
    }

    if (todos->changed)
        save_todos(file_name_save_file, todos);

    for (int i = 0; i < todos->size; ++i)
    {
        free(todos->list[i]->task);
        free(todos->list[i]);
    }
    free(todos->list);
    free(todos);
    todos = NULL;

    return 0;
}
