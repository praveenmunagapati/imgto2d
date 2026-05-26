
import glob, re
for f in glob.glob('pfm/*.h'):
    with open(f, 'r') as file:
        content = file.read()
    if '{ Q_OBJECT public:' in content:
        content = content.replace('{ Q_OBJECT public:', '{\n    Q_OBJECT\npublic:')
        with open(f, 'w') as file:
            file.write(content)

