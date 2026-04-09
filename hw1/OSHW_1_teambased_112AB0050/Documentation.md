# 資財三甲 112AB0050 王界誠

## Runtime Environment:
- VMware
- Ubuntu 24.04.4 LTS
- gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- linux-headers-6.8.0-106-generic

## Ch2
執行Makefile後會編譯出兩個核心模組(jiffies.ko、seconds.ko)，加載後分別會在初始化時在proc創建對應的檔案，對其進行讀取並印出的動作會分別輸出 系統開機以來的jiffies 和 second.ko加載後經過的秒數(都是在讀取時計算得出而非真的儲存在檔案中)，退出兩個核心模組會清理在proc創建的檔案

## Ch3-1
一個在linux上運行的shell介面，運行時會先輸出目前進程所在位置，使用者可以輸入要執行的操作，會由該進程fork子進程，子進程使用execvp呼叫系統API執行指令
- 會儲存上一次使用的指令，輸入!!會自動執行上一次指令
- 最後參數沒有&的情況下進程會wait子進程完成，有&則會讓子進程在背景執行，不干擾使用者繼續使用
- 能夠處理exit、cd這些內部指令
- 支援|合併多個指令，由該進程創建pipe，並讓fork出的子進程透過pipe傳輸資料來實現合併
- 支援<和>改變I/O，會使用dup2重新定向標準輸入輸出

## Ch3-3
執行Makefile後會編譯出兩個核心模組(task_dfs.ko、task_liner.ko)，加載後分別在初始化時透過不同的遍歷方式輸出目前系統所有的進程