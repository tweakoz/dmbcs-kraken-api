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
#include <cstdarg>
#include <map>


namespace  DMBCS  {


  typedef  Kraken_API  K;


  /* Defined in curl.cc. */
  void  query_private  (K &);
  void  query_public   (K &);



  static  auto const  OPTION_STRING
      = map<K::Option, string> { {K::INFO,          "info"             },
                                 {K::ACLASS,        "aclass"           },
                                 {K::ASSET,         "asset"            },
                                 {K::TRADES,        "trades"           },
                                 {K::USERREF,       "userref"          },
                                 {K::START,         "start"            },
                                 {K::END,           "end"              },
                                 {K::OFS,           "ofs"              },
                                 {K::CLOSE_TIME,    "closetime"        },
                                 {K::DO_CALCS,      "docalcs"          },
                                 {K::PAIR,          "pair"             },
                                 {K::FEE_INFO,      "fee-info"         },
                                 {K::OFLAGS,        "oflags"           },
                                 {K::START_TIME,    "starttm"          },
                                 {K::EXPIRE_TIME,   "expiretm"         },
                                 {K::VALIDATE,      "validate"         },
                                 {K::LEVERAGE,      "leverage"         },
                                 {K::TYPE,          "type"             },
                                 {K::CLOSE_TYPE,    "close[ordertype]" },
                                 {K::CLOSE_PRICE_1, "close[price]"     },
                                 {K::CLOSE_PRICE_2, "close[price2]"    },
                                 {K::INTERVAL,      "interval"         },
                                 {K::SINCE,         "since"            },
                                 {K::COUNT,         "count"            }};
  


  static  void  query_add_options (K &k,
                                   vector<K::Option> const &options,
                                   char joiner)
  {
    for (auto const &O  :  options)
      {
        auto const &option  =  k.options_table [O];

        if (option.length ())
          {
            if (joiner)  k.query_url += joiner;
            joiner  =  '&';

            k.query_url  +=  OPTION_STRING.at (O)  +  '='  +  option;
          }
      }
  }



  static  auto  const  ORDER_TYPE__STRING
      = map<K::Order_Type, string>
        { { K::MARKET,                 "market"                 },
          { K::LIMIT,                  "limit"                  },
          { K::STOP_LOSS,              "stop-loss"              },
          { K::TAKE_PROFIT,            "take-profit"            },
          { K::STOP_LOSS_PROFIT,       "stop-loss-profit"       },
          { K::STOP_LOSS_PROFIT_LIMIT, "stop-loss-profit-limit" },
          { K::STOP_LOSS_LIMIT,        "stop-loss-limit"        },
          { K::TAKE_PROFIT_LIMIT,      "take-profit-limit"      },
          { K::TRAILING_STOP,          "trailing-stop"          },
          { K::TRAILING_STOP_LIMIT,    "trailing-stop-limit"    },
          { K::STOP_LOSS_AND_LIMIT,    "stop-loss-and-limit"    },
          { K::SETTLE_POSITION,        "settle-position"        } };



  string  Kraken_API::add_order  (Instruction const &instruction,
                                  Order_Type const &order_type,
                                  string const &asset, string const &volume,
                                  ...)
  {
    va_list ap;
    va_start (ap, volume);

    query_url  =  "AddOrder?pair="      +  asset
                         +  "&type="    +  (instruction == BUY ? "buy" : "sell")
                         +  "&ordertype="  +  ORDER_TYPE__STRING.at (order_type)
                         +  "&volume="  +  volume
                         +  "&trading_agreement=agree";

    switch (order_type)
      {
      case MARKET:
      case SETTLE_POSITION:
        break;

      case LIMIT:
      case STOP_LOSS:
      case TAKE_PROFIT:
      case TRAILING_STOP:
        query_url  +=  string {"&price="}  +  va_arg (ap, char*);
        break;

      case STOP_LOSS_PROFIT:
      case STOP_LOSS_PROFIT_LIMIT:
      case STOP_LOSS_LIMIT:
      case TAKE_PROFIT_LIMIT:
      case TRAILING_STOP_LIMIT:
      case STOP_LOSS_AND_LIMIT:
        query_url  +=  string {"&price="}  +  va_arg (ap, char*)
                            +  string {"&price2="}   +  va_arg (ap, char*);
        break;
      }

    query_add_options (*this,
                       {LEVERAGE, OFLAGS, START_TIME, EXPIRE_TIME, USERREF,
                        VALIDATE, CLOSE_TYPE, CLOSE_PRICE_1, CLOSE_PRICE_2},
                       '&');

    va_end(ap);

    query_private (*this);

    return query_result;
  }



