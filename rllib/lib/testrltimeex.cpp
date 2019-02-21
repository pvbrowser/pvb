#include <stdio.h>
#include "unistd.h"
#include "rltimeex.h"

int main()
{
  rlTimeEx now, start, diff, t;

  now.getLocalTime();
  start = now;
  /*
  while(1)
  {
    now.getLocalTime();
    diff = now - start;
    double ddiff = diff.seconds();
    printf("start=%s now=%s diff=%s seconds=%f\n", start.getIsoTimeString(), now.getIsoTimeString(), diff.getIsoTimeString(),           (float) ddiff);

    t.setTimeFromSeconds(ddiff);
    printf("ddiff=%lf t=%s seconds()=%f\n", ddiff, t.getIsoTimeString(), t.seconds());
    sleep(1);
  }
  */

  /*
  double diffsec = 61;
  printf("start=%s diffsec=%lf\n", start.getIsoTimeString(), diffsec);
  start -= diffsec;
  printf("start=%s diffsec=%lf\n", start.getIsoTimeString(), diffsec);
  */

  /*
  double diffsec = 61;
  printf("start=%s diffsec=%lf\n", start.getIsoTimeString(), diffsec);
  now = start - diffsec;
  printf("  now=%s diffsec=%lf\n", now.getIsoTimeString(), diffsec);
  
  printf("toString dd-MMM-yyyy %s\n", now.toString("dd-MMM-yyyy hh:mm:ss zzz"));
  printf("toString dd-MM-yyyy %s\n", now.toString("dd-MM-yyyy hh:mm:ss zzz"));
  printf("toString d-M-yy %s\n", now.toString("d-M-yy h:m:s z AP"));
  */

  while(1)
  {
    printf("now=%s\n", now.getIsoTimeString());
    now += 1.01;
    sleep(1);
  }  
  return 0;
}
