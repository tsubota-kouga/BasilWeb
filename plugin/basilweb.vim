
scriptencoding utf-8
if exists('g:loaded_basilweb')
    finish
endif
let g:loaded_basilweb = 1
setlocal runtimepath+=.

let s:save_cpo = &cpo
set cpo&vim

if !exists('g:basilweb#default_url')
    let g:basilweb#default_url = 'https://www.google.com'
endif

if !exists('g:basilweb#tab_style_sheet')
    let g:basilweb#tab_style_sheet = ''
endif

if !exists('g:basilweb#progressbar_style_sheet')
    let g:basilweb#progressbar_style_sheet = ''
endif

if !exists('g:basilweb#toolbar_style_sheet')
    let g:basilweb#toolbar_style_sheet = ''
endif

if !exists('g:basilweb#base_style_sheet')
    let g:basilweb#base_style_sheet = ''
endif

if exists('g:basilweb#color_theme')
    if g:basilweb#color_theme == 'dark'
        let g:basilweb#base_style_sheet = '
                \   color: #ffffff;
                \   background-color: #444444;
                \ '
    elseif g:basilweb#color_theme == 'light'
        let g:basilweb#base_style_sheet = '
                \   color:#000000
                \   background-color: #ffffff;
                \ '
    endif
endif

let g:basilweb#selected_text_list = []

command! -nargs=? WebBasil call basilweb#web_start(<f-args>)

command! -nargs=0 WebBasilC call basilweb#openweb_with_cursor_url()


let &cpo = s:save_cpo
unlet s:save_cpo
