#include <zorp/streamfd.h>
#include <zorp/streamline.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

const gchar *expected_outputs[] =
{
  "1",
  "2",
  "3",
  "01234567",
  "012345678", //truncates "9abc"
  "abc",
  "abcdef",
  "012345678", //truncates 90123456789012345678"
  "abcdef",
  "012345678",
  "abcdef"
};

int main(void)
{
  ZStream *input;
  int pair[2], status = 0;
  pid_t pid;
  gchar *line;
  gsize length;
  
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) < 0)
    {
      perror("socketpair()");
      return 254;
    }
  
  pid = fork();
  if (pid < 0)
    {
      perror("fork()");
      return 254;
    }
  else if (pid != 0) 
    {
      close(pair[1]);
      g_return_val_if_fail(write(pair[0], "1\n2\n3\n", 6) == 6, 253);
      g_return_val_if_fail(write(pair[0], "0123", 4) == 4, 253);
      g_return_val_if_fail(write(pair[0], "4567\n", 5) == 5, 253);
      g_return_val_if_fail(write(pair[0], "0123456789", 10) == 10, 253);
      g_return_val_if_fail(write(pair[0], "abc\nAabc\nabcdef\n", 16) == 16, 253); /* Because of truncate A will be eliminated */
      g_return_val_if_fail(write(pair[0], "0123456789", 10) == 10, 253);
      g_return_val_if_fail(write(pair[0], "0123456789", 10) == 10, 253);
      g_return_val_if_fail(write(pair[0], "012345678\nAabcdef\n", 18) == 18, 253); /* Because of truncate A will be eliminated */
      g_return_val_if_fail(write(pair[0], "012345678\nAabcdef", 17) == 17, 253);
      close(pair[0]);
      waitpid(pid, &status, 0);
    }
  else
    {
      gint rc;
      guint i;
      
      printf("%d\n", getpid());
      sleep(1);  
      close(pair[0]);  
      input = z_stream_fd_new(pair[1], "sockpair input");
      input = z_stream_line_new(input, 10, ZRL_EOL_NL | ZRL_TRUNCATE);
      
      i = 0;

      if (!z_stream_unget(input, "A", 1, NULL))
        {
          printf("Error on unget\n");
          _exit(1);
        }
      rc = z_stream_line_get(input, &line, &length, NULL);
      while (rc == G_IO_STATUS_NORMAL)
        {
          if (i >= (sizeof(expected_outputs) / sizeof(gchar *)) ||
              line[0] != 'A' ||
              strlen(expected_outputs[i]) != length - 1 ||
              memcmp(expected_outputs[i], line + 1, length - 1) != 0)
            {
              printf("Error checking line: [%.*s] (length: %"G_GSIZE_FORMAT"), should be: %s\n", (int) length - 1, line + 1, length - 1, expected_outputs[i]);
              _exit(1);
            }
          else
            {
              printf("line ok: %.*s\n", (int) length, line);
            }
          if (!z_stream_unget(input, "A", 1, NULL))
            {
              printf("Error on unget\n");
              _exit(1);
            }
          rc = z_stream_line_get(input, &line, &length, NULL);
          i++;
        }
      if (i < (sizeof(expected_outputs) / sizeof(gchar *)))
        {
          printf("Missing output %u of %"G_GSIZE_FORMAT"\n", i, (sizeof(expected_outputs) / sizeof(gchar *)));
          _exit(1);
        }
      close(pair[1]);
      _exit(0);
    }
  return status >> 8;
}
