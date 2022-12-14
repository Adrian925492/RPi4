#ifndef IOCTRL_TEST_H
#define IOCTRL_TEST_H

/* Here we define what kind of command line arguments we will be able to use to interract with the kernem module */

struct mystruct {
    int repeat;
    char name[64];
};


//Definitions of commands to be used to communicate bu ioctl.

//Typical structore is: #define <CMD_NAME> <IN/OUT>(<user sace dummy variable name>, <kernel space dummy variable name>, <passed data type>)
// The dummy variables name is not important, but with the same command structure remember to have 2nd dummy variable differnt for each define.

#define WRITE_VALUE _IOW('a', 'b', int32_t *)          // _IOW - Input Output write - we will pass an argument from user to kernel module space
#define READ_VALUE _IOR('a', 'b', int32_t *)           // _IOR - Input output read - we will pass an argument frim kernel odule space to user space
#define GREETER _IOR('a', 'c', struct mystruct*)

#endif