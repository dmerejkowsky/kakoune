## Hide release notes from latest version

set-option global startup_info_version 20190120
## LSP
eval %sh{kak-lsp --kakoune -s $kak_session}
lsp-enable
map global user l ':enter-user-mode lsp<ret>' -docstring 'enter lsp user mode'
map global lsp l '<esc>:lsp-diagnostics<ret>'  -docstring 'show all diagnostics'

## UI options
colorscheme desertex
add-highlighter global/ wrap -word
set-option global ui_options 'ncurses_assistant=cat'
# Use only gutter signs for LSP diagnostics,
# but keep the color in the rest of the line as-is
unset-face global DiagnosticWarning
unset-face global DiagnosticError
set-face global LineFlagErrors green+bi
set-option global lsp_diagnostic_line_warning_sign !
set-option global lsp_diagnostic_line_error_sign  ✘

## Plugins customisation
set-option global grepcmd 'rg --no-heading --line-number --column --sort path'

## Working dir stuff
declare-option str project_working_dir %sh{ pwd }
map global user c ':enter-user-mode cd<ret>' -docstring 'enter cd user mode'
map global cd c ':change-directory %val{opt_project_working_dir}<ret>' -docstring 'reset working directory'
map global cd o ':evaluate-commands %sh{ cwd-history list --kakoune }<ret>' -docstring 'open old working directory'
alias global pwd print-working-directory


## Spell check
declare-user-mode spell
map global user s ':enter-user-mode spell<ret>' -docstring 'enter spell user mode'
map global spell n '<esc>:spell-next<ret>' -docstring 'go to next spell error'
map global spell f '<esc>:spell fr<ret>' -docstring 'run spell check in French'
map global spell e '<esc>:spell en<ret>' -docstring 'run spell check in English'

## Custom mappings
map global user m ':make<ret>' -docstring 'run make'
map global user f ':find ' -docstring 'run find'
map global user g ':grep ' -docstring 'run grep'
map global user b ':set-option buffer filetype ' -docstring 'set filetype for the current buffer'

## Copy/pasting
hook global NormalKey y %{ nop %sh{
  printf %s "$kak_main_reg_dquote" | xsel --input --clipboard
}}
map global user P '!xsel --output --clipboard<ret>' -docstring 'yank from system clipboard after selection'
map global user p '<a-!>xsel --output --clipboard<ret>' -docstring 'yank from system clipboard before selection'

## Persistent file and working dirs history
# (forgive me, mawwww, but persisting state is cool sometimes)
hook global BufOpenFile .* %{ %sh{ mru-files add  "${kak_hook_param}" } }
map global user o ':evaluate-commands %sh{ mru-files list --kakoune }<ret>' -docstring 'open old files'


## jj is escape 
hook global InsertChar j %{ try %{
  exec -draft hH <a-k>jj<ret> d
  exec <esc>
}}
