
scriptencoding utf-8
if !exists('g:loaded_basilweb')
    finish
endif

let s:save_cpo = &cpo
set cpo&vim

function! basilweb#web_start(...)
    if a:0 > 0
        call rpcnotify(0, 'NeoVim#plugin', 'BasilWeb', 'factory', a:1)
    else
        call rpcnotify(0, 'NeoVim#plugin', 'BasilWeb', 'factory')
    endif
endfunction

let &cpo = s:save_cpo
unlet s:save_cpo
