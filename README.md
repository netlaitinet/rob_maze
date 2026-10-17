# explainer here

this became 3 projects.
change branch to see them all
```
git switch strafe
git switch arduino
git switch XY
```

# compile instructions

	Only dependency is nix develop,
	just run ./initial-setup.sh and then ./run.sh
	
	If you cant use nix, then:
	Alternatively follow generic ubuntu+webots instructions.
	the project normally runs inside a ubuntu container.
	the compile command is 'make -C "/home/$USER/rob_maze/controllers/e-puck"'
	The project will work as any normal webots project should, the directory structure is identical to the default convention.

	If you are on windows, you probably cant compile due to missing math libraries. 
	I cant really help with that since my computer is so old it cannot run windows.
	
	everything is verified to work on both gentoo and nix os.

# constraints, these are just for fun.

Allowed to *only use infrared* sensors, and the *code must compile for an arduino*.

disallowed: encoders, gyro, compass, IMU, and other special hardware.
disallowed: higher level languages and big libraries.

Allowed to use perfect maze.


# plan and goal

1. Write generic functions in C, and then implement a main file for both e-puck and arduino that call these generic functions.

2. implement a history to keep track of actions, and do math against the history to derive current position in a simulated maze in the robots mind.

3. do math to answer whether we have completed the maze, and whether a shortest path can be calculated.

# design history

copy only the libraries and boilerplate from the e-puck C example, so the simulation has access to IR and wheels only, and can be compiled and started.

refactor the source code files in a way that an arduino code would be written.
The refactor abstracts all the robots internal logic into files that an arduino can consume aswell. theres no higher view of the simulation in the base logic, the simulation is hidden in runtime.c, and hardware folded into hardware.c

The next steps all happen with only the robots internal perspective without a god mode view in order to make final refactor into an arduino version of the same code as streamlined as possible in advance.

1.1 run forward until collision.
1.2 stop before collision
1.3 spin in place 3 seconds (later narrowed down to 2.98) to find mapping of time and full 360 spin.
1.4 use the values from 1.3 to design a 90 turn.
1.5 align the sensor ir[5] to face the wall with step 1.4.

1.5.1 start implementing different algorithms to keep steady distance to the chosen/left wall.

2.0 drop the robot in the maze misaligned so its != 90 degrees to anything useful.
2.1 create small bootstrap() function to get closer and then continuous spin-scan to catch a good reading from ir[5] and conclude a wall can be now hugged.
2.2 create hugging function that can hug either left with ir[5] or right with ir[2] (later just defaulted to always hug with ir[5], but kept the original logic untouched.)

3.0 start designing advanced logic for the robot to integrate its current situation by saving 32 last ir values to a ringbuffer, (later also archive 32 last wheel speeds into a similar ringbuffer.) to benefit in any hard situation that the robot runs into.

also chose to save important reusable ephemereal data in baton.c, and at the very end of main loop command the wheels only once with what was the last thing recorded in baton.
later baton grew to hold variables and locks and so forth and became the central place for a between ticks global state.

3.0.1 situations; left wall gets closer/further, left wall disappears, head-on wall appears, inspecting ringbuffer shows suspicion, and must be acted upon before the worrying situation manifests.

3.0.2 implementation of simulated compass. the robot can evaluate its situation to be "safe" for clamping, and then clamp its simulated compass direction into one of 4 cardinals. the compass uses turns between 0.0 and 1.0 as its unit, and .0,.25,.5,.75 as its cardinal directions.
small wheel slips and collisions should be recoverable from clamping against a wall as a ground truth.

3.0.2.0 the compass will later be used to have more advanced reasoning the other functions can benefit from.

4.0 replaced the conventional wall distance following with "strafing" where the robot tries to achieve parallel orientation to its hugging wall. instead of adjusting towards a sine wave, it tries to compress the wavyness into a single correction move s_manouver() that "departs" from the bad course towards the direction towards center line called INTIMACY. When it departs, it saves its previous trajectory heading, and after a dynamically calculated amount of departing, it flips to "arriving" and takes a sharp contraturn that erases the overshoot resulting to placement that is almost parallel to the original, but closer to the center line, and with less directional error.

