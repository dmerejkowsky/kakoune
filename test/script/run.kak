try %{ chai-eval-file options.test.chai } catch %{
    evaluate-commands "echo %val{error}"
}
#quit

