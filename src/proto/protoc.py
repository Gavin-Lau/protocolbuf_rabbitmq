#!/bin/env python
#-*- encoding=utf8 -*-

import os, sys, platform
from termcolor import colored
import colorama
colorama.init()

def ERROR(msg):
	print colored(msg, 'red')
def WARNING(msg):
	print colored(msg, "yellow")
def DEBUG(msg):
	print colored(msg, "green")

root = ""
if len(sys.argv) < 2:
	root = os.getcwd()
else:
	root = sys.argv[1]
	
# 获取指定路径下所有指定后缀的文件
# dir 指定路径
# ext 指定后缀，链表&不需要带点 或者不指定。例子：['xml', 'java']
def getFiles(dir, ext = "proto"):
	allfiles = []
	if os.path.isfile(root):
		allfiles.append(sys.argv[1])
		return allfiles
	elif os.path.isdir(root):		
		for rootdir,childdirs,files in os.walk(root):
			# 不递归
			if rootdir == root:
				for file in files:
					extension = os.path.splitext(file)[1][1:]
					if extension == ext:
						file = os.path.join(root, file)
						allfiles.append(file)
	else:
		ERROR("Wrong argv[1]")
		return None;
	return allfiles

# 执行protoc 命令
protoFiles = getFiles(dir = root)
#print protoFiles
num = 0
for file in protoFiles:
	num += 1
	count = len(protoFiles)
	ret = os.system("protoc -I=" + root + " --cpp_out=" + root + os.path.sep + ".." + " " + file)
	if ret != 0:
		ERROR("[{0}/{1}]protoc file:{2} something Wrong!".format(num, count, file) )
	else:
		DEBUG("[{0}/{1}]protoc file:{2} Success!".format(num, count, file) )