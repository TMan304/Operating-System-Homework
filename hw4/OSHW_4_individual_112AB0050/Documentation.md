# 資財三甲 112AB0050 王界誠

## Runtime Environment:
- VMware
- Ubuntu 24.04.4 LTS
- gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- linux-headers-6.8.0-106-generic

## 7.17
- 描述: 使用Monte Carlo方法計算Pi。創建多個執行緒產生一堆隨機座標，並透過全域變數記數有多少在圓內，全域變數使用mutex保護，主執行緒在創建的執行緒執行完後輸出估算的Pi為多少。要求兩個輸入參數，第一個參數為模擬總次數(默認10000)，第二個參數為執行緒數量(默認2)。
- 編譯指令: gcc 7_17_Pi.c -o Pi -pthread

## 8.32
- 描述: 創建多個執行緒模擬南北農夫過橋，橋的使用權由bridge_mutex控制，一次限一個人通過。
- 編譯指令: gcc 8_32_Farmer.c -o Farmer -pthread

## 9.28
- 描述: 有一個輸入參數代表十進位的虛擬地址，輸出該虛擬地址的Page number和offset。
- 編譯指令: gcc 9_28_Page.c -o Page -pthread