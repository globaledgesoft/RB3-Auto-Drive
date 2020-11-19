import time
import rospy
from geometry_msgs.msg import Twist

def sendMsg(linear_x,angular_z,t):                                                    
    pub = rospy.Publisher('/cmd_vel', Twist, queue_size=10)                                      
    twist = Twist()                                                         
    twist.linear.x=linear_x                                                                       
    twist.angular.z = angular_z                                                                
    pub.publish(twist)                                                                            
    time.sleep(t)                                                           
    # print("first message executed")                                                          
    twist.linear.x=0                                                                         
    twist.angular.z = 0                                                                        
    pub.publish(twist)                                                                
    # print("message published successfully")                                                  
                                                                    
def Connection():                                                                            
    sendMsg(0,0,0.3) 


rospy.init_node('turtlebot_move')                                                        
Connection()
