#!/bin/bash
_flavor_completions() {
    local cur prev opts
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="--help --github --about --debug --minify --make-plugin"
    case "${prev}" in
        flavor)
            COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
            ;;
        --make-plugin)
            COMPREPLY=( $(compgen -f -- "${cur}") )
            ;;
        --debug|--minify)
            COMPREPLY=( $(compgen -f -X '!*.flv' -- "${cur}") )
            ;;
        *)
            COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
            ;;
    esac
}
complete -F _flavor_completions flavor
