#include <ros/ros.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char** argv){
    ros::init(argc, argv, "laser_broadcaster");
    ros::NodeHandle node;

    tf::TransformBroadcaster broadcaster;

    ros::Rate rate(30); // 30Hz

    while(node.ok()){
        // 创建从base_link到laser的变换
        // laser在base_link正上方20cm处 (0.2m)
        tf::Transform transform;
        transform.setOrigin(tf::Vector3(0.0, 0.0, 0.2)); // x=0, y=0, z=0.2
        tf::Quaternion q;
        q.setRPY(0, 0, 0); // 无旋转
        transform.setRotation(q);

        // 发布变换: child_frame_id="laser", parent_frame_id="base_link"
        broadcaster.sendTransform(tf::StampedTransform(
            transform, 
            ros::Time::now(), 
            "base_link", 
            "laser"));

        rate.sleep();
    }

    return 0;
}