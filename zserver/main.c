#include "zserver.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  zserver_start(atoi(argv[1]));
  zserver_commit_file("key", "/tmp/test");
 
  sleep(60); // some computation
  
  zserver_stop();

  return 0;
}
