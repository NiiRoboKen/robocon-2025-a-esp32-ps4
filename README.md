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

#### 独立ステアリングリセット
```
| 0x10 |
```

#### 独立ステアリング状態送信(並進)
```
| 0x11 | duty (uint8_t) | theta (int16_t) | state (uint8_t) |
```
- `duty`には0~100の値が入る
- `theta`の単位はdegree で-180から180の値が入る
- `state` には車輪を回すかどうかのフラグ(ONなら1、OFFなら0)

#### 独立ステアリング状態送信(車体回転)
```
| 0x12 | is_cw (uint8_t) | duty (uint8_t) | state (uint8_t) |
```
- `is_cw`には回転方向を入れる(時計回りなら1、反時計回りなら0 )
- `duty`には0~100の値が入る
- `state` には車輪を回すかどうかのフラグ(ONなら1、OFFなら0)

#### ロジャー展開
```
| 0x20 | duty (uint8_t) | is_up (uint8_t) | 
```
- `duty`には0から100が入ります
- `is_up`には1か0が入ります(展開なら1)

#### ロジャー展開 右
```
| 0x21 | duty (uint8_t) | is_up (uint8_t) | 
```
- `duty`には0から100が入ります
- `is_up`には1か0が入ります(展開なら1)

#### ロジャー展開 左
```
| 0x22 | duty (uint8_t) | is_up (uint8_t) | 
```
- `duty`には0から100が入ります
- `is_up`には1か0が入ります(展開なら1)

#### 右アーム動作
```
| 0x31 | is_open | is_open_move | is_fold | is_fold_move |
```
- `is_open`には1か0が入ります(展開なら1)
- `is_open_move`には1か0が入ります(動くなら1)
- `is_fold`には1か0が入ります(掴むなら1)
- `is_fold_move`には1か0が入ります(動くなら1)

#### 左アーム動作
```
| 0x32 | is_open | is_open_move | is_fold | is_fold_move |
```
- `is_open`には1か0が入ります(展開なら1)
- `is_open_move`には1か0が入ります(動くなら1)
- `is_fold`には1か0が入ります(掴むなら1)
- `is_fold_move`には1か0が入ります(動くなら1)

#### 右アーム昇降
```
| 0x41 | duty (uint8_t) | is_up (uint8_t) | 
```
- `duty`には0から100が入ります
- `is_up`には1か0が入ります(上昇なら1)

#### 左アーム昇降
```
| 0x42 | duty (uint8_t) | is_up (uint8_t) | 
```
- `duty`には0から100が入ります
- `is_up`には1か0が入ります(上昇なら1)

#### 2リンクアーム右duty
```
| 0x51 | duty1 (uint8_t) | dir1 (uint8_t) | duty2 (uint8_t) | dir1 (uint8_t) |
```
- `duty`には0から100が入ります
- `dir`には0か1が入ります

#### 2リンクアーム左duty
```
| 0x52 | duty1 (uint8_t) | dir1 (uint8_t) | duty2 (uint8_t) | dir1 (uint8_t) |
```
- `duty`には0から100が入ります
- `dir`には0か1が入ります

#### 吸引
```
| 0x60 | is_on (uint8_t) |
```
- `is_on`には0か1が入ります(onなら1)

#### 吸引(Right)
```
| 0x61 | is_on (uint8_t) |
```

#### 吸引(Left)
```
| 0x62 | is_on (uint8_t) |
```
