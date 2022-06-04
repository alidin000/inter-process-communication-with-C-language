#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for pipe()
#include <signal.h>
#include <string.h>
#include "functions2.h"
#include "functions.h"

//signal handlers
void handle_child1(int signum)
{
    printf("\n-First supervisor is ready!\n");
    return;
}
void handle_child2(int signum)
{
    printf("\n-Second supervisor is ready!\n");
    return;
}


void startCompetition()
{
    if (access("applicants.txt", F_OK) != 0)
    {
        printf("\n!!! There are no applicants, first create some!!!\n");
        return;
    }
    

    // declaring signals we will work with                  //SIGNALS
    signal(SIGUSR1, handle_child1);
    signal(SIGUSR2, handle_child2);

    // variables to return
    char winner[32];
    int winnerPoints;

    // creating an array of structs for data to be sent and received
    int count1 = 3;
    int count2 = 3;
    Applicant *supervisor1 = getFirst(&count1);  
    Applicant *supervisor2 = getSecond(&count2);

    // dividingIntoTwo(supervisor1, &count1, supervisor2, &count2);
    printf("%d in first supervisors competition and  %d in second supervisors competition\n", count1, count2);

    Applicant *result = (Applicant *)malloc(sizeof(Applicant) * count1);
    Applicant *result2 = (Applicant *)malloc(sizeof(Applicant) * count2);

    Applicant *data1 = (Applicant *)malloc(sizeof(Applicant) * count1);
    Applicant *data2 = (Applicant *)malloc(sizeof(Applicant) * count2);

    // creating 4 pipes for sending and receiving data
    int pipefd[2];
    int pipefd2[2];
    int pipefdRec[2];
    int pipefd2Rec[2];

    // creating 2 process id
    pid_t pid;
    pid_t pid2;

    // checking for pipes
    if (pipe(pipefd2) == -1)
    {
        perror("Opening error!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd) == -1)
    {
        perror("Opening error!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefdRec) == -1)
    {
        perror("Opening error!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2Rec) == -1)
    {
        perror("Opening error!");
        exit(EXIT_FAILURE);
    }

    // creating parent-child processes
    pid = fork(); 
    srand(time(NULL)+getpid());
    if (pid == -1)
    {
        perror("Fork error");
        exit(EXIT_FAILURE);
    }

    // first supervisor
    if (pid == 0) 
    {
        // sending a signal to the parent proccess  
        sleep(1);
        kill(getppid(), SIGUSR1);

        // closing not used pipes
        close(pipefd[1]);
        close(pipefdRec[0]);
        sleep(1);
        printf("first supervisor starts the competition!\n");

        sleep(1);

        //reading the data from the pipe1
        read(pipefd[0], data1, sizeof(Applicant) * count1);
        for (int i = 0; i < count1; i++)
        {
            data1[i].countOfEggs = rand() % 100 + 1;
            printf("first supervisor got the data:\n area name => %s  name => %s->%d\n", data1[i].areaName, data1[i].bunnyName, data1[i].countOfEggs);
        }
        printf("\n");

        printf("Sending back the results\n");

        //sending back the results of the competition
        write(pipefdRec[1], data1, sizeof(Applicant) * count1);

        // closing used pipes
        close(pipefdRec[1]);
        close(pipefd[0]);
        printf("First supervisor finished the competition and sent the results to the Chief Bunny!\n");
    }
    else // Parent process
    {
        pid2 = fork();
        if (pid2 == -1)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) // second supervisor
        {
            // waiting for the first child to finish
            sleep(4);
            kill(getppid(), SIGUSR2);
            // closing not used pipes
            close(pipefd2[1]);
            close(pipefd2Rec[0]);

            printf("second supervisor started the competition!\n");

            // reading the data from the pipe2
            read(pipefd2[0], data2, sizeof(Applicant) * count2);
            for (int i = 0; i < count2; i++)
            {
                data2[i].countOfEggs = rand() % 100 + 1;
                printf("second supervisor got the data:\n area name => %s name=> %s->%d\n", data2[i].areaName, data2[i].bunnyName, data2[i].countOfEggs);
            }
            printf("\n");

            // sending the results of the competition
            write(pipefd2Rec[1], data2, sizeof(Applicant) * count2);

            // closing all used pipes
            close(pipefd2Rec[1]);
            close(pipefd2[0]);

            printf("Second supervisor finished the competition and sent the results to the Chief Bunny!\n");
        }

        // parent process
        else 
        {
            //creating a file to store the results of the competitions
            FILE *myfile;
            myfile = fopen("results.txt", "w");

            // closing not needed pipes
            close(pipefd[0]);
            close(pipefd2[0]);
            close(pipefdRec[1]);
            close(pipefd2Rec[1]);

            printf("Chief bunny starts!\n");

            // seding data to the first supervisor
            write(pipefd[1], supervisor1, sizeof(Applicant) * count1);

            printf("Chief bunny sent the data to the first supervisor!\n");

            // waiting and then reading the received data
            sleep(3);
            printf("receiving data back from first supervisor\n");
        
            //getting the results of the competition
            read(pipefdRec[0], result, sizeof(Applicant) * count1);
            for (int i = 0; i < count1; i++)
            {
                fwrite(&result[i], sizeof(Applicant), 1, myfile);
                printf("%dth competitor from first supervisor\n name: %s area name: %s count of eggs %d\n",i+1, result[i].bunnyName,result[i].areaName, result[i].countOfEggs);
            }
            printf("\n");

            // seding data to the second supervisor
            sleep(1);
            printf("Chief bunny sent the data to the second supervisor!\n");
            write(pipefd2[1], supervisor2, sizeof(Applicant) * count2);

            // waiting and then reading the received data
            sleep(2);
            printf("receiving from second supervisor\n");
            //getting the results of the competition
            read(pipefd2Rec[0], result2, sizeof(Applicant) * count2);
            for (int i = 0; i < count2; i++)
            {
                fwrite(&result2[i], sizeof(Applicant), 1, myfile);
                printf("%dth competitor from second supervisor\n name: %s area name: %s count of eggs%d\n",i+1, result2[i].bunnyName,result2[i].areaName, result2[i].countOfEggs);
            }

            // closing all used pipes
            close(pipefd2Rec[0]);
            close(pipefd2[1]);
            close(pipefdRec[0]);
            close(pipefd[1]);

            // waiting for all processes to finish

            sleep(1);
            printf("\nChief bunny finished!\n");

            // getting the winner
            winnerPoints = result[0].countOfEggs;
            strcpy(winner, result[0].bunnyName);
            for (int i = 0; i < count1; i++)
            {
                if (winnerPoints < result[i].countOfEggs)
                {
                    winnerPoints = result[i].countOfEggs;
                    strcpy(winner, result[i].bunnyName);
                }
            }
            for (int i = 0; i < count2; i++)
            {
                if (winnerPoints < result2[i].countOfEggs)
                {
                    winnerPoints = result2[i].countOfEggs;
                    strcpy(winner, result2[i].bunnyName);
                }
            }

            fclose(myfile);
        }
    }
    if (pid != 0 && pid2 != 0)
    {
        printf("The winner is %s with %d points\n", winner, winnerPoints);
        // sleep(7);
        char ch;
         printf("Press Any Key to Continue\n"); scanf("%c",&ch); scanf("%c",&ch);

        // running the program again
        system("./main.out");
    }

    // free up the memory used
    free(result2);
    free(result);
    free(supervisor1);
    free(supervisor2);
    free(data1);
    free(data2);

    // exiting the program
    exit(0);
}

//function to get applicants for the first supervisor
Applicant *getFirst(int *count1)
{
    int currSize1 = 4;
    Applicant *firstSup = (Applicant *)malloc(currSize1 * sizeof(Applicant));

    int firstC = 0;

    Applicant ap1;
    FILE *myfile;
    myfile = fopen("applicants.txt", "r");

    while (fread(&ap1, sizeof(Applicant), 1, myfile))
    {
        // resizing the arrays if it's full
        if (firstC >= currSize1)
        {
            currSize1 *= 2;
            firstSup = (Applicant *)realloc(firstSup, currSize1 * sizeof(Applicant));
            if (firstSup == NULL)
            {
                printf("Memory reallocation failed");
                exit(0);
            }
        }

        // checking for the names and adding the to their groups
        if (strcmp(ap1.areaName, "Baratfa") == 0)
        {
            strcpy(firstSup[firstC].bunnyName, ap1.bunnyName);

            strcpy(firstSup[firstC].areaName, ap1.areaName);
            firstSup[firstC].countOfEntering = ap1.countOfEntering;
            firstC++;
        }
        if (strcmp(ap1.areaName, "Lovas") == 0)
        {
            strcpy(firstSup[firstC].bunnyName, ap1.bunnyName);
            strcpy(firstSup[firstC].areaName, ap1.areaName);
            firstSup[firstC].countOfEntering = ap1.countOfEntering;
            firstC++;
        }
        if (strcmp(ap1.areaName, "Szula") == 0)
        {
            strcpy(firstSup[firstC].bunnyName, ap1.bunnyName);
            strcpy(firstSup[firstC].areaName, ap1.areaName);
            firstSup[firstC].countOfEntering = ap1.countOfEntering;
            firstC++;
        }
    }
    // setting sizes of two array
    *count1 = firstC;

    // closing the opened file
    fclose(myfile);

    return firstSup;
}

//function to get applicants for the second supervisor
Applicant *getSecond(int *count2)
{
    int currSize1 = 4;
    Applicant *secondSup = (Applicant *)malloc(currSize1 * sizeof(Applicant));

    int secondC = 0;

    Applicant ap1;
    FILE *myfile;
    myfile = fopen("applicants.txt", "r");

    while (fread(&ap1, sizeof(Applicant), 1, myfile))
    {
        // resizing the arrays if it's full
        if (secondC >= currSize1)
        {
            currSize1 *= 2;
            secondSup = (Applicant *)realloc(secondSup, currSize1 * sizeof(Applicant));
            if (secondSup == NULL)
            {
                printf("Memory reallocation failed");
                exit(0);
            }
        }

        // checking for the names and adding the to their groups
        if (strcmp(ap1.areaName, "Kigyospatak") == 0)
        {
            strcpy(secondSup[secondC].bunnyName, ap1.bunnyName);
            strcpy(secondSup[secondC].areaName, ap1.areaName);
            secondSup[secondC].countOfEntering = ap1.countOfEntering;
            secondC++;
        }
        if (strcmp(ap1.areaName, "Malomtelek") == 0)
        {
            strcpy(secondSup[secondC].bunnyName, ap1.bunnyName);
            strcpy(secondSup[secondC].areaName, ap1.areaName);
            secondSup[secondC].countOfEntering = ap1.countOfEntering;
            secondC++;
        }
        if (strcmp(ap1.areaName, "Paskom") == 0)
        {
            strcpy(secondSup[secondC].bunnyName, ap1.bunnyName);
            strcpy(secondSup[secondC].areaName, ap1.areaName);
            secondSup[secondC].countOfEntering = ap1.countOfEntering;
            secondC++;
        }
        if (strcmp(ap1.areaName, "Kaposztaskert") == 0)
        {
            strcpy(secondSup[secondC].bunnyName, ap1.bunnyName);
            strcpy(secondSup[secondC].areaName, ap1.areaName);
            secondSup[secondC].countOfEntering = ap1.countOfEntering;
            secondC++;
        }
    }
    // setting sizes of two array
    *count2 = secondC;

    // closing the opened file
    fclose(myfile);

    return secondSup;
}

//displaying the result of the competition
void getResults()
{

    //cecking if there is a file
    if (access("results.txt", F_OK) != 0)
    {
        printf("No results before competition!\n");
        return;
    }

    Applicant ap1;
    FILE *myfile;

    myfile = fopen("results.txt", "r");
    int count = 1;

    // creating a user friendly table for output
    printf("------------------------  Results   --------------------------\n");
    printf("--------------------------------------------------------------\n");
    printf("| # |      Name         |  Name of the area  |N.of.Entry| Eggs|\n");
    printf("--------------------------------------------------------------\n");

    // reading till EOF
    while (fread(&ap1, sizeof(Applicant), 1, myfile))
    {
        printf("|%3d|%19s|%20s|%10d|%5d|\n", count++, ap1.bunnyName, ap1.areaName, ap1.countOfEntering, ap1.countOfEggs);
    }
    printf("--------------------------------------------------------------\n");

    // closing the opened file
    fclose(myfile);
}