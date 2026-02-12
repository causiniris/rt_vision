import os
import sys
from ultralytics import YOLO

if __name__ == '__main__':
    # --- 路径配置 ---
    project_root = os.path.dirname(os.path.abspath(__file__))
    
    # 模型路径 (.onnx)
    model_path = os.path.join(project_root, 'models', 'best.onnx')
    img_path = os.path.join(project_root, 'data', 'test_image', 'test.png')
    save_dir = os.path.join(project_root, 'runs', 'detect')

    # --- 检查模型 ---
    if not os.path.exists(model_path):
        print(f"❌ 错误：找不到 ONNX 模型文件 {model_path}")
        print("💡 提示：请确保你已经从 Windows 导出了 .onnx 文件并放入了 models 文件夹")
        sys.exit()

    # --- 加载与推理 ---
    print(f"🚀 [ONNX] 正在加载模型: {model_path}")
    # task='detect' 明确指定任务，防止 ONNX 元数据丢失导致报错
    model = YOLO(model_path, task='detect') 

    print("🤖 [ONNX] 开始推理 (使用 ONNXRuntime 引擎)...")
    # name='onnx_inference' 区分结果文件夹
    model.predict(source=img_path, save=True, device='cpu', project=save_dir, name='onnx_inference')

    print(f"✅ [ONNX] 推理完成！结果保存在: {os.path.join(save_dir, 'onnx_inference')}")