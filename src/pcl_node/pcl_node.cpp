/* 
  This code transforms the pointcloud from camera frame to the 
  base link frame. 
*/

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/PointCloud2.h"

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_ros/transforms.h>

class PCD_Node{
  public:
    std::string base_link = "panda_link0";
    std::string pcd_topic_in;
    std::string pcd_topic_out;
    std::string pcd_frame;
    bool _pcd_is_known;
    bool _tf_is_known;
    ros::NodeHandle local_nh_;
    ros::NodeHandle global_nh_;
    ros::Subscriber pcd_sub;
    ros::Publisher pcd_pub;
    tf::TransformListener listener;
    tf::StampedTransform transform;
  
    void pcd_callback(const sensor_msgs::PointCloud2::ConstPtr msg)
    {
      pcd_frame = msg -> header.frame_id;
      _pcd_is_known = true;
      if (_tf_is_known!=true){
        ROS_INFO("No transform yet...");
        ROS_INFO_STREAM(pcd_frame);
        return;
      }
      // Create a container for the data.
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGB>);
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr temp_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

      pcl::PCLPointCloud2 pcl_pc2;
      pcl_conversions::toPCL(*msg, pcl_pc2);
      pcl::fromPCLPointCloud2(pcl_pc2,*temp_cloud);

      pcl_ros::transformPointCloud(*temp_cloud, *cloud_transformed, transform);

      sensor_msgs::PointCloud2 cloud_publish;
      pcl::toROSMsg(*cloud_transformed,cloud_publish);
      cloud_publish.header = msg->header;
      cloud_publish.header.frame_id = base_link;

      pcd_pub.publish(cloud_publish);
    };

    PCD_Node(ros::NodeHandle local_nh, ros::NodeHandle global_nh){
      _pcd_is_known = false;
      _tf_is_known = false;
      local_nh_ = local_nh;
      global_nh_ = global_nh;
      ROS_INFO("Trying to subscribe to parameter");
      local_nh_.param("pcd_topic_in", pcd_topic_in, std::string("/camera/depth_registered/points"));
      local_nh_.param("pcd_topic_out", pcd_topic_out, std::string("/camera/depth_registered/points"));
      ROS_INFO_STREAM("pcd_topic_in" << pcd_topic_in);
      ROS_INFO_STREAM("pcd_topic_out" << pcd_topic_out);
      pcd_sub = global_nh_.subscribe(pcd_topic_in, 10, 
                              &PCD_Node::pcd_callback,
                              this);
      pcd_pub = global_nh_.advertise<sensor_msgs::PointCloud2>(
                              pcd_topic_out, 10);
    };

    void runOnce(){
      if (_pcd_is_known!=true) {
        ROS_INFO("No PointCloud yet...");
        return;
      }

      try{
        listener.lookupTransform(base_link,pcd_frame, 
                                  ros::Time(), transform);
        _tf_is_known = true;
      }
      catch (tf::TransformException ex){
          ROS_ERROR("%s",ex.what());
      }
    };

};

int main(int argc, char **argv){
  ros::init(argc,argv, "panda_PCL_node");
  ros::NodeHandle nh1("~");
  ros::NodeHandle nh2("/");
  PCD_Node pcd_node(nh1, nh2);

  ROS_INFO("Starting node...\n");
  ros::Rate loop_rate(10);
  while (ros::ok()){
    pcd_node.runOnce();
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}