## ZhouSensorIO
简单的大四游戏控制器IO库，支持多个进程同时访问手台，并针对这一使用场景进行了简单的优化。

### 如何使用
链接 ZhouSensorLib 并引用头文件 ```sensor.h``` , 之后可参考 ZhouSensorTest 项目中的代码来编写。

### 开发指北
首先需要调用 ```sensor_start();``` 进行初始化，之后便可对手台进行操作。

#### 交互相关
```SENSOR_API uint8_t sensor_get_ir(int fix);```
获取IR，参数中的 ```fix``` 为是否对或得到的数据进行修正。

```SENSOR_API uint8_t sensor_get_buttons();```
获取按键状态。

```SENSOR_API void sensor_get_touch(uint8_t* buffer);```
获取触摸板状态，需要传入一个 ```uint8_t[32]``` 数组。

```SENSOR_API void sensor_set_color(uint8_t* color);```
设置触摸板颜色，需要传入一个 ```uint8_t[96]``` 数组，内部有对于颜色的压缩处理。

#### 读卡器相关

```SENSOR_API uint8_t sensor_get_card_statue();```
获取读卡器状态。

```SENSOR_API void sensor_set_card_led(uint8_t r, uint8_t g, uint8_t b);```
设置读卡器LED颜色。

```SENSOR_API HRESULT sensor_get_****_id(***);```
尝试读取指定类型的卡。

### 程序结构
当调用 ```sensor_start();``` 时，会尝试连接到已经启动的手台 ```Host``` ，若未连接成功则会尝试创建新的 ```Host``` 。不同进程之间采用共享内存访问手台状态，包括读卡器，IR以及触摸板，未作同步处理。 ```Host``` 相关代码在  ```Host.c``` 中。