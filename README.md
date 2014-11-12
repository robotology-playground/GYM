GYM
===

The Generic YARP Module is a component model to easily develop software modules for robotics leveraging the YARP ecosystem (http://github.com/robotology/YARP).

It is developed under the European Project WALK-MAN: http://www.walk-man.eu/.


Generic Yarp Module Tutorial
============================

This is a simple tutorial about the use of a generic software architecture to implement a task that will be run on a robot.

Using the Generic YARP Module ( **GYM** from here on ) makes it standard and simpler writing a task; it also provides some built-in features that will be described in this tutorial.

Goal
----
The goal of this tutorial is to understand how to use the **GYM**. For further details about the implementation of the **GYM** please refer to the Doxygen documentation.

Prerequisites
--------------
The **GYM** tutorial assumes some basic knowledge about:

* C++: the **GYM** supports only C++ language and it uses some features (like smart pointers ) from C++ 11.   
* YARP (Yet Another Robot Platform): **GYM** is a piece of software built upon YARP. It can be seen as a "middleware" between YARP and the code of your task.

GYM design
--------------
**GYM** is composed by two main components:

* a template class called **generic_module** derived from YARP _RFModule_ that represents a low-rate thread used to supervise the life cycle of an internal control thread(described below). 
* a class called **generic_thread** derived from YARP _RateThread_ that represents an high-rate control thread.

In order to use the **GYM** you simply have to inherit from this two classes: in the image below you can see an overview of the **GYM** design when we define two classes called **MyModule** and **MyThread** respectively derived from **generic_module** (with MyThread class as template argument) and **generic_thread**.    


![GYM - Design Overview](https://dl.dropboxusercontent.com/u/49470846/GSA_high_level.png)

As you can see **MyModule** inherits from **generic_module** a YARP [Resource Finder](http://wiki.icub.org/yarpdoc/yarp_resource_finder_tutorials.html) , a YARP [Param Helper](https://github.com/robotology-playground/paramHelp) and three [YARP ports](http://wiki.icub.org/yarpdoc/note_ports.html):

1. A **switch port**(input) used to send commands related to the life cycle of the internal control thread:
  * **start**: creates the control thread and starts it.
  * **stop**: stops the control thread and kills it (delete all its resources).
  * **pause**: suspends the control thread.
  * **resume**: resumes the control thread.
  * **quit**: same as **stop**; moreover it closes the module itself.
2. A **status port**(output) that gives information about  the control thread: alive or not alive.
3. An **rpc port**(input/output) opened when the control thread starts and used for the communication with the Param Helper:
  * **help**: help.
  * **save_params**: saves the actual parameters configuration.
  * **set/get [param]**: online sets/gets the value of the parameter passed as argument.
  * **[custom commands]**: possibly you could define your own Param Helper commands in **MyModule**.

GYM constraints
---------------

When using the **GYM** there are few constraints to keep in mind:
 
* **Constraint # 1**
Only **generic_thread** derived classes could be passed as a template argument of a **generic_module**.   

* **Constraint # 2**
You have to specify two mandatory parameters for the Resource Finder(as command line args or inside the .ini configuration file):
  * **thread_period**: the period of the internal control thread in milliseconds (as an int).
  * **robot_name**: the name of the robot (as a string).  

GYM example
===========
Here you can find a simple example about the usage of the GYM.  
**GYM** is a github repo, [repo](https://github.com/robotology-playground/GYM), and the tutorial example is inside the **examples** folder.

Run the example
---------------
The tutorial example consists in a simple task implementation: control the position of the left arm joints in real-time.  

Open 7 terminal:  

On terminal # 1 start YARP name server  
~$ **yarpserver --write**  

On terminal # 2 start gazebo  
~$ **gazebo**  
Inside gazebo load the coman model you prefer.

On terminal # 3 run generic_tutorial executable  
~$ **generic\_tutorial --from tutorial\_configuration.ini**  
As you can see from the output of this terminal, the generic\_tutorial founds the .ini file that you have specified through the resource finder command line param **--from**. Moreover it opens the switch and status ports specified in the image above.

Now the module is started, but for the moment the internal thread does not exist. We can monitor the life-cycle of the internal thread through the module status port:

on terminal # 4 monitor the status of the module in terms of the life-cycle of its internal control thread  
~$ **yarp read ... /generic_tutorial/module/status:o**

On terminal # 5 start the internal control thread through the switch interface  
~$ **yarp write ... /generic_tutorial/switch:i**  
~$ >> **start**

Give the start command to the module make the internal thread starts, you can verify it looking at the output of terminal # 4.
From terminal # 3 output you can see the initialization values of our parameters and all the yarp log messages about new open ports and devices.

The robot left arm is still idle: to make it move to the initial configuration give the "test\_cmd"command to the generic_module command port:  

on  terminal # 6  
~$ **yarp write ... /generic_tutorial/command:i**  
~$ >> **test\_cmd**

The robot left arm is moving to the configuration specified in the .ini file.

How to modify the position of the left arm joints in real-time?
Let's use the Param Helper through the rpc port of the module:

on terminal # 7
~$ **yarp rpc /generic_tutorial/rpc**  
~$ >> **help**  
..... help output .....  
~$ >> **get left_arm**  
..... output of the left_arm actual joint configuration .....  
~$ >> **set left\_arm -90 0 0 0 0 0 0** 

In this way we set the new configuration for the left arm joints.  
To make the robot move to new configuration give the "test_cmd" command on terminal # 6  
~$ >> **test\_cmd**

While moving to a new configuration you can pause/resume the control thread using the switch interface on terminal # 5. You can also stop the module or quit it.

If you like the new parameters configuration, you can easily save it using the "save\_params <filename[.ini]>"  rpc command:

on terminal # 7  
~$ >> **save\_params new\_configuration.ini** 

If you want now you can repeat the test starting the generic\_tutorial with this different initial configuration:

~$ **generic\_tutorial --from new\_configuration.ini**


Installation
------------

To Install the Generic Module Architecture library, it is recommended to use the robotology superbuild http://www.github.com/robotology-playground/robotology-superbuild
