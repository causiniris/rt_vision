# LPR 车牌识别部署项目

这是一个基于 YOLOv8n 的车牌识别项目，部署在 Ubuntu 环境下。

## 目录结构
- `models/`: 存放训练好的权重文件
- `data/`: 存放测试图片
- `main.py`: 推理主程序

## 如何运行
1. 安装依赖: `pip install -r requirements.txt`
2. 运行推理: `python3 main.py`