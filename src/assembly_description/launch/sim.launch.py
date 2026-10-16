import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, SetEnvironmentVariable
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command, LaunchConfiguration, TextSubstitution
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    pkg = get_package_share_directory('assembly_description')
    ros_gz_sim = get_package_share_directory('ros_gz_sim')
    xacro_file = os.path.join(pkg, 'urdf', 'assembly.urdf.xacro')
    worlds_dir = os.path.join(pkg, 'worlds')
    bridge_yaml = os.path.join(pkg, 'config', 'bridge.yaml')

    # package:// meshes (robot) and model:// meshes (gamefield) must both be found by Gazebo
    resource_path = os.pathsep.join([os.path.dirname(pkg), os.path.join(pkg, 'models')])
    if os.environ.get('GZ_SIM_RESOURCE_PATH'):
        resource_path += os.pathsep + os.environ['GZ_SIM_RESOURCE_PATH']

    robot_description = ParameterValue(Command(['xacro ', xacro_file]), value_type=str)

    return LaunchDescription([
        DeclareLaunchArgument('rviz', default_value='true'),
        DeclareLaunchArgument('world', default_value='gamefield_placeholder',
                              description='world file name in worlds/ without .sdf'),
        DeclareLaunchArgument('x', default_value='0.0'),
        DeclareLaunchArgument('y', default_value='0.0'),
        DeclareLaunchArgument('z', default_value='0.05'),
        DeclareLaunchArgument('yaw', default_value='0.0'),
        SetEnvironmentVariable('GZ_SIM_RESOURCE_PATH', resource_path),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(ros_gz_sim, 'launch', 'gz_sim.launch.py')),
            launch_arguments={'gz_args': [
                TextSubstitution(text='-r '),
                TextSubstitution(text=worlds_dir + os.sep),
                LaunchConfiguration('world'),
                TextSubstitution(text='.sdf')]}.items()),
        Node(package='robot_state_publisher', executable='robot_state_publisher',
             parameters=[{'robot_description': robot_description, 'use_sim_time': True}]),
        Node(package='ros_gz_sim', executable='create', output='screen',
             arguments=['-name', 'assembly', '-topic', 'robot_description',
                        '-x', LaunchConfiguration('x'),
                        '-y', LaunchConfiguration('y'),
                        '-z', LaunchConfiguration('z'),
                        '-Y', LaunchConfiguration('yaw')]),
        Node(package='ros_gz_bridge', executable='parameter_bridge', output='screen',
             parameters=[{'config_file': bridge_yaml, 'use_sim_time': True}]),
        Node(package='robot_localization', executable='ekf_node', name='ekf_filter_node',
             output='screen', parameters=[os.path.join(pkg, 'config', 'ekf.yaml')]),
        Node(package='rviz2', executable='rviz2', output='screen',
             condition=IfCondition(LaunchConfiguration('rviz')),
             parameters=[{'use_sim_time': True}],
             arguments=['-d', os.path.join(pkg, 'rviz', 'display.rviz')]),
    ])
