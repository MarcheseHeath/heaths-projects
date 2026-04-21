#! /usr/bin/env python

# Python packages
from threading import Thread
import time

# ROS 2 packages
import rclpy
from rclpy.node import Node
from rclpy.callback_groups import ReentrantCallbackGroup, MutuallyExclusiveCallbackGroup

# Moveit packages
from pymoveit2 import MoveIt2
from pymoveit2.robots import ur3e as robot
from my_robot_msgs.action import UR3eMove

# Actions, Services, Messages
from ur_msgs.srv import SetIO
from ur_msgs.msg import IOStates
from std_msgs.msg import String


class ur3e_commander_class(Node):

    # INITIALIZE THE NODE
    def __init__(self):
        super().__init__('ur3e_commander_node')

        # Create nodes for executor
        self.moveit_node = Node("ex_pose_goal")
        self.io_node = Node("io_service_client")

        # Callback group so color and digital input subscribers do not overlap
        subscriber_callback_group = MutuallyExclusiveCallbackGroup()
              
################################################################################################
######################### MODIFY HERE ##########################################################
        '''
        Create the subscriber to monitor the block's color
        1. Use the same message type and topic name as your publisher in color_node.py

        Create the subscriber to monitor the digital inputs
        1. Use the /IOStates message type
        2. Set the topic to /io_and_status_controller/io_states

        Create the service client to set outputs on the robot
        1. Use the /SetIO service type
        2. Set the service name to /io_and_status_controller/set_io
        '''

        ### CREATE BLOCK COLOR SUBSCRIBER   
        self.color_subscriber = self.create_subscription(
            MessageType,                                        # add correct message type
            '/topic_name',                                      # add correct topic name
            self.block_color_callback, 10, callback_group=subscriber_callback_group)
            
        # CREATE DIGITAL INPUT SUBSCRIBER
        self.DI_states_subscriber = self.create_subscription(
            MessageType,                                        # add correct message type
            '/topic_name',                                      # add correct topic name
            self.digital_input_callback, 10, callback_group=subscriber_callback_group)
        
        # CREATE DIGITAL I/O SERVICE CLIENT
        self.grip_client = self.io_node.create_client(
            ServiceType,                                        # add correct service type
            '/service_name',                                    # add correct service name
            callback_group=MutuallyExclusiveCallbackGroup())

