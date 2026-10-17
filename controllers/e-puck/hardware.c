#include <math.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>
#include "hardware.h"

// boilerplate that needs to be put *somewhere*
// nothing very interesting here.
// some commonly used and/or ugly looking handles to the robots hardware have been wrapped in order to increase developer ergonomics

static WbDeviceTag left_motor;
static WbDeviceTag right_motor;
static WbDeviceTag proximity_sensors[8];
static const char *proximity_sensor_names[8] = {
  "ps0", "ps1", "ps2", "ps3", "ps4", "ps5", "ps6", "ps7",
};
static double boot_time;

void hardware_init(void) {
  boot_time = wb_robot_get_time();
  const int step = (int)wb_robot_get_basic_time_step();

  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  set_wheels(0.0, 0.0);

  for (int i = 0; i < 8; ++i) {
    proximity_sensors[i] = wb_robot_get_device(proximity_sensor_names[i]);
    wb_distance_sensor_enable(proximity_sensors[i], step);
  }
}

double seconds(void) {
  return wb_robot_get_time() - boot_time;
}

void set_wheels(double left, double right) {
  wb_motor_set_velocity(left_motor, left);
  wb_motor_set_velocity(right_motor, right);
}

void read_ir(int values[8]) {
  for (int i = 0; i < 8; ++i){
    values[i] = (int)round(wb_distance_sensor_get_value(proximity_sensors[i]));
  }
}
