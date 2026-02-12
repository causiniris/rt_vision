# 🚗 LPR - Dual Mode License Plate Recognition
> 基于 YOLOv8 的车牌识别项目，支持 **PyTorch** (训练/开发) 和 **ONNX** (部署) 双模式运行。

## 📂 项目结构
```text
LPR/
├── models/             # 模型仓库
│   ├── best.pt         # PyTorch 权重 (用于开发)
│   └── best.onnx       # ONNX 权重 (用于部署)
├── data/               # 测试数据
├── predict_pt.py       # ✅ 方式一：PyTorch 推理脚本
├── predict_onnx.py     # ✅ 方式二：ONNX 推理脚本
├── requirements.txt    # 项目依赖
└── README.md           # 说明文档
```