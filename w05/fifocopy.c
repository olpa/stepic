#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

char fin[] = "fifo.in";
char fout[] = "fifo.out";

int main() {
  int len, hin, hout=-1;
  char *buf[1024];
  //assert(-1 != mkfifo(fin, 0664));
  //assert(-1 != mkfifo(fout, 0664));
  mkfifo(fin, 0664);
  mkfifo(fout, 0664);
  assert(-1 != (hin = open(fin, O_RDONLY)));
  assert(-1 != (hout = open(fout, O_WRONLY)));
  while (-1 < (len = read(hin, buf, sizeof(buf)))) {
    write(hout, buf, len);
  }
  close(hout);
  close(hin);
}
