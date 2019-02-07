colorscheme desertex
add-highlighter global/ wrap -word

eval %sh{kak-lsp --kakoune -s $kak_session}
lsp-enable

map global user c ':enter-user-mode cd<ret>'

map global user f ':find '
map global user g ':grep '
