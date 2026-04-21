#! /usr/bin/env python

# Python packages
from threading import Thread
import time

# ROS 2 packages
import rclpy
from rclpy.node import Node
from rclpy.callback_groups import ReentrantCallbackGroup, MutuallyExclusiveCallbackGroup
from rclpy.action import ActionServer, GoalResponse, CancelResponse

# Moveit packages
from pymoveit2 import MoveIt2
from pymoveit2.robots import ur3e as robot
from my_robot_msgs.action import UR3eMove

# Actions, Services, Messages
from ur_msgs.srv import SetIO
from ur_msgs.msg import IOStates


class ur3e_commander_class(Node):

    def __init__(self):
        super().__init__('ur3e_commander_node')

        # Create nodes for executor
        self.moveit_node = Node("ex_pose_goal")
        self.io_node = Node("io_service_client")
        
################################################################################################
######################### MODIFY HERE ##########################################################
        '''
        Create the service client to set outputs on the robot
        1. Use the /SetIO service type
        2. Set the service name to /io_and_status_controller/set_io
        '''

        # CREATE DIGITAL I/O SERVICE CLIENT
        self.grip_client = self.io_node.create_client(
        ServiceType,                                                # add correct service type
        '/service_name',                                             # add correct service name
        callback_group=MutuallyExclusiveCallbackGroup())
        
######################### END ##################################################################
################################################################################################
        
        # Wait for service server to be available
        while not self.grip_client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('/set_io service not available, waiting again...')
        self.io_rate = self.io_node.create_rate(20)
        # Initialize service call request message
        self.grip = SetIO.Request()
        self.grip.fun = self.grip.FUN_SET_DIGITAL_OUT

        # UR3eMove ACTION SERVER
        self._action_server = ActionServer(self, UR3eMove, 'ur3e_action_move', execute_callback=self.execute_callback, goal_callback=self.goal_callback, cancel_callback=self.cancel_callback, callback_group=MutuallyExclusiveCallbackGroup()) 
        self.abort = False
        self.action_rate = self.create_rate(10)

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
        self.moveit_node.declare_parameter("HOME", [-1.5456875164857884, 1.2788336912738245, 0.2793256479450683, 1.129761815071106, 1.6041396856307983, 2.657310962677002])
        
        ### DECLARE JOINT POSITIONS FOR PICKING UP CUBE ###
        self.moveit_node.declare_parameter("To_Cube", [-1.410846785908081, 1.2792399565326136, 0.0948754984089355, 0.39626285433769226, 1.604156255722046, 1.8850505352020264])
        self.moveit_node.declare_parameter("Appr_Cube", [-1.1623516243747254, 2.020742718373434, -0.8231016558459778, 0.11600375175476074, 1.5825164318084717, 1.6626924276351929])
        self.moveit_node.declare_parameter("Exit_Cube", [-1.410846785908081, 1.2792399565326136, 0.0948754984089355, 0.39626285433769226, 1.604156255722046, 1.8850505352020264])
        
        ### DECLARE JOINT POSITIONS FOR PLACING CUBE ###
        self.moveit_node.declare_parameter("To_Conv", [-1.3073140543750306, 1.8860819975482386, -0.5437562030604859, 1.014665126800537, 1.6138544082641602, 2.627551555633545])
        self.moveit_node.declare_parameter("Place_Cube", [-1.0400911134532471, 2.0200985113727015, -0.9448105853847046, 1.015715479850769, 1.6134867668151855, 2.626913070678711])
        self.moveit_node.declare_parameter("Exit_Conv", [-1.3073140543750306, 1.8860819975482386, -0.5437562030604859, 1.014665126800537, 1.6138544082641602, 2.627551555633545])

        ### DECLARE JOINT POSITIONS FOR BLOCK STACK
        self.moveit_node.declare_parameter("Block1", [-0.6569996637156983, 1.6319029966937464, -0.9653258484653016, 0.4863949716091156, 1.6119602918624878, 2.0311436653137207])
        self.moveit_node.declare_parameter("Block2", [-0.8210097712329407, 1.6346057097064417, -0.8045878571322937, 0.48591166734695435, 1.6125582456588745, 2.03155779838562])
        self.moveit_node.declare_parameter("Block3", [-0.9596116703799744, 1.5872882048236292, -0.6189918679049988, 0.4852601885795593, 1.612962245941162, 2.0318589210510254])                                           
        self.moveit_node.declare_parameter("Block4", [-1.0651090902141114, 1.4897330443011683, -0.41620786607775884, 0.4845596253871918, 1.6131761074066162, 2.0320630073547363])
        
        self.MoveJ_Home = (self.moveit_node.get_parameter("HOME").get_parameter_value().double_array_value)
        self.MoveJ_To_Cube = (self.moveit_node.get_parameter("To_Cube").get_parameter_value().double_array_value)
        self.MoveJ_Appr_Cube = (self.moveit_node.get_parameter("Appr_Cube").get_parameter_value().double_array_value)
        self.MoveJ_Exit_Cube = (self.moveit_node.get_parameter("Exit_Cube").get_parameter_value().double_array_value)
        self.MoveJ_To_Conv = (self.moveit_node.get_parameter("To_Conv").get_parameter_value().double_array_value)
        self.MoveJ_Place_Cube = (self.moveit_node.get_parameter("Place_Cube").get_parameter_value().double_array_value)
        self.MoveJ_Exit_Conv = (self.moveit_node.get_parameter("Exit_Conv").get_parameter_value().double_array_value)
        self.MoveJ_Blocks = [(self.moveit_node.get_parameter("Block1").get_parameter_value().double_array_value),
                            (self.moveit_node.get_parameter("Block2").get_parameter_value().double_array_value),
                            (self.moveit_node.get_parameter("Block3").get_parameter_value().double_array_value),
                            (self.moveit_node.get_parameter("Block4").get_parameter_value().double_array_value)]
        
        self.open_gripper()
        self.get_logger().info("Node Initialized")  
        
        
    def goal_callback(self, goal_object):
        self.get_logger().info('Goal Request Recieved')
        return GoalResponse.ACCEPT
            
    
    def cancel_callback(self, goal_object):
        self.get_logger().info('Goal Cancelled. Completing movement, then aborting')
        self.abort = True
        return CancelResponse.ACCEPT
        

    def execute_callback(self, goal_object):
        if goal_object.request.task == 'grab_cube':
            self.pick_place_cube(goal_object)    
            
        elif goal_object.request.task == 'grab_cube_stack':
            self.destack_sequence(goal_object)

        result = UR3eMove.Result()
        result.completed = f'{goal_object.request.task} COMPLETE'
        self.get_logger().info(f"READY FOR NEXT GOAL")
        return result


