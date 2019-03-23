
scriptencoding utf-8
if exists('g:loaded_basilweb')
    finish
endif
let g:loaded_basilweb = 1
setlocal runtimepath+=.

let s:save_cpo = &cpo
set cpo&vim

if !exists('g:basilweb#setting_dict')
    let g:basilweb#setting_dict = {
                \ 'default_url': 'https://www.google.com',
                \ 'tab_style_sheet': '',
                \ 'progressbar_style_sheet': '',
                \ 'base_style_sheet': '',
                \ 'icon_theme': 'black'
                \ }
else
    if !has_key(g:basilweb#setting_dict, 'default_url')
        let g:basilweb#setting_dict['default_url'] = 'https://google.com'
    endif
    if !has_key(g:basilweb#setting_dict, 'tab_style_sheet')
        let g:basilweb#setting_dict['tab_style_sheet'] =
                    \ 'QTabBar::tab {
                    \   text-align: left;
                    \ }'
    endif
    if !has_key(g:basilweb#setting_dict, 'progressbar_style_sheet')
        let g:basilweb#setting_dict['progressbar_style_sheet'] = ''
    endif
    if !has_key(g:basilweb#setting_dict, 'base_style_sheet')
        let g:basilweb#setting_dict['base_style_sheet'] = ''
    endif
    if !has_key(g:basilweb#setting_dict, 'icon_theme')
        let g:basilweb#setting_dict['icon_theme'] = 'black'
    endif
endif

if !exists('g:basilweb#color_scheme')
    let g:basilweb#color_scheme = 'dark'
endif

if exists('g:basilweb#color_scheme')
    if g:basilweb#color_scheme == 'light'
        if g:basilweb#setting_dict['base_style_sheet'] == ''
            let g:basilweb#setting_dict['base_style_sheet'] =
                        \ 'color:#000000;background-color: #ffffff;'
            let g:basilweb#setting_dict['icon_theme'] = 'black'
        endif
    elseif g:basilweb#color_scheme == 'dark'
        if g:basilweb#setting_dict['base_style_sheet'] == ''
            let g:basilweb#setting_dict['base_style_sheet'] =
                        \ 'color: #ffffff;background-color: #444444;'
            let g:basilweb#setting_dict['icon_theme'] = 'light'
        endif
    elseif g:basilweb#color_scheme == 'black'
        if g:basilweb#setting_dict['base_style_sheet'] == ''
            let g:basilweb#setting_dict['base_style_sheet'] =
                        \ 'color:#ffffff;background-color: #000000;'
            let g:basilweb#setting_dict['icon_theme'] = 'light'
        endif
    endif
endif

let g:basilweb#selected_text_list = []

command! -nargs=? WebBasil call basilweb#web_start(<f-args>)

command! -nargs=0 WebBasilC call basilweb#openweb_with_cursor_url()


let &cpo = s:save_cpo
unlet s:save_cpo
