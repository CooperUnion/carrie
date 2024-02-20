# Kinematic Model
<img src="../images/model.png"
     alt="Kinematic Model"
     style="max-height:300px;" />

While writing the model and controls for Carrie, we specify the curvature of the path. To calculate the required steering angle, we use the bicycle model for front wheel steering vehicles. Because the vehicle is a rigid body, we can assume the entire vehicle has the same angular velocity around a a point of rotation with a radius $R$. The curvature $\kappa$ is calculated using equation below based on based on the steering angle $\delta$ and the wheel base $L$. Rearranging this equation yields the steering angle based on the desired curvature.

$\kappa = \frac{1}{R} = \frac{tan(\delta)}{L}$

Since we cannot directly measure the steering angle of the vehicle, we can use the same model to calculate the curvature the vehicle is traveling at based on the measured velocities of the rear wheels. The radius of curvature is calculated based on the velocities and half the track width $t$.

$R = \frac{t}{2} \left ( \frac{v_{R}+v_{L}}{v_{R}-v_{L}} \right )$