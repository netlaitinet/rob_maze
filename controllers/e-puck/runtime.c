#include <webots/robot.h>
#include "hardware.h"
#include "baton.h"
#include "runtime.h"
#include "history.h"
#include "grid.h"

// this is an adapter to make the webots simulation use a arduino-style code
// structure.

// hide webots only specific boring boilerplate in here.

int main(void) {
  wb_robot_init();
  hardware_init();

  const int step = (int)wb_robot_get_basic_time_step();

  // prevent empty history
  if (wb_robot_step(step) != -1) {
    read_ir(baton.ir);
    for (int i = 0; i < 32; ++i) {
      history_push();
    }
    // save space in the important files by putting the *single use initial direction* here.
    baton.left=3.0;
    baton.right=3.0;
    set_wheels(baton.left, baton.right);
    while (wb_robot_step(step) != -1) {
      loop();
    }
  }

  wb_robot_cleanup();
  return 0;
}
