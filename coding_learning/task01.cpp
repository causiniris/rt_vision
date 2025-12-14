#include <iostream>
#include <string>

using namespace std;

//MotorController类定义
class MotorController {
private:
    bool Running;
    int speed;
    bool Clockwise;

public:
    MotorController() {
        Running = false;
        speed = 0;
        Clockwise = true;
    }

    // 获取状态
    bool getRunning() const { return Running; }
    int getSpeed() const { return speed; }
    bool getClockwise() const { return Clockwise; }

    // 开机
    void startMotor() { 
        if(Running) {
            cout << ">> 提示：电机已经在运行了。\n";
        } 
        else {
            Running = true; 
            cout << ">>电机已成功启动。\n";
        }
    }

    // 关机
    void stopMotor() { 
        Running = false; 
        speed = 0;
        cout << ">> 电机停止运行，正在关机...\n";
    }

    void changeSpeed(int delta) {
        if (Running == false) {
            cout << ">> 错误：请先开启电机 ！\n";
            return;
        }
        speed += delta;
        // 限制范围 0 - 100
        if (speed > 100) {
            speed = 100;
            cout << ">> 速度已达最大值 100。\n";
        }
        if (speed < 0) {
            speed = 0;
            cout << ">> 速度已达最小值 0。\n";
        }
        cout << ">> 速度调整为: " << speed << "\n";
    }

    void toggleDirection() {
        if (Running == false) {
             cout << ">> 错误：请先开启电机 ！\n";
             return;
        }
        Clockwise = !Clockwise;
        string dir; 
        if (Clockwise == true) {
            dir = "顺时针";  
        } 
        else {
            dir = "逆时针";  
        }
        cout << ">> 方向已切换为: " << dir << "\n";
    }
};

// 图形化进度条显示速度
void printProgressBar(int value) {
    cout << "速度: [";
    int bars = value / 5; 
    for (int i = 0; i < 20; ++i) {
        if (i < bars) {
            cout << "#";
        }
        else {
            cout << "-";
        }    
    }
    cout << "] " << value << "%\n";
}

// 打印状态
void printStatus(const MotorController& motor) {
    cout << "\n---------------- [当前状态] ----------------\n";
    cout << "状态: " << (motor.getRunning() ? "运行中 (ON)" : "已停止 (OFF)") << "\n";
    
    string dir = "---";
    if (motor.getRunning()) {
        dir = motor.getClockwise() ? "顺时针" : "逆时针";
    }
    cout << "方向: " << dir << "\n";

    printProgressBar(motor.getSpeed());
    cout << "--------------------------------------------\n";
}

// main函数
int main() {
    MotorController motor;
    char choice;
    bool keepRunning = true; // 控制循环是否继续

    cout << "=== 电机控制程序 ===\n";

    while (keepRunning) {
        //显示状态和菜单
        printStatus(motor);
        cout << "操作: 1.开启 | 0.关闭 | 2.加速 | 3.减速 | 4.换向\n";
        cout << "请输入指令: ";

        //输入
        cin >> choice;

        //逻辑判断
        switch (choice) {
            case '1': 
                motor.startMotor(); 
                break;
            
            case '0': 
                motor.stopMotor(); 
                keepRunning = false; 
                break;

            case '2': 
                motor.changeSpeed(10); 
                break;
            
            case '3': 
                motor.changeSpeed(-10); 
                break;
            
            case '4': 
                motor.toggleDirection(); 
                break;
            
            default: 
                cout << ">> 无效指令，请重新输入。\n";
                break;
        }
    }

    cout << "\n=== 程序已安全退出 ===\n";
    return 0;
}