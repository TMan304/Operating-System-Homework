# 資財三甲 112AB0050 王界誠

## Runtime Environment:
- VMware
- Ubuntu 24.04.4 LTS
- gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- linux-headers-6.8.0-106-generic

## 2.24
使用者輸入兩個位置，第一次輸入為要複製的檔案位置，第二次輸入為要複製到的位置，複製的檔案權限設為0644

## 3.20
模擬一個PIDManager，由一個全域變數*pid_map模擬，有三個函數操控allocate_map(初始化)、allocate_pid(分配Pid)、release_pid(釋放指定Pid)，main函數裡有簡單的測試

## 3.21
透過呼叫該程序同時輸入一個參數(like: ./Collatz 35)，該程序會fork一個子程序，由子程序計算並輸出數字序列，父程序等子程序完成後也終止

## 3.22
透過呼叫該程序同時輸入一個參數(like: ./CollatzIPC 35)，該程序會先創建一塊共享記憶體以及pipe並fork一個子程序，由子程序計算並透過pipe輸出數字序列到共享記憶體，父程序等子程序完成後讀取該共享記憶體的資料並輸出，最後關閉該塊記憶體以及pipe