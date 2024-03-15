#include <rockrobo_ros_bridge/xiaomi_topic_handler.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "rockrobo_ros_bridge_node");
  ros::NodeHandle nh("~");

  XiaomiTopicHandler *handler = new XiaomiTopicHandler(nh);
  handler->run();

  delete handler;
  return 0;
}
