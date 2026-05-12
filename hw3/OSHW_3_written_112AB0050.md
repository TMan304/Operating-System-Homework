# 資財三甲 112AB0050 王界誠

## Ch7
### 7.8
因為當semaphore已經被占用時，嘗試獲取會讓進程進入等待佇列並釋放cpu資源，但spinlock使用時需要一直占用cpu資源，且釋放資源時不會釋放spinlock，這有可能導致deadlock發生。

## Ch8
### 8.20
- a: 任何情況下安全
- b: 可能不安全
- c: 可能不安全
- d: 任何情況下安全
- e: 可能不安全
- f: 任何情況下安全
### 8.28
- a: <T2, T1, T0, T3, T4>
- d: 可以被批准，因為有序列<T2, T1, T0, T3, T4>可以執行完，代表系統仍舊是安全狀態
### 8.30
```pseudocode
north() {
    mutex.lock()
    cross bridge...
    mutex.unlock()
}

south() {
    mutex.lock()
    cross bridge...
    mutex.unlock()
}
```

## Ch9
### 9.13
| 記憶體區域 | 100MB      | 170MB | 40MB | 205MB | 300MB       | 185MB      |
|------------|------------|-------|------|-------|-------------|------------|
| First-fit  | 15MB, 75MB | 80MB  |      | 200MB | 185MB       | 175MB      |
| Best-fit   | 75MB       |       | 15MB | 200MB | 175MB, 80MB | 185MB      |
| Worst-fit  |            | 75MB  |      | 185MB | 200MB       | 15MB, 80MB | 

在Worst-fit中，175MB的程序沒有足夠的連續空間可以放入
- Best-fit在利用記憶體空間效率最高(雖然會誕生很難再利用的小碎片空間)
- First-fit速度更快，且在這題中能夠分配所有程序(與Best-fit相同)
- Worst-fit速度與Best-fit相同，但是是這題唯一沒能分配所有程序的算法
### 9.24
- a: 2^19個條目
- b: 2^17個條目