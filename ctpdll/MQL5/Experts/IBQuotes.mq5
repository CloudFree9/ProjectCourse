//+------------------------------------------------------------------+
//|                                                     IBQuotes.mq5 |
//|                        Copyright 2018, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2018, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#include "..\Include\JAson.mqh"
//--- input parameters

input string   QuotesHubURL="http://127.0.0.1/ibquotes/quotes";

string   underlying="";
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {

   underlying = _Symbol;
   
   if (underlying == "") return(INIT_FAILED);

   EventSetTimer(5);
   
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//--- destroy timer
   EventKillTimer();
   
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
   
  }
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
  {
//---
   string cookie=NULL,headers;
   char post[],result[];
   int res;
   
   string sym = _Symbol;
   string id = SymbolInfoString(sym, SYMBOL_BASIS);
   string url = QuotesHubURL + "?instrument=" + id;
   
   ResetLastError();
   
   int timeout=50000; 

   res=WebRequest("GET", url, cookie, NULL, timeout, post, 0, result, headers);
   
   if(res!=200) {
      Print("Error in WebRequest. Error code  = " + GetLastError());
      return;
   }

   string msg = CharArrayToString(result);

   CJAVal js(NULL, jtUNDEF);
   js.Deserialize(result);

   string httpres = js["res"].ToStr();
   
   if (httpres != "ok") {
      Print("Failed to get quotes data: " + msg);
      return;
   }
   
   double price = js["price"].ToDbl();
   double vix = js["vix"].ToDbl();

   if (price <=0 || vix <=0) {
      Print("Either price or vix is ZERO, skipped. Price=", price, ", VIX=", vix);
      return;
   }
   
   MqlTick tick[1];

   tick[0].ask       = price;
   tick[0].bid       = price;
   tick[0].last      = price;
   tick[0].volume    = 1;
   tick[0].time_msc  = StringToInteger(js["timestamp"].ToStr()) - TimeGMTOffset() * 1000;
   tick[0].time      = tick[0].time_msc / 1000;

   tick[0].flags     = TICK_FLAG_BID | TICK_FLAG_ASK | TICK_FLAG_LAST | TICK_FLAG_SELL;

   Print("GMT time:", TimeGMT(), ", Quote | bid:", tick[0].bid, ", ask:", tick[0].ask, ", price:", tick[0].last, ", time:", tick[0].time, ", vix:", vix);
   
   res = CustomTicksAdd(_Symbol, tick);
   if (res != 1) {
      Print("更新报价错误码 : '", res, "' 错误信息:'", GetLastError(), "'");
   }
   
   tick[0].ask       = vix;
   tick[0].bid       = vix;
   tick[0].last      = vix;
   
   sym = "VIX_" + _Symbol;
   
   res = CustomTicksAdd(sym, tick);
   if (res != 1) {
      Print(sym, " 更新报价错误码 : '", res, "' 错误信息:'", GetLastError(), "'");
   }

  }
//+------------------------------------------------------------------+
// {'res':'ok', 'timestamp':'1531911296410', 'price': '0.0089000', 'vix': '0.0000'}
// CJAVal{  m_e:[2] m_key:"" m_lkey:"" m_parent:NULL m_type:jtOBJ m_bv:false m_iv:0 m_dv:0.0 m_prec:8 m_sv:"" }