4.0.1 all of the stuff from 4.0 is simply done by looking at the custom built "instuments" and adjusting state markers and wheel speeds by simple arithmetic, like reusing a detected errors value as the input to the correcting function, or comparing the reading of ir[5] to all the previous readings in the ringbuffer in case a situation is confusing and needs extra processing.

4.0.2 implementation of rising/falling ir readings graphing function to start predicting misalignment in advance. the detection algorithm can be reused to catch spikes too, and returns a +- integer to tell which way and how many consecutive matches.

4.1 once strafing basics was implemented, added a boosting logic that starts accelerating the appropriate wheel to at the same time preadjust against a possible drift, and cause speed increase when there is only slight worry, and kill the boost when worry grows. now the robot can run parallel to a wall at average 4.6 and spikes of max 6.28 speed. with boost == 0.0 the robot is constrained to 3.0 speed to avoid slipping.

# the base logic of general traversal is now in place, but the edge cases are various and hard to catch.

5.0 a lot of fine tuning of strafing variables was done to make it more reliable.

5.1 edge case catching special functions was started to add on top of the base "behaviour"

Notoriously bad edge case was a 180 degree U-turn around a thin wall. Fixing it needed various little logic functions that narrowed the decision logic further down until it finally worked every time. example: did left wall disappear, did we drift, or did we turn away from it, and during the tip of the turn, did we already pass the brief tick of catching an ir reading from the walls tip, Or are we still expecting it?

it also had its worst form in spiralling question mark shaped part of the maze:
```
   __
  |  |__

```

# Currently the robot can visit all cells of the maze without an error.

6.0 now its time to start building an internal map of the maze based on the tools from earlier steps.

6.1 adding a X/Y estimate is as easy as taking the sin/cos of orientation of last tick, and multiplying it by average wheel value. The robot can now estimate its orientation, and how much it has spatially moved away from the starting point in a X/Y cardinals.

6.2 im planning to now do all the "advanced" stuff like shortest path algorithm starting from here.

# this is where everything starts falling apart.

somehow the robots self calculated compass gets completely spun over by errors big as 90 degrees and clamps wrong every time after especially restless maze partitions.

The soft compass is useful short term, yet completely unreliable in long term.

# The reason for the error turned out to be my accelerating strafing function.

When the robot aggressively adjusts its wheels to accelerate and avoid collision at the same time, the wheels are in a constant state of *drifting* as in car racing. the code is fast and complicated enough to keep full control of the vehicle for extended periods of quick continuous adjustment.

This in itself is impressive in my opinion, and while it looks fun, the amount of continuous micro slipping causes the compass to gradually lose its heading and clamp a wrong cardinal if enough drifting has occured between the previous and the next clamp.

6.3 added smartwheels() function to limit overly drastic change to wheels requested by the other less gentle functions.

# without a reliable long term compass, the long term X/Y map also loses any meaning.

At this point im considering to not fix it, and give up instead. I think i have already done enough work to go well beyond the minimum for the course.

Also the strafing function is a *work of art* and would feel bad to omit.


7.0 flashing the code on an arduino mega by making the e-puck.c into e-puck.ino and changing the motor and ir handles to match arduino. The code compiles and fits. The variables used dont match exatcly, but by just finetuning the variables again would result in the same behaviour, since none of the code is different, only the familiar sensor and motor values are different between them.

8.0 Replacing stafe.c with no_strafe.c that achieves generic sine wave shaped wall following with a bit of smoothing and runs very slow to be sure not to ever slip.

8.1 With this more boring no_strafe the orientation and location estimation can be retried.

8.2 The robot visits every cell of the maze with so little slipping that the compass self-heals it all to 0.

8.3 simple X/Y coordinate system where wheel spinning at 3.0 for 1 second measures a unit.

8.4 taking sin() and cos() of the compass' orientation and multiplying by last turns wheel speed average yields in approximate X and Y delta, that can be collected to a "current position estimate" in X,Y.

8.5 pythagorean of the position estimate can now roughly answer the question: "are we near the opposite corner, and is the pythagorean value decreasing" and if so, conclude that the exact corner has been surpassed.

9.0 Collect a sample of X,Y and put them in a stack.

9.1 If the sample matches previous sample, pop it.

9.2 once the corner is reached, the stack can be inspected to reveal a backwards path but with every dead-end filtered out already, and thus in theory; the shortest path.

