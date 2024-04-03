OUTFILE_A = carsystem
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -pthread -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wshadow 
PUBDIR = $(HOME)/cmpt433/public/myApps
DEPLOY_PATH= $(HOME)/cmpt433/public/myApps/$(OUTFILE_A)server-copy

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: app wav deploy node_install
app:
	$(CC_C) $(CFLAGS) main.c gps.c screen.c listener.c udpSocketInterface.c sharedMem-Linux.c SeeedGrayOLED.c joystick.c musicPlayer.c libportaudio.a  wavParser.c helper.c -lm -o $(OUTDIR)/$(OUTFILE_A) $(LFLAGS) -lasound
clean:
	rm $(OUTDIR)/$(OUTFILE_A)
	rm $(OUTDIR)/$(OUTFILE_B)

# Copy wave files to the shared folder
wav:
	mkdir -p $(OUTDIR)/beatbox-wav-files/
	cp wave-files/* $(OUTDIR)/beatbox-wav-files/ 

#copy node files and install node
deploy:
	@echo 'COPYING THE NODE.JS FILES TO $(DEPLOY_PATH)'
	@echo ''
	mkdir -p $(DEPLOY_PATH)
	chmod a+rwx $(DEPLOY_PATH)
	cp -r server/* $(DEPLOY_PATH)
	@echo 'Do not edit any files in this folder; they are copied!' > $(DEPLOY_PATH)/DO_NOT_EDIT-FILES_COPIED.txt
	@echo ''
	@echo 'NOTE: On the host, in $(DEPLOY_PATH), it is best to run: npm install'
	@echo '      Or, just run the node_install target in this makefile.'

node_install:
	@echo ''
	@echo ''
	@echo 'INSTALLING REQUIRED NODE PACKAGES'
	@echo '(This may take some time)'
	@echo ''
	cd $(DEPLOY_PATH) && npm install