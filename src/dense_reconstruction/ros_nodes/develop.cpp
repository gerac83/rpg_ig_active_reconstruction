/* Copyright (c) 2015, Stefan Isler, islerstefan@bluewin.ch
*
This file is part of dense_reconstruction, a ROS package for...well,

dense_reconstruction is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
dense_reconstruction is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License
along with dense_reconstruction. If not, see <http://www.gnu.org/licenses/>.
*/

 
#include "dense_reconstruction/youbot_planning.h"
#include "dense_reconstruction/robot_planning_interface.h"
#include "boost/foreach.hpp"

#include <random>

using namespace std;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "youbot_reconstruction_controller");
  ros::NodeHandle n;
    
  dense_reconstruction::YoubotPlanner youbot(&n);
  youbot.initializePlanningFrame();
  
  dense_reconstruction::RobotPlanningInterface::PlanningSpaceInitializationInfo simple_setup;
  boost::shared_ptr<dense_reconstruction::YoubotPlanner::SpaceInfo> specifics( new dense_reconstruction::YoubotPlanner::SpaceInfo() );
  
  specifics->approximate_relative_object_position_ << 1,0,0;
  specifics->base_pts_per_circle_ = 2;
  specifics->arm_min_view_distance_ = 0.3;
  specifics->arm_view_resolution_ = 100; // [pts/m]
  
  simple_setup.setSpecifics(specifics);
  
  youbot.initializePlanningSpace(simple_setup);
  
  dense_reconstruction::ViewSpace view_space;
  youbot.getPlanningSpace( &view_space );
    
  dense_reconstruction::View current_view = youbot.getCurrentView();
  
  cout<<"current view:"<<endl;
  cout<<current_view;
  
  
  using namespace dense_reconstruction;
  
  dense_reconstruction::View closest = view_space.getAClosestNeighbour(current_view);
  
  /*
  boost::shared_ptr<dense_reconstruction::View::ViewInfo> info_reference = closest.associatedData();
  boost::shared_ptr<YoubotPlanner::ViewInfo> info = boost::dynamic_pointer_cast<YoubotPlanner::ViewInfo>(info_reference);
  YoubotPlanner::ViewPointData* data = info->getViewPointData();
  ROS_ERROR_STREAM("data in main link1 angle is: "<<data->link1_config_.angle_);*/
  
  youbot.moveTo(closest);  
  youbot.retrieveData();
  
  
  ROS_INFO("If the program terminates now it has reached the correct exit point");
  return 0;
  
  
  
  
  
  
  std::vector< Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > joint_values;
  std::vector< Eigen::Vector2d, Eigen::aligned_allocator<Eigen::Vector2d> > grid;
  youbot.calculateArmGrid( 60, 60, joint_values, &grid );
  
  std::ofstream out("/home/stewess/Documents/youbot_arm_grid_50pts_per_m.txt", std::ofstream::trunc);
  BOOST_FOREACH( auto point, grid )
  {
    out<<point(0)<<" "<<point(1)<<"\n";
  }
  out.close();
  
  std::ofstream out2("/home/stewess/Documents/youbot_arm_joint_values_50pts_per_m.txt", std::ofstream::trunc);
  BOOST_FOREACH( auto value, joint_values )
  {
    out2<<value(0)<<" "<<value(1)<<" "<<value(2)<<"\n";
  }
  out2.close();
  
  return 0;
} 
