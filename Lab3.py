import os
import re

class Metrics:
    def __init__(self):
        self.total_lines = 0
        self.empty_lines = 0
        self.logical_lines = 0
        self.physical_lines = 0
        self.comment_lines = 0
        self.cyclomatic_complexity = 1
        
def process_file(file_path, metrics):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            logical_ops_regex = re.compile(r"(if|while|switch|case|&&|\|\||\?|\:|;)")
            for_loop_regex = re.compile(r"\bfor\s*\(")
            comment_regex = re.compile(r"^\s*//")
            block_comment_start_regex = re.compile(r"^\s*/\*")
            block_comment_end_regex = re.compile(r"\*/")
            in_block_comment = False

            for line in file:
                metrics.total_lines += 1

                if line.strip() == '':
                    metrics.empty_lines += 1
                else:
                    metrics.physical_lines += 1

                if in_block_comment:
                    metrics.comment_lines += 1
                    if block_comment_end_regex.search(line):
                        in_block_comment = False
                elif comment_regex.search(line):
                    metrics.comment_lines += 1
                elif block_comment_start_regex.search(line):
                    metrics.comment_lines += 1
                    in_block_comment = True
                else:
                    for_loop_found = bool(for_loop_regex.search(line))
                    logical_ops_count = len(logical_ops_regex.findall(line))

                    if for_loop_found:
                        metrics.logical_lines += 1  
                        metrics.cyclomatic_complexity += 1  
                        logical_ops_count -= 1  

                    metrics.logical_lines += logical_ops_count
                    metrics.cyclomatic_complexity += logical_ops_count
    except IOError:
        print(f"Could not open file: {file_path}")

def traverse_directory(dir_path, metrics):
    for root, _, files in os.walk(dir_path):
        for file in files:
            file_path = os.path.join(root, file)
            process_file(file_path, metrics)

def main():
    folder_path = input("Enter the folder path: ")
    if not os.path.isdir(folder_path):
        print("Invalid folder path.")
        return

    metrics = Metrics()
    traverse_directory(folder_path, metrics)

    print(f"Total Lines: {metrics.total_lines}")
    print(f"Empty Lines: {metrics.empty_lines}")
    print(f"Logical Lines: {metrics.logical_lines}")
    print(f"Physical Lines: {metrics.physical_lines}")
    print(f"Comment Lines: {metrics.comment_lines}")
    print(f"Cyclomatic Complexity: {metrics.cyclomatic_complexity}")

    if metrics.total_lines > 0:
        comment_level = (metrics.comment_lines / metrics.total_lines) * 100
    else:
        comment_level = 0
    print(f"Level of Commenting: {comment_level:.2f}%")

if __name__ == "__main__":
    main()
