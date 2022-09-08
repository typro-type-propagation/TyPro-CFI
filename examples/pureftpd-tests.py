from pwn import *

# context.log_level = 'debug'

conn = remote('localhost', 2121)
conn.sendline(b'USER anonymous')
conn.sendline(b'EPSV')
conn.recvuntil(b'229 ')
port = re.findall(r'\d+', conn.recvline().decode())[0]

conn2 = remote('localhost', int(port))

print('> LIST a/*')
conn.sendline(b'LIST a/*')


print('<', conn.recvall(timeout=1).decode().replace('\r', ''))
print('<', conn2.recvall(timeout=1).decode().replace('\r', ''))

print('[OK]')
