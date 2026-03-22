# 資財三甲 112AB0050 王界誠

## Ch1
### 1.16
- a: CPU將指令(包括資料記憶體位址、傳輸方向等)發送給目標設備DMA，由DMA負責控制記憶體和設備之間的資料傳輸
- b: DMA將資料搬運完後，會發送interrupt訊號給CPU
- c: 是，DMA搬運資料時會跟CPU共享匯流排控制權限，導致CPU執行用戶程序時可能需要等待，這稱作Cycle Stealing

## Ch2
### 2.19
- advantage: 易於擴充、單一系統服務崩潰不會造成整體當機
- interact: 由於kernel只有精簡的服務，大多數系統服務是以進程的方式存在，因此使用者程序與系統服務的交互方式為IPC，會由發送端傳給kernel，再由kernel傳給接收端
- disadvantage: 由於IPC會頻繁切換user mode/kernel mode，進程間的通訊較低效
### 2.20
- advantage: 相較Monolith kernel架構，可以有更靈活的擴充性，並且相較Microkernel，系統服務皆為kernel的一部份，都在kernel mode進行，不需要使用IPC頻繁切換mode，通訊效率更高

## Ch3
### 3.12
- actions: dispatcher將舊process的狀態存進PCB，scheduler將該PCB的state根據切換原因變為waiting或ready，dispatcher再去載入新process的PCB到暫存器交給CPU執行(dispatcher、scheduler都是os的程式，發送指令給CPU執行)
### 3.18
- ordinary pipes: parent process和child process之間的通訊，pipe的生命週期隨著child process完成而關閉
- named pipes: 2個或以上無血緣的process之間通訊