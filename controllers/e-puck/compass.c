// this file in general does the heavy lifting of the robots "navigating without using extra sensors"
// The XY branch uses it in more depth.
#include "compass.h"
#include "orient.h"
#include "baton.h"
#include <math.h>
#include <stdbool.h>

// these track the amount of incremental error in each direction.
static double discard_cw; 
static double discard_ccw;

static const double tau = 2.0 * M_PI; // requires a math library that some operating systems might lack beyond my control. So please test my project with linux, It will work guaranteed.
// TODO add all compilation and run commands.

// normally when called, compares the robots orientation to the example
// directions, and if they are inside the margin, the orientation gets clamped
// to fit the margin, and the leftover gets added to a discard variable. When
// clamping next time, the discard variables are taken in account to not permit
// re-clamping again in the same direction.

// To flexibly recover from a previously accumulated mistake, every rejection also decays the collected discard value. sometimes the compass can be up to 15 degrees slanted for a short moment, but during normal traversal in a balanced maze, will always return back to 0 slant.
// It can be regarded as "self-healing", because the rate of mistake never grows past the rate of decay in any normal maze. Possible exception for some very weird mazes.

// "bool request" is sometimes passed when deliberately clamping from a known
// bad state, for instance the robot spawns weirdly and must orient itself. the
// perfect clamping margins have not been searched, these are "just works enough" values.
void clamp(bool request){
  double cumulative_error = discard_cw - discard_ccw;
  double error_margin = 0.05;
  if (request) error_margin = 0.07;
  if (!oriented) error_margin = 0.25;
  double margin = 0.015;
  double before = baton.heading;
  double cardinal = round(before * 4.0) / 4.0; // creates cardinal example directions
  bool denied = false; 
  if (fabs(cardinal - before) > error_margin) {
    denied = true;
  } else if (fabs(cardinal - before) > margin) { // check for fit in margin, and clamp as old value + margin instead of the new value to clamp.
    if (cardinal < before && cumulative_error <= 0.005)
      baton.heading = cardinal + margin;
    else if (cardinal > before && cumulative_error >= -0.005)
      baton.heading = cardinal - margin;
    else
      denied = true;
  }
  double discard = before - baton.heading; // collect discard amount, and later sort it.
  baton.heading -= floor(baton.heading); // "-m.0" is used to wrap values of over "m.n" back to "0.n"
  if (denied) { // decay.
    discard_cw *= 0.8;
    discard_ccw *= 0.8;
  } else { // sort the discard.
    if (discard > 0) discard_cw = discard_cw * 0.8 + discard;
    else if (discard < 0) discard_ccw = discard_ccw * 0.8 - discard;
  }
}

// normal compass operation each tick where the last wheels effects are calculated into the current heading.
// does not try to correct.
// a heading means 0-1 value representing a full 360 turn in place with 3/-3 wheel speeds which takes 2.980 seconds.  for instance 0.25 means 90 degrees, and (2.98/4)*1 seconds
// the compass is not "based" on anything, its just a incremental "guesser" that uses historical recorded data and new ir readings to guess again. (+ clamping to periodically prune)
// any major wheel slipping will destroy the heading completely in long term.
// this is why the X/Y branch doesnt use strafing at all.
void compass(void) {
  double before = baton.heading;
  double left = baton.left;
  double right = baton.right;
  double l_r_delta = (left - right) / 6.0;
  double heading = before + l_r_delta * 32.0 / 1000.0 / 2.980;
  baton.heading = heading - floor(heading);
}

// this is only useful if the compass is good, so not relevant for the strafing branch.
// this is for the X/Y branch.
// unit of movement = 1 second of 3/3 wheel speed.
// maze side length ~750 units 
void XY_record(){
  double last_speed = (baton.left + baton.right)/6.0;
  double vector = baton.heading * tau;
  baton.X = baton.X+sin(vector) * last_speed;
  baton.Y = baton.Y+cos(vector) * last_speed;
}
