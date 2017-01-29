
/*
   适用于各种Arduino主板
   如果连接9g舵机需要给主板9V供电或者直接在面包板上输出电源
   优化了LCD显示,解决出现Stepper出现大于1023和Servo大于180的情况
*/
//为I2C LED屏输出 加载库
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
//设置I2C地址 屏幕尺寸
LiquidCrystal_I2C lcd(0x3F, 20, 4);
//为步进电机加载库
#include <Stepper.h>
#define STEPS 100            // 这里设置步进电机旋转一圈是多少步
#include <Servo.h> //引入lib
//为舵机初始化环境
Servo myservo;  // 创建一个伺服电机对象
Stepper stepper(STEPS, 3, 5, 4, 6); // attached to设置步进电机的步数和引脚
int previous = 0;           // 定义变量用来存储历史读数
int val;  // 创建变量，储存从模拟端口读取的值（0到1023）
int vall; // 舵机旋转角度一般为0-180，所以需要另一个变量存储

void setup()//初始化设置
{
  lcd.init();//初始化LCD
  lcd.backlight();//打开屏幕背光
  stepper.setSpeed(100);         // 设置电机每分钟的转速为90步
  Serial.begin(9600);//串口波特率9600
  myservo.attach(9);  // 9号引脚输出电机控制信号 只能用9或10号引脚控制舵机
}

void loop()
{
  int val = analogRead(0);        // 获取传感器读数 0-1023
  vall = map(val, 0, 1023, 0, 180);//通过map把val的0-1023范围映射到vall的0-180范围

  //串口输出
  Serial.print("Stepper = ");
  Serial.println(val);
  Serial.print("Servo = ");
  Serial.println(vall);

  //LCD输出
  lcd.setCursor(0, 0); //设置开始写入的位置 第一个字符为0, 第一排为0 以此类推
  lcd.print("====================");//显示 一串=
  lcd.setCursor(0, 1);
  lcd.print("Stepper = ");
  /*
    lcd.setCursor(10, 1);//如果这里加入这句话
    lcd.print("    ");//则下面val值出现不停闪烁情况。
  */
  lcd.setCursor(10, 1);
  lcd.print(val);
  lcd.setCursor(0, 2);
  lcd.print("Servo = ");
  lcd.setCursor(8, 2);
  lcd.print(vall);
  //开始控制步进电机和舵机
  myservo.write(vall);  //输出给舵机

  if (abs(analogRead(0) - val) >= 5)//去除抖动
  {
    stepper.step(val - previous);     // 移动步数为当前读数减去历史读数
    //    lcd.clear();// 如果需要变化电机则清除显示，否则会出现显示val=9000+和vall为900+的情况 缺点是整屏闪烁
    lcd.setCursor(10,1);//
    lcd.print("    ");//打印4个空格，这样只数字部分闪烁
    lcd.setCursor(8,2);
    lcd.print("    ");//打印4个空格
  }
  previous = val;             // 保存历史读数
  //    lcd.clear();// 如果这句话放在这里会出现屏闪现象 基本看不清字

  delay(200);//延迟200毫秒
  //      lcd.clear();// 如果这句话放在这里会出现屏闪现象 闪的眼晕
  lcd.setCursor(1, 3);
  lcd.print("Powered by : Mr.Wu");
}
