#Ahmet Yiğit Turhan 2448942
#Furkan Numanoğlu 2448710

import socket
import time
import os
type = "utf-8"
def send_object(path_list, dest):
    start = time.time()
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(dest)
            for path in path_list:
                send_file(s, path)
            s.send(b"END") #the signal for client to close
            print("Files are sent.")
    except Exception as e:
        print(e)
    end = time.time()
    return end - start #return the total time to send 20 files

def send_file(s, filePath):
    with open(filePath, "rb") as f:
        fileName = filePath[14:] #get rid of the /root/objects/ part at path
        fileSize = os.path.getsize(filePath)# get the file size
        s.send((fileName+"_"+str(fileSize)).encode(type)) #send filename and filesize to client. filename is for opening the file, filesize is for the loop at client
        message = s.recv(10)
        flag = False #it should not be happen but for if a packet losts. we have faced with this issue but not sure weather it was our fault. therefore we added that flag part
        while True:
            try:
                if not flag: #we would not expect that but the try block failed and the code continued to except block
                    chunk = f.read(1024) #if no retransmission
                if not chunk:
                    break
                s.sendall(chunk)    #send the chunk to the client             
                flag = False
            except Exception as e:
                print(e)
                flag = True #if fails than make a retransmission
                continue
        message = s.recv(8)
host = "172.17.0.2"
port = 12345
dest = (host, port)
send_object(["/root/objects/large-0.obj", "/root/objects/small-0.obj", "/root/objects/large-1.obj", "/root/objects/small-1.obj",
"/root/objects/large-2.obj", "/root/objects/small-2.obj","/root/objects/large-3.obj", "/root/objects/small-3.obj",
"/root/objects/large-4.obj", "/root/objects/small-4.obj","/root/objects/large-5.obj", "/root/objects/small-5.obj",
"/root/objects/large-6.obj", "/root/objects/small-6.obj","/root/objects/large-7.obj", "/root/objects/small-7.obj",
"/root/objects/large-8.obj", "/root/objects/small-8.obj","/root/objects/large-9.obj", "/root/objects/small-9.obj"], dest)

