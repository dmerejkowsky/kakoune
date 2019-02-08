colorscheme desertex
add-highlighter global/ wrap -word

eval %sh{kak-lsp --kakoune -s $kak_session}
lsp-enable

set-option global ui_options 'ncurses_assistant=cat'
map global user c ':enter-user-mode cd<ret>'
map global user m ':make<ret>'

map global user f ':find '
map global user g ':grep '

# Put yanked stuff in the clipboard
hook global NormalKey y %{ nop %sh{
  printf %s "$kak_main_reg_dquote" | xsel --input --clipboard
}}

# Use only gutter signs for lsp diagnostics,
# but keep the color in the rest of the line as-is
unset-face global DiagnosticWarning
unset-face global DiagnosticError

hook global InsertChar j %{ try %{
  exec -draft hH <a-k>jj<ret> d
  exec <esc>
}}
