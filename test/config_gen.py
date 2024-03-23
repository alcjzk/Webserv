import string
import random

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

def corrupt_conf(s, n):
    r = range(0, len(s))
    for i in range(1, 100);
        s.replace(random.choice(s), )
    

# def s_isvalid(c, p):
#     if p == '\\' or c == '\\':
#         return True
#     if c.isalpha() or c == '/':
#         return True
#     return False

def main():
    config_file = ''
    for i in range(1, 10):
        config_file += gen_valid_ctx_switch() + '\n'
        for j in range(1, 5):
            config_file += gen_valid_assignment() + '\n'
    print(config_file)
    
    
if __name__ == "__main__":
    main()
    # state_transitions = {'.' : [isalpha, ]}
