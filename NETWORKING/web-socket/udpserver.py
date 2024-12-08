# Ahmet Yiğit Turhan 2448942
# Furkan Numanoğlu 2448710

import socket
import os
import hashlib
import time

host, port = "172.17.0.2", 12345


def send_object(pathlist, host, port):
    pipe, ack_header, end_header, end, ok, = "|", "ACK_HEADER", "END_HEADER", "END", "OK"
    start = time.time()
    encoded_vals = {pipe: pipe.encode(),
                    ack_header: ack_header.encode(),
                    end_header: end_header.encode(),
                    end: end.encode(),
                    ok: ok.encode()}
    hashed_vals = get_sha_list(encoded_vals)
    dest = (host, port)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect(dest)
        rtt_sum, rtt_count = 0, 0
        while rtt_count < 4:
            rtt = calculate_rtt(s, dest, encoded_vals[pipe])
            if rtt != 0:
                rtt_sum += rtt
                rtt_count += 1
        rtt = rtt_sum / 4
        rtt = 0.011 if rtt < 0.011 else rtt
        s.settimeout(rtt)
        data_list = read_data(pathlist)
        index_list = get_index_list(data_list)
        header_to_sent = create_one_header_with_hash(pathlist, encoded_vals[pipe])
        while True:
            s.sendto(header_to_sent, dest)
            try:
                message = s.recv(1024)
                if message == hashed_vals[ack_header] + encoded_vals[ack_header]:
                    break
            except:
                pass
        for _ in range(5):
            s.sendto(hashed_vals[end_header] + encoded_vals[end_header], dest)
            try:
                message = s.recv(128)
                if message[:64] == hashed_vals[ok] and message[64:] == encoded_vals[ok]:
                    break
            except:
                pass
        while index_list:
            try:
                length = 0
                for x, y in index_list:
                    length += 1
                    packet_to_send = data_list[x][y]
                    index_of_packet = str(y).encode()
                    file_name_2 = pathlist[x]
                    file_name_as_bytes = file_name_2.encode()
                    hash = compute_sha256((file_name_2 + "|" + str(y)).encode() + encoded_vals[pipe] + data_list[x][y])
                    hashed_packet = hash + file_name_as_bytes + encoded_vals[pipe] + index_of_packet + encoded_vals[
                        pipe] + packet_to_send
                    s.sendto(hashed_packet, dest)
                for i in range(length):
                    message = s.recv(1024)
                    hash = message[:64]
                    rec_ok, rec_filename, rec_index = message[64:].decode().split("_")
                    rec_index = int(rec_index)
                    if rec_ok == ok and hash == compute_sha256(("OK_" + rec_filename + "_" + str(rec_index)).encode()):
                        try:
                            index_list.remove((get_index_of_file(pathlist, rec_filename), rec_index))
                        except:
                            pass
            except:
                pass
        for _ in range(5):
            s.sendto(hashed_vals[end] + encoded_vals[end], dest)
            try:
                message = s.recv(1024)
                if message[:64] == hashed_vals[ok]:
                    s.sendto(hashed_vals[end] + encoded_vals[end], dest)
                    break
            except:
                pass
    end = time.time()
    return end - start


def create_header(path):
    filename = path
    byte_count = os.path.getsize(path)
    return (filename + "_" + str(byte_count)).encode()


def get_index_of_file(path_list, file_name):
    for path in path_list:
        if path == file_name:
            return path_list.index(path)
    raise Exception


def get_sha_list(data):
    res = {}
    for key, item in data:
        res[key] = compute_sha256(item)
    return res


def create_one_header_with_hash(path_list, encoded_pipe):
    paths = bytearray()
    for index, path in enumerate(path_list):
        paths.extend(create_header(path))
        if index != len(path_list) - 1:
            paths.extend(encoded_pipe)
    result = bytes(paths)
    hashed = compute_sha256(result)
    return hashed + result


def read_data(path_list):
    data = []
    for path in path_list:
        with open(path, "rb") as f:
            temp = []
            while True:
                chunk = f.read(1024)
                if not chunk:
                    break
                temp.append(chunk)
            data.append(temp)
    return data


def compute_sha256(data):
    return hashlib.sha256(data).hexdigest().encode()


def get_index_list(data_list):
    res = [(i, j) for i in range(len(data_list)) for j in range(len(data_list[i]))]
    return res


def calculate_rtt(s, dest, encoded_pipe):
    s.settimeout(0.3)
    s.sendto(encoded_pipe, dest)
    try:
        a = time.time()
        s.recv(10)
        b = time.time()
        return b - a
    except:
        return 0


paths = []
print(send_object(paths, host, port))
