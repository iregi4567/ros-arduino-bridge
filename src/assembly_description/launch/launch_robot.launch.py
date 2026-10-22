import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command
from launch.actions import RegisterEventHandler
from launch.event_handlers import OnProcessStart

from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    # Include the robot_state_publisher launch file, provided by our own package.
    package_name = "assembly_description"

    rsp = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [
                os.path.join(
                    get_package_share_directory(package_name),
                    "launch",
                    "rsp.launch.py",
                )
            ]
        ),
        launch_arguments={
            "use_sim_time": "false",
            "use_ros2_control": "true",
        }.items(),
    )

    twist_mux_params = os.path.join(
        get_package_share_directory(package_name), "config", "twist_mux.yaml"
    )

    # twist_mux outputs plain Twist on an intermediate topic
    twist_mux = Node(
        package="twist_mux",
        executable="twist_mux",
        parameters=[twist_mux_params, {"use_stamped": False}],
        remappings=[("/cmd_vel_out", "/cmd_vel_unstamped")],
    )

    # twist_stamper adds the header so diff_cont (stamped-only) accepts it
    twist_stamper = Node(
        package="twist_stamper",
        executable="twist_stamper",
        remappings=[
            ("cmd_vel_in", "/cmd_vel_unstamped"),
            ("cmd_vel_out", "/diff_cont/cmd_vel"),
        ],
    )

    # Wrap the command output with ParameterValue to prevent YAML parsing errors
    robot_description = ParameterValue(
        Command(
            ["ros2 param get --hide-type /robot_state_publisher robot_description"]
        ),
        value_type=str,
    )

    controller_params_file = os.path.join(
        get_package_share_directory(package_name),
        "config",
        "my_controllers.yaml",
    )

    controller_manager = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[
            {"robot_description": robot_description},
            controller_params_file,
        ],
    )

    delayed_controller_manager = TimerAction(
        period=3.0, actions=[controller_manager]
    )

    diff_drive_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_cont"],
    )

    delayed_diff_drive_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[diff_drive_spawner],
        )
    )

    joint_broad_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_broad"],
    )

    delayed_joint_broad_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[joint_broad_spawner],
        )
    )

    imu_broad_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["imu_broad"],
    )

    delayed_imu_broad_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[imu_broad_spawner],
        )
    )

    # Launch them all!
    return LaunchDescription(
        [
            rsp,
            twist_mux,
            twist_stamper,
            delayed_controller_manager,
            delayed_diff_drive_spawner,
            delayed_joint_broad_spawner,
            delayed_imu_broad_spawner,
        ]
    )