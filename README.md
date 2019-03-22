
# BasilWeb

## Required
* NeoVim

* Basilico
    - Qt version is 5.7 or later

## Install
If you use dein.vim, you can write on dein.toml like this.
```toml
[[plugin]]
repo='tsubota-kouga/BasilWeb'
build='bash install.sh'
hook_add='''
let g:basilweb#color_scheme='dark'
nnoremap <silent><C-LeftMouse> :call basilweb#openweb_with_cursor_url()<CR> " set mouse=a is required
'''
```
or write on init.vim
```vim
call dein#add('tsubota-kouga/BasilWeb')
let g:basilweb#color_scheme='dark'
nnoremap <silent><C-LeftMouse> :call basilweb#openweb_with_cursor_url()<CR>
```

## Feature

* Deoplete.nvim support

    - selected text is sent to deoplete

## Feature



## Licence

MIT Licence
