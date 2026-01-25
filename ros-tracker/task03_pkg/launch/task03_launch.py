import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # 获取 yaml 文件的路径
    config_file = os.path.join(
        get_package_share_directory('task03_pkg'),
        'config',
        'params.yaml'
    )

    return LaunchDescription([
        # 启动 Founder，并加载参数
        Node(
            package='task03_pkg',
            executable='founder',
            name='founder_node',
            output='screen',
            parameters=[config_file]  # 加载参数文件
        ),
        # 启动 Tracker
        Node(
            package='task03_pkg',
            executable='tracker',
            name='tracker_node',
            output='screen'
        )
    ])