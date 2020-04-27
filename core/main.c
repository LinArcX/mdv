// Man pages: http://man7.org/linux/man-pages
// Posix: https://pubs.opengroup.org/onlinepubs/9699919799/functions/

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

#include <extern.h>
#include <server.h>

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
                    //update_html();
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
        //printf("Watching:: %s\n", path);
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

char* remove_last_character(char* str, char c)
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

    // You should pass a file address to read by mdv.
    if (-1 != getopt(argc, argv, ""))
        goto END;

    if (argc > 0) {
        fname = argv[0];
    } else {
        fprintf(stdout, "Please pass your markdown file as first argument: mdv <foo.md>");
        exit(1);
    }

    char* path = realpath(fname, NULL);
    if (path == NULL) {
        printf("Cannot find file: %s\n", fname);
    } else {
        run_server();
        //update_html();
        file_name = remove_last_character(path, '/');
        monitor(path);
        free(path);
    }
    return (EXIT_SUCCESS);
END:
    return (EXIT_FAILURE);
}
