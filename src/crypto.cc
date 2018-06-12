/*
 *  dmbcs-kraken-api   A C++ encapsulation of the API to Krakenʼs e-currency
 *                     exchange
 *
 *  Copyright (C) 2018  DM Bespoke Computer Solutions Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/buffer.h>
#include <dmbcs-kraken-api.h>


namespace  DMBCS  {


  array<uint8_t, SHA256_DIGEST_LENGTH>  sha256  (string const &data)
  {
    auto  ret  =  array<uint8_t, SHA256_DIGEST_LENGTH> {};

    auto digest  =  EVP_MD_CTX_new ();
    EVP_DigestInit_ex   (digest,  EVP_sha256 (),  nullptr);
    EVP_DigestUpdate    (digest,  data.data (),  data.length ());
    EVP_DigestFinal_ex  (digest,  ret.data (),  nullptr);
    EVP_MD_CTX_free     (digest);

    return  ret;
  }



  vector<uint8_t>  hmac_sha512  (vector<uint8_t> const &data,
                                 vector<uint8_t> const &key)
  {
    auto length  =  unsigned {EVP_MAX_MD_SIZE};

    auto  ret  =  vector<uint8_t> (length);

    HMAC_CTX *const ctx  =  HMAC_CTX_new ();
    HMAC_Init_ex   (ctx, key.data (), key.size (), EVP_sha512 (), nullptr);
    HMAC_Update    (ctx, data.data (), data.size ());
    HMAC_Final     (ctx, ret.data (),  &length);
    HMAC_CTX_free  (ctx);

    ret.resize (length);

    return  ret;
  }



  vector<uint8_t>  base64_decode  (string const  &data)
  {
    auto b64  =  BIO_new (BIO_f_base64 ());
    BIO_set_flags (b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_push (b64, BIO_new_mem_buf  (data.data (), data.length ()));

    auto  output  =  vector<uint8_t> {};    output.reserve (data.length ());
    auto  buffer  =  array<uint8_t, 512> {};

    for (;;)
      {
        auto const len  =  BIO_read (b64, buffer.data (), buffer.size ());
        if (len <= 0)   break;
        output.insert (end (output), begin (buffer), begin (buffer) + len);
      }

    BIO_free_all  (b64);

    return output;
  }



  string  base64_encode  (vector<uint8_t> const &data)
  {
    auto *const  b64   =   BIO_new (BIO_f_base64 ());

    BIO_set_flags  (b64,  BIO_FLAGS_BASE64_NO_NL);
    BIO_push       (b64,  BIO_new (BIO_s_mem ()));
    BIO_write      (b64,  data.data (),  data.size ());
    BIO_flush      (b64);

    BUF_MEM* bptr = nullptr;   BIO_get_mem_ptr (b64, &bptr);

    auto const ret  =  string {bptr->data, bptr->data + bptr->length};

    BIO_free_all (b64);

    return ret;
  }


}  /* End of namespace DMBCS. */
