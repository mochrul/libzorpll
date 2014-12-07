#include <zorp/packetbuf.h>
#include <glib.h>
#include <string.h>

static void
test_pktbuf_split(void)
{
  const char data[] = "first.last";
  const gsize at = 5;
  ZPktBuf *buf, *newbuf;

  buf = z_pktbuf_new();
  z_pktbuf_copy(buf, data, sizeof(data));
  newbuf =  z_pktbuf_split(buf, at);

  g_assert_cmpint(z_pktbuf_length(buf), ==, 5); /* "first", without terminating 0 */
  g_assert(memcmp(buf->data, "first", z_pktbuf_length(buf)) == 0);

  g_assert_cmpint(z_pktbuf_length(newbuf), ==, 6); /* ".last", with terminating 0 */
  g_assert(memcmp(newbuf->data, ".last", z_pktbuf_length(newbuf)) == 0);

  z_pktbuf_unref(buf);
  z_pktbuf_unref(newbuf);
}

static void
test_pktbuf_append_pktbuf(void)
{
  ZPktBuf *buf, *other;

  buf = z_pktbuf_new();

  other = z_pktbuf_new();
  z_pktbuf_put_string(other, "qwerty");

  z_pktbuf_append_pktbuf(buf, other);

  g_assert(z_pktbuf_length(buf) == 6);
  g_assert(memcmp(buf->data, "qwerty", z_pktbuf_length(buf)) == 0);

  other = z_pktbuf_new();
  z_pktbuf_append_pktbuf(buf, other);

  g_assert(z_pktbuf_length(buf) == 6);
  g_assert(memcmp(buf->data, "qwerty", z_pktbuf_length(buf)) == 0);

  other = z_pktbuf_new();
  z_pktbuf_put_string(other, "qwerty");

  z_pktbuf_append_pktbuf(buf, other);

  g_assert(z_pktbuf_length(buf) == 12);
  g_assert(memcmp(buf->data, "qwertyqwerty", z_pktbuf_length(buf)) == 0);
}

int
main(int argc, char *argv[])
{
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/pktbuf/split", test_pktbuf_split);
  g_test_add_func("/pktbuf/append_pktbuf", test_pktbuf_append_pktbuf);
  g_test_run();
  return 0;
}
