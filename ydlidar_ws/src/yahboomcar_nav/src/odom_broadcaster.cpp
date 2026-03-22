#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_datatypes.h>

class OdomBroadcaster {
private:
    ros::NodeHandle nh_;
    ros::Subscriber cmd_vel_sub_;
    tf::TransformBroadcaster broadcaster_;
    
    // 里程计参数
    double x_;
    double y_;
    double th_;
    ros::Time last_time_;
    boost::mutex mutex_; // 添加互斥锁保护共享数据

public:
    OdomBroadcaster() : 
        nh_(),
        x_(0.0),
        y_(0.0),
        th_(0.0)
    {
        cmd_vel_sub_ = nh_.subscribe("cmd_vel", 1, &OdomBroadcaster::cmdVelCallback, this);
        last_time_ = ros::Time::now();
    }

    void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& vel_cmd) {
        boost::mutex::scoped_lock lock(mutex_); // 加锁保护
        
        ros::Time current_time = ros::Time::now();
        double dt = (current_time - last_time_).toSec();
        
        if (dt > 0 && dt < 1.0) { // 添加时间合理性检查
            // 获取线速度和角速度
            double vx = vel_cmd->linear.x;
            double vy = 0.0; // 通常移动机器人只有x方向线速度
            double vth = vel_cmd->angular.z;
            
            // 计算位移
            double delta_x = (vx * cos(th_) - vy * sin(th_)) * dt;
            double delta_y = (vx * sin(th_) + vy * cos(th_)) * dt;
            double delta_th = vth * dt;
            
            // 积分得到当前位置
            x_ += delta_x;
            y_ += delta_y;
            th_ += delta_th;
        }
        
        last_time_ = ros::Time::now(); // 更新时间
    }

    void run() {
        ros::Rate rate(100); // 100Hz
        
        while (nh_.ok()) {
            // 获取当前时间
            ros::Time current_time = ros::Time::now();
            
            // 锁定共享数据以读取
            {
                boost::mutex::scoped_lock lock(mutex_);
                
                // 创建变换
                tf::Transform transform;
                transform.setOrigin(tf::Vector3(x_, y_, 0.0));
                tf::Quaternion q;
                q.setRPY(0, 0, th_);
                transform.setRotation(q);

                // 发布从 odom 到 base_footprint 的变换
                broadcaster_.sendTransform(
                    tf::StampedTransform(
                        transform,
                        current_time,
                        "odom",
                        "base_footprint"
                    )
                );
            }
            
            rate.sleep();
            ros::spinOnce(); // 处理订阅的消息
        }
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "odom_broadcaster");
    
    OdomBroadcaster odom_broadcaster;
    odom_broadcaster.run();
    
    return 0;
}