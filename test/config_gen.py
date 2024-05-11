import string
import random
import sys

def gen_valid_assignment():
    left = [random.choice(string.ascii_letters) for j in range(4, 10)]
    right = [random.choice(string.ascii_letters) for j in range(4, 10)]
    return "".join(left) + '=' + "".join(right)

def gen_valid_ctx_switch():
    middle = ''
    middle += random.choice(string.ascii_letters)
    curc = middle[-1]
    for i in range(1, random.randint(20, 200)):
        if curc.isalpha():
            middle += random.choice(string.ascii_letters + ('.' * 20))
        else:
            middle += random.choice(string.ascii_letters)
        curc = middle[-1]
    first, second = f"[{middle.strip('.')}]", f"[[{middle.strip('.')}]]"
    middle = random.choice([first, second])
    return middle

def corrupt_conf(s):
    byte_arr = bytearray(s.encode())
    for i in range(1, len(byte_arr) // 10):
        byte_arr[random.randint(0, len(byte_arr))] = random.randint(0, 125)
    return byte_arr.decode('utf-8')

def main():
    config_file = ''
    for i in range(1, random.randint(10, 1000)):
        config_file += gen_valid_ctx_switch() + '\n'
        for j in range(1, random.randint(2, 8)):
            config_file += gen_valid_assignment() + '\n'
    print(config_file, file=sys.stdout)
    print(corrupt_conf(config_file), file=sys.stderr)
    
if __name__ == "__main__":
    main()
