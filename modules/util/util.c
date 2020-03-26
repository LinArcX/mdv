#include <stdlib.h>
#include <string.h>

#include "util.h"
void abAppend(struct abuf* ab, const char* s)
{
    char* new = realloc(ab->b, ab->len + strlen(s));
    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, strlen(s));
    ab->b = new;
    ab->len += strlen(s);
}

void abFree(struct abuf* ab)
{
    free(ab->b);
}

char* get_month_name(int month_number)
{
    //char* month_name = "";
    switch (month_number) {
    case 1:
        return "Jan";
        break;
    case 2:
        return "Feb";
        break;
    case 3:
        return "Mar";
        break;
    case 4:
        return "Apr";
        break;
    case 5:
        return "May";
        break;
    case 6:
        return "Jun";
        break;
    case 7:
        return "Jul";
        break;
    case 8:
        return "Aug";
        break;
    case 9:
        return "Sep";
        break;
    case 10:
        return "Oct";
        break;
    case 11:
        return "Nov";
        break;
    case 12:
        return "Dec";
        break;
    default:
        return "!";
        break;
    }
}
