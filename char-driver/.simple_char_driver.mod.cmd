savedcmd_simple_char_driver.mod := printf '%s\n'   simple_char_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > simple_char_driver.mod
