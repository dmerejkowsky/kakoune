define-command -params .. \
  -docstring "Run arbitrary command and display its output in the *run* buffer" \
  save-and-run %{ evaluate-commands %sh{
     # Create a temporary fifo for communication
     output=$(mktemp -d -t kak-temp-XXXXXXXX)/fifo
     mkfifo ${output}
     # run command detached from the shell
     ( $@ > ${output} ) > /dev/null 2>&1 < /dev/null &
     echo "write-all"
     # Open the file in Kakoune and add a hook to remove the fifo
     echo "edit! -fifo ${output} *run*"
     echo "hook buffer BufClose .* %{ nop %sh{ rm -r $(dirname ${output})} }"
     echo "buffer-previous"
  }
}
