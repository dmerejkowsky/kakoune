try %{
  chai-eval-file %opt{test_file}
} catch %{
  execute-keys -draft "<esc> i %val{error} <esc> :write <ret>"
}
write
quit

