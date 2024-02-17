// set car down at any steering angle desired initial driving is straight as
// approaches parking spot

// way to track current position ~ always updated with new getCount()
int steering_count = 0;

//steering motor - actuate all the way left go left until count no longer
//updating? something like if cur - prev <= some threshold? or if cur == prev?
//not sure

//measure total count from all the way left to all the way right
int total_count = 0;

//actuate motor all the way right record tickmarks in total_count until motor
//all the way to right

//having this just incase we want to change it later? might never need but might
//be nice 
float inital_position_shifter = 1;

int des_start_pos = total_count/2 * inital_position_shifter;

//first error loop of our inner steering loop wheel is all the way right~
//desired is des_start_pos error = des_start_pos - steering_count;

//actuate steering motor until error = 0

//inital kp for this guessing max error is gonna be total count max actuation is
// 12V so kp = 0.75* total_count/12 ? we can try something like that to start
// with ki can be 1/10 kp... 



