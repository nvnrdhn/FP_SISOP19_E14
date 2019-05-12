#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define MAX 10    // jumlah maksimal config yg dapat berjalan secara bersamaan (max thread)

pthread_t tid[MAX];  

void* run_command(void *arg)
{
  char *dir;
  dir = (char *) arg;
  signal(SIGCHLD, SIG_IGN);
  pid_t pid, sid;
  pid = fork();
  if (pid == 0) {
    sid = setsid();

    if (sid < 0) {
      exit(EXIT_FAILURE);
    }

    execlp("bash", "bash", dir, (char *)NULL);
  }
}

int main(void *arg) {
  pid_t pid, sid;

  pid = fork();

  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(0);

  sid = setsid();

  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  if ((chdir("/")) < 0) {
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  while(1) {

    time_t now = time(NULL);
    struct tm tm = *localtime(&now);

    int mi, h, dm, mo, dw;
    mi = tm.tm_min;     // 0-59
    h = tm.tm_hour;     // 0-23
    dm = tm.tm_mday;    // 1-31
    mo = tm.tm_mon + 1; // (0-11) + 1
    dw = tm.tm_wday;    // sun=0, mon=1, etc.

    char smi[3], sh[3], sdm[3], smo[3], sdw[2];
    sprintf(smi, "%d", mi);
    sprintf(sh, "%d", h);
    sprintf(sdm, "%d", dm);
    sprintf(smo, "%d", mo);
    sprintf(sdw, "%d", dw);

    FILE* config = fopen("/home/somi/crontab.data", "r");
    // FILE* log = fopen("/home/somi/crontab.log", "a");

    char cmi[3], ch[3], cdm[3], cmo[3], cdw[2];
    char s[500] = "";
    int i = 0;
    while (fscanf(config, "%s %s %s %s %s %s", cmi, ch, cdm, cmo, cdw, s) != EOF) {
      if ((!strcmp(smi, cmi) || !strcmp(cmi, "*")) && (!strcmp(sh, ch) || !strcmp(ch, "*")) && (!strcmp(sdm, cdm) || !strcmp(cdm, "*")) && (!strcmp(smo, cmo) || !strcmp(cmo, "*")) && (!strcmp(sdw, cdw) || !strcmp(cdw, "*"))) {
        // fprintf(log, "%s%s %s %s %s %s %s\n\n", asctime(&tm), cmi, ch, cdm, cmo, cdw, s);
        pthread_create( &tid[i++], NULL, run_command, (void*) s);
      }
    }

    fclose(config);
    // fclose(log);
    sleep(60);
  }
  
  exit(EXIT_SUCCESS);
}
