declare-option -docstring "shell command to which the contents of the current buffer is piped" \
    str formatcmd

define-command format -docstring "Format the contents of the current buffer" %{ evaluate-commands -draft -no-hooks %{
    evaluate-commands %sh{
        if [ -n "${kak_opt_formatcmd}" ]; then
            path_file_tmp="$(dirname $kak_buffile)/$(basename $kak_buffile).fmt"
            printf %s\\n "
                write -sync \"${path_file_tmp}\"

                evaluate-commands %sh{
                    if ${kak_opt_formatcmd} ${path_file_tmp} > /dev/null; then
                        printf '%s\\n' \"execute-keys \\%|cat<space> ${path_file_tmp} <ret>\"
                    else
                        printf '%s\\n' \"
                            evaluate-commands -client '${kak_client}' echo -markup '{Error}formatter returned an error (\$?)'
                        \"
                    fi

                    printf '%s\\n' \"nop %sh{ rm -f ${path_file_tmp} }\"
                }
            "
        else
            printf '%s\n' "evaluate-commands -client '${kak_client}' echo -markup '{Error}formatcmd option not specified'"
        fi
    }
} }
