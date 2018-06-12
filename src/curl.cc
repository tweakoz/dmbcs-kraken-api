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


#include <dmbcs-kraken-api.h>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>


namespace  DMBCS  {


  namespace C = curlpp;
  namespace CO = C::Options;


  /*  Defined in crypto.cc.  */
  array<uint8_t, 32>  sha256         (string const &);
  vector<uint8_t>     hmac_sha512    (vector<uint8_t> const &data,
                                      vector<uint8_t> const &key);
  vector<uint8_t>     base64_decode  (string const  &);
  string              base64_encode  (vector<uint8_t> const &);



  void  query_public  (Kraken_API &K)
  {
    auto  request  =  C::Easy {};

    request.setOpt (CO::Url {string {K.url_base} + "public/" + K.query_url});
    
    K.query_result.clear ();

    request.setOpt (CO::WriteFunction 
                    {[&K] (char *buffer, size_t size, size_t n) 
                              { K.query_result += string {buffer, 
                                                          buffer + size * n};
                                return size * n; }});

    request.perform ();
  }



  void  query_private  (Kraken_API &K)
  {
    if (K.secret.length ()  !=  88)
      throw runtime_error {"private key must be 88 characters long"};

    auto const quiz  =  K.query_url.find ('?');

    auto  post_data  =  string {};
    if (quiz != K.query_url.npos)
      {
        post_data = K.query_url.substr (quiz+1);
        K.query_url  =  K.query_url.substr (0, quiz);
      }

    struct timeval sys_time;   gettimeofday  (&sys_time, nullptr);

    auto nonce  =  ostringstream {};
    
    nonce << (uint64_t) ((sys_time.tv_sec * 1000000) + sys_time.tv_usec);

    auto  short_url  =  string {"/0/private/"}  +  K.query_url;
    K.query_url  =  string{K.url_base}  +  "private/"  +  K.query_url;
    

    if (! post_data.empty ())   post_data += '&';
    post_data  +=  "nonce=" + nonce.str ();

    auto const  D       =  sha256 (nonce.str ()  +  post_data);
    auto const  digest  =  short_url  +  string {begin (D), end (D)};

    auto const  hmac
      =  base64_encode  
                 (hmac_sha512  (vector<uint8_t> {begin (digest), end (digest)},
                                base64_decode (K.secret)));

    auto  request  =  C::Easy {};

    request.setOpt (CO::Url {K.query_url});
    request.setOpt (CO::PostFields {post_data});
    request.setOpt (CO::HttpHeader {{{"API-Key: " + K.key},
                                     {"API-Sign: " + hmac}}});
    
    K.query_result.clear ();

    request.setOpt (CO::WriteFunction 
                    {[&K] (char *buffer, size_t size, size_t n) 
                              { K.query_result += string {buffer, 
                                                          buffer + size * n};
                                return size * n; }});

    request.perform ();
  }


}  /* End of namespace DMBCS. */
