#ifndef FUNCTIONS_H_DEFINED
#define FUNCTIONS_H_DEFINED

typedef struct
{
    char bunnyName[32];
    char areaName[32];
    int countOfEntering;
    int countOfEggs;
} Applicant;

void create();
void display();
void modify();
void _delete();
void selectByArea();

#endif