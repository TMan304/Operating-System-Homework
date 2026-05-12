# 資財三甲 112AB0050 王界誠

## Runtime Environment:
- VMware
- Ubuntu 24.04.4 LTS
- gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- linux-headers-6.8.0-106-generic

## Ch7-1
- 描述: 用pthread維護一個thread pool。main函式內有簡單的測試。
- 編譯指令: gcc ThreadPool.c -o ThreadPool -pthread

## Ch8
- 描述: 模擬執行銀行家算法，第一個輸入參數為客戶數(默認5)，後續輸入參數皆視為某種資源的數量(默認3種資源，各10個)。
- 編譯指令: gcc Banker.c -o Banker -pthread

## Ch9
- 描述: 模擬管理連續的記憶體空間，使用時須輸入參數作為記憶體空間大小。輸入X退出，共有四種操作分別是
    - 請求: RQ \<id> \<size> 使用First-fit
    - 釋放: RL \<id>
    - 壓縮: C
    - 報告: STAT
- 編譯指令: gcc Memory.c -o Memory