colorscheme desertex
add-highlighter global/ wrap -word

eval %sh{kak-lsp --kakoune -s $kak_session}
lsp-enable

set-option global ui_options 'ncurses_assistant=cat'

map global user c ':enter-user-mode cd<ret>' -docstring 'enter cd user mode'

map global user l ':enter-user-mode lsp<ret>' -docstring 'enter lsp user mode'
map global lsp l '<esc>: lsp-diagnostics<ret>'  -docstring 'show all diagnostics'

map global user m ':make<ret>' -docstring 'run make'

declare-user-mode spell
map global user s ':enter-user-mode spell<ret>' -docstring 'enter spell user mode'
map global spell n '<esc>:spell-next<ret>' -docstring 'go to next spell error'

map global user f ':find ' -docstring 'run find'
map global user g ':grep ' -docstring 'run grep'

# Put yanked stuff in the clipboard
hook global NormalKey y %{ nop %sh{
  printf %s "$kak_main_reg_dquote" | xsel --input --clipboard
}}

# Yank stuff from the system clipboard
map global user P '!xsel --output --clipboard<ret>' -docstring 'yank from system clipboard after selection'
map global user p '<a-!>xsel --output --clipboard<ret>' -docstring 'yank from system clipboard before selection'

# Use only gutter signs for lsp diagnostics,
# but keep the color in the rest of the line as-is
unset-face global DiagnosticWarning
unset-face global DiagnosticError
set-option global startup_info_version 20190120

hook global InsertChar j %{ try %{
  exec -draft hH <a-k>jj<ret> d
  exec <esc>
}}
