# Overview 
In the Spring of 2023, under the supervision of Professor Shlayan, Cat and Jeannette undertook the task of bringing Carrie to life. Our research goal was to create a vehicle capable of parallel parking, no easy feat. When Carrie first came to us, she was an elegant mechanical design, but required a brain and power distribution to all of her components. 

## Research Background
When designing vehicle controls for parallel parking, there are two problems to solve: creating the trajectory and calculating the steering angle. 

First, the trajectory of the parallel parking manuever. Since the typical manuever involves steering at the maximum angle both ways while moving backwards, the simplest trajectory is the combination of two circular paths [[1]]. Due to the sudden change in steering angle, the vehicle must stop midway through the manuever so that it is not thrown off the path [[2]]. This stop may be avoided if instead a more continuous path is chosen as the trajectory [[3]]. 

Continuous paths for parallel parking include polynomials [[4]], b-spline curves [[5]], and clothoid curves [[3],[6]]. Clothoid curves [[7]] are curves with linearly changing curvature. They are used to design railways [[8]] and for vehicle trajectories because their curvature changes continuously, allowing the vehicle to follow said trajectory without stopping. Some have claimed clothoid curves are too computationally intensive [[9]] but they are still one of the most desireable trajectories for this type of vehicle, especially in parallel parking.

The second problem is designing the controller for steering angle based on the desired trajectory of the vehicle. For parallel parking specifically, many have employed fuzzy controllers [[10], [11], [12]]. Many path following controllers have been developed for autonomous vehicles [[13]]. For geometric controllers, once the path is generated, the error (and eventual control signal) is based on the closest point along the path and the vehicles position and orientation relative to the path. 

For parallel parking, various geometric techniques have been used: a look-ahead yaw error [[14]], and pure pursuit [[15]]. To our knowledge, Stanley control [[16]] has not been tested for parallel parking. Stanley geometric control is based on a combination of the heading (yaw) error and the cross track error (the distance the vehicle is from the path tangential to the path).

[1]:(https://www.researchgate.net/publication/328798507_Automatic_Parallel_Parking_Algorithm_for_a_Carlike_Robot_using_Fuzzy_PDI_Control)
[2]:(https://ieeexplore.ieee.org/document/620143)
[3]:(https://ieeexplore.ieee.org/document/6548775)
[4]:(https://ieeexplore.ieee.org/document/6395363)
[5]:(https://journals.sagepub.com/doi/full/10.1177/0020294020944961)
[6]:(https://ieeexplore.ieee.org/document/7962257)
[7]:(https://pwayblog.com/2016/07/03/the-clothoid/)
[8]:(https://archive.org/details/railwaytransiti02talbgoog/)
[9]:(https://ieeexplore.ieee.org/document/9669069)
[10]:(https://ieeexplore.ieee.org/document/5945221)
[11]:(https://ieeexplore.ieee.org/document/1548645)
[12]:(https://www.researchgate.net/publication/224350298_Development_of_an_automatic_parking_system_for_vehicle)
[13]:(https://ieeexplore.ieee.org/document/8191998)
[14]:(https://ieeexplore.ieee.org/document/6704211)
[15]:(https://ieeexplore.ieee.org/document/4677655)
[16]:(https://ieeexplore.ieee.org/document/4282788)