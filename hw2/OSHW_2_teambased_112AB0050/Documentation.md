# 資財三甲 112AB0050 王界誠

## Runtime Environment:
- VMware
- Ubuntu 24.04.4 LTS
- gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- linux-headers-6.8.0-106-generic

## Ch4-1
- 描述: 執行後讀取參數檔案位置(沒輸入參數預設是input.txt)中的數字作為數獨解答，並創建共11個執行緒檢查該數獨解答是否合法，1個執行緒檢查所有欄，1個執行緒檢查所有列，9個執行緒分別檢查3*3的小九宮格，並將結果透過寫入全域變數的方法回傳給父執行緒，讓父執行緒判斷解答是否合法。
- 編譯指令: gcc Sudoku_Solution_Validator.c -o Sudoku_Solution_Validator -pthread

## Ch4-2
- 描述: 執行後讀取參數檔案位置(沒輸入參數預設是input.txt)中的數字作為排序數列，先創建2個執行緒讓其分別排序數列的前半段和後半段，結果放到全域變數中(雖然是操作同個陣列但是不同位置，所以不用管同步問題)，等執行完後再創建1個合併用的執行緒，將結果放到另一個全域變數，最後由主執行緒輸出結果。
- 編譯指令: gcc MergeSort.c -o MergeSort -pthread

## Ch5
- 描述: 執行後讀取參數檔案位置(沒輸入參數預設是task.txt)中的任務資訊，並輪流使用FCFS、SJF、Priority-based、Round-Robin、Priority with round-robin排序法來實際跑一輪，運行過程會輸出目前狀態在終端機。
- 編譯指令: gcc Scheduler.c -o Scheduler