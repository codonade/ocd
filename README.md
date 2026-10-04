# 🫣 On Change Do

Utility to execute shell commands on file changes written in C89.

## ⚠️ Disclaimer

OCD doesn't know how to parse file patterns (globs) like `*.c` or `**/*`. It completely relies on the
shell to expand them before they're passed to it. If a pattern isn't expanded by your shell, OCD will
receive it literally and won't work as expected. Consult your shell's documentation for details on
glob expression and how to enable or configure it.
