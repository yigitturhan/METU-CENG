
#Ahmet Yiğit Turhan 2448942
#Furkan Numanoğlu 2448710

import socket
import time
host, port,type = "", 12345, "utf-8"
def get_objects(dest):
    start = time.time()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(dest)
        s.listen()
        conn, addr = s.accept()
        for _ in range(20):
            message = conn.recv(20).decode(type) #get the 20 files headers
            if message == "END": #that means the process is finished
                break
            fileName, fileSize = message.split("_") #extract filename and filesize
            fileSize, received_bytes = int(fileSize), 0
            conn.send(b"NO_PROBLEM") #send no_problem message to server to let it know everything ok
            with open(fileName, "wb") as f: #open the file with extracted filename
                while received_bytes < fileSize: #if received bytes is equal to filesize than break. it is for not to write other files data on top of that
                    chunk = conn.recv(1024)
                    received_bytes += len(chunk)
                    f.write(chunk)
            conn.send(b"RECEIVED") #sen received message to sender to know the situation is ok
            print(fileName, " received.")
    end = time.time()
    return end - start #return the total time
        
dest = (host, port)
print(get_objects(dest))


