import re

content = open('main.cpp', 'r', encoding='utf-8').read()

classes = re.findall(r'class\s+(\w+PFM)\s*:\s*public\s+\w+', content)
classes = sorted(list(set(classes)))

factory = "std::unique_ptr<PathFindingModule> create_pfm(const std::string& name) {\n"
for c in classes:
    if c.startswith('Base') or c.startswith('_'):
        continue
    if c in ['BaseTSPPFM', 'TSPClassicPFM']:
        factory += f'    if (name == "{c}") return std::make_unique<{c}>();\n'
    else:
        factory += f'    if (name == "{c}") return std::make_unique<pfm_ported::{c}>();\n'
factory += "    return nullptr;\n}\n\n"

# We will replace the old factory with the new one
content = re.sub(r'std::unique_ptr<PathFindingModule> create_pfm\(const std::string& name\) \{.*?(?=int main)', factory, content, flags=re.DOTALL)

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
print("Updated factory to skip abstract classes.")
