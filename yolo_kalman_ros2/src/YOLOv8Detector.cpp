#include "yolo_kalman_ros2/YOLOv8Detector.hpp"
#include <rclcpp/rclcpp.hpp>

YOLOv8Detector::YOLOv8Detector(const std::string& model_path) 
    : env(ORT_LOGGING_LEVEL_WARNING, "YOLOv8") {
    
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
    
    try {
        session = Ort::Session(env, model_path.c_str(), session_options);
        RCLCPP_INFO(rclcpp::get_logger("YOLOv8Detector"), "✅ 模型加载成功: %s", model_path.c_str());
    } catch (const std::exception& e) {
        RCLCPP_ERROR(rclcpp::get_logger("YOLOv8Detector"), "❌ 模型加载失败: %s", e.what());
    }
}

std::vector<Detection> YOLOv8Detector::detect(cv::Mat& frame) {
    std::vector<Detection> detections;
    if (frame.empty()) return detections;

    // 1. 预处理
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1/255.0, cv::Size(640, 640), cv::Scalar(0,0,0), true, false);

    // 2. 推理
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::vector<int64_t> input_shape = {1, 3, 640, 640};
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, (float*)blob.data, blob.total(), input_shape.data(), input_shape.size());
    
    auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names.data(), &input_tensor, 1, output_names.data(), 1);
    
    // 3. 解析输出
    float* data = output_tensors[0].GetTensorMutableData<float>();
    auto shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
    
    // 维度判断
    int rows, cols; // rows=属性数量(84), cols=锚点数量(8400)
    bool is_transposed = false;

    if (shape[1] < shape[2]) {
        // [1, 84, 8400] -> 标准格式
        rows = shape[1]; 
        cols = shape[2]; 
    } else {
        // [1, 8400, 84] -> 已转置
        rows = shape[2];
        cols = shape[1];
        is_transposed = true;
    }

    std::vector<cv::Rect> boxes;
    std::vector<float> confs;
    float max_score_log = 0.0f;
    int detected_class_log = -1;

    // 遍历所有 8400 个锚点
    for (int i = 0; i < cols; ++i) {
        float cx, cy, w, h;
        float max_class_score = 0.0f;
        int max_class_id = -1;

        if (!is_transposed) {
            // 标准格式 [1, 84, 8400]: 需跨步读取
            // 坐标在前4行，类别在后80行
            
            // 寻找该锚点在所有类别中的最大概率
            for (int c = 0; c < (rows - 4); ++c) {
                float score = data[(4 + c) * cols + i]; // 第 4+c 行，第 i 列
                if (score > max_class_score) {
                    max_class_score = score;
                    max_class_id = c;
                }
            }
            
            if (max_class_score > 0.25f) {
                cx = data[0 * cols + i];
                cy = data[1 * cols + i];
                w  = data[2 * cols + i];
                h  = data[3 * cols + i];
            } else continue;

        } else {
            // 转置格式 [1, 8400, 84]: 连续读取
            float* ptr = data + (i * rows);
            
            // 寻找最大概率
            for (int c = 0; c < (rows - 4); ++c) {
                float score = ptr[4 + c];
                if (score > max_class_score) {
                    max_class_score = score;
                    max_class_id = c;
                }
            }

            if (max_class_score > 0.25f) {
                cx = ptr[0];
                cy = ptr[1];
                w  = ptr[2];
                h  = ptr[3];
            } else continue;
        }

        // 记录日志用的最大值
        if (max_class_score > max_score_log) {
            max_score_log = max_class_score;
            detected_class_log = max_class_id;
        }

        // 坐标还原
        int left = static_cast<int>((cx - 0.5 * w) * frame.cols / 640.0);
        int top = static_cast<int>((cy - 0.5 * h) * frame.rows / 640.0);
        int width = static_cast<int>(w * frame.cols / 640.0);
        int height = static_cast<int>(h * frame.rows / 640.0);

        boxes.push_back(cv::Rect(left, top, width, height));
        confs.push_back(max_class_score);
    }

    // 调试日志：告诉你到底识别到了什么
    static int log_counter = 0;
    if (log_counter++ % 30 == 0) {
        RCLCPP_INFO(rclcpp::get_logger("Debug"), 
            "📊 最高分: %.2f | 对应类别ID: %d (2=Car, 5=Bus, 7=Truck)", 
            max_score_log, detected_class_log);
    }

    // 4. NMS 过滤
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confs, 0.25f, 0.45f, indices);

    for (int idx : indices) {
        // 这里可以加一个过滤器：只画车(2)、公交(5)、卡车(7)
        // 但为了先看到框，暂时全部画出来
        detections.push_back({boxes[idx], confs[idx]});
        cv::rectangle(frame, boxes[idx], cv::Scalar(0, 0, 255), 2);
    }
    
    return detections;
}