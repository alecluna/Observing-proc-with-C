#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define LB_SIZE 20
#define LONG "long"
#define SHORT "short"
#define STANDARD "standard"

int main(int argc, char *argv[])
{

    //open files from each proc/ file
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    FILE *version = fopen("/proc/version", "r");
    FILE *uptime = fopen("/proc/uptime", "r");

    struct timeval currentTime;
    char repTypeName[16];

    int iteration = 0;
    int interval = 0;
    int duration = 0;
    FILE *thisProcFile;
    char c;
    char c1[1];
    char c2[1];
    char lineBuf[LB_SIZE];
    /* Determine report type */
    char *reportType = STANDARD;
    char line[1000];
    int count = 0;
    int t_days, t_hours, t_minutes, t_seconds, boot_time;

    strcpy(repTypeName, "Standard");
    if (argc > 1)
    {
        sscanf(argv[1], "%c%c", &c1, &c2);
        if (c1 != "-")
        {
            fprintf(stderr, "usage: observer [-s][-l int dur]\n");
            exit(1);
        }
        if (c2 == "s")
        {
            reportType = SHORT;
            strcpy(repTypeName, "Short");
        }
        else if (c2 == "l")
        {
            reportType = LONG;
            strcpy(repTypeName, "Long");
            interval = atoi(argv[2]);
            duration = atoi(argv[3]);
        }
    }
    /* Finish initialization */
    /* Code to read the relevant /proc files */

    while ((fgets(line, sizeof(line), cpuinfo)))
    {
        count++;
        if (count == 5)
        {
            printf("CPU %s", line);
        }
    }
    while ((c = fgetc(version)) != EOF)
    {
        printf("%c", c);
    }
    while ((fgets(line, sizeof(line), uptime)))
    {
        sscanf(line, "%d", &boot_time);
        t_days = boot_time / 60 / 60 / 24;
        t_hours = boot_time / 60 / 60 % 24;
        t_minutes = boot_time / 60 % 60;
        t_seconds = boot_time % 60;
        printf("Boot time: %02d:%02d:%02d:%02d\n", t_days, t_hours, t_minutes, t_seconds);
    }
    gettimeofday(&currentTime, NULL); // Get the current time

    printf("Status report type %s at %s", repTypeName, ctime(&(currentTime.tv_sec)));
    /* retrieve the current  the host filename and print it */
    thisProcFile = fopen("/proc/sys/kernel/hostname", "r");
    fgets(lineBuf, LB_SIZE + 1, thisProcFile);
    printf("Machine hostname: %s", lineBuf);
    fclose(thisProcFile);

    while (iteration < duration)
    {
        sleep(interval);
        //fill this out later
        iteration += interval;
    }
    return (0);
}
