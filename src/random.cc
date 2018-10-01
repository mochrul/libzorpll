/***************************************************************************
 *
 * This file is covered by a dual licence. You can choose whether you
 * want to use it according to the terms of the GNU GPL version 2, or
 * under the terms of Zorp Professional Firewall System EULA located
 * on the Zorp installation CD.
 *
 ***************************************************************************/

#include <zorpll/random.h>
#include <zorpll/log.h>
#include <numeric>
#include <algorithm>

#include <openssl/rand.h>

#define MASK_FOR_N_BITS(n) ((1 << (n)) - 1)

/**
 * Generate random bytes via OpenSSL.
 *
 * @param[in]  strength random strength 
 * @param[out] target target buffer
 * @param[in]  target_len size of target
 *
 * This function returns a sequence of random bytes generated by the PRNG
 * found in OpenSSL. Currently there's no difference between various random
 * strengths but they are defined as below:
 * 
 *   Z_RANDOM_STRONG -- key grade random numbers, might be used for
 *                      generating keys and/or checking authentication
 *                      information
 *
 *   Z_RANDOM_BASIC  -- anything that needs to be unpredictable but might
 *                      somewhat be less random, typical use is random padding
 *                      in cryptographic protocols
 *
 *   Z_RANDOM_WEAK   -- anything that needs to be something but does not really
 *                      need cryptographic strength, for example source port
 *                      selection.
 * 
 * @returns TRUE on success
 **/
gboolean
z_random_sequence_get(ZRandomStrength strength, guchar *target, gsize target_len)
{
  g_return_val_if_fail(strength < Z_RANDOM_NUM_STRENGTHS, FALSE);
  
  return RAND_bytes(target, target_len);
}

/**
 * This function generates a random sequence where each value is in the
 * [min, max] range.
 *
 * @param[in]  strength random strength 
 * @param[out] target target buffer
 * @param[in]  target_len size of target
 * @param[in]  min minimum value
 * @param[in]  max maximum value
 *
 * This is useful to strictly generate printable characters.
 *
 * @returns TRUE on success
 **/
gboolean
z_random_sequence_get_bounded(ZRandomStrength strength, 
                              guchar *target, gsize target_len,
                              guchar min, guchar max)
{
  guint bound;
  guchar num_bits;
  guint needed_bytes;
  guchar *buf;
  guchar offset;
  guint i, j;
  guchar unused_bit_count = 0;
  guchar bit_buffer = 0;

  z_enter();
  
  g_return_val_if_fail(strength < Z_RANDOM_NUM_STRENGTHS, FALSE);
  g_return_val_if_fail(min < max, FALSE);
  
  buf = static_cast<guchar *>(alloca(target_len + 1));
  bound = max - min;
  num_bits = 0;
  while (bound)
    {
      num_bits++;
      bound >>= 1;
    }
  
  needed_bytes = ((num_bits * target_len) - unused_bit_count + 7) / 8;
  if (!z_random_sequence_get(strength, buf, needed_bytes))
    z_return(FALSE);
  for (i = 0, j = 0; i < target_len; i++)
    {
      if (num_bits <= unused_bit_count)
        {
          /* we still have enough bits in the buffer */
          target[i] = bit_buffer & MASK_FOR_N_BITS(num_bits);
          bit_buffer >>= num_bits;
          unused_bit_count -= num_bits;
        }
      else
        {
          /* we need some more bits */
          
          /* buffered bits go in as the high order bits */
          offset = num_bits - unused_bit_count;
          target[i] = bit_buffer << offset;
          
          /* new buffer */
          bit_buffer = buf[j++];
          unused_bit_count = 8;
          
          /* new bits go as the low order bits */
          target[i] |= bit_buffer & MASK_FOR_N_BITS(offset);
          bit_buffer >>= offset;
          unused_bit_count -= offset;
        }
      
      /* ok, we now have num_bits in target[i] */
      
      target[i] = ((((guint) target[i] * (max - min)) / MASK_FOR_N_BITS(num_bits)) + min) & 0xFF;

      g_assert(target[i] >= min && target[i] <= max);
    }
  z_return(TRUE);
}

class NotEnoughEntropyException : public std::exception
{
};

/**
 * This function generates a random sequence where each value is in the
 * [min, max] range. It uses cryptographically secure random.
 *
 * @param[in]  min minimum value
 * @param[in]  max maximum value
 *
 * @returns std::vector<unsigned int> with random numbers between [min, max]
 *          or an empty vector if there was not enough entropy.
 **/
std::vector<unsigned int>
z_random_sequence_create(const unsigned int min, const unsigned int max)
{
  std::vector<unsigned int> sequence(max - min + 1);

  std::iota(sequence.begin(), sequence.end(), min);
  try
    {
      std::random_shuffle(sequence.begin(), sequence.end(), [] (int max) {
        unsigned char random;
        do
         {
           if (RAND_bytes(&random, 1) != 1)
             throw NotEnoughEntropyException();

         } while (random >= max);
        return random;
        });
    }
  catch (const NotEnoughEntropyException &e)
    {
      sequence.clear();
    }

  return sequence;
}
