#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"
#include "functions2.h"

int check = 1;

int main(int argc, char *argv[])
{
    system("clear");
    // checking if there are already some applicants in the file
    if (access("applicants.txt", F_OK) == 0)
    {
        display();
        // system("rm applicants.txt");
    }
    else
    {
    }
    int choice = 0;
    do
    {
        printf("\n1.CREATE");
        printf("\n2.DISPLAY");
        printf("\n3.MODIFY");
        printf("\n4.DELETE");
        printf("\n5.SELECT BY AREA");
        printf("\n6.START THE COMPETITION");
        printf("\n7.GET RESULTS OF COMPETITION");
        printf("\n0.EXIT");
        printf("\n\nENTER YOUR CHOICE >>>");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            create();
            system("clear");
            break;
        case 2:
            system("clear");
            display();
            break;
        case 3:
            system("clear");
            display();
            modify();
            break;

        case 4:
            system("clear");
            display();
            _delete();
            break;
        case 5:
            system("clear");
            display();
            selectByArea();
            break;
        case 6:
            system("clear");
            startCompetition();
            break;
        case 7:
            system("clear");
            getResults();
            break;
        default:
            break;
        }
    } while (choice != 0);

    return 0;
}