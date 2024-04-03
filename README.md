Simulating a Car System on our Beaglebone

Feature #1: Play Audio by recieving UDP Packets containing a mp3 song file and then use aplay to play it inside the beaglebone Audio output (Currently WIP)


In order to build and run our app.
0. Create a folder called wave-files at upmost level in this repo the folder should be at the same level as the ultraSonic,PRU, etc. You will need to download all the song files into the  wave-files folder  from this link https://drive.google.com/drive/folders/1IJScEo_HhHm00GDfbZiphJzX0QAvcmUG?usp=sharing .Do not change the name of the files and ensure that they are placed in the wave-files folder so the relative path in this repo to them would be /wave-files/song_example

1. First run the make file from the outermost level of the repo. 
2. Then run the make file in the PRU folder. These will build and copy over the necessary executables and files onto the beagle bone.
3. on the Beaglebone you will need to navigate to the  /mnt/remote/pru/neoPixelRGBBasic and run make. Then run make install_PRU0.
4. on the Beaglebone you will need to navigate to the /mnt/remote/pru/Ultrasonic and run make. Then run make install_PRU1.
5. To run the node server you will need to navigate to /mnt/remote/myApps/carsystemserver-copy and run node server.js
6. finally to run the executable you will need to navigate to /mnt/remote/myApps/ and run the carsystem executable there.

You will need all the required hardware setup on your board otherwise issues could arise when trying to run the program


