#!/bin/env python
#-*- encoding=utf8 -*-

import os
import shutil

# Windows
if (os.path.exists("./x64")):
	shutil.rmtree("./x64",True)
if (os.path.exists("./bin")):	
	shutil.rmtree("./bin",True)
	
if (os.path.exists("./test/x64")):
	shutil.rmtree("./test/x64",True)

if (os.path.isfile("ProtOnRbt.sdf")):
	os.remove("ProtOnRbt.sdf")
if (os.path.isfile("ProtOnRbt.v12.suo")):
	os.remove("ProtOnRbt.v12.suo")
if (os.path.isfile("./src/stulist.pb.cc")):
	os.remove("./src/stulist.pb.cc")
if (os.path.isfile("./src/stulist.pb.h")):	
	os.remove("./src/stulist.pb.h")
	
#Linux

if (not os.path.exists("./bin")):
	os.makedirs("./bin")
