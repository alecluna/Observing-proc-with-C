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
  FILE *stat = fopen("/proc/stat", "r");
  //partc begins here
  FILE *disks = fopen("/proc/diskstats", "r");
  FILE *contact = fopen("/proc/stat", "r");
  FILE *btime = fopen("/proc/stat", "r");
  FILE *processes = fopen("/proc/stat", "r");
  //partd begins here
  FILE *memTotal = fopen("/proc/meminfo", "r");
  FILE *memFree = fopen("/proc/meminfo", "r");

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
  int count2 = 0;
  int count3 = 0;
  int count4 = 0;
  int count5 = 0;
  int count6 = 0;
  int t_days, t_hours, t_minutes, t_seconds, boot_time;
  int user_mode, system_mode, idle_mode, filler, reads_completed, writes_completed, contact_switch, b_time, process;
  int mem_Total, mem_Free;

  //iterating through the cpuinfo procfile by grabbing each "line" and
  //printing it to console
  while ((fgets(line, sizeof(line), cpuinfo)))
  {
    count++;
    if (count == 5)
    {
      printf("CPU %s\n", line);
    }
  }
  while ((c = fgetc(version)) != EOF)
  {
    printf("%c", c);
  }

  printf("\n"); //add newline here
  //normally we could check if proc is null here as an edge case test...
  while ((fgets(line, sizeof(line), uptime)))
  {
    sscanf(line, "%d", &boot_time);
    t_days = boot_time / 60 / 60 / 24;
    t_hours = boot_time / 60 / 60 % 24;
    t_minutes = boot_time / 60 % 60;
    t_seconds = boot_time % 60;
    printf("Boot time: %02d:%02d:%02d:%02d\n\n", t_days, t_hours, t_minutes, t_seconds);
  }

  //copied from notes
  strcpy(repTypeName, "Standard");
  if (argc > 1)
  {
    sscanf(argv[1], "%s", c1);
    if (c1[0] != '-' || c1[1] != 's' && c1[1] != 'l')
    {
      fprintf(stderr, "usage: observer [-s][-l int dur]\n");
      exit(1);
    }
    if (c1[1] == 's')
    {
      reportType = SHORT;
      strcpy(repTypeName, "Short");

      fgets(line, sizeof(line), stat);
      sscanf(line, "cpu %d %d %d %d", &user_mode, &filler, &system_mode, &idle_mode);
      printf("User mode :%d\n", user_mode);
      printf("System mode :%d\n", system_mode);
      printf("Idle mode :%d\n\n", idle_mode);

      //checking if any of the proc files are empty...
      if (stat == NULL || disks == NULL || contact == NULL || btime == NULL || processes == NULL)
      {
        printf("Files could not be opened, proc may or may not be empty :(  ");
        exit(0);
      }
      else
      {
        fgets(line, sizeof(line), stat);
        sscanf(line, "CPU:  %d %d %d %d", &user_mode, &filler, &system_mode, &idle_mode);
        printf("User Mode :%d\n", user_mode);
        printf("System Mode :%d\n", system_mode);
        printf("Idle Mode :%d\n\n", idle_mode);
      }

      while ((fgets(line, sizeof(line), disks)))
      {
        count2++;
        if (count2 == 26)
        {
          sscanf(line, "%d %d %s %d %d %d %d %d", &filler, &filler, &filler, &reads_completed, &filler, &filler, &filler, &writes_completed);
          printf("Reads Completed: %d\n", reads_completed);
          printf("Writes Completed: %d\n\n", writes_completed);
        }
      }
      while ((fgets(line, sizeof(line), contact)))
      {
        count3++;
        if (count3 == 11)
        {
          sscanf(line, "%s %d", &filler, &contact_switch);
          printf("Contact Switches: %d\n\n", contact_switch);
        }
      }

      while ((fgets(line, sizeof(line), btime)))
      {
        count4++;
        if (count4 == 12)
        {
          sscanf(line, "%s %d", &filler, &b_time);
          printf("Boot Time: %d\n\n", b_time);
        }
      }
      while ((fgets(line, sizeof(line), processes)))
      {
        count5++;
        if (count5 == 13)
        {
          sscanf(line, "%s %d", &filler, &process);
          printf("Processes: %d\n\n", process);
        }
      }

      if (memTotal == NULL)
      {
        printf("File not opened");
        exit(0);
      }
      else
      {
        fgets(line, sizeof(line), memTotal);
        sscanf(line, "%s %d", &filler, &mem_Total);
        printf("Memory Configured:%d\n\n", mem_Total);
      }

      while ((fgets(line, sizeof(line), memFree)))
      {
        count6++;
        if (count6 == 2)
        {
          sscanf(line, "%s %d", &filler, &mem_Free);
          printf("Memory Available:%d\n\n", mem_Free);
        }
      }
    }
    else if (c1[1] == 'l')
    {

      if (argv[2] == NULL || argv[3] == NULL)
      {
        fprintf(stderr, "usage: observer [-s][-l int dur]\n");
        exit(1);
      }
      else
      {
        reportType = LONG;
        strcpy(repTypeName, "Long");
        interval = atoi(argv[2]);
        duration = atoi(argv[3]);
      }
    }
  }

  /* Finish initialization */
  /* Code to read the relevant /proc files */

  gettimeofday(&currentTime, NULL); // Get the current time
  printf("Status Report Type %s at %s\n", repTypeName, ctime(&(currentTime.tv_sec)));
  /* Get the host filename and print it */
  thisProcFile = fopen("/proc/sys/kernel/hostname", "r");
  fgets(lineBuf, LB_SIZE + 1, thisProcFile);
  printf("Machine hostname: %s\n", lineBuf);
  fclose(thisProcFile);

  while (iteration < duration)
  {
    sleep(interval);
    FILE *loadavg = fopen("/proc/loadavg", "r");
    fgets(line, sizeof(line), loadavg);
    printf("Interval: %d Iteration: %d Load Average: %s", interval, iteration, line);
    //Fill this out at Part D
    //        /*sampleLoadAvg();*/
    iteration += interval;
  }
  return (0);
}
