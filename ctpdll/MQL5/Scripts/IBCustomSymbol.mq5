//+------------------------------------------------------------------+
//|                                                 customsymbol.mq5 |
//|                        Copyright 2018, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2018, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"

input string Class = "1";

string   instrumentsfile = "Class" + Class + "Symbols.json";
string   provider = "IB";

#include "..\Include\JAson.mqh"

void GetCustomSymbols(string provider, string &res[]) {

   int c = SymbolsTotal(false);  // 获得所有活跃symbol的个数
   for (int j=0;j<c;j++) {
   
      string n = SymbolName(j, false);
      string p = SymbolInfoString(n, SYMBOL_PATH);
      string res1[];
      int i = StringSplit(p, StringGetCharacter("\\", 0), res1);
      
      if (i<2 || (res1[0] != "Custom" && res1[0] != "IB")) {
         continue;
      }
      
      ArrayResize(res, ArraySize(res) + 1);
      res[ArraySize(res) - 1] = n;
      
   }
}

bool SymbolExists(string provider, string s) {
   string res[];
   GetCustomSymbols(provider, res);
   
   for(int i=0;i<ArraySize(res);i++) {
      if (res[i] == s) return true;
   }
   
   return false;
}

CJAVal GetSyms(string provider) {

   string filename=instrumentsfile;
   if (provider != "") {
      filename = provider + "\\" + filename;
   }
   
   string buf = "";
   CJAVal js(NULL, jtUNDEF);
   
   int file_handle=FileOpen(filename,FILE_TXT | FILE_READ | FILE_ANSI); 
 
   if (file_handle == INVALID_HANDLE) {
      Print("Last error:", GetLastError());
      return js;
   }
   
   while(!FileIsEnding(file_handle)) {
      buf = buf + FileReadString(file_handle);
   }
      
   FileClose(file_handle);
   js.Deserialize(buf);
   return js["ConfigSymbols"];
}

void CreateSyms(string provider)
  {
  
   CJAVal symbolsjs = GetSyms(provider);
 
   for(int i=0;i<symbolsjs.Size();i++) {
      string s = symbolsjs[i]["Symbol"].ToStr();
      if (SymbolExists(provider, s)) {
         PrintFormat("Symbol: %s already exists, skip.", s);
         continue;
      }
      CustomSymbolCreate(s, provider);
      PrintFormat("symbol: \"%s\" created or already existing.", s);
      CustomSymbolSetString(s, SYMBOL_DESCRIPTION, symbolsjs[i]["Description"].ToStr());
      CustomSymbolSetString(s, SYMBOL_BASIS, symbolsjs[i]["ContractID"].ToStr());
      string digits = symbolsjs[i]["Digits"].ToStr();
      string currency = symbolsjs[i]["CurrencyBase"].ToStr();
      CustomSymbolSetInteger(s, SYMBOL_DIGITS, symbolsjs[i]["Digits"].ToInt());
      CustomSymbolSetString(s, SYMBOL_CURRENCY_BASE, symbolsjs[i]["CurrencyBase"].ToStr());
      SymbolSelect(s, true);
      
      string vix = symbolsjs[i]["VIX"].ToStr();
      
      if (vix != "yes") continue;
      s = "VIX_" + s;
      
      CustomSymbolCreate(s, provider);
      CustomSymbolSetString(s, SYMBOL_DESCRIPTION, "VIX: " + symbolsjs[i]["Description"].ToStr());
      CustomSymbolSetString(s, SYMBOL_BASIS, "VIX@" + symbolsjs[i]["ContractID"].ToStr());
      CustomSymbolSetInteger(s, SYMBOL_DIGITS, 4);
      CustomSymbolSetString(s, SYMBOL_CURRENCY_BASE, symbolsjs[i]["CurrencyBase"].ToStr());      
      SymbolSelect(s, true);
   }
   
 }
 
 void OnStart() {
   CreateSyms(provider);
   MessageBox("IB定制标的创建完成!", "IB symbols");
 }
