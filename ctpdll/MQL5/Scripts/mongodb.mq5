//+------------------------------------------------------------------+
//|                                                      mongodb.mq5 |
//|                        Copyright 2018, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2018, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property script_show_inputs
//--- input parameters
input string   dbhost="localhost";
input int      dbport=27017;
input string   dbname="quotesdb";
input ulong    timestamp=0;

//+------------------------------------------------------------------+
//| Script program start function                                    |
//+------------------------------------------------------------------+

void ConnectToDB() {
}

void Disconnect() {
}

void LoadData() {
}

void OnStart()
  {
//---
   Print("Start to connect to MongoDB...");
   ConnectToDB();
   LoadData();
   Disconnect();
   Print("Done, bye!");
   
  }
//+------------------------------------------------------------------+
