# 資財三甲 112AB0050 王界誠

## Runtime Environment:
- VMware
- Ubuntu 24.04.4 LTS
- gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- linux-headers-6.8.0-106-generic

## 4.24
- 描述: 使用Monte Carlo方法計算Pi。創建一個執行緒產生一堆隨機座標，並記數有多少在圓內，主執行緒在創建的執行緒執行完後輸出估算的Pi為多少。
- 編譯指令: gcc 4_24_Pi.c -o Pi -pthread

## 4.28
- 描述: 模擬一個PIDManager，由一個全域變數*pid_map模擬，有三個函數操控allocate_map(初始化)、allocate_pid(分配Pid)、release_pid(釋放指定Pid)，main函數裡有簡單的測試。
- 編譯指令: gcc 4_28_PidManager.c -o PidManager -pthread

## 6.33
- a: 全域變數available_resources，可能多個執行緒同時存取
- b:
    - decrease_count()中的if (available resources < count)有讀取操作，available resources -= count;有讀取&運算&賦值操作  
    - increase_count()中的available resources += count;有讀取&運算&賦值操作
- c: 添加mutex和cond，mutex負責確保全域變數available_resources不會被同時存取，cond負責資源不夠時執行緒會阻塞而非退出