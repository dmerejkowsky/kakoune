define-command -docstring \
"find <filename>: search for file recusively under path option: %opt{path}" \
find -params 1 %{ evaluate-commands %sh{
    file=$1
    eval "set -- $kak_opt_path"
    while [ $# -gt 0 ]; do
        case $1 in                        # Since we want to check fewer places
            ./) path=${kak_buffile%/*} ;; # I've swapped ./ and %/ because
            %/) path=$PWD ;;              # %/ usually has smaller scope. So
            *)  path=$1 ;;                # this trick is a speedi-up hack.
        esac
        if [ -z "${file##*/*}" ]; then # test if filename contains path
            if [ -e "$path/$file" ]; then
                printf "%s\n" "edit -existing %{$path/$file}"
                exit
            fi
        else # build list of candidates or automatically select if only one found
            for candidate in $(fd --type f "$file"); do
                if [ -n "$candidate" ]; then
                    candidates="$candidates %{$candidate} %{evaluate-commands %{edit -existing %{$candidate}}}"
                fi
            done
            if [ -n "$candidates" ]; then
                printf "%s\n" "menu -auto-single $candidates"
                exit
            fi
        fi
        shift
    done
    printf "%s\n" "echo -markup %{{Error}unable to find file '$file'}"
}}
