/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   struct account_statistics {
         account_name      owner = 0;  /// the owner
         uint64_t          create_time;
         uint64_t          first_send_time = 0; /// the first time to transfer ACTX to other
         uint64_t          first_receive_time = 0; /// the first time receive ACTX from other
         uint64_t          total_send_amount; /// The total amount of ACTX transferred
         uint64_t          total_receive_amount; /// The total amount of ACTX received
         uint16_t          res1 = 0;
         uint16_t          res2 = 0;

         uint64_t primary_key()const { return owner; }

         EOSLIB_SERIALIZE( account_statistics, (owner)(create_time)(first_send_time)(first_receive_time)
                          (total_send_amount)(total_receive_amount)(res1)(res2) )
      }

   typedef eosio::multi_index< N(statistics), account_statistics>  account_statistics_table;

   class token : public contract {
      public:
         token( account_name self ):contract(self), _statistics_table(N(actx), N(actx)){}

         void create( account_name issuer,
                      asset        maximum_supply);

         void issue( account_name to, asset quantity, string memo );

         void transfer( account_name from,
                        account_name to,
                        asset        quantity,
                        string       memo );
      
      
         inline asset get_supply( symbol_name sym )const;
         
         inline asset get_balance( account_name owner, symbol_name sym )const;

      private:
         struct account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.name(); }
         };

         struct currency_stats {
            asset          supply;
            asset          max_supply;
            account_name   issuer;

            uint64_t primary_key()const { return supply.symbol.name(); }
         };

         typedef eosio::multi_index<N(accounts), account> accounts;
         typedef eosio::multi_index<N(stat), currency_stats> stats;

         account_statistics_table   _statistics_table;

         void sub_balance( account_name owner, asset value );
         void add_balance( account_name owner, asset value, account_name ram_payer );

         void update_account_total_transfer(account_name from, account_name to, asset value);

      public:
         struct transfer_args {
            account_name  from;
            account_name  to;
            asset         quantity;
            string        memo;
         };
   };

   asset token::get_supply( symbol_name sym )const
   {
      stats statstable( _self, sym );
      const auto& st = statstable.get( sym );
      return st.supply;
   }

   asset token::get_balance( account_name owner, symbol_name sym )const
   {
      accounts accountstable( _self, owner );
      const auto& ac = accountstable.get( sym );
      return ac.balance;
   }

} /// namespace eosio
