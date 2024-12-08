# Ahmet Yiğit Turhan 2448942
# Furkan Numanoğlu 2448710

import socket
import hashlib
import time

dest = ("", 12345)


def get_objects(dest):
    ok, end_header, nack_header, ack_header, end, pipe = "OK", "END_HEADER", "NACKHEADER", "ACK_HEADER", "END", "|"
    start = time.time()
    encoded_vals = {ok: ok.encode(),
                    end_header: end_header.encode(),
                    nack_header: nack_header.encode(),
                    ack_header: ack_header.encode(),
                    end: end.encode(),
                    pipe: pipe.encode()}
    hashed_vals = compute_sha256_list(encoded_vals)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind(dest)
        file_names, files = [], []
        s.settimeout(0.3)
        for _ in range(8):
            try:
                message, addr = s.recvfrom(1024)
                s.sendto(encoded_vals[ok], addr)
            except:
                pass
        s.settimeout(None)
        while True:
            message, addr = s.recvfrom(1024)
            hash = message[:64]
            message = message[64:]
            if hash == hashed_vals[end_header]:
                s.sendto(hashed_vals[ack_header] + encoded_vals[ack_header], addr)
                break
            if len(message) + len(hash) < 64:
                s.sendto(encoded_vals[ok], addr)
                continue
            try:
                if compute_sha256(message) != hash:
                    s.sendto(hashed_vals[nack_header] + encoded_vals[nack_header], addr)
                    continue
                if len(file_names) != 0:
                    s.sendto(hashlib[ack_header] + encoded_vals[ack_header], addr)
                    continue
                data = message.decode()
                headers = data.split("|")
                for header in headers:
                    fileName, fileSize = header.split('_')
                    fileSize = int(fileSize)
                    if fileName not in file_names:
                        file_names.append(fileName)
                        files.append([False] * ceil(fileSize, 1024))
                s.sendto(hashed_vals[ack_header] + encoded_vals[ack_header], addr)
            except:
                s.sendto(hashed_vals[ok] + encoded_vals[ok], addr)

        while True:
            message = s.recv(1250)
            hash = message[:64]
            if hash == hashed_vals[end_header]:
                s.sendto(hashed_vals[ack_header] + encoded_vals[ack_header], addr)
                continue
            if hash == hashed_vals[end]:
                s.sendto(hashed_vals[ok] + encoded_vals[ok], addr)
                break
            lst = message[64:].decode().split("|")
            file_name = lst[0]
            ind = lst[1]
            data = "|".join(lst[2:])
            data, ind = data.encode(), int(ind)
            if compute_sha256(
                    file_name.encode() + encoded_vals[pipe] + str(ind).encode() + encoded_vals[pipe] + data) == hash:
                files[file_names.index(file_name)][ind] = data
                ack_data = ("OK_" + file_name + "_" + str(ind)).encode()
                hash_of_ack = compute_sha256(ack_data)
                s.sendto(hash_of_ack + ack_data, addr)
        write_files(file_names, files)
    end = time.time()
    return end - start


def compute_sha256(data):
    return hashlib.sha256(data).hexdigest().encode()


def compute_sha256_list(data):
    res = {}
    for key, item in data:
        res[key] = compute_sha256(item)
    return res


def ceil(num, divider):
    if num // divider == num / divider:
        return num // divider
    return num // divider + 1


def write_files(file_names, file_data):
    for i in range(len(file_names)):
        with open(file_names[i], "wb") as f:
            for data in file_data[i]:
                f.write(data)


print(get_objects(dest))
