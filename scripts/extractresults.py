import json
import os
import sys

def extract_results(log_file_path, output_file_path):
    with open(log_file_path, 'r') as log_file:
        log_content = log_file.read()

    # Split the log content by the delimiters
    sections = log_content.split('<<<<<<< Test Report Start >>>>>>')
    results = []

    for section in sections[1:]:  # Skip the first split part as it is before the first delimiter
        json_part = section.split('<<<<<<< Test Report End >>>>>>')[0].strip()
        try:
            result = json.loads(json_part)
            results.append(result)
        except json.JSONDecodeError:
            print("Skipping invalid JSON section")

    # Write the extracted results to the output file
    with open(output_file_path, 'w') as output_file:
        json.dump({"test_results": results}, output_file, indent=4)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python extractresults.py <log_file_path>")
    else:
        log_file_path = sys.argv[1]
        script_dir = os.path.dirname(os.path.abspath(__file__))
        output_file_path = os.path.join(script_dir, 'results.json')
        extract_results(log_file_path, output_file_path)
        print(f"Results saved as {output_file_path}")