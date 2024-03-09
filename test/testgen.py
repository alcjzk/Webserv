import sys
import re

def collect_tests(name):
    file = open(name)
    if file.closed:
        return []
    class_name = None
    symbols = []
    for line in file:
        line = line.strip()
        if class_name is None:
            res = re.search("(class|namespace) (.+Test)", line)
            if res:
                class_name = res.group(2)
        else:
            res = re.search("\\s?([^();\\s]+_test)\\(", line)
            if res:
                symbol = str(class_name + "::" + res.group(1))
                symbols.append(symbol)
    file.close()
    return symbols

PREFIX = """
#include <iostream>
int main() {
    #ifdef TEST\
"""

SUFFIX = """
    std::cout << \"\\nTests OK.\\n\";
    #endif
}
"""

if __name__ == "__main__":
    argc = len(sys.argv)
    if argc < 3:
        print("invalid input" + str(argc))
        exit(1)
    target_path = sys.argv[1]
    tests = []
    headers = []
    for filename in sys.argv[2:]:
        symbols = collect_tests(filename)
        tests.extend(symbols)
        headers.append(filename.split("/")[-1])
    file = open(target_path, "w")
    for header in headers:
        file.write("#include \"" + header + "\"\n")
    file.write(PREFIX)
    for test in tests:
        file.write("\n\t" + test + "();")
    file.write(SUFFIX)
    file.close()
