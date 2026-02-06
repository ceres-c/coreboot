## Modified ucode files to be used with `TARGET_UCODE_UPDATE`
### 06-5c-0a
Official update file, version 0x28, no changes

### 06-5c-0a_mod_uop0_PORTIN_DSZ64_ASZ16_SC1-to-something_else
Changed uop0 in update file (`PORTIN_DSZ64_ASZ16_SC1`) to another uop. IIRC,
crc has been changed accordingly. This test was done to verify if it would be
possible to exploit rc4 bit flipping capabilities to change an uop "blindly".

### 06-5c-0a_mod_uop47_nop_add_to_tmp0
Changed uop47 (`NOP`) to add to tmp0. Made to explore wether it would be
possible to change uops and get the ucode to load knowing the RC4 password.

### 06-5c-0a_patched_re_encrypt_nop_add
-- Don't remember, sorry --

### 06-5c-0a_rsamod
Changed the RSA modulus to a different key and generated a new signature with
the corresponding private key. This test is to verify if we can get the ucode
update to accept a different public key.

### 06-5c-0a_signature_change
Flipped a bit in the signature without changing anything else. This test is to
see if we can glitch the final signature check.

### 06-5c-0a_signed
Idk what this is, sorry.