  static  string  api_function  (K &k,
                                 string const &api_function_,
                                 vector<K::Option> const &options,
                                 function <void (K &)> do_query)
  {
    k.query_url  =  api_function_;
    query_add_options (k, options, '?');
    do_query (k);
    return k.query_result;
  }



  static  string  api_function  (K &k,
                                 string const &api_function_,
                                 string const &arg,
                                 string const &value,
                                 vector<K::Option> const &options,
                                 function <void (K &)> do_query)
  {
    k.query_url  =  api_function_  +  "?"  +  arg  +  "="  +  value;
    query_add_options (k, options, '&');
    do_query (k);
    return k.query_result;
  }



  string  Kraken_API::cancel_order  (string const &txid)
  {  return api_function
                   (*this, "CancelOrder", "txid", txid, {}, query_private);  }

  string  Kraken_API::account_balance  ()
  {  return api_function (*this, "Balance", {}, query_private);  }

  string  Kraken_API::trade_balance  ()
  {  return api_function (*this, "TradeBalance", {ASSET}, query_private);  }

  string  Kraken_API::open_orders  ()
  {  return api_function
                 (*this, "OpenOrders", {TRADES, USERREF}, query_private);  }

  string  Kraken_API::closed_orders  ()
  {
    return api_function (*this, "ClosedOrders",
                         {TRADES, USERREF, START, END, OFS, CLOSE_TIME},
                         query_private);
  }

  string  Kraken_API::query_orders  (string const &txid)
  {
    return api_function 
        (*this, "QueryOrders", "txid", txid, {TRADES, USERREF}, query_private);
  }

  string  Kraken_API::trades_history  ()
  {
    return api_function
      (*this, "TradesHistory", {TYPE, TRADES, START, END, OFS}, query_private);
  }

  string  Kraken_API::trades_info  (string const  &txid)
  {
    return api_function 
                (*this, "QueryTrades", "txid", txid, {TRADES}, query_private);
  }

  string  Kraken_API::open_positions  (string const &txid)
  {
    return api_function 
             (*this, "OpenPositions", "txid", txid, {DO_CALCS}, query_private);
  }

  string  Kraken_API::ledgers_info  ()
  {
    return api_function (*this, "Ledgers",
                         {ACLASS, ASSET, TYPE, START, END, OFS}, 
                         query_private);
  }

  string  Kraken_API::query_ledgers  (string const  &id)
  {  return api_function
                    (*this, "QueryLedgers", "id", id, {}, query_private);  }
  
  string  Kraken_API::trade_volume  ()
  {  return api_function
                   (*this, "TradeVolume", {PAIR, FEE_INFO}, query_private);  }

  string  Kraken_API::server_time  ()
  {  return api_function (*this, "Time", {}, query_public);  }

  string  Kraken_API::asset_info  ()
  {  return api_function
                  (*this, "Assets", {INFO, ACLASS, ASSET}, query_public);  }

  string  Kraken_API::asset_pairs  ()
  {  return api_function (*this, "AssetPairs", {INFO, PAIR}, query_public);  }

  string  Kraken_API::ticker_info  (string const &pair)
  {  return api_function (*this, "Ticker", "pair", pair, {}, query_public);  }

  string  Kraken_API::ohlc_data  (string const &pair)
  {  return api_function
           (*this, "OHLC", "pair", pair, {INTERVAL, SINCE}, query_public); }

  string  Kraken_API::order_book  (string const &pair)
  {  return api_function
                    (*this, "Depth", "pair", pair, {COUNT}, query_public);  }

  string  Kraken_API::recent_trades  (string const &pair)
  {  return api_function 
                    (*this, "Trades", "pair", pair, {SINCE}, query_public);  }

  string  Kraken_API::spread_data  (string const &pair)
  {  return api_function
                  (*this, "Spread", "pair", pair, {SINCE}, query_public);  }



}  /* End of namespace DMBCS. */
