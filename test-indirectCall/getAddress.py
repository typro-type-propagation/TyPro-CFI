import angr
import struct

binary_path = "./a.out"
project = angr.Project(binary_path, load_options={'auto_load_libs': False})
print(project.loader.main_object.sections)

target_section_name = ".section_for_indirectcall"
sections = project.loader.main_object.sections
target_section = next(
    (section for section in sections if section.name == target_section_name), None)
print(target_section)
data_address = target_section.vaddr
data_size = target_section.memsize
data_as_bytes = project.loader.memory.load(data_address, data_size)
print(data_as_bytes)
mem_size = 8
mem_set = []

data_arr = [data_as_bytes[i:i+8] for i in range(0, len(data_as_bytes), 8)]
print(data_arr)
for i in range(len(data_arr)):
    address = struct.unpack('<Q', data_arr[i])[0]
    print(address)
    mem_set.append(hex(address))

print(mem_set)

