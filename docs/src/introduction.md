# Meet Carrie
<img src="images/carrie-cover.JPG"
     alt="Carrie in her former glory in 2023"
     style="max-height:300px;" />

Carrie is the vehicle originally designed for a study on geometric control for parallel parking by Catherine Van West and Jeannette Circe. 

We designed Carrie to have a computer (a Raspberry Pi) and microcontroller (a Pico) on board, and a custom PCB for power conversion from a power supply or a battery, which provides the necessary voltages for all systems. The PCB contains two motor drivers, one connecting to two motors with a differential so that the rear wheels can follow any path without slipping. The second motor driver controls the steering motor. The steering is an Ackermann configuration for stability and so that the vehicle can drive without slipping, with the same kinematics as any car on the road. We control the steering angle by a motor on a set of gears that attach to one side of the front axle. Each of the rear wheels has a quadrature encoder attached so that we can measure both position and direction of the each wheel. The vehicle has a large power supply so that it can operate for long periods of time without needing to recharge or replace the battery. Impact attenuators (pool noodles) are attached so that in the case of a crash, the components will not break. 

The high level geometric control signals are calculated based on the current position and calculated path based on the desired end position and orientation sent by the user. The required steering angle at any point on the path is calculated by the Raspberry Pi and sent to the Pico microcontroller. The Pico contains the low-level control firmware with a PID controller on both the rear motor speed and the front motor steering actuation based on the encoder readings. Carrie is able to travel anywhere in the room along a calculated clothoid trajectory.  

Inside this documentation, you'll find everything we know about Carrie and how you can use her to explore trajectory following controls for steering vehicles! 