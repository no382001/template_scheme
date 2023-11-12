import re
import sys

def replace_with_ascii(match):
    ascii_value = int(match.group(1))
    return chr(ascii_value)

def process_file(input_file, output_file):
    with open(input_file, 'r') as file:
        content = file.read()
    
    new_content = re.sub(r'c_<(\d+)>', replace_with_ascii, content)

    with open(output_file, 'w') as file:
        file.write(new_content)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    process_file(input_file, output_file)