import hashlib
from hlextend import sha1

k = b"The algorithm stores the hash of each message block in a group of internal state registers. 078"
p = b"Dev Manish Bandhiya"
append = b"se22ucse078"

# Task 1

combined = k + p
og_digest = hashlib.sha1(combined).hexdigest()

print("Original message (secret || data):")
print(combined)
print("\nHash(secret || data) =", og_digest)

# Task 2

print("\nLength Extension Attack:\n")

known_part = p

# Trying code obfuscation :)
for guessed_len in range(8, 17):

    sha_ext = sha1()


    forged_block = sha_ext.extend(
        append,
        known_part,
        guessed_len,
        og_digest
    )

    final_digest = sha_ext.hexdigest()

    print(f"Assuming secret length = {guessed_len} bytes")
    print("Forged payload (data + padding + extra):")
    print(forged_block)
    print("Resulting forged hash:")
    print(final_digest)
    print("")
