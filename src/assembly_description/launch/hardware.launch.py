import os
from launch import LaunchDescription
from launch.substitutions import Command, FindExecutable, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    # 1. Locate the package and configuration files
    pkg_share = FindPackageShare('assembly_description')
    urdf_file = PathJoinSubstitution([pkg_share, 'urdf', 'assembly.urdf.xacro'])
    controller_params = PathJoinSubstitution([pkg_share, 'config', 'controllers.yaml'])

    # 2. Process the Xacro file and strictly set use_sim:=false for physical hardware
    robot_description_content = Command(
        [PathJoinSubstitution([FindExecutable(name='xacro')]), ' ', urdf_file, ' use_sim:=false']
    )
    robot_description = {'robot_description': robot_description_content}

    # 3. Define the core hardware nodes
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[robot_description]
    )

    controller_manager_node = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[robot_description, controller_params],
        output='screen'
    )

    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_broad', '--controller-manager', '/controller_manager'],
    )

    diff_drive_controller_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['diff_cont', '--controller-manager', '/controller_manager'],
    )

    # 4. Launch everything simultaneously
    return LaunchDescription([
        robot_state_publisher_node,
        controller_manager_node,
        joint_state_broadcaster_spawner,
        diff_drive_controller_spawner,
    ])