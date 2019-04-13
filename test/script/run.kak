try %{
  chai-eval-file %opt{test_file}
  execute-keys -draft "<esc> i ok <esc> :write <ret>"
} catch %{
  execute-keys -draft "<esc> i %val{error} <esc> :write <ret>"
}
quit

