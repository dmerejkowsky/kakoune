colorscheme default
add-highlighter global/ wrap -word

eval %sh{kak-lsp --kakoune -s $kak_session}
lsp-enable

map global user c ':enter-user-mode cd<ret>'

map global user f ':find '
map global user g ':grep '

# Put yanked stuff in the clipboard
hook global NormalKey y %{ nop %sh{
  printf %s "$kak_main_reg_dquote" | xsel --input --clipboard
}}

unset-face global DiagnosticWarning
unset-face global DiagnosticError

hook global InsertChar j %{ try %{
  exec -draft hH <a-k>jj<ret> d
  exec <esc>
}}
