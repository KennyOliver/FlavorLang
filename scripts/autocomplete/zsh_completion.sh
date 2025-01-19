#compdef flavor

_flavor_completions() {
    local -a opts
    opts=(
        '--help:Show help information'
        '--github:Open GitHub repository'
        '--about:Show FlavorLang details'
        '--debug:Enable debug mode'
        '--minify:Minify a script'
        '--make-plugin:Compile a shared library plugin'
    )
    _describe 'flavor options' opts
}

_flavor_completions "$@"
