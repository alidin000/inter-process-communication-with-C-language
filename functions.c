#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"

#define false 0
#define true 1

// declaring the areas
char *areas[] = {"Baratfa", "Lovas", "Szula", "Kigyospatak", "Malomtelek", "Paskom", "Kaposztaskert"};

// display function
void display()
{
    if (access("applicants.txt", F_OK) != 0)
    {
        printf("There are no applicants yet!\n");
        return;
    }

    Applicant ap1;
    FILE *myfile;

    myfile = fopen("applicants.txt", "r");
    int count = 1;

    // creating a user friendly table for output
    printf("------------------------Current data---------------------\n");
    printf("---------------------------------------------------------\n");
    printf("| # |      Name         |  Name of the area  |N.of.Entry|\n");
    printf("---------------------------------------------------------\n");

    // reading till EOF
    while (fread(&ap1, sizeof(Applicant), 1, myfile))
    {
        printf("|%3d|%19s|%20s|%10d|\n", count++, ap1.bunnyName, ap1.areaName, ap1.countOfEntering);
    }
    printf("---------------------------------------------------------\n");

    // closing the opened file
    fclose(myfile);
}

// modifying the applicants data using temporary file
void modify()
{
    Applicant ap1;
    FILE *myfile, *myfile1;
    myfile = fopen("applicants.txt", "r");
    myfile1 = fopen("temp.txt", "w");
    int count = 1, found = 0;
    char name[32];
    printf("Please enter the applicants name to modify >>>");
    scanf("%s", name);
    while (fread(&ap1, sizeof(Applicant), 1, myfile))
    {
        if (strcmp(ap1.bunnyName, name) == 0)
        {
            found = 1;

            printf("Please input new name >>>");
            char tempName[32];
            scanf("%32s", tempName);
            strcpy(ap1.bunnyName, tempName);
            printf("Choose which are you belong to? \n");
            for (int i = 1; i <= 7; i++)
            {
                printf("%d: %s\t", i, areas[i - 1]);
            }
            printf("\n");

            int a;
            scanf("%d", &a);
            while (a > 6 || a < 1)
            {
                printf("wrong input, your input should between 0-6 >>>");
                scanf("%d", &a);
            }
            strcpy(ap1.areaName, areas[a]);

            printf("how many times you are entering this competition? >>>");
            int a2;
            scanf("%d", &a);
            ap1.countOfEntering = a;
        }

        fwrite(&ap1, sizeof(Applicant), 1, myfile1);
    }

    // free up the memory used and closing the opened file
    fclose(myfile1);
    fclose(myfile);

    if (!found)
    {
        printf("applicant not found!!!");
    }
    else
    {
        myfile1 = fopen("applicants.txt", "w");
        myfile1 = fopen("temp.txt", "r");

        while (fread(&ap1, sizeof(Applicant), 1, myfile1))
            fwrite(&ap1, sizeof(Applicant), 1, myfile);

        // free up the memory used and closing the opened file
        fclose(myfile1);
        fclose(myfile);
    }
}

// deleting an applicant using temporary file
void _delete()
{
    Applicant ap1;
    FILE *myfile, *myfile1;

    myfile = fopen("applicants.txt", "r");
    myfile1 = fopen("temp.txt", "w");

    int found = 0;
    char name[32];

    printf("Please enter the applicants name to delete >>>");
    scanf("%s", name);
    // reading from a file using fread till EOF
    while (fread(&ap1, sizeof(Applicant), 1, myfile))
    {
        if (strcmp(ap1.bunnyName, name) == 0)
        {
            found = 1;
            continue;
        }
        fwrite(&ap1, sizeof(Applicant), 1, myfile1);
    }

    // free up the memory used and closing the opened file
    fclose(myfile1);
    fclose(myfile);

    if (!found)
    {
        printf("applicant not found!!!\n");
    }
    else
    {
        myfile1 = fopen("applicants.txt", "w");
        myfile1 = fopen("temp.txt", "r");

        // reading from a file using fread
        while (fread(&ap1, sizeof(Applicant), 1, myfile1))
            fwrite(&ap1, sizeof(Applicant), 1, myfile);

        // free up the memory used and closing the opened file
        fclose(myfile1);
        fclose(myfile);
    }
}

// creating an applicant

void create()
{
    FILE *myfile;
    myfile = fopen("applicants.txt", "a");

    int currenSize = 2;
    Applicant *Applicants = (Applicant *)malloc(currenSize * sizeof(Applicant));
    int aplicantCount = 0;

    while (true)
    {
        if (aplicantCount >= currenSize)
        {
            currenSize *= 2;
            Applicants = (Applicant *)realloc(Applicants, currenSize * sizeof(Applicant));
            if (Applicants == NULL)
            {
                printf("Memory reallocation failed");
                exit(0);
            }
        }

        printf("*****************************************************************\n");
        printf("Please input your name (without any spaces) >>>");

        char tempName[32];
        scanf("%32s", tempName);
        strcpy(Applicants[aplicantCount].bunnyName, tempName);

        printf("Choose which are you belong to? \n");
        for (int i = 1; i <= 7; i++)
        {
            printf("%d: %s\t", i, areas[i - 1]);
        }
        printf("\n");

        int a;
        scanf("%d", &a);

        while (a > 7 || a < 1)
        {
            printf("wrong input, your input should between 1-7 >>>");
            scanf("%d", &a);
        }

        strcpy(Applicants[aplicantCount].areaName, areas[a - 1]);

        printf("how many times you are entering this competition? >>>");
        int a2;
        scanf("%d", &a);
        Applicants[aplicantCount].countOfEntering = a;
        aplicantCount++;

        // writing the the file using fwrite
        fwrite(&Applicants[aplicantCount - 1], sizeof(Applicant), 1, myfile);

        printf("any other competitors?:  ");
        printf("Yes(y)  No(n) >>>");
        char a3;
        char a33;
        scanf("%c", &a33);
        scanf("%c", &a3);
        if (a3 == 'n')
            break;
    }

    // free up the memory used and closing the opened file
    free(Applicants);
    fclose(myfile);
}

// selecting by area given by the user
void selectByArea()
{
    Applicant ap1;
    FILE *myfile;
    myfile = fopen("applicants.txt", "r");

    printf("Choose which area do you want to check? \n");
    for (int i = 1; i <= 7; i++)
    {
        printf("%d: %s\t", i, areas[i - 1]);
    }
    printf("\n");

    int a;
    scanf("%d", &a);
    while (a > 7 || a < 1)
    {
        printf("wrong input, your input should between 1-7 >>>");
        scanf("%d", &a);
    }

    int count = 1;

    // creating a user friendly table for output
    printf("------------------------Current data---------------------\n");
    printf("---------------------------------------------------------\n");
    printf("| # |      Name         |  Name of the area  |N.of.Entry|\n");
    printf("---------------------------------------------------------\n");

    while (fread(&ap1, sizeof(Applicant), 1, myfile))
        if (strcmp(areas[a - 1], ap1.areaName) == 0)
            printf("|%3d|%19s|%20s|%10d|\n", count++, ap1.bunnyName, ap1.areaName, ap1.countOfEntering);

    printf("---------------------------------------------------------\n");

    // closing the opened file
    fclose(myfile);
}
