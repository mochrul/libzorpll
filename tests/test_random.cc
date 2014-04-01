#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <zorp/random.h>

#include <stdio.h>

#define ROUNDS 1000

BOOST_AUTO_TEST_CASE(test_random)
{
  guchar buf[64];
  guint i, j;
  bool error = false;

  for (i = 0; i < ROUNDS; i++)
    {
      z_random_sequence_get_bounded(Z_RANDOM_STRONG, buf, sizeof(buf), 'A', 'Z');
      for (j = 0; j < sizeof(buf); j++)
        {
          if (buf[j] < 'A' || buf[j] > 'Z')
            {
              error = true;
              break;
            }
        }
    }
  BOOST_CHECK_MESSAGE(!error, "Invalid character in bounded random data");
}
