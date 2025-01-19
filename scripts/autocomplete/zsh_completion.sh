#compdef flavor
_flavor_completions() {
    local -a opts
    opts=(--help --github --about --debug --minify --make-plugin)
    _describe -t commands 'flavor commands' opts
}
