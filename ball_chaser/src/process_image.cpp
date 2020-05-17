#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client; // Define a global client that can request services

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
 
    ROS_INFO_STREAM("Requesting the robot to move");

    // 	Request robot to move with linear velocity lin_x and angular velocity ang_z 
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the service
    if (!client.call(srv))
	ROS_ERROR("Failed to call service");	
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255; 
    int pixel_id = 0; 
    int k;
    float vel_lin_x = 0.0;
    float vel_ang_z = 0.0;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    for (int i = 0;i <img.height*img.step; i=i+3){
		if ((img.data[i]==white_pixel) && (img.data[i+1]==white_pixel) && (img.data[i+2]==white_pixel)) // Compare R, G and B channels with white pixel
		{
			k = (pixel_id % img.width);

			if ((k > 0 ) && (k <= (img.width/3))){  // Check if pixel is in left part of image
			vel_lin_x = 0;
			vel_ang_z = 0.5;
			 // Move left
			}		
			else if (k > (img.width/3) && (k <= (2*img.width/3))){  // Check if pixel is in middle part of image
			vel_lin_x = 0.5;
			vel_ang_z = 0;
			 // Move forward
			}
			else if (k > (2*img.width/3) && (k < img.width)){    // Check if pixel is in the right part of the image
			vel_lin_x = 0.0;
			vel_ang_z = -0.5;
			// Move right
			}
		break;
		}
	pixel_id = pixel_id + 1;  // Increment pixel number
    }
    drive_robot(vel_lin_x,vel_ang_z);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
