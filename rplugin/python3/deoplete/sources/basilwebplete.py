'''
deoplete source by using BasilWeb
'''

from .base import Base
import re

class Source(Base):
    def __init__(self, vim):
        Base.__init__(self, vim)
        self.name = 'BasilWeb'
        self.mark = '[BasilWeb]'
        self.min_pattern_length = 1
        self.rank = 500
        self.webtext = []

    def on_init(self, context: dict):
        vars = context['vars']

    def on_event(self, context):
        if context['event'] == 'BufRead':
            pass

    def get_complete_position(self, context: dict):
        self.vim.command(
                "call rpcnotify(0,'NeoVim#plugin','BasilWeb','selectedTextVector')")
        m = re.search(r'\w*$', context['input'])
        return m.start() if m else -1

    def gather_candidates(self, context: dict):
        self.webtext = self.vim.eval('g:basilweb#selected_text_list')
        return [word
                for line in self.webtext
                for word in re.split(r'\W', line)]
