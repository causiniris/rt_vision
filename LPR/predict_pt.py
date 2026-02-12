import os
import sys
from ultralytics import YOLO

if __name__ == '__main__':
    # --- 路径配置 ---
    # 获取当前脚本所在目录 (LPR 文件夹根目录)
    project_root = os.path.dirname(os.path.abspath(__file__))
    
    # 模型路径 (.pt)
    model_path = os.path.join(project_root, 'models', 'best.pt')
    # 图片路径
    img_path = os.path.join(project_root, 'data', 'test_image', 'test.png')
    # 结果保存路径 (强制在 LPR/runs 下)
    save_dir = os.path.join(project_root, 'runs', 'detect')

    # --- 检查模型 ---
    if not os.path.exists(model_path):
        print(f"❌ 错误：找不到 PyTorch 模型文件 {model_path}")
        sys.exit()

    # --- 加载与推理 ---
    print(f"🔥 [PyTorch] 正在加载模型: {model_path}")
    model = YOLO(model_path) 

    print("🤖 [PyTorch] 开始推理...")
    # name='pt_inference' 区分结果文件夹
    model.predict(source=img_path, save=True, device='cpu', project=save_dir, name='pt_inference')

    print(f"✅ [PyTorch] 推理完成！结果保存在: {os.path.join(save_dir, 'pt_inference')}")