######################### END ##################################################################
################################################################################################

        # Wait for service server to be available
        while not self.grip_client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('/set_io service not available, waiting again...')
        self.io_rate = self.io_node.create_rate(20)
        # Initialize service call request message
        self.grip = SetIO.Request()
        
        # Initialize block color attribute
        self.block_color = None

        # Call the subscribers (avoid never used warning)
        self.color_subscriber
        self.DI_states_subscriber

        # Create MoveIt 2 interface
        self.moveit2 = MoveIt2(
            node=self.moveit_node,
            joint_names=robot.joint_names(),
            base_link_name=robot.base_link_name(),
            end_effector_name=robot.end_effector_name(),
            group_name=robot.MOVE_GROUP_ARM,
            callback_group=ReentrantCallbackGroup()
        )
        # Scale down velocity and acceleration of joints (percentage of maximum)
        self.moveit2.max_velocity = 0.5
        self.moveit2.max_acceleration = 0.5
        # Set the planning time (Joint)
        self.moveit2.planning_time = 10.0
        # Set the planner (Joint)
        self.moveit2.planner = "RRTConnectkConfigDefault"

        # Spin the nodes in background threads
        executor = rclpy.executors.MultiThreadedExecutor(2)
        executor.add_node(self.moveit_node)
        executor.add_node(self.io_node)
        executor_thread = Thread(target=executor.spin, daemon=True, args=())
        executor_thread.start()

        #Declare parameter for joint positions (from JointState)
        #MATCH /joint_states TOPIC ORDER
        #["shoulder_lift_joint", 
        #"elbow_joint", 
        #"wrist_1_joint", 
        #"wrist_2_joint", 
        #"wrist_3_joint", 
        #"shoulder_pan_joint"]

        ### DECLARE JOINT POSITIONS FOR 'HOME' ###
        self.moveit_node.declare_parameter("HOME", [-1.1530533593944092, -2.039259672164917, -3.122103830377096, -0.7799976507769983, -1.5435832182513636, -2.354912821446554])
        
        ### DECLARE JOINT POSITIONS FOR PICKING UP CUBE ###
        self.moveit_node.declare_parameter("Appr_Conv", [-1.7767912350096644, -2.691538095474243, -1.8474327526488246, -0.7395189444171351, -1.541957203541891, -2.3182433287249964])
        self.moveit_node.declare_parameter("Pick_Cube", [-2.001524110833639, -2.239919424057007, -2.0687858066954554, -0.9804046789752405, -1.549370590840475, -2.55831486383547])
        self.moveit_node.declare_parameter("Exit_Conv", [-1.1542927783778687, -2.0393526554107666, -3.1221281490721644, -0.7800138632403772, -1.5435951391803187, -0.6639063994037073])

        ### DECLARE JOINT POSITIONS FOR PlACING CUBE IN RED BIN ###
        self.moveit_node.declare_parameter("Appr_RED", [-1.1549317401698609, -2.039299726486206, -3.1221610508360804, -0.7800067106830042, -1.5435989538775843, 1.1388180255889893])
        self.moveit_node.declare_parameter("Place_RED", [-1.4921811309507866, -2.1673946380615234, -3.1644379101195277, -1.0599163214312952, -1.3268874327289026, 1.042476773262024])
        self.moveit_node.declare_parameter("Exit_RED", [-1.1549317401698609, -2.039299726486206, -3.1221610508360804, -0.7800067106830042, -1.5435989538775843, 1.1388180255889893])

        ### DECLARE JOINT POSITIONS FOR PlACING CUBE IN GREEN BIN ###
        self.moveit_node.declare_parameter("Appr_GREEN", [-1.1548482936671753, -2.0393221378326416, -3.122120042840475, -0.7800181547748011, -1.543542210255758, 0.5776258111000061])
        self.moveit_node.declare_parameter("Place_GREEN", [-1.228507713680603, -2.2540295124053955, -3.0917870006956996, -1.017245117818014, -1.4339044729815882, 0.48578473925590515])
        self.moveit_node.declare_parameter("Exit_GREEN", [-1.1548482936671753, -2.0393221378326416, -3.122120042840475, -0.7800181547748011, -1.543542210255758, 0.5776258111000061])
        
        ### DECLARE JOINT POSITIONS FOR PlACING CUBE IN BLUE BIN ###
        self.moveit_node.declare_parameter("Appr_BLUE", [-1.1548482936671753, -2.0393221378326416, -3.122120042840475, -0.7800181547748011, -1.543542210255758, 0.1776258111000061])
        self.moveit_node.declare_parameter("Place_BLUE", [-1.9547835789122523, -1.7393049001693726, -2.9696079693236292, -1.4314940611468714, -1.5433462301837366, -0.19698745409120733])
        self.moveit_node.declare_parameter("Exit_BLUE", [-1.1548482936671753, -2.0393221378326416, -3.122120042840475, -0.7800181547748011, -1.543542210255758, 0.1776258111000061])

        self.MoveJ_Home = (self.moveit_node.get_parameter("HOME").get_parameter_value().double_array_value)
        self.MoveJ_Appr_Conv = (self.moveit_node.get_parameter("Appr_Conv").get_parameter_value().double_array_value)
        self.MoveJ_Pick_Cube = (self.moveit_node.get_parameter("Pick_Cube").get_parameter_value().double_array_value)
        self.MoveJ_Exit_Conv = (self.moveit_node.get_parameter("Exit_Conv").get_parameter_value().double_array_value)
        self.MoveJ_Appr_RED = (self.moveit_node.get_parameter("Appr_RED").get_parameter_value().double_array_value)
        self.MoveJ_Place_RED = (self.moveit_node.get_parameter("Place_RED").get_parameter_value().double_array_value)
        self.MoveJ_Exit_RED = (self.moveit_node.get_parameter("Exit_RED").get_parameter_value().double_array_value)
        self.MoveJ_Appr_GREEN = (self.moveit_node.get_parameter("Appr_GREEN").get_parameter_value().double_array_value)
        self.MoveJ_Place_GREEN = (self.moveit_node.get_parameter("Place_GREEN").get_parameter_value().double_array_value)
        self.MoveJ_Exit_GREEN = (self.moveit_node.get_parameter("Exit_GREEN").get_parameter_value().double_array_value)
        self.MoveJ_Appr_BLUE = (self.moveit_node.get_parameter("Appr_BLUE").get_parameter_value().double_array_value)
        self.MoveJ_Place_BLUE = (self.moveit_node.get_parameter("Place_BLUE").get_parameter_value().double_array_value)
        self.MoveJ_Exit_BLUE = (self.moveit_node.get_parameter("Exit_BLUE").get_parameter_value().double_array_value)
        
        self.open_gripper()
        self.get_logger().info("Node Initialized")    

