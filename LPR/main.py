import os
from ultralytics import YOLO

if __name__ == '__main__':
    # 1. 动态获取当前 main.py 所在的 LPR 文件夹绝对路径
    current_dir = os.path.dirname(os.path.abspath(__file__))
    
    # 2. 拼接出我们想要的、不容拒绝的 runs 路径
    save_path = os.path.join(current_dir, 'runs', 'detect')

    # 3. 加载模型
    model_path = os.path.join(current_dir, 'models', 'best.pt')
    model = YOLO(model_path) 

    # 4. 指定图片路径
    img_path = os.path.join(current_dir, 'data', 'test_image', 'test.png')

    print("🤖 正在加载模型并识别...")
    
    # 5. 强行指路：把拼接好的绝对路径直接扔给 project 参数
    results = model.predict(source=img_path, save=True, device='cpu', project=save_path)

    print(f"✅ 识别完成！结果已强制保存在: {save_path}")