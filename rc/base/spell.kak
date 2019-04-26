declare-option -hidden range-specs spell_regions
declare-option -hidden str spell_lang
declare-option -hidden str spell_tmp_file

define-command spell-check %{ evaluate-commands %sh{
    kak-spell \
      --lang ${kak_opt_spell_lang} \
      check \
       --timestamp ${kak_timestamp} \
       ${kak_buffile}
} }

define-command spell-enable -params 1 %{
  try %{ add-highlighter window/ ranges 'spell_regions' }
  evaluate-commands %sh{
    printf 'set-option buffer spell_lang %s\n' "$1"
  }
  hook -group "spell-hooks" buffer BufWritePost ".*" spell-check
  write
}

define-command spell-clear %{
    unset-option buffer spell_regions
    remove-hooks buffer spell-hooks
}

define-command spell-add %{
  execute-keys <a-i> w
  evaluate-commands %sh{
    kak-spell --lang $kak_opt_spell_lang add ${kak_selection}
  }
  write
}

define-command spell-remove %{
  execute-keys <a-i> w
  evaluate-commands %sh{
    kak-spell --lang $kak_opt_spell_lang remove ${kak_selection}
  }
  write
}

define-command spell-next %{ evaluate-commands %sh{
} }

define-command spell-replace %{
  execute-keys <a-i> w
  evaluate-commands %sh{
    kak-spell --lang $kak_opt_spell_lang replace ${kak_selection}
  }
}