################################################################################################
######################### MODIFY HERE ##########################################################
    '''
    This function performs a destacking sequence by picking and placing blocks from a stack
    1. Start with the top block in a stack of 4 blocks
        - Create a counter variable that starts at 3 and goes down to zero
    2. While there are blocks remaining:
       a. Call self.pick_place_cube() to pick and place the current block
            - Pass the {goal_object} and {block_pos} parameters
       b. Wait for 7 seconds to allow the placement to complete
       c. Decrease the counter to move to the next block down
    3. After all blocks are placed, return goal_object.succeed() to indicate success
    '''

    def destack_sequence(self, goal_object):
        # INITIALIZE COUNTER
        
        # LOOP THROUGH BLOCK POSITIONS
        
            # CALL MOVEMENT METHOD
        
            # DECREMENT COUNTER AND WAIT
        
        # RETURN SUCCESS
            
######################### END ##################################################################
################################################################################################


    def pick_place_cube(self, goal_object, block_pos=0):
        feedback_msg = UR3eMove.Feedback()
        feedback_msg.status = f'Starting Pick & Place on block {block_pos+1}...'
        goal_object.publish_feedback(feedback_msg)
        
        ### Commands to move the robot to the specified poses ###
        ### Think of this section as the program tree on PolyScope ###

        # Move to joint configuration1
        feedback_msg.status = 'Moving to HOME...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_Home)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Open gripper
        feedback_msg.status = 'Opening gripper...'
        goal_object.publish_feedback(feedback_msg)
        self.open_gripper()
        time.sleep(0.2)

        # Move to approach
        feedback_msg.status = 'Approaching block...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_To_Cube)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)
        
        self.moveit2.move_to_configuration(self.MoveJ_Appr_Cube)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Pick up block
        feedback_msg.status = 'Picking up block...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_Blocks[block_pos])
        self.moveit2.wait_until_executed()
        time.sleep(0.1)
        
        # Close gripper
        feedback_msg.status = 'Closing gripper...'
        goal_object.publish_feedback(feedback_msg)
        self.close_gripper()
        time.sleep(0.2)
        
        # Move away from pickup
        feedback_msg.status = 'Leaving block...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_Exit_Cube)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        feedback_msg.status = 'Moving to HOME...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_Home)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Move to conveyor
        feedback_msg.status = 'Approaching conveyor...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_To_Conv)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)
        
        self.moveit2.move_to_configuration(self.MoveJ_Place_Cube)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)
        
        # Open gripper
        feedback_msg.status = 'Opening gripper...'
        goal_object.publish_feedback(feedback_msg)
        self.open_gripper()
        time.sleep(0.2)
        
        # Move away from conveyor
        feedback_msg.status = 'Leaving conveyor...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_Exit_Conv)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        # Move to return_home
        feedback_msg.status = 'Moving to HOME...'
        goal_object.publish_feedback(feedback_msg)
        self.moveit2.move_to_configuration(self.MoveJ_Home)
        self.moveit2.wait_until_executed()
        time.sleep(0.1)

        feedback_msg.status = f'Pick & Place on block {block_pos+1} COMPLETE'
        goal_object.publish_feedback(feedback_msg)
        
        if self.abort:
            goal_object.canceled()
            self.abort = False
            self.get_logger().info("GOAL ABORTED")
            return


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
    
    rclpy.get_logger().info(f"ALL DONE")
    ur3eMoveClass.destroy_node()
    rclpy.shutdown()
    

if __name__ == "__main__":
    main()
