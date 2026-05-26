import re
import subprocess
import os
import sys

content = open('main.cpp', 'r', encoding='utf-8').read()
matches = re.findall(r'if \(name == "(\w+PFM)"\)', content)
algorithms = sorted(list(set(matches)))

print(f"Found {len(algorithms)} algorithms.")

os.environ['PATH'] = f"C:\\msys64\\mingw64\\bin;{os.environ.get('PATH', '')}"
cli_path = os.path.join('build', 'imgto2d_standalone_cli.exe')

if not os.path.exists(cli_path):
    print("CLI executable not found!")
    sys.exit(1)

success_count = 0
fail_count = 0

with open('test_outputs/test_results.log', 'w') as log:
    for alg in algorithms:
        out_svg = f"test_outputs/out_{alg}.svg"
        cmd = [cli_path, alg, "test_outputs/test_input.png", out_svg, "500", "128"]
        print(f"Running {alg}...", end=' ', flush=True)
        try:
            res = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
            if res.returncode == 0:
                print("SUCCESS")
                success_count += 1
                log.write(f"[SUCCESS] {alg}\n")
            else:
                print(f"FAILED (Exit {res.returncode})")
                fail_count += 1
                log.write(f"[FAILED] {alg}\n{res.stderr}\n")
        except subprocess.TimeoutExpired:
            print("TIMEOUT")
            fail_count += 1
            log.write(f"[TIMEOUT] {alg}\n")

print(f"\nCompleted: {success_count} succeeded, {fail_count} failed.")
