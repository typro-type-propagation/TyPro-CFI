from pwn import *

# context.log_level = 'debug'

conn = remote('localhost', 2121)
conn.sendline('USER anonymous')
conn.sendline('EPSV')
conn.recvuntil('229 ')
port = re.findall(r'\d+', conn.recvline().decode())[0]

conn2 = remote('localhost', int(port))

print('> LIST a/*')
conn.sendline('LIST a/*')


print(conn.recvall(timeout=1).decode())
print(conn2.recvall(timeout=1).decode())
