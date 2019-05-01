
scriptencoding utf-8
if !exists('g:loaded_basilweb')
    finish
endif

let s:save_cpo = &cpo
set cpo&vim

function! basilweb#web_start(...)
    if a:0 == 1
        call rpcnotify(0, 'NeoVim#plugin', 'BasilWeb', 'factory', a:1) " with url
    elseif a:0 == 2
        call rpcnotify(0, 'NeoVim#plugin', 'BasilWeb', 'factory', a:1, a:2) " with url and plugin type
    else
        call rpcnotify(0, 'NeoVim#plugin', 'BasilWeb', 'factory')
    endif
endfunction

function! s:is_url(str) abort
    if a:str =~ '^https\?://'  " str is url
        return v:true
    endif
    return v:false
endfunction

function! basilweb#openweb_with_cursor_url(...) abort
    let str = expand('<cfile>')
    if s:is_url(str)
        if a:0 > 0 " with plugin type
            call basilweb#web_start(str, a:1)
        else
            call basilweb#web_start(str)
        endif
    endif
endfunction

let &cpo = s:save_cpo
unlet s:save_cpo

