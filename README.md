# Robocon 2025 A Team PS4 Node

## UART通信
Netetra作のプロトコルSBTPを使用します。
https://github.com/Netetra/sbtp

基本的に送信しか行いません。

### NRCC-2025 (Niihama Robot Control Commands 2025)
#### 特徴
- 最初の1byteはコマンド
- ビックデンディアンを使用
- 1byteずつ送る

#### 受信成功
```
| 0x00 |
```

#### ping
```
| 0x01 |
```

#### 受信失敗
```
| 0x02 | error code (uint_8)|
```

---

#### 独立ステアリング状態送信(並進)
```
| 0x10 | duty (uint8_t) | theta (int16_t) | state (uint8_t) |
```
- `duty`には0~100の値が入る
- `theta`の単位はdegree で-180から180の値が入る
- `state` には車輪を回すかどうかのフラグ(ONなら1、OFFなら0)

#### 独立ステアリング状態送信(車体回転)
```
| 0x11 | is_cw (uint8_t) | duty (uint8_t) | state (uint8_t) |
```
- `is_cw`には回転方向を入れる(時計回りなら1、反時計回りなら0 )
- `duty`には0~100の値が入る
- `state` には車輪を回すかどうかのフラグ(ONなら1、OFFなら0)

### ロジャー展開
```
| 0x20 | is_move (uint8_t) | is_up (uint8_t) | 
```
- `is_move`には1か0が入ります(動くなら1)
- `is_up`には1か0が入ります(展開なら1)

