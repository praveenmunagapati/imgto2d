content = open('main.cpp', 'r', encoding='utf-8').read()

fixed = content.replace('"\n;', '\\n";')
fixed = fixed.replace('[threshold]\n"', '[threshold]\\n"')
fixed = fixed.replace('2000 128\n"', '2000 128\\n"')
fixed = fixed.replace('algName << "\\'\n"', 'algName << "\\'\\n"')
fixed = fixed.replace('inputPath << "\n"', 'inputPath << "\\n"')
fixed = fixed.replace('outputPath << "\n"', 'outputPath << "\\n"')
fixed = fixed.replace('CLI\n"', 'CLI\\n"')
fixed = fixed.replace('algName << "\n"', 'algName << "\\n"')

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(fixed)
print("Fixed newlines")
