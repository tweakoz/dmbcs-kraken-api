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


#ifndef DMBCS_KRAKEN_API__H
#define DMBCS_KRAKEN_API__H


#include <functional>
#include <sstream>
#include <curlpp/cURLpp.hpp>


namespace  DMBCS  {

  
  using namespace std;


  struct Kraken_API
  {
    enum  Option
      {
        INFO = 0, ACLASS, ASSET, TRADES, USERREF, START, END, OFS, CLOSE_TIME,
        DO_CALCS, PAIR, FEE_INFO, OFLAGS, START_TIME, EXPIRE_TIME, VALIDATE,
        LEVERAGE, TYPE, CLOSE_TYPE, CLOSE_PRICE_1, CLOSE_PRICE_2, INTERVAL,
        SINCE, COUNT, __CEILING
      };


    enum  Instruction
      {
        BUY, SELL
      };


    enum  Order_Type
      {
        MARKET = 0, LIMIT, STOP_LOSS, TAKE_PROFIT, STOP_LOSS_PROFIT,
        STOP_LOSS_PROFIT_LIMIT, STOP_LOSS_LIMIT, TAKE_PROFIT_LIMIT,
        TRAILING_STOP, TRAILING_STOP_LIMIT, STOP_LOSS_AND_LIMIT,
        SETTLE_POSITION
      };


    Kraken_API  (string const &K,  string const &S)  :  key {K},  secret {S}
    {}

    Kraken_API  (Kraken_API const &K)  =  delete;
    
    Kraken_API  (Kraken_API &&K) : key {move (K.key)},
                                   secret {move (K.secret)},
                                   options_table {move (K.options_table)}
    {}

    Kraken_API &  operator=  (Kraken_API const &K)  =  delete;
    
    Kraken_API &  operator=  (Kraken_API &&K)  =  delete;



    template <typename T>
    void  set_opt  (Option const &opt, T const &val);


    void  clear_opt  (Option const  &opt);


    /* Trading functions. */

    string add_order   (Instruction const &instruction,
                        Order_Type const &order,
                        string const &asset,
                        string const &volume,
                        ...);
    
    string cancel_order    (string const &txid);


    /* User account inquiry functions.  */

    string account_balance      (                  );
    string trade_balance        (                  );
    string open_orders          (                  );
    string closed_orders        (                  );
    string query_orders         (string const &txid);
    string trades_history       (                  );
    string trades_info          (string const &txid);
    string open_positions       (string const &txid);
    string ledgers_info         (                  );
    string query_ledgers        (string const &txid);
    string trade_volume         (                  );


    /* Exchange inquiry functions. */

    string server_time          (                  );
    string asset_info           (                  );
    string asset_pairs          (                  );
    string ticker_info          (string const &pair);
    string ohlc_data            (string const &pair);
    string order_book           (string const &pair);
    string recent_trades        (string const &pair);
    string spread_data          (string const &pair);


    /******* Private stuff below here. ******************/

    static curlpp::Cleanup curl_lifetime;

    string const key;
    string const secret;
    static constexpr char const *const  url_base  {"https://api.kraken.com/0/"};

    string query_url;
    string query_result;

    array<string, Option::__CEILING>  options_table;


  };  /*  End of class Kraken_API. */



  template <>
  inline  void  Kraken_API::set_opt  (Kraken_API::Option const &opt, 
                                      string const &val)
  {   options_table [opt]  =  val;   }



  template <typename T>
  inline  void  Kraken_API::set_opt  (Kraken_API::Option const &opt,
                                      T const &val)
  {
    auto  O  =  ostringstream {};
    O  <<  val;
    set_opt  (opt,  O.str ());
  }



  inline  void  Kraken_API::clear_opt  (Option const  &opt)
  {   set_opt  (opt,  string {});   }

    
}  /* End of namespace DMBCS. */


#endif   /* Undefined  DMBCS_KRAKEN_API__H.  */
