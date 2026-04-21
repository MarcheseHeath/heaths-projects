# Import ROS 2 Python client library
import rclpy
from rclpy.node import Node

# Import the standard ROS message type used for publishing text
from std_msgs.msg import String

# Import libraries to interface with the I2C-based color sensor
import board
import busio
import adafruit_tcs34725


# Define a custom ROS node called ColorPublisher
class ColorPublisher(Node):

    def __init__(self):
        # Initialize the ROS node with the name 'color_publisher'
        super().__init__('color_publisher')

        # Create a publisher that will publish RGB data
        self.rgb_publisher = self.create_publisher(String, '/rgb', 10)

        # Try to establish I2C communication with the color sensor
        try:
            self.i2c = busio.I2C(board.SCL, board.SDA)
            self.sensor = adafruit_tcs34725.TCS34725(self.i2c)
        except:
            self.get_logger().error("Could not establish I2C Connection")

        # Set integration time and gain for the color sensor 
        # (tweak for better readings)
        self.sensor.integration_time = 100
        self.sensor.gain = 16

#######################################################################
######################### MODIFY HERE #################################
        '''
        Create the subscriber to send the block's color information
        1. Use a message type that will allow you to distinguish between:
            Red, Green, Blue, or None (no colored block detected)
        2. Assign a unique topic name for the publisher to publish to
            Standard naming convention is snake case (lowercase_underlines)
        '''
        
        self.box_publisher = self.create_publisher(
            MessageType,                         # add correct message type
            '/topic_name',                    # add correct topic name
            10)

######################### END #########################################
#######################################################################
        
        # Create a timer to repeatedly call the callback function
        timer_period = 0.2  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)

    # A helper function to evaluate the color based on RGB values
    def evaulate_color(self, rgb):
        if rgb[2] > 9:
            return "blue"
        elif rgb[0] > 16:
            return "red"
        elif rgb[1] > 21:
            return "green"
        else:
            return "none"

    # This function is called repeatedly by the timer
    def timer_callback(self):
        # Read RGB values from the sensor
        rgb_list = self.sensor.color_rgb_bytes  
        
        # Format the RGB values into a string and publish it
        rgb_msg = String()
        rgb_msg.data = 'RGB: ({0}, {1}, {2})'.format(*rgb_list)
        self.rgb_publisher.publish(rgb_msg)
        self.get_logger().info(rgb_msg.data)
            
#######################################################################
######################### MODIFY HERE #################################
        '''
        Evaluate the block's color and publish it
        1. Create a message of an appropriate type
        2. Store the color in the message created
        3. Publish the message
            - Use the self.box_publisher created earlier
        '''
        # RECEIVES THE BLOCK'S COLOR
        box_color = self.evaluate_color(rgb_list)

        # CREATE MESSAGE
        
        # STORE VALUE(S) IN MESSAGE 
        
        # PUBLISH MESSAGE
        

######################### END #########################################
#######################################################################


# ROS 2 main function
def main(args=None):
    rclpy.init(args=args) 

    color_publisher = ColorPublisher()

    rclpy.spin(color_publisher) 

    # Clean up after the node is stopped
    color_publisher.destroy_node()
    rclpy.shutdown()


# Entry point when the script is run directly
if __name__ == '__main__':
    main()