# I wish it was that easy.

Since my maze isnt exactly fit for the robots dimension, the robot can drive a donut inside a single cell, the X,Y coordinate will always be different during exploring and returning.

Im not sure if a maze even counts as perfect if it has big empty areas, but that is an unrelated discussion.

9.3 approximate the next best middle point to be around 25 units to the right of the robot.

9.4 generalize the stacking algorithm to collect 2 sizes side by side. 8x8 and 256x256.

9.5 the 8x8 is coarse enough that a location will not be sidestepped like in the bigger one.

9.6 the 8x8 resulting structure with the knots already discarded can now be intersected with the 256x256 to reveal the positions that were visited, but were not part of a backtracked neighbourhood block.

9.6 since the creating and collecting of the stacks can be made any arbitrary size and amount, eventially even this crude technique can become basis for increasingly better estimate the more computing time and space is given to collect and calculate different dimensionality grids and intersecting them together in more advanced ways. (like dynamically combining blocks from different grids, applying transformations to offset grids, create a cloud of offset grids, and readjust offsets in runtime for instance.) 

9.6.1 The basic technique was tested with various grid sizes and resulted in the robot finding the exact opposite corner, stopping and saving a txt file that describes the path it took, along with a suggested shorter path next to it.

Generally the blockier grid size, the more greedily it discards. The system is not tuned to match the dimensions of my maze.
Nothin here is finetuned to match anything perfectly, and im trusting the self-correcting mechanisms to carry all the weight instead.

A unrelated python script was used to convert the robots answer to a visual ASCII drawing as part of the developer tooling. The python never ran on the robot, and was purely a visual side effect to improve ergonomics. it had no effect on the robot whatsoever.

10.0 redesigned the grid/stack/mapping in grid.c, made it more modular and added many improvements. It can now add a safety margin around the cells, prevent popping too quickly, adjust the margins and robots "fatness" to target the center line better. By small improvements, It actually produced the "perfect" shortest path eventually.

This was suprisingly hard to do in a maze that is "too loose". counterintuitively having a tighter maze is a constraint that allows for fewer mistakes.

The path the robot draws is its own real center path instead of the "fattened" path used to intersect the shortest path. The maze is so big that the robots ir sensors become immediately useless trying to travel at the center line, and the constraints I set for myself made this last step especially hard regarding this weird side effect. Even though the maze is perfect, it doesnt "look or behave like a perfect maze" from the robots perspective.

The robot needs to do *a lot* of extra "thinking" to make sense of the weirdly stretching and spinning representation of the surroundings. The amount of total combined error it can recover from, and still produce a detailed correct answer at the end is something that even made me suprised. I assumed to somewhat succeed, but not this perfectly.

# coclusion

It proves the whole chain of techniques will work in concert to achieve the goal end to end.

I'll conclude the experiment here as success, and i learned a lot having to implement every low-level detail myself from scratch.

It was a huge undertaking, but I specially wanted to learn (C and microcontrollers), so i was highly motivated at every step of the process.

# Its not the most "conventional" solution, but It achieves what i wanted it to, and within all the constraints.

I essentially reinvented everything from scratch. I have found this a good way to really internalize any problem and know what its really about, instead of just borrowing a library thats the most commonly used, and calling it done.

The robot has an internal representation of the shape of the maze. And it can *answer the question* "is there a shorter path" by describing a path that is shorter than the path the robot arrived via.

The arduino still runs the strafing version of the code only. The code fits on the cheapest model of arduino. The total 8 ir sensors were put in the code as is, but in reality only sensors 5,6,7 and 0 got any active use. The XY version of the code would take more memory than fits on the arduino, but since i havent built an H-bridge arduino car with atleast 4 ir sensors, and accupied a room for its maze, It doesnt really make sense to adjust the code for it before the physical. What i would do is convert all floating points used throughout the code to uint_8 and scale appropriately to get the maximum accuracy from its range of 256. Then save the big grid in smaller detail, and tune the constants like turn time with its wheels to something else than 3 seconds, adjust the INTIMACY value, and relateds to match its ir sensors measuring power, and thats about it.


# final todos before submitment
prepare for video and presenteation, fill diary and check course page.
