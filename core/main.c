#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_EVENTS 1024 /*Max. number of events to process at one go*/
#define LEN_NAME 1024 /*Assuming length of the filename won't exceed 16 bytes*/
#define EVENT_SIZE (sizeof(struct inotify_event)) /*size of one event*/
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME)) /*buffer to store the data of events*/

void get_event(int fd)
{
    char buffer[BUF_LEN];
    int length, i = 0;

    length = read(fd, buffer, BUF_LEN);
    if (length < 0) {
        perror("read");
    }

    while (i < length) {
        struct inotify_event* event = (struct inotify_event*)&buffer[i];
        if (event->len) {
            if (event->mask & IN_MODIFY) {
                if (event->mask & IN_ISDIR) {
                    printf("The directory %s was modified.\n", event->name);
                } else {
                    printf("The file %s was modified with WD %d\n", event->name, event->wd);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
}

int monitor(char* path)
{
    int wd, fd;

    fd = inotify_init();
    if (fd < 0) {
        perror("Couldn't initialize inotify");
    }

    wd = inotify_add_watch(fd, path, IN_MODIFY);
    if (wd == -1) {
        printf("Couldn't add watch to %s\n", path);
    } else {
        printf("Watching:: %s\n", path);
    }

    /* do it forever*/
    while (1) {
        get_event(fd);
    }

    /* Clean up*/
    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}

char* remove_end(char* str, char c)
{
    char* last_pos = strrchr(str, c);

    if (last_pos != NULL) {
        *last_pos = '\0';
        return last_pos + 1; /* pointer to the removed part of the string */
    }

    return NULL; /* c wasn't found in the string */
}

int main(int argc, char* argv[])
{
    if (-1 != getopt(argc, argv, ""))
        goto END;

    const char* fname = "<stdin>";
    FILE* file = stdin;
    char* file_name;

    argc -= optind;
    argv += optind;

    if (argc > 0) {
        fname = argv[0];
        if (NULL == (file = fopen(fname, "r")))
            err(EXIT_FAILURE, "%s", fname);
    } else {
        fprintf(stdout, "Please pass your markdown file as first argument: mdv <foo.md>");
        exit(1);
    }

    char* path = realpath(fname, NULL);
    if (path == NULL) {
        printf("cannot find file with name[%s]\n", fname);
    } else {
        run_server();
        file_name = remove_end(path, '/');
        monitor(path);
        free(path);
    }

    return (EXIT_SUCCESS);
END:
    return (EXIT_FAILURE);
}

//struct abuf full_path = ABUF_INIT;

//// Extract the first token
//char* token = strtok(path, "/");
//abAppend(&full_path, token);

//// loop through the string to extract all other tokens
//while (token != NULL) {
//    token = strtok(NULL, " ");
//    //abAppend(&full_path, token);
//}
////abFree(&full_path);

////printf(" %s\n", full_path.b); //prin.ting each token
