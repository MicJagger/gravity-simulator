This project requires glad and SDL to function
Glad version 3.3 core
SDL (version Release 2.30.9 originally)
The installation scripts unpack glad and install SDL, select the one for your OS
The Linux one requires sudo priveleges to install SDL

these are not mine
i did not write them
i have no ownership of them
this is included for convenience
i did not feel like setting up all sorts of fancy include paths
i just want me / others to be able to 'git clone' and make

I edited one #include line in glad.c to work in this context

The included include_lib.hpp headers are due to the nature of changing file names as to not have to
change include directories all the time in a ton of other places