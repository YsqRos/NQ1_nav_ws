#include <ros/ros.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char** argv){
    ros::init(argc, argv, "base_link_broadcaster");
    ros::NodeHandle n;

    ros::Rate r(100); // 100Hz

    tf::TransformBroadcaster broadcaster;

    while(n.ok()){
        // 发布从 base_footprint 到 base_link 的变换
        broadcaster.sendTransform(
            tf::StampedTransform(
                tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0, 0, 0)),
                ros::Time::now(),
                "base_footprint",
                "base_link"
            )
        );
        r.sleep();
    }
    return 0;
}