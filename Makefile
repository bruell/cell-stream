#
# Makefile for conv-test
#

LOCAL_DIR	= /media/sda2/project/cell-stream
REMOTE_DIR	= /usr/local/cell-stream
SERVER_DIR	= server
CLIENT_DIR	= client

ID_FILE		= /home/phil/Ids/allegro-root.private
REMOTE_USER	= root
REMOTE_HOST	= allegro

sync:
#	scp -i $(ID_FILE) -r $(LOCAL_DIR)/* $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)/
	rsync -e "ssh -i $(ID_FILE)" -vrcC $(LOCAL_DIR)/src/* $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)/src/

build-server:
#	ssh -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST) "cd $(REMOTE_DIR)/src/$(SERVER_DIR) ; make all"
	cd /media/sda2/project/cell-stream/src/server ; make all
	rsync -e "ssh -i $(ID_FILE)" -vrcC $(LOCAL_DIR)/bin/* $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)/bin/

rebuild-server:
#	ssh -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST) "cd $(REMOTE_DIR)/src/$(SERVER_DIR) ; make clean all"
	cd /media/sda2/project/cell-stream/src/server ; make clean all
	rsync -e "ssh -i $(ID_FILE)" -vrcC $(LOCAL_DIR)/bin/* $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)/bin/

run-server:
	ssh -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST) "cd $(REMOTE_DIR)/bin ; ./csserver"

build-client:	sync
	ssh -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST) "cd $(REMOTE_DIR)/src/$(CLIENT_DIR) ; make all"

rebuild-client:	sync
	ssh -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST) "cd $(REMOTE_DIR)/src/$(CLIENT_DIR) ; make clean all"

run-client:
	ssh -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST) "cd $(REMOTE_DIR)/bin ; ./cscapp"
	
fetch-result:
	scp -i $(ID_FILE) $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)/bin/out.jpg $(LOCAL_DIR)/bin

build:			build-server build-client

run:			run-server run-client

sync-share:
	rsync -vrcC -exclude=*/.* $(LOCAL_DIR) /media/share/project/

build-client-win32:
	ssh -i $(ID_FILE) -p 2222 -l Philipp localhost build-cs.cmd

rebuild-client-win32:
	ssh -i $(ID_FILE) -p 2222 -l Philipp localhost rebuild-cs.cmd

run-client-win32:
#	gnome-terminal --geometry 130x45 --hide-menubar -x ssh -i $(ID_FILE) -p 2222 -l Philipp localhost run-cs.cmd
	gnome-terminal --geometry 130x45 --hide-menubar -x wine /media/sda2/project/cell-stream/bin/cscapp-w32.exe
	