################################################################################################
######################### MODIFY HERE ##########################################################
    '''
    For the digital_input_callback method:
    Check if DI[0] is HIGH (see /ur_msgs/msg/IOStates for reference)
    If so: (otherwise do nothing)
    1. Log the info "DI[0] is HIGH"
    2. Depending on the block color (red/green/blue):
        a. Log the info "Block is {color}"
        b. Set the {task} parameter to "RED/GREEN/BLUE CUBE"
        c. Set the {approach}, {place}, and {exit} parameters to the appropriate waypoints
        If the block color is not R/G/B, exit the function
    3. call grab_cube()
    4. call place_cube() using the set parameters
    5. Log the info "{task} TASK COMPLETE"

    For the block_color_callback method:
        store the block color in the self.block_color attribute
    '''
    
    def digital_input_callback(self, msg):
        # CHECK DI[0] is HIGH
        
            # CHECK BLOCK'S COLOR
            
            # ASSIGN PARAMETERS

            # CALL MOVE METHODS
        

    def block_color_callback(self, msg):
        # STORE BLOCK'S COLOR

######################### END ##################################################################
################################################################################################


    def grab_cube(self):
        self.get_logger().info(f"RUNNING: GRAB CUBE")

        # Move to HOME
        self.get_logger().info(f"...Moving to HOME...")
        self.moveit2.move_to_configuration(self.MoveJ_Home)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Move to approach
        self.get_logger().info(f"...Moving to Conveyor...")
        self.moveit2.move_to_configuration(self.MoveJ_Appr_Conv)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Move to cube
        self.get_logger().info(f"...Picking cube...")
        self.moveit2.move_to_configuration(self.MoveJ_Pick_Cube)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Close gripper
        self.get_logger().info(f"...Closing GRIPPER...")
        self.close_gripper()
        time.sleep(0.2)

        # Move to HOME
        self.get_logger().info(f"...Moving to HOME...")
        self.moveit2.move_to_configuration(self.MoveJ_Home)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)


    def place_cube(self, task, approach, place, exit):
        self.get_logger().info(f"RUNNING: PLACE {task}")

        # Move from conveyor
        self.moveit2.move_to_configuration(self.MoveJ_Exit_Conv)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Move to bin
        self.get_logger().info(f"...Placing in {task} bin...")
        self.moveit2.move_to_configuration(approach)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        self.moveit2.move_to_configuration(place)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Open gripper
        self.get_logger().info(f"...Opening GRIPPER...")
        self.open_gripper()
        time.sleep(0.2)

        self.moveit2.move_to_configuration(exit)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Move to HOME
        self.get_logger().info(f"...Moving to HOME...")
        self.moveit2.move_to_configuration(self.MoveJ_Home)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)
        

    def io_set_service(self, gripper_request):
        # CALL THE SERVICE
        future = self.grip_client.call_async(gripper_request)
        while rclpy.ok():
            rclpy.spin_once(self.io_node)
            if future.done():
                break


################################################################################################
######################### MODIFY HERE ##########################################################
    '''
    Use the /SetIO service type to send output signals to control the gripper
    self.grip is your service request handler to store the request in (see line 54 for reference)
    for each function:
    1. Set the request pin
        - TO[0] opens the gripper, TO[1] closes the gripper
        - Use the service type's constants defined in the /SetIO service type (see line 55 for reference)
    2. Set the request state to on
        - Use the service type's constants defined in the /SetIO service type
        - convert the constant to a float before storing the value
    3. Call the self.io_set_service() service method (defined above) to set the output
    4. Call self.io_rate.sleep() to add a short delay
    5. Repeat steps 2-4 to set the output to off
    '''

    def close_gripper(self):
        # SET TO[1] to HIGH

        # CALL THE SERVICE
        
        # DELAY
       
        # THEN SET TO[1] to LOW

        # CALL THE SERVICE

        # DELAY


    def open_gripper(self):
        # SET TO[0] to HIGH
        
        # CALL THE SERVICE

        # DELAY

        # THEN SET TO[0] to LOW

        # CALL THE SERVICE

        # DELAY

######################### END ##################################################################
################################################################################################


def main(args=None):
    rclpy.init(args=args)

    ur3eMoveClass = ur3e_commander_class()

    rclpy.spin(ur3eMoveClass)
    
    ur3eMoveClass.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
