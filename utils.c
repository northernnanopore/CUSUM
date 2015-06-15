#include"utils.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

inline void progressbar(uint64_t pos, uint64_t finish)
{
    // Calculuate the ratio of complete-to-incomplete.
    double ratio = pos/(double)finish;
    int   c     = (int) (ratio * 40)+1;

    // Show the percentage complete.
    printf("%3d%% [", (int)(ratio*100+1) );

    // Show the load bar.
    int i;
    for (i=0; i<c; i++)
       printf("=");

    for (i=c; i<40; i++)
       printf(" ");

    // ANSI Control codes to go back to the
    // previous line and clear it.
    printf("]\r");
    fflush(stdout);
}

uint64_t get_filesize(FILE *input, int datatype)
{
    uint64_t length;
    fseeko64(input, 0, SEEK_END);
    length = ftello64(input);
    fseeko64(input, 0, SEEK_SET);
    return length / (datatype / 8 * 2);
}


int signum(double num)
{
    return (EPS<num)-(num<-EPS);
}
double my_min(double a, double b)
{
    return a < b ? a : b;
}

double my_max(double a, double b)
{
    return a > b ? a : b;
}

double d_abs(double num)
{
    return num > 0 ? num : -num;
}

int64_t intmin(int64_t a, int64_t b)
{
    return a < b ? a : b;
}
int64_t intmax(int64_t a, int64_t b)
{
    return a > b ? a : b;
}

edge *initialize_edges(void)
{
    edge *head;
    if ((head = malloc(sizeof(edge)))==NULL)
    {
        printf("Cannot allocate head node\n");
        abort();
    }
    head->next = NULL;
    head->location = 0;
    head->type = HEAD;
    return head;
}

event *delete_bad_events(event *head)
{
    event *newhead;
    event *current;
    event *tempnext;
    event *tempprev;
    event *temp;
    current = head;
    newhead = head;
    while (current)
    {
        if (current->type != 0)
        {
           tempnext = current->next;
           tempprev = current->prev;
           temp = current;
           current = current->next;
           free_single_event(temp);
           if (tempprev && tempnext)
           {
               tempprev->next = tempnext;
               tempnext->prev = tempprev;
           }
           else if (!tempprev && tempnext) //head of the list
           {
               newhead = tempnext;
               newhead->index = 0;
               tempnext->prev = NULL;
           }
           else if (tempprev && !tempnext) //end of the list
           {
               tempprev->next = NULL;
           }
           else if (!tempprev && !tempnext) //singleton list, odd case
           {
               newhead = NULL;
           }
        }
        else
        {
            current = current->next;
        }
    }
    return newhead;
}

edge *add_edge(edge *current, uint64_t location, int type)
{
    if (current->type == HEAD) //if we are adding a new node to the head node that hasn't been filled yet
    {
        current->location = location;
        current->type = type;
    }
    else
    {//if the current node is filled with useful information and we actually need more memory
        if ((current->next = malloc(sizeof(edge)))==NULL)
        {
            printf("Cannot allocate new node after %" PRIu64 "\n",current->location);
            abort();
        }
        current->next->location = location;
        current->next->type = type;
        current->next->next = NULL;
        current = current->next;
    }
    return current;
}

void free_edges(edge *current)
{
    if (current)
    {
        edge *temp;
        while (current->next)
        {
            temp = current;
            current = current->next;
            free(temp);
        }
        free(current);
    }
}


event *initialize_events(void)
{
    event *head;
    if ((head = malloc(sizeof(event)))==NULL)
    {
        printf("Cannot allocate head node for event list\n");
        abort();
    }
    head->type = 0;
    head->index = HEAD;
    head->first_edge = NULL;
    head->next = NULL;
    head->prev = NULL;
    return head;
}

event *add_event(event *current, uint64_t start, uint64_t finish)
{
    if (current->index == HEAD) //if we are adding a new node to the head node that hasn't been filled yet
    {
        current->index = 0;
        current->start = start;
        current->finish = finish;
        current->length = finish-start;
        current->first_edge = NULL;
        current->next = NULL;
        current->prev = NULL;
    }
    else
    {//if the current node is filled with useful information and we actually need more memory
        if ((current->next = malloc(sizeof(event)))==NULL)
        {
            printf("Cannot allocate new event node after %" PRIu64 "\n",current->index);
            abort();
        }
        current->next->type = 0;
        current->next->first_edge = NULL;
        current->next->next = NULL;
        current->next->signal = NULL;
        current->next->prev = current;
        current->next->index = current->index + 1;
        current->next->start = start;
        current->next->finish = finish;
        current->next->length = finish-start;
        current = current->next;
    }
    return current;
}

void free_events(event *current)
{
    while (current->next)
    {
        current = current->next;
        free_single_event(current->prev);
    }
    free_single_event(current);
}

void free_single_event(event *current)
{
    if (current)
    {
        if (current->type == 0)
        {
            free(current->signal);
            free(current->filtered_signal);
            if (current->first_edge)
            {
                free(current->first_edge);
            }
        }
        free(current);
    }
}

double ARL(uint64_t length, double sigma, double mun, double h)
{
    return (exp(-2.0*mun*(h/sigma+1.166))-1.0+2.0*mun*(h/sigma+1.166))/(2.0*mun*mun)-(double) length;
}

