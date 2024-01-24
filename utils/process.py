import re
import os

def replace_with_ascii(match):
    ascii_value = int(match.group(1))
    return chr(ascii_value)

def process_file(input_file, output_file):
    '''replace all ASCII values in the file with their char counterparts'''
    with open(input_file, 'r') as file:
        content = file.read()
    
    new_content = re.sub(r'c_<(\d+)>', replace_with_ascii, content)

    with open(output_file, 'w') as file:
        file.write(new_content)

# replace ascii values
process_file("build/err.log", "build/err.log")

# ---------------------------------------------

def find_cpp_headers(directory):
    '''find all C++ header files in the given directory'''
    headers = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".h") or file.endswith(".hpp"):
                headers.append(os.path.join(root, file))
    return headers

def extract_defined_types(file_path):
    '''extract defined types from a C++ header file'''
    with open(file_path, 'r') as file:
        content = file.read()

    patterns = [
        r'class\s+(\w+)',
        r'struct\s+(\w+)',
        r'enum\s+(\w+)',
        r'typedef\s+(\w+)',
        r'using\s+(\w+)',
        r'typename\s+(\w+)'
    ]

    types = set()
    for pattern in patterns:
        matches = re.findall(pattern, content)
        types.update(matches)

    return types

# ------------------------------------------------

# get all typenames
headers = find_cpp_headers("h")
all_types = set()
for header in headers:
    all_types.update(extract_defined_types(header))

# pad them so less accidental matches
padded = [f" {string} " for string in all_types]

# make a regex for each
pattern = r"(" + "|".join(re.escape(name) for name in padded) + ")"

# do it
with open('build/err.log', 'r') as file:
    lines = file.readlines()

with open('build/err.log', 'w') as file:
    for line in lines:
        processed_line = re.sub(pattern, r"\n\t\1", line)
        file.write(processed_line)